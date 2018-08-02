/*
 * File      : comfig_param.c
 * This file is part of RT-Thread minifly
 * COPYRIGHT (C) 2018, RT-Thread minifly Development Team
 *
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-08-02     xinping yang      the first version
 */

#include "config_param.h"
#include <rtthread.h>
#include "stmflash.h"

configParam_t configParam;

static rt_bool_t isInit;
static rt_bool_t isConfigParamOK;

static configParam_t configParamDefault=
{
	.version = VERSION,
	.language = DISPLAY_LANGUAGE,
	
	.radio.channel = RADIO_CHANNEL,
	.radio.dataRate = RADIO_DATARATE,
	.radio.addressHigh = ((uint64_t)RADIO_ADDRESS >> 32),
	.radio.addressLow = (RADIO_ADDRESS & 0xFFFFFFFFULL),
	
	.flight.ctrl = FLIGHT_CTRL_MODE,
	.flight.mode = FLIGHT_MODE,
	.flight.speed = FLIGHT_SPEED,
	.flight.flip = FLIP_SET,
	
	.jsParam.pitch.mid = 2000,
	.jsParam.pitch.range_neg = 2000,
	.jsParam.pitch.range_pos = 2000,
	
	.jsParam.roll.mid = 2000,
	.jsParam.roll.range_neg = 2000,
	.jsParam.roll.range_pos =2000,
	
	.jsParam.yaw.mid = 2000,
	.jsParam.yaw.range_neg = 2000,
	.jsParam.yaw.range_pos =2000,
	
	.jsParam.thrust.mid = 2000,
	.jsParam.thrust.range_neg = 2000,
	.jsParam.thrust.range_pos =2000,
	
	.trim.pitch = 0.0,
    .trim.roll = 0.0,
};



static u8 calculate_cksum(configParam_t* data)
{ 
	u8 cksum=0;	
	u8* c = (u8*)data;
	
	for (int i=0; i<sizeof(configParam_t); i++)
		cksum += *(c++);
	cksum -= data->cksum;
	
	return cksum;
}

void writeConfigParamToFlash(void)
{
	u8 cksum = calculate_cksum(&configParam);
	configParam.cksum = cksum;
	STMFLASH_Write(CONFIG_PARAM_ADDR,(u16*)&configParam,sizeof(configParam)/2);
}

void configParamInit(void)
{
	if(isInit) return;
	

	STMFLASH_Read(CONFIG_PARAM_ADDR,(u16*)&configParam,sizeof(configParam)/2);
	
	if(configParam.version == VERSION)
	{
		u8 cksum = calculate_cksum(&configParam);
		if(cksum == configParam.cksum)
			isConfigParamOK = RT_TRUE;
		else
			isConfigParamOK = RT_FALSE;
	}
	else
	{
		isConfigParamOK = RT_FALSE;
	}
	
	if(isConfigParamOK == RT_FALSE)	
	{
		configParam = configParamDefault;
		writeConfigParamToFlash();
		isConfigParamOK=RT_TRUE;
	}
}
void configParamTask(void* parameter)
{
	//configParamInit();
	rt_kprintf("config param task entry\n");
}
