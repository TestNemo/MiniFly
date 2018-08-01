#include <math.h>
#include "maths.h"
#include "commander.h"
#include "atkp.h"
#include "config_param.h"
#include "radiolink.h"
#include "remoter_ctrl.h"
#include "stabilizer.h"
#include "state_estimator.h"
#include "position_pid.h"
#include "module_mgt.h"
#include "optical_flow.h"

/*FreeRTOS���ͷ�ļ�*/
#include "FreeRTOS.h"
#include "task.h"

/********************************************************************************	 
 * ������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
 * ALIENTEK MiniFly
 * ��ȡң��������������
 * ����ԭ��@ALIENTEK
 * ������̳:www.openedv.com
 * ��������:2018/6/22
 * �汾��V1.2
 * ��Ȩ���У�����ؾ���
 * Copyright(C) �������������ӿƼ����޹�˾ 2014-2024
 * All rights reserved
********************************************************************************/

#define CLIMB_RATE			100.f
#define MAX_CLIMB_UP		120.f
#define MAX_CLIMB_DOWN		100.f

#define MIN_THRUST  	5000
#define MAX_THRUST  	60000

static bool isRCLocked;				/* ң������״̬ */
static ctrlValCache_t remoteCache;	/* ң�ػ������� */
static ctrlValCache_t wifiCache;	/* wifi�������� */
static ctrlValCache_t* nowCache = &remoteCache;/*Ĭ��Ϊң��*/
static ctrlVal_t ctrlValLpf = {0.f};/* �������ݵ�ͨ */

static YawModeType yawMode = XMODE;	/* Ĭ��ΪX����ģʽ */
static commanderBits_t commander;

static void commanderLevelRPY(void)
{
	ctrlValLpf.roll = 0;
	ctrlValLpf.pitch = 0;
	ctrlValLpf.yaw = 0;
}

static void commanderDropToGround(void)
{
	commanderLevelRPY();
	ctrlValLpf.thrust = 0;
}

/********************************************************
 *ctrlDataUpdate()	���¿�������
 *ң������ ���ȼ�����wifi��������
*********************************************************/
static void ctrlDataUpdate(void)	
{
	static float lpfVal = 0.2f;
	u32 tickNow = getSysTickCnt();	

	if ((tickNow - remoteCache.timestamp) < COMMANDER_WDT_TIMEOUT_STABILIZE) 
	{
		isRCLocked = false;			/*����*/
		nowCache = &remoteCache;	/* ң�ػ������� */
	}else if ((tickNow - wifiCache.timestamp) < COMMANDER_WDT_TIMEOUT_STABILIZE) 
	{
		isRCLocked = false;			/*����*/
		nowCache = &wifiCache;		/* wifi�������� */
	}else if ((tickNow - remoteCache.timestamp) < COMMANDER_WDT_TIMEOUT_SHUTDOWN) 
	{
		nowCache = &remoteCache;	/* ң�ػ������� */
		commanderLevelRPY();
	}else if ((tickNow - wifiCache.timestamp) < COMMANDER_WDT_TIMEOUT_SHUTDOWN) 
	{
		nowCache = &wifiCache;		/* wifi�������� */
		commanderLevelRPY();
	} else 
	{
		isRCLocked = true;			/*����*/
		nowCache = &remoteCache;
		commanderDropToGround();
	}
	
	if(!isRCLocked)	/*����״̬*/
	{
		ctrlVal_t ctrlVal =  nowCache->tarVal[nowCache->activeSide];	/*��ȡ����*/
		
//		ctrlValLpf.thrust = ctrlVal.thrust;
//		ctrlValLpf.pitch = ctrlVal.pitch;
//		ctrlValLpf.roll = ctrlVal.roll;
//		ctrlValLpf.yaw = ctrlVal.yaw;
		
		ctrlValLpf.thrust += (ctrlVal.thrust - ctrlValLpf.thrust) * lpfVal;
		ctrlValLpf.pitch += (ctrlVal.pitch - ctrlValLpf.pitch) * lpfVal;
		ctrlValLpf.roll += (ctrlVal.roll - ctrlValLpf.roll) * lpfVal;
		ctrlValLpf.yaw += (ctrlVal.yaw - ctrlValLpf.yaw) * lpfVal;
		
		configParam.trimP = ctrlVal.trimPitch;	/*����΢��ֵ*/
		configParam.trimR = ctrlVal.trimRoll;
		
		if (ctrlValLpf.thrust < MIN_THRUST)
			ctrlValLpf.thrust = 0;	
		else 		
			ctrlValLpf.thrust = (ctrlValLpf.thrust>=MAX_THRUST) ? MAX_THRUST:ctrlValLpf.thrust;
	}
}

