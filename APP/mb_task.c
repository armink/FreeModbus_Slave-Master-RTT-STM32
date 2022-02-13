/* Includes ------------------------------------------------------------------*/
#include "mb.h"
#include "mb_stack.h"
#include "main.h"
#include "gpio.h"
#include "usart.h"
#include "tim.h"

MB_StackTypeDef mbStack;

void Mb_Task(void *argument)
{
    mbStack.hardware.max485.phuart = &huart2;
    mbStack.hardware.max485.dirPin = USART2_DIR_Pin;
    mbStack.hardware.max485.dirPort = USART2_DIR_GPIO_Port;
    mbStack.hardware.phtim = &htim3;
    eMBInit(&mbStack, MB_RTU, 0x01, 1, 115200, MB_PAR_NONE);
    eMBEnable(&mbStack);
    while (1)
    {
        eMBPoll(&mbStack);
    }
}
