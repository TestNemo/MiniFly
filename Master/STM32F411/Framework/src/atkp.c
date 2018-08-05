/*
 * File      :atkp.c
 * This file is part of RT-Thread minifly
 * COPYRIGHT (C) 2018, RT-Thread minifly Development Team
 *
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-08-06     xinping yang      the first version
 */

#include "atkp.h"

void atkpTxTask(void* param)
{
  rt_kprintf("atkpTx Task entry\n");
}
void atkpRxAnlTask(void* param)
{
  rt_kprintf("atkpRxAnl Task entry\n");
}