/*
 * File      :usblink.h
 * This file is part of RT-Thread minifly
 * COPYRIGHT (C) 2018, RT-Thread minifly Development Team
 *
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-08-02     xinping yang      the first version
 */

#ifndef __USB_LINK_H
#define __USB_LINK_H
#include "atkp.h"
#include <stdbool.h>

void usblinkInit(void);
bool usblinkSendPacket(const atkp_t *p);
bool usblinkSendPacketBlocking(const atkp_t *p);
bool usblinkReceivePacket(atkp_t *p);
bool usblinkReceivePacketBlocking(atkp_t *p);
void usblinkTxTask(void* param);
void usblinkRxTask(void *param);


#endif

