#include <math.h>
#include "flip.h"
#include "config_param.h"
#include "commander.h"
#include "stabilizer.h"

/*FreeRTOS���ͷ�ļ�*/
#include "FreeRTOS.h"
#include "task.h"

/********************************************************************************	 
 * ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
 * ALIENTEK MiniFly
 * ����շ����ƴ���	
 * ����ԭ��@ALIENTEK
 * ������̳:www.openedv.com
 * ��������:2018/6/22
 * �汾��V1.2
 * ��Ȩ���У�����ؾ���
 * Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
 * All rights reserved
********************************************************************************/

#define FLIP_RATE		RATE_500_HZ				/*����*/	
#define MID_ANGLE		(180.f * FLIP_RATE)		/*�м�Ƕ� �ŵ�500��*/
#define MAX_FLIP_RATE	1200					/* <2000 */
#define DELTA_RATE		(26000.f/MAX_FLIP_RATE)	/*��������*/

#define FLIP_TIMEOUT		500			/*�������̳�ʱʱ��*/
#define SPEED_UP_TIMEOUT	500			/*����������ʱʱ��*/
#define REVER_SPEEDUP_TIME	230			/*�������ʱ��*/

#define FLIP_MAX_THRUST		56000		/*��󷭹�����ֵ*/

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

u8 fstate;				/*����״̬*/
enum dir_e flipDir;		/*��������*/
static u16 maxRateCnt = 0;			/*������ʼ���*/
static float desiredVelZ = 120.f;	/*�������������ٶ�*/
static float currentRate = 0.f;		/*��ǰ����*/
static float currentAngle = 0.f;	/*��ǰ�Ƕ� �Ŵ�500��*/

/********************************************************
* Flyer ������� 
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
		case FLIP_IDLE:	/*��������״̬*/
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
		case FLIP_SET:	/*��������*/
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
		case FLIP_SPEED_UP:	/*������������*/
		{
			if(state->velocity.z < desiredVelZ)
			{
				setpoint->mode.z = modeDisable;
				if(tempThrust < FLIP_MAX_THRUST)
					tempThrust += deltaThrust;
				setpoint->thrust = tempThrust;
				
				if(flipTimeout++ > SPEED_UP_TIMEOUT)	/*��ʱ����*/
				{
					flipTimeout = 0;
					flipState = FLIP_SLOW_DOWN;			/*ֱ�ӽ�����һ��״̬*/
				}														
			}else	
			{	
				flipTimeout = 0;				
				flipState = FLIP_SLOW_DOWN;
			}		
			break;
		}
		case FLIP_SLOW_DOWN:	/*���ٹ���*/
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
		case FLIP_PERIOD:	/*��������*/
		{
			if(flipTimeout++ > FLIP_TIMEOUT)	/*��ʱ����*/
			{
				flipTimeout = 0;
				flipState = FLIP_ERROR;
			}
			
			setpoint->mode.z = modeDisable;
			setpoint->thrust = flipThrust - 3*currentRate;
			
			currentAngle += currentRate;		/*��ǰ�Ƕ� �Ŵ�500��*/
			
			if(currentAngle < MID_ANGLE)		/*�ϰ�Ȧ*/
			{
				if(currentRate < MAX_FLIP_RATE)/*С��������ʣ����ʼ�������*/
					currentRate += DELTA_RATE;
				else			/*����������ʣ����ʱ���*/
					maxRateCnt++;					
			}else	/*�°�Ȧ*/
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
		case FLIP_FINISHED:	/*�������*/
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
		case REVER_SPEED_UP:	/*������ɺ� �������*/
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


//���÷�������
void setFlipDir(u8 dir)
{
	flipDir = (enum dir_e)dir;	
}