/************************************************************************
* ����carefree(��ͷģʽ)���ο���������ϵ��������Χ��YAW��ת��
* ����ǰ����Ȼ���ֿ�ʼ�ķ������ģʽ�����ַǳ�ʵ��
************************************************************************/
static void rotateYawCarefree(setpoint_t *setpoint, const state_t *state)
{
	float yawRad = state->attitude.yaw * DEG2RAD;
	float cosy = cosf(yawRad);
	float siny = sinf(yawRad);
	
	if(setpoint->mode.x ==  modeDisable || setpoint->mode.y ==  modeDisable)	/*�ֶ��Ͷ���ģʽ*/
	{
		float originalRoll = setpoint->attitude.roll;
		float originalPitch = setpoint->attitude.pitch;

		setpoint->attitude.roll = originalRoll * cosy + originalPitch * siny;
		setpoint->attitude.pitch = originalPitch * cosy - originalRoll * siny;
	}
	else if(setpoint->mode.x ==  modeVelocity || setpoint->mode.y ==  modeVelocity)	/*����ģʽ*/
	{
		float originalVy = setpoint->velocity.y;
		float originalVx = setpoint->velocity.x;

		setpoint->velocity.y = originalVy * cosy + originalVx * siny;
		setpoint->velocity.x = originalVx * cosy - originalVy * siny;
	}
}

/*�ɿ����ݻ���*/
void flightCtrldataCache(ctrlSrc_e ctrlSrc, ctrlVal_t pk)
{
	switch(ctrlSrc)
	{
		case ATK_REMOTER:
			remoteCache.tarVal[!remoteCache.activeSide] = pk;
			remoteCache.activeSide = !remoteCache.activeSide;
			remoteCache.timestamp = getSysTickCnt();
			break;
		
		case WIFI:
			wifiCache.tarVal[!remoteCache.activeSide] = pk;
			wifiCache.activeSide = !remoteCache.activeSide;
			wifiCache.timestamp = getSysTickCnt();
			break;
	}
}

/********************************************************
* flyerAutoLand()
* �����Զ�����
*********************************************************/
void flyerAutoLand(setpoint_t *setpoint,const state_t *state)
{	
	static float lastAccZ = 0.f; 
	static u8 lowThrustCnt = 0;

	setpoint->mode.z = modeVelocity;
	setpoint->velocity.z = -70.f - state->velocity.z;	/*�����ٶ� ��λcm/s*/

	if(getAltholdThrust() < 20000.f)	/*��������ֵ�ϵ�*/
	{
		lowThrustCnt++;
		if(lowThrustCnt > 10)
		{
			lowThrustCnt = 0;
			commander.keyLand = false;
			commander.keyFlight = false;
			estRstAll();	/*��λ����*/
		}	
	}else
	{
		lowThrustCnt = 0;
	}
		
	if (lastAccZ <250.f && state->acc.z > 500.f)
	{
		commander.keyLand = false;
		commander.keyFlight = false;
		estRstAll();	/*��λ����*/
	}
	lastAccZ = state->acc.z;
}


static bool initHigh = false;
static bool isMuchPullDown = false;
static bool isAdjustingPosZ = false;/*����Zλ��*/
static bool isAdjustingPosXY = true;/*����XYλ��*/
static u8 adjustPosXYTime = 0;		/*XYλ�õ���ʱ��*/
static float errorPosX = 0.f;		/*Xλ�����*/
static float errorPosY = 0.f;		/*Yλ�����*/
static float errorPosZ = 0.f;		/*Zλ�����*/


