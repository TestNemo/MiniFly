#ifndef __OPTICAL_FLOW_H
#define __OPTICAL_FLOW_H
#include "sys.h"
#include <stdbool.h>
#include "spi.h"
#include "stabilizer_types.h"
#include "module_mgt.h"

/********************************************************************************	 
 * ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
 * ALIENTEK MiniFly
 * ����ģ����������	
 * ����ԭ��@ALIENTEK
 * ������̳:www.openedv.com
 * ��������:2018/5/2
 * �汾��V1.2
 * ��Ȩ���У�����ؾ���
 * Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
 * All rights reserved
********************************************************************************/

void opticalFlowPowerControl(bool state);	//������Դ����
bool getFlowData(state_t *state);			//��ȡ��������
void opticalFlowInit(void);		/*��ʼ������ģ��*/
bool getOpDataState(void);		/*��������״̬*/

#endif
