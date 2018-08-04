/*
 * File      : radiolink.c
 * This file is part of RT-Thread minifly
 * COPYRIGHT (C) 2018, RT-Thread minifly Development Team
 *
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-08-02     xinping yang      the first version
 */
 
#include "radiolink.h"
#include <rtthread.h>
#include "led.h"
#include "24l01.h"
#include "oled.h"
#include "config_param.h"
#include "debug_assert.h"

//void radiolinkTask(void* parameter)
//{
//	rt_kprintf("radiolink task entry\n");
//}

#define  RADIOLINK_TX_QUEUE_SIZE  10
#define  RADIOLINK_RX_QUEUE_SIZE  10

//xTaskHandle radiolinkTaskHandle;
//static xQueueHandle  txQueue;
static struct rt_messagequeue txQueue;
//static xQueueHandle  rxQueue;
static struct rt_messagequeue rxQueue;
static atkp_t msg_pool[RADIOLINK_TX_QUEUE_SIZE];
//static xSemaphoreHandle nrfIT;
static rt_sem_t nrfIT;
static bool isInit;
static bool connectStatus;
static atkp_t tx_p;
static u8 statusCount;
static u16 failRxCount;
static u16 failReceiveNum;
//static TickType_t failRxcountTime;
static rt_tick_t failRxcountTime;


static void nrf_interruptCallback(void)
{
//	portBASE_TYPE  xHigherPriorityTaskWoken = pdFALSE;
//	xSemaphoreGiveFromISR(nrfIT, &xHigherPriorityTaskWoken);
	rt_sem_release(nrfIT);
}


static void radioInit(void)
{
	uint64_t addr = (uint64_t)configParam.radio.addressHigh<<32 | configParam.radio.addressLow;
	if(nrf_check() == SUCCESS)
	{
		nrfInit(PTX_MODE);
		nrf_setIterruptCallback(nrf_interruptCallback);
	}
	else
	{
		oledInit();
		oled_showString(0,0,(u8*)"NRF24L01 CHECK FAIL !",6,12);
		oled_refreshGram();
		while(1);
	}
	nrf_setAddress(addr);
	nrf_setChannel(configParam.radio.channel);
	nrf_setDataRate(configParam.radio.dataRate);
}


void radiolinkInit(void)
{
	rt_err_t result;
	if (isInit) return;
	radioInit();
	
//	txQueue = xQueueCreate(RADIOLINK_TX_QUEUE_SIZE, sizeof(atkp_t));
	result = rt_mq_init(&txQueue, "txQueue", 
	    &msg_pool[0],
	    128 - sizeof(void*),
			sizeof(msg_pool),
			RT_IPC_FLAG_FIFO);
	if (result != RT_EOK)
  {
	  rt_kprintf("tx message queue failed.\n");
		return;
	}
	result = rt_mq_init(&rxQueue, "rxQueue", 
	    &msg_pool[0],
	    128 - sizeof(void*),
			sizeof(msg_pool),
			RT_IPC_FLAG_FIFO);
	if (result != RT_EOK)
  {
	  rt_kprintf("rx message queue failed.\n");
		return;
	}
//	ASSERT(txQueue);
//	rxQueue = xQueueCreate(RADIOLINK_RX_QUEUE_SIZE, sizeof(atkp_t));
//	ASSERT(rxQueue);
	
	//nrfIT = xSemaphoreCreateBinary();
	nrfIT = rt_sem_create("nrfIT", 0, RT_IPC_FLAG_FIFO);

	
	tx_p.msgID = DOWN_RADIO;
	tx_p.dataLen = 1;
	tx_p.data[0] = D_RADIO_HEARTBEAT;
	connectStatus = false;
	isInit = true;
}


