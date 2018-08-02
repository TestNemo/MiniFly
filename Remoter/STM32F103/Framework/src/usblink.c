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

void usblinkTxTask(void* parameter)
{
	rt_kprintf("usblink tx task entry\n");
}
void usblinkRxTask(void* parameter)
{
	rt_kprintf("usblink rx task entry\n");
}
