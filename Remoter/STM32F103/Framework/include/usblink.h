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

void usblinkTxTask(void* parameter);
void usblinkRxTask(void* parameter);

#endif

