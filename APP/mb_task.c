/* Includes ------------------------------------------------------------------*/
#include "mb.h"
#include "mb_stack.h"

MB_StackTypeDef mbStack = NEW_MB_StackTypeDef;

void Mb_Task(void *argument)
{
    mbStack.hardware.max485.phuart = &huart1;
    mbStack.hardware.max485.dirPin = USART1_DIR_Pin;
    mbStack.hardware.max485.dirPort = USART1_DIR_GPIO_Port;
    mbStack.hardware.phtim = &htim4;
    eMBInit(&mbStack, MB_RTU, 0x01, 1, 115200, MB_PAR_NONE);
    eMBEnable(&mbStack);
    while (1)
    {
        eMBPoll(&mbStack);
    }
}
