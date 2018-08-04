/*
 * File      : keyTask.h
 * This file is part of RT-Thread minifly
 * COPYRIGHT (C) 2018, RT-Thread minifly Development Team
 *
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-08-02     xinping yang      the first version
 */

#ifndef __KEY_TASK_H__
#define __KEY_TASK_H__

#include "system.h"

#define KEY_L  		GPIO_Pin_11
#define KEY_R  		GPIO_Pin_15
#define KEY_S1   	GPIO_Pin_10
#define KEY_S2   	GPIO_Pin_8

#define READ_KEY_L()  	GPIO_ReadInputDataBit(GPIOB,KEY_L)	
#define READ_KEY_R() 	GPIO_ReadInputDataBit(GPIOC,KEY_R)	
#define READ_KEY_J1()  	GPIO_ReadInputDataBit(GPIOB,KEY_S1)	
#define READ_KEY_J2()  	GPIO_ReadInputDataBit(GPIOA,KEY_S2)	

//Key Status
#define KEY_L_SHORT_PRESS  	1	
#define KEY_L_LONG_PRESS	  2
#define KEY_R_SHORT_PRESS  	3	
#define KEY_R_LONG_PRESS	  4	
#define KEY_J1_SHORT_PRESS	5	
#define KEY_J1_LONG_PRESS	  6
#define KEY_J2_SHORT_PRESS	7
#define KEY_J2_LONG_PRESS  	8




void keyInit(void);
	
void KEY_Scan(void);

void keyTask(void* parameter);
u8 getKeyState(void);
#endif

