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
 * 2014-04-27     Bernard      make code cleanup. 
 */

#include <board.h>
#include <rtthread.h>
#include "atkp.h"
#include "config_param.h"
#include "module_mgt.h"
#include "pm.h"
#include "radiolink.h"
#include "usblink.h"
#include "sensors.h"
#include "stabilizer.h"


#ifdef RT_USING_LWIP
#include <lwip/sys.h>
#include <lwip/api.h>
#include <netif/ethernetif.h>
#include "stm32f4xx_eth.h"
#endif

#ifdef RT_USING_FINSH
#include <shell.h>
#include <finsh.h>
#endif

#ifdef RT_USING_GDB
#include <gdb_stub.h>
#endif

#define RADIOLINK_TASK_PRIORITY			                5
#define USBLINK_TX_TASK_PRIORITY			              4		
#define USBLINK_RX_TASK_PRIORITY			              3	
#define ATKP_TX_TASK_PRIORITY			                  3	
#define ATKP_RX_ANL_TASK_PRIORITY			              6	
#define CONFIG_PARAM_TASK_PRIORITY			            1
#define PM_TASK_PRIORITY			                      2	
#define SENSORS_TASK_PRIORITY			                  4
#define STABILIZER_TASK_PRIORITY			              5	
#define EXP_MODULE_MGT_TASK_PRIORITY			          1


static char thread_radiolink_stack[512];
struct rt_thread thread_radiolink_handle;
static char thread_usblink_tx_stack[512];
struct rt_thread thread_usblink_tx_handle;
static char thread_usblink_rx_stack[512];
struct rt_thread thread_usblink_rx_handle;
static char thread_atkp_tx_stack[512];
struct rt_thread thread_atkp_tx_handle;
static char thread_atkp_rx_anl_stack[512];
struct rt_thread thread_atkp_rx_anl_handle;
static char thread_config_param_stack[512];
struct rt_thread thread_config_param_handle;
static char thread_pm_task_stack[512];
struct rt_thread thread_pm_task_handle;
static char thread_sensors_task_stack[512];
struct rt_thread thread_sensors_task_handle;
static char thread_stabilizer_task_stack[512];
struct rt_thread thread_stabilizer_task_handle;
static char thread_exp_module_mgt_task_stack[512];
struct rt_thread thread_exp_module_mgt_task_handle;


void rt_init_thread_entry(void* parameter)
{
    /* initialization RT-Thread Components */
    rt_components_init();
	
    /* GDB STUB */
#ifdef RT_USING_GDB
    gdb_set_device("uart6");
    gdb_start();
#endif
}

int rt_application_init()
{
    rt_thread_t tid;
    rt_err_t res;
    tid = rt_thread_create("init",
        rt_init_thread_entry, RT_NULL,
        2048, RT_THREAD_PRIORITY_MAX/3, 20);

    if (tid != RT_NULL)
        rt_thread_startup(tid);
    res = rt_thread_init(&thread_radiolink_handle,
						   "RADIOLINK",
						   radiolinkTask,
						   RT_NULL,
						   &thread_radiolink_stack[0],
						   sizeof(thread_radiolink_stack),RADIOLINK_TASK_PRIORITY,1);
    if (res == RT_EOK)
        rt_thread_startup(&thread_radiolink_handle);
		
		res = rt_thread_init(&thread_usblink_rx_handle,
						   "USBLINK RX",
						   usblinkRxTask,
						   RT_NULL,
						   &thread_usblink_rx_stack[0],
						   sizeof(thread_usblink_rx_stack),USBLINK_RX_TASK_PRIORITY,1);
    if (res == RT_EOK)
        rt_thread_startup(&thread_usblink_rx_handle);
		
	 		res = rt_thread_init(&thread_usblink_tx_handle,
						   "USBLINK TX",
						   usblinkTxTask,
						   RT_NULL,
						   &thread_usblink_tx_stack[0],
						   sizeof(thread_usblink_tx_stack),USBLINK_TX_TASK_PRIORITY,1);
    if (res == RT_EOK)
        rt_thread_startup(&thread_usblink_tx_handle);

	  res = rt_thread_init(&thread_atkp_tx_handle,
						   "ATKP TX",
						   atkpTxTask,
						   RT_NULL,
						   &thread_atkp_tx_stack[0],
						   sizeof(thread_atkp_tx_stack),ATKP_TX_TASK_PRIORITY,1);
    if (res == RT_EOK)
        rt_thread_startup(&thread_atkp_tx_handle);
		
		
	  res = rt_thread_init(&thread_atkp_rx_anl_handle,
						   "ATKP RX ANL",
						   atkpRxAnlTask,
						   RT_NULL,
						   &thread_atkp_rx_anl_stack[0],
						   sizeof(thread_atkp_rx_anl_stack),ATKP_RX_ANL_TASK_PRIORITY,1);
    if (res == RT_EOK)
        rt_thread_startup(&thread_atkp_rx_anl_handle);
		
	  res = rt_thread_init(&thread_config_param_handle,
						   "CONFIG PARAM",
						   configParamTask,
						   RT_NULL,
						   &thread_config_param_stack[0],
						   sizeof(thread_config_param_stack),CONFIG_PARAM_TASK_PRIORITY,1);
    if (res == RT_EOK)
        rt_thread_startup(&thread_config_param_handle);
		
	  res = rt_thread_init(&thread_pm_task_handle,
						   "PM Task",
						   pmTask,
						   RT_NULL,
						   &thread_pm_task_stack[0],
						   sizeof(thread_pm_task_stack),PM_TASK_PRIORITY,1);
    if (res == RT_EOK)
        rt_thread_startup(&thread_pm_task_handle);		
		
		res = rt_thread_init(&thread_sensors_task_handle,
						   "SENSORS Task",
						   sensorsTask,
						   RT_NULL,
						   &thread_sensors_task_stack[0],
						   sizeof(thread_sensors_task_stack),SENSORS_TASK_PRIORITY,1);
    if (res == RT_EOK)
        rt_thread_startup(&thread_sensors_task_handle);
		
		res = rt_thread_init(&thread_stabilizer_task_handle,
						   "SATBILIZER Task",
						   stabilizerTask,
						   RT_NULL,
						   &thread_stabilizer_task_stack[0],
						   sizeof(thread_stabilizer_task_stack),STABILIZER_TASK_PRIORITY,1);
    if (res == RT_EOK)
        rt_thread_startup(&thread_stabilizer_task_handle);
		
		res = rt_thread_init(&thread_exp_module_mgt_task_handle,
						   "PM Task",
						   expModuleMgtTask,
						   RT_NULL,
						   &thread_exp_module_mgt_task_stack[0],
						   sizeof(thread_exp_module_mgt_task_stack), EXP_MODULE_MGT_TASK_PRIORITY,1);
    if (res == RT_EOK)
        rt_thread_startup(&thread_exp_module_mgt_task_handle);
		
    return 0;
}