void commanderGetSetpoint(setpoint_t *setpoint, state_t *state)
{	
	ctrlDataUpdate();	/*���¿�������*/
	
	state->isRCLocked = isRCLocked;	/*����ң��������״̬*/
	
	if(commander.ctrlMode & 0x01)/*����ģʽ*/
	{
		if(commander.keyLand)/*һ������*/
		{
			flyerAutoLand(setpoint,state);
		}
		else if(commander.keyFlight)/*һ�����*/ 
		{	
			setpoint->thrust = 0;
			setpoint->mode.z = modeAbs;		
			
			if (initHigh == false)
			{
				initHigh = true;	
				isAdjustingPosXY = true;
				errorPosX = 0.f;
				errorPosY = 0.f;
				errorPosZ = 0.f;
				
				setFastAdjustPosParam(0, 1, 0, 80.f);	/*һ����ɸ߶�80cm*/															
			}		
				
			float climb = ((ctrlValLpf.thrust - 32767.f) / 32767.f);
			if(climb > 0.f) 
				climb *= MAX_CLIMB_UP;
			else
				climb *= MAX_CLIMB_DOWN;
				
			if (fabsf(climb) > 5.f)
			{
				isAdjustingPosZ = true;												
				setpoint->mode.z = modeVelocity;
				setpoint->velocity.z = climb;

				if(climb < -(CLIMB_RATE/3.f))	/*������������*/
				{
					isMuchPullDown = true;
				}
			}
			else if (isAdjustingPosZ == true)
			{
				isAdjustingPosZ = false;
				isMuchPullDown = false;
			
				setpoint->mode.z = modeAbs;
				setpoint->position.z = state->position.z + errorPosZ;	/*������λ��*/									
			}
			else if(isAdjustingPosZ == false)	/*Zλ�����*/
			{
				errorPosZ = setpoint->position.z - state->position.z;
				errorPosZ = constrainf(errorPosZ, -10.f, 10.f);	/*����޷� ��λcm*/
			}
			
			if(state->acc.z > 500.f && isMuchPullDown == true)	/*�����������󣬷ɻ�����ͣ��*/
			{
				isMuchPullDown = false;
				commander.keyFlight = false;
				estRstAll();	/*��λ����*/
			}
		}
		else/*��½״̬*/
		{
			setpoint->mode.z = modeDisable;
			setpoint->thrust = 0;
			setpoint->velocity.z = 0;
			setpoint->position.z = 0;
			initHigh = false;
			isAdjustingPosZ = false;
		}
	}
	else /*�ֶ���ģʽ*/
	{
		setpoint->mode.z = modeDisable;
		setpoint->thrust = ctrlValLpf.thrust;
	}	
 	
	setpoint->attitude.roll = ctrlValLpf.roll;
	setpoint->attitude.pitch = ctrlValLpf.pitch;
	setpoint->attitude.yaw  = -ctrlValLpf.yaw;	/*ҡ�˷����yaw�����෴*/
	
	if(getOpDataState() && commander.ctrlMode == 0x03)	/*�������ݿ��ã�����ģʽ*/ 
	{
		setpoint->attitude.yaw *= 0.5f;	/*����ģʽ����yaw����*/
		
		/*����λ�� �ٶ�ģʽ*/
		if(fabsf(setpoint->attitude.roll) > 3.f || fabsf(setpoint->attitude.pitch) > 3.f)
		{
			adjustPosXYTime = 0;
			isAdjustingPosXY = true;
			setpoint->mode.x = modeVelocity;
			setpoint->mode.y = modeVelocity;
			setpoint->velocity.x = setpoint->attitude.pitch / 15.f;
			setpoint->velocity.y = setpoint->attitude.roll / 15.f;	
		}
		else if(isAdjustingPosXY == true)
		{
			if(adjustPosXYTime++ > 100)
			{
				adjustPosXYTime = 0;
				isAdjustingPosXY = false;
			}		
			setpoint->mode.x = modeAbs;
			setpoint->mode.y = modeAbs;
			setpoint->position.x = state->position.x + errorPosX;	//������λ��
			setpoint->position.y = state->position.y + errorPosY;	//������λ��
		}
		else if(isAdjustingPosXY == false)	/*λ�����*/
		{	
			errorPosX = setpoint->position.x - state->position.x;
			errorPosY = setpoint->position.y - state->position.y;
//			errorPosX = constrainf(errorPosX, -0.3f, 0.3f);	/*����޷� ��λm*/
//			errorPosY = constrainf(errorPosY, -0.3f, 0.3f);	/*����޷� ��λm*/
		}
	}
	else	/*�ֶ�ģʽ*/
	{
		setpoint->mode.x = modeDisable;
		setpoint->mode.y = modeDisable;		
	}
	
	setpoint->mode.roll = modeDisable;	
	setpoint->mode.pitch = modeDisable;	
	
	if(commander.flightMode)/*��ͷģʽ*/
	{
		yawMode = CAREFREE;		
		rotateYawCarefree(setpoint, state);
	}		
	else	/*X����ģʽ*/
	{
		yawMode = XMODE;
	}		
}

/* ��ȡ������΢��ֵ */
void getAndUpdateTrim(float* pitch, float* roll)
{
	*pitch = nowCache->tarVal[nowCache->activeSide].trimPitch;
	*roll = nowCache->tarVal[nowCache->activeSide].trimRoll;
}

void setCommanderCtrlMode(u8 set)
{
	commander.ctrlMode = (set & 0x03);
}
u8 getCommanderCtrlMode(void)
{
	return (commander.ctrlMode & 0x03);
}

u8 getCommanderFlightMode(void)
{
	return (yawMode & 0x01);
}

void setCommanderKeyFlight(bool set)
{
	commander.keyFlight = set;
}
bool getCommanderKeyFlight(void)
{
	return commander.keyFlight;
}

void setCommanderKeyland(bool set)
{
	commander.keyLand = set;
}
bool getCommanderKeyland(void)
{
	return commander.keyLand;
}

void setCommanderFlightmode(bool set)
{
	commander.flightMode = set;
}

void setCommanderEmerStop(bool set)
{
	commander.emerStop = set;
}

