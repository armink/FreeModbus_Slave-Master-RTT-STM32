#include "app_task.h"

#ifdef __CC_ARM
extern int Image$$RW_IRAM1$$ZI$$Limit;
#elif __ICCARM__
#pragma section="HEAP"
#else
extern int __bss_end;
#endif

uint8_t CpuUsageMajor, CpuUsageMinor; //CPU使用率

//====================操作系统各线程优先级==================================
#define thread_SysMonitor_Prio		    	11
#define thread_ModbusSlavePoll_Prio    	10
ALIGN(RT_ALIGN_SIZE)
//====================操作系统各线程堆栈====================================
static rt_uint8_t thread_SysMonitor_stack[256];
static rt_uint8_t thread_ModbusSlavePoll_stack[512];

struct rt_thread thread_SysMonitor;
struct rt_thread thread_ModbusSlavePoll;

//***************************系统监控线程***************************
//函数定义: void thread_entry_SysRunLed(void* parameter)
//入口参数：无
//出口参数：无
//备    注：Editor：Liuqiuhu   2013-1-30    Company: BXXJS
//******************************************************************
void thread_entry_SysMonitor(void* parameter)
{
	while (1)
	{
		cpu_usage_get(&CpuUsageMajor, &CpuUsageMinor);
		usRegHoldingBuf[HD_CPU_USAGE_MAJOR] = CpuUsageMajor;
		usRegHoldingBuf[HD_CPU_USAGE_MINOR] = CpuUsageMinor;
		LED_LED1_ON;
		LED_LED2_ON;
		rt_thread_delay(DELAY_SYS_RUN_LED);
		LED_LED1_OFF;
		LED_LED2_OFF;
		rt_thread_delay(DELAY_SYS_RUN_LED);
		IWDG_Feed(); //喂狗
	}
}

//*************************** Modbus从机线程***************************
//函数定义: void thread_entry_ModbusSlavePoll(void* parameter)
//入口参数：无
//出口参数：无
//备    注：Editor：Armink   2013-08-02    Company: BXXJS
//******************************************************************
void thread_entry_ModbusSlavePoll(void* parameter)
{
	eMBInit(MB_RTU, 0x01, 1, 115200,  MB_PAR_EVEN);
	eMBEnable();
	while (1)
	{
		eMBPoll();
		rt_thread_delay(DELAY_MB_SLAVE_POLL);
	}
}

//**********************系统初始化函数********************************
//函数定义: int rt_application_init(void)
//入口参数：无
//出口参数：无
//备    注：Editor：Liuqiuhu   2013-1-31   Company: BXXJS
//********************************************************************
int rt_application_init(void)
{
	rt_thread_init(&thread_SysMonitor, "SysMonitor", thread_entry_SysMonitor,
			RT_NULL, thread_SysMonitor_stack, sizeof(thread_SysMonitor_stack),
			thread_SysMonitor_Prio, 5);
	rt_thread_startup(&thread_SysMonitor);

	rt_thread_init(&thread_ModbusSlavePoll, "MBSlavePoll",
			thread_entry_ModbusSlavePoll, RT_NULL,
			thread_ModbusSlavePoll_stack,
			sizeof(thread_ModbusSlavePoll_stack), thread_ModbusSlavePoll_Prio,
			5);
	rt_thread_startup(&thread_ModbusSlavePoll);

	return 0;
}

//**************************初始化RT-Thread函数*************************************
//函数定义: void rtthread_startup(void)
//入口参数：无
//出口参数：无
//备    注：Editor：Armink 2011-04-04    Company: BXXJS
//**********************************************************************************
void rtthread_startup(void)
{
	/* init board */
	rt_hw_board_init();

	/* show version */
	rt_show_version();

	/* init tick */
	rt_system_tick_init();

	/* init kernel object */
	rt_system_object_init();

	/* init timer system */
	rt_system_timer_init();

#ifdef RT_USING_HEAP
#ifdef __CC_ARM
	rt_system_heap_init((void*)&Image$$RW_IRAM1$$ZI$$Limit, (void*)STM32_SRAM_END);
#elif __ICCARM__
	rt_system_heap_init(__segment_end("HEAP"), (void*)STM32_SRAM_END);
#else
	/* init memory system */
	rt_system_heap_init((void*)&__bss_end, (void*)STM32_SRAM_END);
#endif
#endif

	/* init scheduler system */
	rt_system_scheduler_init();

	/* init all device */
	rt_device_init_all();

	/* init application */
	rt_application_init();

#ifdef RT_USING_FINSH
	/* init finsh */
	finsh_system_init();
	finsh_set_device("uart1");
#endif

	/* init timer thread */
	rt_system_timer_thread_init();

	/* init idle thread */
	rt_thread_idle_init();

	/* Add CPU usage to system */
	cpu_usage_init();

	/* start scheduler */
	rt_system_scheduler_start();

	/* never reach here */
	return;
}

