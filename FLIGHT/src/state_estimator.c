#include "state_estimator.h"
#include "attitude_pid.h"
#include "position_pid.h"
#include "maths.h"
#include "vl53l0x.h"
#include "stabilizer.h"

/********************************************************************************	 
 * ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
 * ALIENTEK MiniFly
 * ��̬�������	
 * ����ԭ��@ALIENTEK
 * ������̳:www.openedv.com
 * ��������:2018/6/22
 * �汾��V1.2
 * ��Ȩ���У�����ؾ���
 * Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
 * All rights reserved
********************************************************************************/

#define VELOCITY_LIMIT		(130.f)	/*�ٶ��޷� ��λcm/s*/

static float errorZ = 0.f;			/*λ�����*/
static float wBaro = 0.35f;			/*��ѹУ��Ȩ��*/

static bool isRstHeight = false;	/*��λ�߶�*/
static bool isRstAll = true;		/*��λ����*/

static float fusedHeight;			/*�ںϸ߶ȣ���ɵ�Ϊ0*/
static float fusedHeightLpf = 0.f;	/*�ںϸ߶ȣ���ͨ*/
static float startBaroAsl = 0.f;	/*��ɵ㺣��*/


static estimator_t estimator = 
{
	.vAccDeadband = 4.0f,
	.accX = 0.f,
	.accY = 0.f,
	.accZ = 0.f,
	.velocityX = 0.0f,
	.velocityY = 0.0f,
	.velocityZ = 0.0f,
	.positonX = 0.0f,
	.positonY = 0.0f,
	.positonZ = 0.0f,
};


void positionEstimate(sensorData_t* sensorData, state_t* state, float dt) 
{	
	static float rangeLpf = 0.f;
	static float accZLpf = 0.f;			/*Z����ٶȵ�ͨ*/
	
	float weight = wBaro;
	
	float relateHight = sensorData->baro.asl - startBaroAsl;	/*��ѹ��Ը߶�*/
	
	if(getModuleID() == OPTICAL_FLOW)	/*����ģ�����*/
	{
		vl53l0xReadRange(&sensorData->zrange);	/*��ȡ��������*/
	
		rangeLpf += (sensorData->zrange.distance - rangeLpf) * 0.1f;	/*��ͨ ��λcm*/		
			
		float quality = sensorData->zrange.quality;
	
		if(quality < 0.3f)	/*����������жȣ��������ݲ�����*/
		{
			quality = 0.f;
		}else
		{
			weight = quality;
//			if(quality > 0.8f)
				startBaroAsl = sensorData->baro.asl - rangeLpf;
		}
		fusedHeight = rangeLpf * quality + (1.0f - quality) * relateHight;/*�ںϸ߶�*/					
	}
	else	/*�޹���ģ��*/
	{
		fusedHeight = relateHight;	/*�ںϸ߶�*/
	}
	
	fusedHeightLpf += (fusedHeight - fusedHeightLpf) * 0.1f;	/*�ںϸ߶� ��ͨ*/
	
	if(isRstHeight)
	{	
		isRstHeight = false;
		
		weight = 0.95f;		/*����Ȩ�أ����ٵ���*/	
		
		startBaroAsl = sensorData->baro.asl;
		
		if(getModuleID() == OPTICAL_FLOW)
		{
			if(sensorData->zrange.distance < VL53L0X_MAX_RANGE)
			{
				startBaroAsl -= sensorData->zrange.distance;
				fusedHeight = sensorData->zrange.distance;
			}
		}
		
		estimator.positonZ = fusedHeight;
	}
	else if(isRstAll)
	{
		isRstAll = false;
		
		accZLpf = 0.f;
//		rangeLpf = 0.f;
	
		fusedHeight  = 0.f;
		fusedHeightLpf = 0.f;
		startBaroAsl = sensorData->baro.asl;
		
		if(getModuleID() == OPTICAL_FLOW)
		{
			if(sensorData->zrange.distance < VL53L0X_MAX_RANGE)
			{
				startBaroAsl -= sensorData->zrange.distance;
				fusedHeight = sensorData->zrange.distance;
			}
		}
		
		estimator.velocityZ = 0.f;
		estimator.positonZ = fusedHeight;
	}
	
	bool isKeyFlightLand = ((getCommanderKeyFlight()==true) || (getCommanderKeyland()==true));	/*���߷ɻ��߽���״̬*/
	
	float accZRemovalDead = applyDeadbandf(state->acc.z, estimator.vAccDeadband);/*ȥ��������Z����ٶ�*/
	accZLpf += (accZRemovalDead - accZLpf) * 0.1f;		/*��ͨ*/
	
	if(isKeyFlightLand == true)		/*���߷ɻ��߽���״̬*/
		state->acc.z = constrainf(accZLpf, -1000.f, 1000.f);	/*���ٶ��޷�*/
	else
		state->acc.z = accZRemovalDead;
	
	estimator.accZ = accZRemovalDead;
	estimator.accZ -= 0.02f * errorZ * weight * weight * dt;	/*�������ٶ�*/		
	
	
	/*λ�ú��ٶȹ���*/
	estimator.positonZ += estimator.velocityZ * dt + estimator.accZ * dt * dt / 2.0f;
	estimator.velocityZ += estimator.accZ * dt;
	
	/*�߶����*/
	errorZ = fusedHeight - estimator.positonZ;		
	
	/*У��λ�ƺ��ٶ�*/
	float ewdt = errorZ * weight * dt;
	estimator.positonZ += ewdt;
	estimator.velocityZ += weight * ewdt;
	
	
	if(isKeyFlightLand == true)		/*���߷ɻ��߽���״̬*/
		estimator.velocityZ = constrainf(estimator.velocityZ, -VELOCITY_LIMIT, VELOCITY_LIMIT);	/*�ٶ��޷� VELOCITY_LIMIT*/
	
	state->position.z = estimator.positonZ;	
	state->velocity.z = estimator.velocityZ;
}

/*��ȡ�ںϸ߶� ��λm*/	
float getFusedHeight(void)
{
	return (0.01f * fusedHeightLpf);
}

/*��λ����߶�*/
void estRstHeight(void)
{
	isRstHeight = true;
}

/*��λ���й���*/
void estRstAll(void)
{
	isRstAll = true;
}


