#include "beep.h"
/*FreeRtos includes*/
#include <rtthread.h>

/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ALIENTEK MiniFly_Remotor
 * 蜂鸣器驱动代码	
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/


/*蜂鸣器对应报警的延时时间（ms）*/
#define  TRIM			100
#define  FLIP			50 
#define  LOWPOWER		500
#define  CANFLY			100

beepSeq_t beepAction[] =
{
	{false, true, TRIM},
	{false, true, FLIP},
	{false, true, LOWPOWER},
	{false, false, CANFLY},
};

#define  ACTION_NUM	(sizeof(beepAction)/sizeof(beepSeq_t))//动作个数


static bool isInit;
static bool beepEnable;
static rt_timer_t  beepTimer;
enum beepAction currentRun;
	
/* 蜂鸣器IO口初始化 */
static void beepIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
 	/* 初始化蜂鸣器(PC14) */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOC,GPIO_Pin_14);
}

void runBeepseq(void* parameter)
{
	u8 i;
	if(beepAction[currentRun].isLoop == false)
	{
		beepAction[currentRun].isActive = false;
	}	
	for(i=0; i<ACTION_NUM; i++)
	{
		if(beepAction[i].isActive == true) 
		{
			rt_timer_control(beepTimer, RT_TIMER_CTRL_SET_TIME, (void *)&beepAction[i].waitMS);
			break;
		}
	}
	if(i == ACTION_NUM)/*当前无动作运行*/
	{
		beepEnable = false;
	}
	if(beepEnable)
		BEEP = !BEEP;
	else
		BEEP = 0;
}

void beepInit(void)
{
	if(isInit) return;
	beepIO_Init();
	beepTimer = rt_timer_create("beepTimer", runBeepseq, RT_NULL, 1000,  RT_TIMER_FLAG_PERIODIC);
	if (beepTimer != RT_NULL)
        rt_timer_start(beepTimer);
	isInit = true;
}

void runBeepAcktion(enum beepAction action)
{
	currentRun = action;
	beepAction[action].isActive = true;
	rt_timer_control(beepTimer, RT_TIMER_CTRL_SET_TIME, (void *)&beepAction[action].waitMS);
	BEEP = 1;
	beepEnable = true;
}

void stopBeepAcktion(enum beepAction action)
{
	beepAction[action].isActive = false;
	for(u8 i=0; i<ACTION_NUM; i++)
	{
		if(beepAction[i].isActive == true)
		{
			currentRun = (enum beepAction)i;
			rt_timer_control(beepTimer, RT_TIMER_CTRL_SET_TIME, (void *)&beepAction[currentRun].waitMS);
			return;
		}	
	}
	beepEnable = false;
}

