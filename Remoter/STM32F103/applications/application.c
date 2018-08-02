/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2013-07-12     aozima       update for auto initial.
 */

/**
 * @addtogroup STM32
 */
/*@{*/

#include <board.h>
#include <rtthread.h>
//#include "config_param.h"
#include "global.h"
#include "radiolink.h"
#include "atkp.h"
#include "config_param.h"
#include "display.h"
#include "keytask.h"
#include "remoter_ctrl.h"
#include "usblink.h"
#ifdef RT_USING_DFS
/* dfs filesystem:ELM filesystem init */
#include <dfs_elm.h>
/* dfs Filesystem APIs */
#include <dfs_fs.h>
#endif

#ifdef RT_USING_RTGUI
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_server.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/driver.h>
#include <rtgui/calibration.h>
#endif

#include "led.h"
#include "oled.h"
#include "beep.h"
#include "joystick.h"

ALIGN(RT_ALIGN_SIZE)
static char thread_radiolink_stack[512];
struct rt_thread thread_radiolink_handle;
static char thread_usblinkTx_stack[512];
struct rt_thread thread_usblinkTx_handle;
static char thread_usblinkRx_stack[512];
struct rt_thread thread_usblinkRx_handle;
static char thread_commander_stack[512];
struct rt_thread thread_commander_handle;
static char thread_key_stack[512];
struct rt_thread thread_key_handle;
static char thread_display_stack[512];
struct rt_thread thread_display_handle;
static char thread_configParam_stack[512];
struct rt_thread thread_configParam_handle;
static char thread_radiolinkDataProcess_stack[512];
struct rt_thread thread_radiolinkDataProcess_handle;
static char thread_usblinkDataProcess_stack[512];
struct rt_thread thread_usblinkDataProcess_handle;
#ifdef RT_USING_RTGUI
rt_bool_t cali_setup(void)
{
    rt_kprintf("cali setup entered\n");
    return RT_FALSE;
}

void cali_store(struct calibration_data *data)
{
    rt_kprintf("cali finished (%d, %d), (%d, %d)\n",
               data->min_x,
               data->max_x,
               data->min_y,
               data->max_y);
}
#endif /* RT_USING_RTGUI */

