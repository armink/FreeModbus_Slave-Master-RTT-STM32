/* Includes ------------------------------------------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "mb_m_stack.h"

MB_M_StackTypeDef mbMasterStack = NEW_MB_M_StackTypeDef;

void Mb_m_Task(void *argument)
{
    mbMasterStack.hardware.max485.phuart = &huart2;
    mbMasterStack.hardware.max485.dirPin = USART2_DIR_Pin;
    mbMasterStack.hardware.max485.dirPort = USART2_DIR_GPIO_Port;
    mbMasterStack.hardware.phtim = &htim3;
    eMBMasterInit(&mbMasterStack, MB_RTU, 2, 115200, MB_PAR_NONE);
    eMBMasterEnable(&mbMasterStack);
    while (1)
    {
        eMBMasterPoll(&mbMasterStack);
    }
}

void Mb_m_ComTask(void *argument)
{
    for (;;)
    {
        eMBMasterReqReadHoldingRegister(&mbMasterStack, 1, 1, 1, 100);
        osDelay(300);
    }
}
