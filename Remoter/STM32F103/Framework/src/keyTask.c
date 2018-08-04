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
#include <stdbool.h>

#define LONG_PRESS_COUNT 	500	

#define PRESSED		0
#define RELEASED	1

static bool keyL_pressed;
static bool keyR_pressed;
static bool keyJ1_pressed;
static bool keyJ2_pressed;
static u8 keyState;
static u32 pressedTime;

//按键IO初始化函数
void keyInit(void) 
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  = KEY_S2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = KEY_S1 | KEY_L;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin  = KEY_R;
	
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void keyTask(void* parameter)
{
	keyInit();
	rt_kprintf("key task entry\n");
	while(1)
	{
		//vTaskDelay(20);
		rt_thread_delay(20);
		if(keyL_pressed==false && READ_KEY_L()==PRESSED)
		{
			keyL_pressed = true;
//			pressedTime = xTaskGetTickCount();
			pressedTime = rt_tick_get();
		}
		if(keyR_pressed==false && READ_KEY_R()==PRESSED)
		{
			keyR_pressed = true;
//			pressedTime = xTaskGetTickCount();
			pressedTime = rt_tick_get();
		}
		if(keyJ1_pressed==false && READ_KEY_J1()==PRESSED)
		{
			keyJ1_pressed = true;
//			pressedTime = xTaskGetTickCount();
			pressedTime = rt_tick_get();
		}
		if(keyJ2_pressed==false && READ_KEY_J2()==PRESSED)
		{
			keyJ2_pressed = true;
//			pressedTime = xTaskGetTickCount();
			pressedTime = rt_tick_get();
		}
		
		if(keyL_pressed==true)
		{
			if(READ_KEY_L()==RELEASED)
				keyL_pressed = false;
//			if((xTaskGetTickCount() - pressedTime) > LONG_PRESS_COUNT)
			if((rt_tick_get() - pressedTime) > LONG_PRESS_COUNT)
				keyState = KEY_L_LONG_PRESS;
			else if(READ_KEY_L()==RELEASED)
				keyState = KEY_L_SHORT_PRESS;
		}
		if(keyR_pressed==true)
		{
			if(READ_KEY_R()==RELEASED)
				keyR_pressed = false;
			//if((xTaskGetTickCount() - pressedTime) > LONG_PRESS_COUNT)
			if((rt_tick_get() - pressedTime) > LONG_PRESS_COUNT)
				keyState = KEY_R_LONG_PRESS;
			else if(READ_KEY_R()==RELEASED)
				keyState = KEY_R_SHORT_PRESS;
		}
		if(keyJ1_pressed==true)
		{
			if(READ_KEY_J1()==RELEASED)
				keyJ1_pressed = false;
//			if((xTaskGetTickCount() - pressedTime) > LONG_PRESS_COUNT)
			if((rt_tick_get() - pressedTime) > LONG_PRESS_COUNT)
				keyState = KEY_J1_LONG_PRESS;
			else if(READ_KEY_J1()==RELEASED)
				keyState = KEY_J1_SHORT_PRESS;
		}
		if(keyJ2_pressed==true)
		{
			if(READ_KEY_J2()==RELEASED)
				keyJ2_pressed = false;
//			if((xTaskGetTickCount() - pressedTime) > LONG_PRESS_COUNT)
			if((rt_tick_get() - pressedTime) > LONG_PRESS_COUNT)
				keyState = KEY_J2_LONG_PRESS;
			else if(READ_KEY_J2()==RELEASED)
				keyState = KEY_J2_SHORT_PRESS;
		}

	}
}


u8 getKeyState(void)
{
	u8 temp;
	temp = keyState;
	keyState = 0;
	return temp;
}
