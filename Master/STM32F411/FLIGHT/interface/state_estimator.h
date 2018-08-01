#ifndef __STATE_ESTIMATOR_H
#define __STATE_ESTIMATOR_H
#include "stabilizer_types.h"

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

typedef struct
{
	float vAccDeadband; /* ���ٶ����� */
	float accX;			/* x ���������ٶ� ��λ(cm/s/s)*/
	float accY;			/* y ���������ٶ� ��λ(cm/s/s)*/
	float accZ;			/* z ���������ٶ� ��λ(cm/s/s)*/
	float velocityX;	/* x ��������ٶ� ��λ(cm/s)*/
	float velocityY;	/* y ��������ٶ� ��λ(cm/s)*/
	float velocityZ;	/* z ��������ٶ� ��λ(cm/s)*/
	float positonX; 	/* x �������λ�� ��λ(cm)*/
	float positonY; 	/* x �������λ�� ��λ(cm)*/
	float positonZ; 	/* x �������λ�� ��λ(cm)*/
} estimator_t;

void positionEstimate(sensorData_t* sensorData, state_t* state, float dt);	
float getFusedHeight(void);	/*��ȡ�ںϸ߶�*/
void estRstHeight(void);	/*��λ����߶�*/
void estRstAll(void);		/*��λ���й���*/

#endif /* __STATE_ESTIMATOR_H */


