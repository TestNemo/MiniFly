/*
 * File      :usblink.c
 * This file is part of RT-Thread minifly
 * COPYRIGHT (C) 2018, RT-Thread minifly Development Team
 *
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-08-02     xinping yang      the first version
 */
 
#include "usblink.h"
#include <rtthread.h>
#include <string.h>
#include "hw_config.h"
#include "usb_pwr.h"
#include "debug_assert.h"
/*FreeRtos includes*/
//#include "FreeRTOS.h"
//#include "task.h"
//#include "queue.h"
//void usblinkTxTask(void* parameter)
//{
//	rt_kprintf("usblink tx task entry\n");
//}
//void usblinkRxTask(void* parameter)
//{
//	rt_kprintf("usblink rx task entry\n");
//}
#define USBLINK_TX_QUEUE_SIZE 16
#define USBLINK_RX_QUEUE_SIZE 16

static enum
{
	waitForStartByte1,
	waitForStartByte2,
	waitForMsgID,
	waitForDataLength,
	waitForData,
	waitForChksum1,
}rxState;

static bool isInit;
static atkp_t rxPacket;
//static xQueueHandle  txQueue;
//static xQueueHandle  rxQueue;
static rt_mq_t txQueue;
static rt_mq_t rxQueue;


void usblinkInit(void)
{
	if (isInit) return;
//	txQueue = xQueueCreate(USBLINK_TX_QUEUE_SIZE, sizeof(atkp_t));
//	ASSERT(txQueue);
//	rxQueue = xQueueCreate(USBLINK_RX_QUEUE_SIZE, sizeof(atkp_t));
//	ASSERT(rxQueue);
	txQueue = rt_mq_create("rtQueue", sizeof(atkp_t), USBLINK_TX_QUEUE_SIZE,RT_IPC_FLAG_FIFO);
	ASSERT(txQueue);
	rxQueue = rt_mq_create("rxQueue", sizeof(atkp_t), USBLINK_RX_QUEUE_SIZE,RT_IPC_FLAG_FIFO);
	ASSERT(rxQueue);
	isInit = true;
}

bool usblinkSendPacket(const atkp_t *p)
{
	ASSERT(p);
	ASSERT(p->dataLen <= ATKP_MAX_DATA_SIZE);
	//return xQueueSend(txQueue, p, 0);
	return rt_mq_send(txQueue, &p, sizeof(p));
}
bool usblinkSendPacketBlocking(const atkp_t *p)
{
	ASSERT(p);
	ASSERT(p->dataLen <= ATKP_MAX_DATA_SIZE);
//	return xQueueSend(txQueue, p, portMAX_DELAY);
	return rt_mq_send(txQueue, &p, sizeof(p));
}


bool usblinkReceivePacket(atkp_t *p)
{
	ASSERT(p);
//	return xQueueReceive(rxQueue, p, 0);
	return rt_mq_recv(rxQueue, p, sizeof(p), 0);
}
bool usblinkReceivePacketBlocking(atkp_t *p)
{
	ASSERT(p);
//	return xQueueReceive(rxQueue, p, portMAX_DELAY);
	return rt_mq_recv(rxQueue, p, sizeof(p),RT_WAITING_FOREVER); 
}


void usblinkTxTask(void* param)
{
	atkp_t p;
	u8 sendBuffer[64];
	u8 cksum;
	u8 dataLen;
	while(bDeviceState != CONFIGURED)
	{
//		vTaskDelay(1000);
		rt_thread_delay(1000);
	}
	while(1)
	{
//		xQueueReceive(txQueue, &p, portMAX_DELAY);
		rt_mq_recv(txQueue, &p, sizeof(atkp_t), RT_WAITING_FOREVER);
		if(p.msgID != UP_RADIO)
		{
			if(p.msgID == UP_PRINTF)
			{
				memcpy(&sendBuffer, p.data, p.dataLen);
				dataLen = p.dataLen;
			}
			else
			{
				sendBuffer[0] = UP_BYTE1;
				sendBuffer[1] = UP_BYTE2;
				sendBuffer[2] = p.msgID;
				sendBuffer[3] = p.dataLen;
				memcpy(&sendBuffer[4], p.data, p.dataLen);
				cksum = 0;
				for (int i=0; i<p.dataLen+4; i++)
				{
					cksum += sendBuffer[i];
				}
				dataLen = p.dataLen+5;
				sendBuffer[dataLen - 1] = cksum;
			}
			usbsendData(sendBuffer, dataLen);
		}		
	}
}


void usblinkRxTask(void *param)
{
	u8 c;
	u8 dataIndex = 0;
	u8 cksum = 0;
	rxState = waitForStartByte1;
	while(1)
	{
		if (usbGetDataWithTimout(&c))
		{
			switch(rxState)
			{
				case waitForStartByte1:
					rxState = (c == DOWN_BYTE1) ? waitForStartByte2 : waitForStartByte1;
					cksum = c;
					break;
				case waitForStartByte2:
					rxState = (c == DOWN_BYTE2) ? waitForMsgID : waitForStartByte1;
					cksum += c;
					break;
				case waitForMsgID:
					rxPacket.msgID = c;
					rxState = waitForDataLength;
					cksum += c;
					break;
				case waitForDataLength:
					if (c <= ATKP_MAX_DATA_SIZE)
					{
						rxPacket.dataLen = c;
						dataIndex = 0;
						rxState = (c > 0) ? waitForData : waitForChksum1;	/*c=0,?????0,??1*/
						cksum += c;
					} else 
					{
						rxState = waitForStartByte1;
					}
					break;
				case waitForData:
					rxPacket.data[dataIndex] = c;
					dataIndex++;
					cksum += c;
					if (dataIndex == rxPacket.dataLen)
					{
						rxState = waitForChksum1;
					}
					break;
				case waitForChksum1:
					if (cksum == c)
					{
//						xQueueSend(rxQueue, &rxPacket, 0);
						rt_mq_send(rxQueue, &rxPacket, sizeof(rxPacket));
					} 
					else
					{
						rxState = waitForStartByte1;
					}
					rxState = waitForStartByte1;
					break;
				default:
					break;
			}
		}
		else
		{
			rxState = waitForStartByte1;
		}
	}
}
