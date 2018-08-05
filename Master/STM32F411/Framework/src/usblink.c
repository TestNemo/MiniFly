/*
 * File      :usblink.c
 * This file is part of RT-Thread minifly
 * COPYRIGHT (C) 2018, RT-Thread minifly Development Team
 *
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-08-06     xinping yang      the first version
 */

#include "usblink.h"

void usblinkRxTask(void* param)
{
  rt_kprintf("usblinkRx Task entry\n");
}
void usblinkTxTask(void* param)
{
  rt_kprintf("usblinkTx Task entry\n");
}