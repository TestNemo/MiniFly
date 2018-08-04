/*
 * File      : display.h
 * This file is part of RT-Thread minifly
 * COPYRIGHT (C) 2018, RT-Thread minifly Development Team
 *
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-08-02     xinping yang      the first version
 */
 
#ifndef __DISPLAY_H
#define __DISPLAY_H

enum ui_e
{
	MAIN_UI,
	TRIM_UI,
	MENU_UI,
	DEBUG_UI,
	JOYSTICK_CALIB_UI,
	MATCH_UI,
	RESET_UI,
};

void setShow_ui(enum ui_e ui);
void displayTask(void* param);
void displayInit(void);
#endif

