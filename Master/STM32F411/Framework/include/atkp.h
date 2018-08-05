/*
 * File      :atkp.h
 * This file is part of RT-Thread minifly
 * COPYRIGHT (C) 2018, RT-Thread minifly Development Team
 *
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-08-06     xinping yang      the first version
 */

// <<< Use Configuration Wizard in Context Menu >>>
#ifndef __ATKP_H__
#define __ATKP_H__

#include <stm32f4xx.h>
#include <rtthread.h>

void atkpTxTask(void* param);
void atkpRxAnlTask(void* param);

#endif

