#include <math.h>
#include "flip.h"
#include "config_param.h"
#include "commander.h"
#include "stabilizer.h"

/*FreeRTOS相关头文件*/
#include "FreeRTOS.h"
#include "task.h"

/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ALIENTEK MiniFly
 * 四轴空翻控制代码	
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2018/6/22
 * 版本：V1.2
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/

#define FLIP_RATE		RATE_500_HZ				/*周期*/	
#define MID_ANGLE		(180.f * FLIP_RATE)		/*中间角度 放到500倍*/
#define MAX_FLIP_RATE	1200					/* <2000 */
#define DELTA_RATE		(26000.f/MAX_FLIP_RATE)	/*递增速率*/

#define FLIP_TIMEOUT		500			/*翻滚过程超时时间*/
#define SPEED_UP_TIMEOUT	500			/*加速上升超时时间*/
#define REVER_SPEEDUP_TIME	230			/*反向加速时间*/

#define FLIP_MAX_THRUST		56000		/*最大翻滚油门值*/

static enum
{
	FLIP_IDLE = 0,
	FLIP_SET,
	FLIP_SPEED_UP,
	FLIP_SLOW_DOWN,
	FLIP_PERIOD,
	FLIP_FINISHED,
	REVER_SPEED_UP,
	FLIP_ERROR,
}flipState = FLIP_IDLE;

u8 fstate;				/*翻滚状态*/
enum dir_e flipDir;		/*翻滚方向*/
static u16 maxRateCnt = 0;			/*最大速率计数*/
static float desiredVelZ = 120.f;	/*加速上升期望速度*/
static float currentRate = 0.f;		/*当前速率*/
static float currentAngle = 0.f;	/*当前角度 放大500倍*/

