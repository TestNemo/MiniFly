/*
 * File      : radiolink.h
 * This file is part of RT-Thread minifly
 * COPYRIGHT (C) 2018, RT-Thread minifly Development Team
 *
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-08-02     xinping yang      the first version
 */

#ifndef __RADIO_LINK_H
#define __RADIO_LINK_H

#include <stdbool.h>
#include "system.h"
#include "atkp.h"
/*FreeRtos includes*/
//#include "FreeRTOS.h"
//#include "task.h"


#define  U_RADIO_RSSI		0x01
#define  U_RADIO_CONFIG		0x02	


#define  D_RADIO_HEARTBEAT	0xFF
#define  D_RADIO_GET_CONFIG	0x01
#define  D_RADIO_SET_CONFIG	0x02

//extern xTaskHandle radiolinkTaskHandle;

void radiolinkInit(void);
bool radiolinkSendPacket(const atkp_t *p);
bool radiolinkSendPacketBlocking(const atkp_t *p);
bool radiolinkReceivePacket(atkp_t *p);
bool radiolinkReceivePacketBlocking(atkp_t *p);
void radiolinkTask(void* param);
u16 radioinkFailRxcount(void);
bool radioinkConnectStatus(void);
void radiolinkEnable(FunctionalState state);

void radiolinkTask(void* parameter);
#endif

