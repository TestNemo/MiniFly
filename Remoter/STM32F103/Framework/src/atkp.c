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

void radiolinkDataProcessTask(void* parameter)
{
	rt_kprintf("radiolink data process task entry\n");
}
void usblinkDataProcessTask(void* parameter)
{
	rt_kprintf("usblink data process task entry\n");
}
