
#ifndef  BSP_PRESENT
#define  BSP_PRESENT

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/



#include <stm32f10x_conf.h>

/*
*********************************************************************************************************
*                                                 DEFINES
*********************************************************************************************************
*/
/* board configuration */
// <o> SDCard Driver <1=>SDIO sdcard <0=>SPI MMC card
//  <i>Default: 1
#define STM32_USE_SDIO          0

/* whether use board external SRAM memory */
// <e>Use external SRAM memory on the board
//  <i>Enable External SRAM memory
#define STM32_EXT_SRAM          0
//  <o>Begin Address of External SRAM
//      <i>Default: 0x68000000
#define STM32_EXT_SRAM_BEGIN    0x68000000 /* the begining address of external SRAM */
//  <o>End Address of External SRAM
//      <i>Default: 0x68080000
#define STM32_EXT_SRAM_END      0x68080000 /* the end address of external SRAM */
// </e>

// <o> Internal SRAM memory size[Kbytes] <8-64>
//  <i>Default: 64
#define STM32_SRAM_SIZE         20
#define STM32_SRAM_END          (0x20000000 + STM32_SRAM_SIZE * 1024)

/* RT_USING_UART */
#define RT_USING_UART3
#define RT_USING_UART2
#define RT_USING_UART1
#define RT_UART_RX_BUFFER_SIZE  64

enum {
    /* modbus slave 485 receive and transmit control pin index */
    MODBUS_SLAVE_RT_CONTROL_PIN_INDEX = 0,
    /* modbus master 485 receive and transmit control pin index */
    MODBUS_MASTER_RT_CONTROL_PIN_INDEX = 1,
};

#define LED_LED1_ON                GPIO_SetBits  (GPIOA,GPIO_Pin_11)       //LED1
#define LED_LED1_OFF               GPIO_ResetBits(GPIOA,GPIO_Pin_11)       //LED1

#define LED_LED2_ON                GPIO_SetBits  (GPIOA,GPIO_Pin_12)       //LED1
#define LED_LED2_OFF               GPIO_ResetBits(GPIOA,GPIO_Pin_12)       //LED2

void rt_hw_board_init(void);

void IWDG_Configuration(void);
void IWDG_Feed(void);




/*********************************************************************************************************/
/**                                                 MACRO'S                                              */
/***********************************************************************************************************/


//软、硬件版本号定义
#define VERSION_SOFTWARE_MAJOR      1
#define VERSION_SOFTWARE_MINOR      0
#define VERSION_HARDWARE_MAJOR      1
#define VERSION_HARDWARE_MINOR      0

/***********************************************************************************************************/
/*                                               DATA TYPES                                              */
/***********************************************************************************************************/


/**********************************************************************************************************
*                                            GLOBAL VARIABLES
**********************************************************************************************************/




/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void BSP_Init(void);

void rt_hw_board_init(void);
void IWDG_Feed(void);
void rt_hw_timer_handler(void);

uint8_t AvoidTimeout(uint32_t TimeOfTimeout,uint32_t Period,uint8_t (*DetectCondition)(),uint8_t ConditionValue);
void Delay(vu32 nCount);
/*
*********************************************************************************************************
*                                           INTERRUPT SERVICES
*********************************************************************************************************
*/


#endif                                                          /* End of module include.                               */
