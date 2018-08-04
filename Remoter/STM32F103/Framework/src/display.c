/*
 * File      : display.c
 * This file is part of RT-Thread minifly
 * COPYRIGHT (C) 2018, RT-Thread minifly Development Team
 *
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-08-02     xinping yang      the first version
 */
#include "display.h"
#include <rtthread.h>
#include "main_ui.h"
#include "trim_ui.h"
#include "debug_ui.h"
#include "menu_ui.h"
#include "match_ui.h"
#include "joystickCalib_ui.h"
#include "menuL1_item.h"
#include "menuL2_item.h"
#include "menuL3_item.h"
#include "reset_ui.h"

//void displayTask(void* parameter)
//{
//	rt_kprintf("display task entry\n");
//}

static enum ui_e show_ui = MAIN_UI;


void setShow_ui(enum ui_e ui)
{
	show_ui = ui;
	GUI_ClearSCR();
}


void displayTask(void* param)
{
	while(1)
	{
//		vTaskDelay(50);
		rt_thread_delay(50);
		switch(show_ui)
		{
			case MAIN_UI:
				main_ui();
				break;
			case TRIM_UI:
				trim_ui();
				break;
			case MENU_UI:
				Menu_Run();
				break;
			case DEBUG_UI:
				debug_ui();
				break;
			case JOYSTICK_CALIB_UI:
				joystickCalib_ui();
				break;
			case MATCH_UI:
				match_ui();
				break;
			case RESET_UI:
				reset_ui();
				break;
			default:break;
		}
		GUI_Refresh();
	}
}

void displayInit(void)
{

	mainMenuInit();
	flymodeMenuInit();
	controlmodeMenuInit();
	languageMenuInit();
	flySpeedMenuInit();
	flipEnableMenuInit();
	expModuleMenuInit();
	ledringMenuInit();
	cameraMenuInit();
}
