/*
 * File      :usblink.h
 * This file is part of RT-Thread minifly
 * COPYRIGHT (C) 2018, RT-Thread minifly Development Team
 *
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-08-06     xinping yang      the first version
 */


// <<< Use Configuration Wizard in Context Menu >>>
#ifndef __USB_LINK_H__
#define __USB_LINK_H__

#include <stm32f4xx.h>
#include <rtthread.h>


void usblinkRxTask(void* param);
void usblinkTxTask(void* param);

#endif

// <<< Use Configuration Wizard in Context Menu >>>
