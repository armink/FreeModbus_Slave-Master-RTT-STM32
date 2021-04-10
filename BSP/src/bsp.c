/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   STM3210B-EVAL Evaluation Board
*
* Filename      : bsp.c
* Version       : V1.00
* Programmer(s) : STM32F103X RT-Thread 0.3.1 USB-CDC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#define  BSP_MODULE

#include <bsp.h>
#include <rthw.h>
#include <rtthread.h>
#include "usart.h"
/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/** This function will initial STM32 board**/
void rt_hw_board_init()
{
    BSP_Init();
    stm32_hw_usart_init();
    stm32_hw_pin_init();
}

/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the RCC.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void RCC_Configuration(void)
{
    //下面是给各模块开启时钟
    //启动GPIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | \
                           RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | \
                           RCC_APB2Periph_GPIOE ,
                           ENABLE);
    //启动AFIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    //配置ADC转换时钟
    RCC_ADCCLKConfig(RCC_PCLK2_Div8); //9M
    //启动DMA时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//使能DMA时钟
    /* Enable ADC1 and GPIOC clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 , ENABLE);
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configer NVIC
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

#ifdef  VECT_TAB_RAM
    // Set the Vector Table base location at 0x20000000
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  // VECT_TAB_FLASH
    // Set the Vector Table base location at 0x08000000
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif
}
/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /***************数字输出IO初始化*********************/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12;  //继电器1  LED1  LED2
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_14 | GPIO_Pin_15;  //蜂鸣器 继电器3   继电器2
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /*************数字输入IO初始化*********************/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_6;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

}

 //*******************初始化独立看门狗*************************************
//函数定义: void IWDG_Configuration(void)
//描    述：初始化独立看门狗
//入口参数：无
//出口参数：无
//备    注：分频因子=4*2^prer.但最大值只能是256!时间计算(大概):Tout=40K/((4*2^prer)*rlr)值    2S超时
//Editor：liuqh 2013-1-16  Company: BXXJS
//*******************************************************************
static void IWDG_Configuration(void)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//使能对IWDG->PR和IWDG->RLR的写
    IWDG_SetPrescaler(IWDG_Prescaler_64);//64分频
    IWDG_SetReload(1300);
    IWDG_ReloadCounter();
    IWDG_Enable();
}
//*******************喂独立看门狗*************************************
//函数定义: void IWDG_Feed(void)
//描    述：初始化独立看门狗
//入口参数：无
//出口参数：prer:分频数:0~7(只有低3位有效!)，rlr:重装载寄存器值:低11位有效.
//备    注：分频因子=4*2^prer.但最大值只能是256!时间计算(大概):Tout=40K/((4*2^prer)*rlr)值
//Editor：liuqh 2013-1-16  Company: BXXJS
//*******************************************************************

void IWDG_Feed(void)
{
    IWDG_ReloadCounter();//reload
}


/*******************************************************************************
 * Function Name  : SysTick_Configuration
 * Description    : Configures the SysTick for OS tick.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void  SysTick_Configuration(void)
{
    RCC_ClocksTypeDef  rcc_clocks;
    rt_uint32_t         cnts;

    RCC_GetClocksFreq(&rcc_clocks);

    cnts = (rt_uint32_t)rcc_clocks.HCLK_Frequency / RT_TICK_PER_SECOND;

    SysTick_Config(cnts);
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
}
/**
 * This is the timer interrupt service routine.
 *
 */
void rt_hw_timer_handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();
}

/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               BSP_Init()
*
* Description : Initialize the Board Support Package (BSP).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) This function SHOULD be called before any other BSP function is called.
*********************************************************************************************************
*/

void  BSP_Init (void)
{
    RCC_Configuration();
    NVIC_Configuration();
    SysTick_Configuration();
    GPIO_Configuration();
//  TODO  方便调试，暂时注释看门狗，正式发布时需要打开
//  IWDG_Configuration();
}
//****************************防超时程序********************************
//函数定义: uint8_t AvoidTimeout(uint32_t TimeOfTimeout,uint32_t Period,uint8_t (*DetectCondition)())
//描    述：在TimeOfTimeout时间内，每Period时间检测一次DetectCondition()返回的值是否有效
//入口参数：TimeOfTimeout：防超时总时间（单位：systick）
//          Period       ：每Period时间检测一次，即时间因子（单位：systick）
//          (*DetectCondition)()：检测条件，等于ConditionValue则条件满足，检测结束，否则延时Period时间继续检测
//          ConditionValue      ；条件成立的值
//出口参数：0：在TimeOfTimeout时间内，检测到条件成立
//          1：在TimeOfTimeout时间内，没有检测到条件成立
//备    注：Editor：Armink 2012-03-09    Company: BXXJS
//**********************************************************************
uint8_t AvoidTimeout(uint32_t TimeOfTimeout,uint32_t Period,uint8_t (*DetectCondition)(),uint8_t ConditionValue)
{
    uint32_t LastTimeLocal, CurTimeLocal;
    uint8_t ConditionValueLocal;
    LastTimeLocal = rt_tick_get();
    CurTimeLocal  =  LastTimeLocal;
    while(CurTimeLocal - LastTimeLocal < TimeOfTimeout)
    {
        CurTimeLocal = rt_tick_get();
        ConditionValueLocal = DetectCondition();
        if (ConditionValueLocal == ConditionValue) return 0;
        rt_thread_delay(Period);
    }
    return 1;
}


//************************************延时函数**************************************
//函数定义: void Delay(vu32 nCount)
//入口参数：nCount ：延时函数中，循环的次数
//出口参数：无
//备    注：Editor：Armink 2011-03-18    Company: BXXJS
//**********************************************************************************
void Delay(vu32 nCount)
{
  for(; nCount!= 0;nCount--);
}


void assert_failed(u8* file, u32 line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1)
  {
  }
}
