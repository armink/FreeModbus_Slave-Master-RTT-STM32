#ifndef _MB_STACK_H_
#define _MB_STACK_H_

#include "mb.h"
#include "mbframe.h"
#include "mbport.h"
#include "gpio.h"
#include "usart.h"
#include "tim.h"

typedef struct
{
#if MB_MASTER_RTU_ENABLED > 0

    volatile eMBSndState eSndState;
    volatile eMBRcvState eRcvState;
    volatile UCHAR ucRTUBuf[MB_SER_PDU_SIZE_MAX];
    volatile UCHAR *pucSndBufferCur;
    volatile USHORT usSndBufferCount;
    volatile USHORT usRcvBufferPos;

    /* Callback functions required by the porting layer. They are called when
     * an external event has happend which includes a timeout or the reception
     * or transmission of a character.
     * Using for Modbus Slave
     */
    pxMBFrameCBByteReceived peMBFrameCBByteReceivedCur;
    pxMBFrameCBTransmitterEmpty peMBFrameCBTransmitterEmptyCur;
    pxMBPortCBTimerExpired peMBPortCBTimerExpiredCur;

    MB_RTU_Hardware hardware;
#endif
    eMBState eState;
    UCHAR ucMBAddress;
    /* Functions pointer which are initialized in eMBInit( ). Depending on the
     * mode (RTU or ASCII) the are set to the correct implementations.
     * Using for Modbus Slave
     */
    peMBFrameSend peMBFrameSendCur;
    pvMBFrameStart pvMBFrameStartCur;
    pvMBFrameStop pvMBFrameStopCur;
    peMBFrameReceive peMBFrameReceiveCur;
    pvMBFrameClose pvMBFrameCloseCur;

    osEventFlagsId_t xSlaveOsEvent;

} MB_StackTypeDef, *pMB_StackTypeDef;

#endif