void rt_init_thread_entry(void* parameter)
{
	
#ifdef RT_USING_COMPONENTS_INIT
    /* initialization RT-Thread Components */
    rt_components_init();
#endif

    /* Filesystem Initialization */
#if defined(RT_USING_DFS) && defined(RT_USING_DFS_ELMFAT)
    /* mount sd card fat partition 1 as root directory */
    if (dfs_mount("sd0", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("File System initialized!\n");
    }
    else
        rt_kprintf("File System initialzation failed!\n");
#endif  /* RT_USING_DFS */

#ifdef RT_USING_RTGUI
    {
        extern void rt_hw_lcd_init();
        extern void rtgui_touch_hw_init(void);

        rt_device_t lcd;

        /* init lcd */
        rt_hw_lcd_init();

        /* init touch panel */
        rtgui_touch_hw_init();

        /* find lcd device */
        lcd = rt_device_find("lcd");

        /* set lcd device as rtgui graphic driver */
        rtgui_graphic_set_device(lcd);

#ifndef RT_USING_COMPONENTS_INIT
        /* init rtgui system server */
        rtgui_system_server_init();
#endif

        calibration_set_restore(cali_setup);
        calibration_set_after(cali_store);
        calibration_init();
    }
#endif /* #ifdef RT_USING_RTGUI */
}










int rt_application_init(void)
{
	  rt_err_t res;
    rt_thread_t init_thread;
//  ---------below comments is the hardware init ---------------------	
//	  NVIC_SetVectorTable(FIRMWARE_START_ADDR,0);
//	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);/*中断配置初始化*/
//	delay_init(); 		/*delay初始化*/  no need, with rt_thread_delay to replace
  	configParamInit();	/*配置参数初始化*/ 
//	ledInit();			/*led初始化*/ ok, with rt_hw_led_init
	  rt_hw_led_init();
  	oledInit(); 		/*oled初始化*/
   	beepInit();			/*蜂鸣器初始化*/
//	keyInit();			/*按键初始化*/ // done in keytask entry
  	joystickInit();		/*摇杆初始化*/
//	usb_vcp_init();		/*usb虚拟串口初始化*/
//	
//	radiolinkInit();	/*无线通信初始化*/
//	usblinkInit();		/*usb通信初始化*/
//	displayInit();		/*显示初始化*/

#if (RT_THREAD_PRIORITY_MAX == 32)
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 8, 20);
#else
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 80, 20);
#endif

    res = rt_thread_init(&thread_radiolink_handle,
						   "RADIOLINK",
						   radiolinkTask,
						   RT_NULL,
						   &thread_radiolink_stack[0],
						   sizeof(thread_radiolink_stack),RADIOLINK_TASK_PRIORITY,1);
    if (res == RT_EOK)
        rt_thread_startup(&thread_radiolink_handle);
		
		res = rt_thread_init(&thread_usblinkTx_handle,
						   "USBLINK TX",
						   usblinkTxTask,
						   RT_NULL,
						   &thread_usblinkTx_stack[0],
						   sizeof(thread_usblinkTx_stack),USBLINK_TX_TASK_PRIORITY,1);
    if (res == RT_EOK)
        rt_thread_startup(&thread_usblinkTx_handle);
		
		res = rt_thread_init(&thread_usblinkRx_handle,
						   "USBLINK RX",
						   usblinkRxTask,
						   RT_NULL,
						   &thread_usblinkRx_stack[0],
						   sizeof(thread_usblinkRx_stack),USBLINK_RX_TASK_PRIORITY,1);
    if (res == RT_EOK)
        rt_thread_startup(&thread_usblinkRx_handle);
		
		res = rt_thread_init(&thread_commander_handle,
						   "COMMANDER",
						   commanderTask,
						   RT_NULL,
						   &thread_commander_stack[0],
						   sizeof(thread_commander_stack),COMMANDER_TASK_PRIORITY,1);
    if (res == RT_EOK)
        rt_thread_startup(&thread_commander_handle);
		
	  res = rt_thread_init(&thread_key_handle,
						   "COMMANDER",
						   keyTask,
						   RT_NULL,
						   &thread_key_stack[0],
						   sizeof(thread_key_stack),KEY_TASK_PRIORITY,1);
    if (res == RT_EOK)
        rt_thread_startup(&thread_key_handle);
		
		res = rt_thread_init(&thread_display_handle,
						   "DISPLAY",
						   displayTask,
						   RT_NULL,
						   &thread_display_stack[0],
						   sizeof(thread_display_stack),DISPLAY_TASK_PRIORITY,1);
    if (res == RT_EOK)
        rt_thread_startup(&thread_display_handle);

		res = rt_thread_init(&thread_configParam_handle,
						   "DISPLAY",
						   configParamTask,
						   RT_NULL,
						   &thread_configParam_stack[0],
						   sizeof(thread_configParam_stack),CONFIG_PARAM_TASK_PRIORITY,1);
    if (res == RT_EOK)
        rt_thread_startup(&thread_configParam_handle);
		
		res = rt_thread_init(&thread_radiolinkDataProcess_handle,
						   "DISPLAY",
						   radiolinkDataProcessTask,
						   RT_NULL,
						   &thread_radiolinkDataProcess_stack[0],
						   sizeof(thread_radiolinkDataProcess_stack),RADIOLINK_DATA_PROCESS_TASK_PRIORITY,1);
    if (res == RT_EOK)
        rt_thread_startup(&thread_radiolinkDataProcess_handle);
		
		res = rt_thread_init(&thread_usblinkDataProcess_handle,
						   "DISPLAY",
						   usblinkDataProcessTask,
						   RT_NULL,
						   &thread_usblinkDataProcess_stack[0],
						   sizeof(thread_usblinkDataProcess_stack),USBLINK_DATA_PROCESS_TASK_PRIORITY,1);
    if (res == RT_EOK)
        rt_thread_startup(&thread_usblinkDataProcess_handle);
    return 0;
}

/*@}*/
