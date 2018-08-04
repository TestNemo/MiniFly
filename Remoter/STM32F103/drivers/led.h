/*
 * File      : led.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */

#ifndef __LED_H__
#define __LED_H__

#include <rtthread.h>
#include "system.h"
// led define
#ifdef STM32_SIMULATOR
#define led1_rcc                    RCC_APB2Periph_GPIOA
#define led1_gpio                   GPIOA
#define led1_pin                    (GPIO_Pin_5)
#define led2_rcc                    RCC_APB2Periph_GPIOA
#define led2_gpio                   GPIOA
#define led2_pin                    (GPIO_Pin_6)
#else
#define led1_rcc                    RCC_APB2Periph_GPIOB
#define led1_gpio                   GPIOB
#define led1_pin                    (GPIO_Pin_3)

#define led2_rcc                    RCC_APB2Periph_GPIOB
#define led2_gpio                   GPIOB
#define led2_pin                    (GPIO_Pin_7)
#endif // led define #ifdef STM32_SIMULATOR

#define LED_BLUE 		PBout(3)
#define LED_RED 		PBout(7)

void ledInit(void);
void rt_hw_led_init(void);
void rt_hw_led_on(rt_uint32_t led);
void rt_hw_led_off(rt_uint32_t led);

#endif
