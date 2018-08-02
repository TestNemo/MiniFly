/*
 * File      : keyTask.c
 * This file is part of RT-Thread minifly
 * COPYRIGHT (C) 2018, RT-Thread minifly Development Team
 *
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-08-02     xinping yang      the first version
 */
#include "keytask.h"
#include <rtthread.h>

void keyTask(void* parameter)
{
	rt_kprintf("key task entry\n");
}
