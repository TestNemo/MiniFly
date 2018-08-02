/*
 * File      : display.c
 * This file is part of RT-Thread minifly
 * COPYRIGHT (C) 2018, RT-Thread minifly Development Team
 *
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-08-02     xinping yang      the first version
 */
#include "display.h"
#include <rtthread.h>

void displayTask(void* parameter)
{
	rt_kprintf("display task entry\n");
}
