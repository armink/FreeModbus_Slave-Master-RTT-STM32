/* Includes ------------------------------------------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "mb_m_stack.h"

MB_M_StackTypeDef mbMasterStack;

void Mb_m_Task(void *argument)
{
    mbMasterStack.hardware.max485.phuart = &huart1;
    mbMasterStack.hardware.max485.dirPin = USART1_DIR_Pin;
    mbMasterStack.hardware.max485.dirPort = USART1_DIR_GPIO_Port;
    mbMasterStack.hardware.phtim = &htim4;
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
