#ifndef __SENSFUSION6_H
#define __SENSFUSION6_H
#include "stabilizer_types.h"

/********************************************************************************	 
 * ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
 * ALIENTEK MiniFly
 * 6�������ںϴ���	
 * ����ԭ��@ALIENTEK
 * ������̳:www.openedv.com
 * ��������:2018/6/22
 * �汾��V1.2
 * ��Ȩ���У�����ؾ���
 * Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
 * All rights reserved
********************************************************************************/

void imuUpdate(Axis3f acc, Axis3f gyro, state_t *state , float dt);	/*�����ں� �����˲�*/
bool getIsCalibrated(void);

#endif