bool radiolinkSendPacket(const atkp_t *p)
{
	int result;
	ASSERT(p);
	ASSERT(p->dataLen <= ATKP_MAX_DATA_SIZE);
	result = rt_mq_send(&txQueue, (void*)p, sizeof(atkp_t));
	if (result != RT_EOK)
  {
    return RT_TRUE;	  
	}
	else 
  {
	   return RT_FALSE;
	}
	//return xQueueSend(txQueue, p, 0);
}
bool radiolinkSendPacketBlocking(const atkp_t *p)
{
	int result;
	ASSERT(p);
	ASSERT(p->dataLen <= ATKP_MAX_DATA_SIZE);
	result = rt_mq_send(&txQueue, (void*)p, sizeof(p));
	if (result != RT_EOK)
  {
    return RT_TRUE;	  
	}
	else 
  {
	   return RT_FALSE;
	}
	
	//return xQueueSend(txQueue, p, 100);//portMAX_DELAY
}


bool radiolinkReceivePacket(atkp_t *p)
{
	
	ASSERT(p);
	if (rt_mq_recv(&rxQueue, &p[0], sizeof(p), 0) == RT_EOK)
  {
	  return RT_TRUE;
	}
	else 
  {
	  return RT_FALSE;
	}
	//return xQueueReceive(rxQueue, p, 0);
}
bool radiolinkReceivePacketBlocking(atkp_t *p)
{
	ASSERT(p);
	//return xQueueReceive(rxQueue, p, portMAX_DELAY);
	if (rt_mq_recv(&rxQueue, &p[0], sizeof(p), RT_WAITING_FOREVER) == RT_EOK)
  {
	  return RT_TRUE;
	}
	else 
  {
	  return RT_FALSE;
	}
}


void radiolinkTask(void* param)
{
	u8 rx_len;
	atkp_t rx_p;
	rt_kprintf("radiolink task entry\n");
	while(1)
	{
		nrf_txPacket((u8*)&tx_p, tx_p.dataLen+2);
		//xSemaphoreTake(nrfIT, 1000);
		rt_sem_take(nrfIT, 1000);
		nrfEvent_e status = nrf_checkEventandRxPacket((u8*)&rx_p, &rx_len);
		if(status == RX_DR)//
		{	
			LED_BLUE = 0;
			LED_RED  = 1;
			statusCount = 0;
			connectStatus = true;
//			if(rx_p.dataLen <= ATKP_MAX_DATA_SIZE)
//			{
//				xQueueSend(rxQueue, &rx_p, portMAX_DELAY);
//			}
			rt_mq_send(&txQueue, &rx_p, sizeof(rx_p));

			//if(xQueueReceive(txQueue, &tx_p, 0) == pdFALSE)
			if (rt_mq_recv(&txQueue, &tx_p, sizeof(tx_p), RT_WAITING_FOREVER))
			{
				tx_p.msgID = DOWN_RADIO;
				tx_p.dataLen = 1;
				tx_p.data[0] = D_RADIO_HEARTBEAT;
			}
		}
		else if(status == MAX_RT)
		{
			LED_BLUE = 1;
			LED_RED  = 0;
			failRxCount++;
			if(++statusCount > 10)
			{
				statusCount = 0;
				connectStatus = false;
			}
		}
		
		
		//if(connectStatus==true && xTaskGetTickCount()>=failRxcountTime+1000)
		if(connectStatus==true && rt_tick_get()>=failRxcountTime+1000)
		{
			//failRxcountTime = xTaskGetTickCount();
			failRxcountTime = rt_tick_get();
			failReceiveNum = failRxCount;
			failRxCount = 0;
		}
		
	}
}


u16 radioinkFailRxcount(void)
{
	return failReceiveNum;
}


bool radioinkConnectStatus(void)
{
	return connectStatus;
}


void radiolinkEnable(FunctionalState state)
{
	rt_thread_t radiolinkTaskHandle;
	radiolinkTaskHandle = rt_thread_self();
	if(state == ENABLE)
		rt_thread_resume(radiolinkTaskHandle);
	else
		rt_thread_suspend(radiolinkTaskHandle);
//	if(state == ENABLE)
//		vTaskResume(radiolinkTaskHandle);
//	else
//		vTaskSuspend(radiolinkTaskHandle);
}
