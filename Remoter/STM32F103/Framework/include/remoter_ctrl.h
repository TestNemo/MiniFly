/*
 * File      : remoter_ctrl.h
 * This file is part of RT-Thread minifly
 * COPYRIGHT (C) 2018, RT-Thread minifly Development Team
 *
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-08-02     xinping yang      the first version
 */

#ifndef __COMMANDER_H
#define __COMMANDER_H
#include "system.h"
#include "joystick.h"
#include <stdbool.h>

typedef enum 
{
	REMOTOR_CMD,
	REMOTOR_DATA,
}remoterType_e;


#define  CMD_GET_MSG		    0x01	
#define  CMD_GET_CANFLY		  0x02	
#define  CMD_FLIGHT_LAND	  0x03	
#define  CMD_EMER_STOP		  0x04	
#define  CMD_FLIP			      0x05	
#define  CMD_POWER_MODULE	  0x06	
#define  CMD_LEDRING_EFFECT	0x07	

#define  ACK_MSG			      0x01	


enum expModuleID
{
	NO_MODULE,
	LED_RING,
	WIFI_CAMERA,
	OPTICAL_FLOW,
	MODULE1,
};


typedef __packed struct
{
	float roll;      
	float pitch;  
	float yaw;      
	float thrust;
	float trimPitch;
	float trimRoll;
	u8 ctrlMode;
	bool flightMode;
	bool RCLock;
} remoterData_t;

typedef __packed struct
{
	u8 version;
	bool mpu_selfTest;
	bool baro_slfTest;
	bool isCanFly;
	bool isLowpower;
	enum expModuleID moduleID;
} MiniFlyMsg_t;

void commanderTask(void* param);
joystickFlyf_t getFlyControlData(void);
float limit(float value,float min, float max);
void sendRmotorCmd(u8 cmd, u8 data);
void sendRmotorData(u8 *data, u8 len);
#endif