/********************************************************
* Flyer 翻滚检测 
*********************************************************/
void flyerFlipCheck(setpoint_t* setpoint, control_t* control, state_t* state)
{
	static u16 flipThrust = 0;
	static u16 tempThrust = 0;
	static u16 reverTime = 0;
	static u16 flipTimeout = 0;	
	static float pitchTemp = 0.0;
	static float rollTemp = 0.0;
	static float yawTemp = 0.0;
	static float deltaThrust = 0.0;

	fstate = (u8)flipState;
	
	switch(flipState)
	{
		case FLIP_IDLE:	/*翻滚空闲状态*/
		{
			if(flipDir!=CENTER)
			{
				if(control->thrust > 28000 && state->velocity.z > -20.f)
					flipState = FLIP_SET;
				else
					flipDir = CENTER;
			}			
			break;
		}
		case FLIP_SET:	/*翻滚设置*/
		{
			currentRate = 0.f;
			maxRateCnt = 0;
			currentAngle = 0.f;
			
			flipTimeout = 0;
			control->flipDir = flipDir;
			flipThrust = configParam.thrustBase - 2500.0f;
			deltaThrust = configParam.thrustBase / 30.0f;
			tempThrust = flipThrust; 
			
			rollTemp = state->attitude.roll;
			pitchTemp = state->attitude.pitch;									
			yawTemp = state->attitude.yaw;
			
			flipState = FLIP_SPEED_UP;
			break;
		}
		case FLIP_SPEED_UP:	/*加速上升过程*/
		{
			if(state->velocity.z < desiredVelZ)
			{
				setpoint->mode.z = modeDisable;
				if(tempThrust < FLIP_MAX_THRUST)
					tempThrust += deltaThrust;
				setpoint->thrust = tempThrust;
				
				if(flipTimeout++ > SPEED_UP_TIMEOUT)	/*超时处理*/
				{
					flipTimeout = 0;
					flipState = FLIP_SLOW_DOWN;			/*直接进入下一个状态*/
				}														
			}else	
			{	
				flipTimeout = 0;				
				flipState = FLIP_SLOW_DOWN;
			}		
			break;
		}
		case FLIP_SLOW_DOWN:	/*减速过程*/
		{
			if(tempThrust > flipThrust - 3000.f)
			{
				tempThrust -= 2.5f * deltaThrust;
				setpoint->mode.z = modeDisable;
				setpoint->thrust = tempThrust;
			}else
			{
				flipState = FLIP_PERIOD;
			}
		}
		case FLIP_PERIOD:	/*翻滚过程*/
		{
			if(flipTimeout++ > FLIP_TIMEOUT)	/*超时处理*/
			{
				flipTimeout = 0;
				flipState = FLIP_ERROR;
			}
			
			setpoint->mode.z = modeDisable;
			setpoint->thrust = flipThrust - 3*currentRate;
			
			currentAngle += currentRate;		/*当前角度 放大500倍*/
			
			if(currentAngle < MID_ANGLE)		/*上半圈*/
			{
				if(currentRate < MAX_FLIP_RATE)/*小于最大速率，速率继续增大*/
					currentRate += DELTA_RATE;
				else			/*大于最大速率，速率保持*/
					maxRateCnt++;					
			}else	/*下半圈*/
			{
				if(maxRateCnt > 1)
				{						
					maxRateCnt--;			
				}else
				{
					if(currentRate >= DELTA_RATE && currentAngle < 2*MID_ANGLE)
					{
						currentRate -= DELTA_RATE;	
					}																
					else							
						flipState = FLIP_FINISHED;						
				}
			}
			
			switch(control->flipDir)	
			{
				case FORWARD:	/* pitch+ */
					setpoint->attitude.pitch = currentRate;	
					setpoint->attitude.roll = state->attitude.roll = rollTemp;
					setpoint->attitude.yaw = state->attitude.yaw = yawTemp;
					break;				
				case BACK:		/* pitch- */
					setpoint->attitude.pitch = -currentRate;
					setpoint->attitude.roll = state->attitude.roll = rollTemp;
					setpoint->attitude.yaw = state->attitude.yaw = yawTemp;
					break;
				case LEFT:		/* roll- */
					setpoint->attitude.roll = -currentRate;	
					setpoint->attitude.pitch = state->attitude.pitch = pitchTemp;
					setpoint->attitude.yaw = state->attitude.yaw = yawTemp;
					break;
				case RIGHT:		/* roll+ */					
					setpoint->attitude.roll = currentRate;
					setpoint->attitude.pitch = state->attitude.pitch = pitchTemp;
					setpoint->attitude.yaw = state->attitude.yaw = yawTemp;
					break;
				default :break;
			}
			break;
		}
		case FLIP_FINISHED:	/*翻滚完成*/
		{
			setpoint->mode.z = modeDisable;
			setpoint->thrust = tempThrust;
			tempThrust = flipThrust;
			
			reverTime = 0;
			flipTimeout = 0;
			flipDir = CENTER;	
			control->flipDir = flipDir;

			flipState = REVER_SPEED_UP;
			break;
		}
		case REVER_SPEED_UP:	/*翻滚完成后 反向加速*/
		{			
			if(reverTime++<REVER_SPEEDUP_TIME)	
			{
				if(tempThrust < FLIP_MAX_THRUST)
					tempThrust += 2.5f * deltaThrust;
				setpoint->mode.z = modeDisable;
				setpoint->thrust = tempThrust;
			}else
			{				
				flipTimeout = 0;
				flipState = FLIP_IDLE;	
			}
			break;
		}
		case FLIP_ERROR:
		{
			reverTime = 0;
			flipDir = CENTER;	
			control->flipDir = CENTER;
			
			setpoint->mode.z = modeDisable;
			setpoint->thrust = 0;
			if(flipTimeout++ > 1)
			{
				flipTimeout = 0;
				flipState = FLIP_IDLE;
			}
			break;
		}
		default : break;
	}			
}


//设置翻滚方向
void setFlipDir(u8 dir)
{
	flipDir = (enum dir_e)dir;	
}

