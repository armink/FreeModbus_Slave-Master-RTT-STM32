/*********************************************************************************************************/
// Filename      : delay_config.h
// Version       : V1.00
// Programmer(s) : Liuqiuhu
// funcion		 : This file is used to configure the delay time
/*********************************************************************************************************/
#ifndef __DELAY_CONF_H__
#define __DELAY_CONF_H__

#include"rtconfig.h"

#if RT_TICK_PER_SECOND == 1
#define DELAY_1S			(RT_TICK_PER_SECOND)		
#define DELAY_S(X) 			(X*DELAY_1S)

#elif RT_TICK_PER_SECOND == 10				
#define DELAY_100MS(X)		(X)
#define DELAY_S(X)			(X*10)

#elif RT_TICK_PER_SECOND == 100		
#define	DELAY_10MS(X)		(X)
#define	DELAY_100MS(X)		(X*10)
#define DELAY_S(X)			(X*100)

#elif (RT_TICK_PER_SECOND == 1000)

#define DELAY_1MS           (RT_TICK_PER_SECOND/1000)		
#define DELAY_MS(X)         (X*DELAY_1MS)
#define DELAY_S(X)		    (X*1000*DELAY_1MS)

#elif (RT_TICK_PER_SECOND == 10000)||(RT_TICK_PER_SECOND == 100000)||(RT_TICK_PER_SECOND == 100000)
#define DELAY_100US(X)      (X*RT_TICK_PER_SECOND/10000)
#define DELAY_1MS           (RT_TICK_PER_SECOND/1000)		
#define DELAY_MS(X)         (X*DELAY_1MS)
#define DELAY_S(X)		    (X*1000*DELAY_1MS)

#endif

#define DELAY_SYS_RUN_LED			DELAY_MS(500)  //系统指示灯延时500ms
#define DELAY_SYS_SLEEP_LED			DELAY_MS(1000) //系统指示灯延时1s
#define DELAY_MB_SLAVE_POLL		    DELAY_MS(10)   //Modbus从机轮训时间
#define DELAY_MB_MASTER_POLL	    DELAY_MS(10)   //Modbus主机轮训时间

#endif  

