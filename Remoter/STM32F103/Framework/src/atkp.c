/*
 * File      : atkp.c
 * This file is part of RT-Thread minifly
 * COPYRIGHT (C) 2018, RT-Thread minifly Development Team
 *
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-08-02     xinping yang      the first version
 */

#include "atkp.h"
#include <rtthread.h>
#include <string.h>
#include "remoter_ctrl.h"
#include "radiolink.h"
#include "usblink.h"
#include "trim_ui.h"
#include "beep.h"
#include "main_ui.h"
#include "match_ui.h"

float plane_yaw,plane_roll,plane_pitch;
float plane_bat;
u8 rssi;


static void atkpAnalyze(atkp_t *p)
{
	if(p->msgID == UP_STATUS)
	{
		plane_roll = (s16)(*(p->data+0)<<8)|*(p->data+1);
		plane_roll = plane_roll/100;
		plane_pitch = (s16)(*(p->data+2)<<8)|*(p->data+3);
		plane_pitch = plane_pitch/100;
		plane_yaw = (s16)(*(p->data+4)<<8)|*(p->data+5);
		plane_yaw = plane_yaw/100;
	}
	else if(p->msgID == UP_POWER)
	{
		plane_bat = (s16)(*(p->data+0)<<8)|*(p->data+1);
		plane_bat = plane_bat/100;
	}
	else if(p->msgID == UP_REMOTOR)	
	{
		switch(p->data[0])
		{
			case ACK_MSG:
				miniFlyMsgACKProcess(p);
				break;
		}
	}
	else if(p->msgID == UP_RADIO)
	{
		radioConfig_t radio;
		switch(p->data[0])
		{
			case U_RADIO_RSSI:
				rssi = p->data[1];
				break;
			
			case U_RADIO_CONFIG:
				memcpy(&radio, p->data+1, sizeof(radio));
				setMatchRadioConfig(&radio);
				break;
		}
	}
}

void radiolinkDataProcessTask(void *param) 
{
	atkp_t p;
	while(1)
	{
		radiolinkReceivePacketBlocking(&p);
		atkpAnalyze(&p);
		usblinkSendPacket(&p);
		rt_thread_delay(1);
//		vTaskDelay(1);
	}
}

void usblinkDataProcessTask(void *param)
{
	atkp_t p;
	while(1)
	{
		usblinkReceivePacketBlocking(&p);
		atkpAnalyze(&p);
		radiolinkSendPacket(&p);
	}
}

//void radiolinkDataProcessTask(void* parameter)
//{
//	rt_kprintf("radiolink data process task entry\n");
//}
//void usblinkDataProcessTask(void* parameter)
//{
//	rt_kprintf("usblink data process task entry\n");
//}
