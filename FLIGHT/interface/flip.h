#ifndef __FLIP_H
#define __FLIP_H 
#include "stabilizer_types.h"

/********************************************************************************	 
 * ������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
 * ALIENTEK MiniFly
 * ����շ����ƴ���	
 * ����ԭ��@ALIENTEK
 * ������̳:www.openedv.com
 * ��������:2018/6/22
 * �汾��V1.2
 * ��Ȩ���У�����ؾ���
 * Copyright(C) �������������ӿƼ����޹�˾ 2014-2024
 * All rights reserved
********************************************************************************/


void flyerFlipCheck(setpoint_t* setpoint,control_t* control,state_t* state);	/* Flyer �������*/

void setFlipDir(u8 dir);

#endif 
