/*
 * File      : comfig_param.h
 * This file is part of RT-Thread minifly
 * COPYRIGHT (C) 2018, RT-Thread minifly Development Team
 *
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-08-02     xinping yang      the first version
 */
 
#ifndef __CONFIG_PARAM_H
#define __CONFIG_PARAM_H

#include "system.h"
#include "24l01.h"

#define BOOTLOADER_SIZE		(9*1024)		/*9K bootloader*/
#define CONFIG_PARAM_SIZE	(127*1024)		

#define CONFIG_PARAM_ADDR 	(FLASH_BASE + CONFIG_PARAM_SIZE)
#define FIRMWARE_START_ADDR (FLASH_BASE + BOOTLOADER_SIZE)


#define  VERSION	12		
#define  DISPLAY_LANGUAGE	SIMPLE_CHINESE

#define  RADIO_CHANNEL 		2
#define  RADIO_DATARATE 	DATA_RATE_250K
#define  RADIO_ADDRESS 		0x123456789AULL

#define  FLIGHT_CTRL_MODE	ALTHOLD_MODE
#define  FLIGHT_MODE		HEAD_LESS
#define  FLIGHT_SPEED		LOW_SPEED
#define  FLIP_SET			FLIP_DISABLE


enum ctrlMode
{
	ALTHOLD_MODE,
	MANUAL_MODE,
	THREEHOLD_MODE,
};

enum flightMode
{
	HEAD_LESS,
	X_MODE,
};

enum flightSpeed
{
	LOW_SPEED,
	MID_SPEED,
	HIGH_SPEED,
};

enum flipEnable
{
	FLIP_ENABLE,
	FLIP_DISABLE,
};

enum language
{
	SIMPLE_CHINESE,
	ENGLISH,
	COMPLEX_CHINESE,
};


typedef struct{
	enum ctrlMode ctrl;
	enum flightMode mode;
	enum flightSpeed speed;
	enum flipEnable flip;
}flightConfig_t;


typedef struct{
	u8 channel;		
	enum nrfRate dataRate;
	u32 addressHigh;
	u32 addressLow;	
}radioConfig_t;


typedef __packed struct{
	float pitch;
	float roll;
}trim_t;


typedef struct{
	u8 version;			
	enum language language;	
	radioConfig_t radio;	
	flightConfig_t flight;	
	//joystickParam_t jsParam;  // ToDo
	trim_t trim;			
	u8 cksum;				
} configParam_t;


extern configParam_t configParam;


void configParamInit(void);
void configParamTask(void* param);
void writeConfigParamToFlash(void);
void configParamReset(void);
#endif

