#ifndef _MB_M_STACK_H_
#define _MB_M_STACK_H_

#include "mb.h"
#include "mb_m.h"
#include "mbframe.h"
#include "mbport.h"
#include "gpio.h"
#include "usart.h"
#include "tim.h"

typedef struct
{
#if MB_MASTER_RTU_ENABLED > 0
    volatile eMBMasterSndState eSndState;
    volatile eMBMasterRcvState eRcvState;
    volatile UCHAR ucMasterRTUSndBuf[MB_PDU_SIZE_MAX];
    volatile UCHAR ucMasterRTURcvBuf[MB_SER_PDU_SIZE_MAX];
    volatile UCHAR *pucMasterSndBufferCur;
    volatile USHORT usMasterSndBufferCount;
    volatile USHORT usMasterSendPDULength;
    volatile USHORT usMasterRcvBufferPos;
    volatile BOOL xFrameIsBroadcast;
    volatile eMBMasterTimerMode eMasterCurTimerMode;

    USHORT usT35TimeOut50us;

    /* Callback functions required by the porting layer. They are called when
     * an external event has happend which includes a timeout or the reception
     * or transmission of a character.
     * Using for Modbus Master,Add by Armink 20130813
     */
    pxMBMasterFrameCBByteReceived peMBMasterFrameCBByteReceivedCur;
    pxMBMasterFrameCBTransmitterEmpty peMBMasterFrameCBTransmitterEmptyCur;
    pxMBMasterPortCBTimerExpired peMBMasterPortCBTimerExpiredCur;

    MB_RTU_Hardware hardware;
#endif
    eMBState eState;

    UCHAR ucMBMasterDestAddress;

    eMBMasterErrorEventType eMBMasterCurErrorType;

    /* Functions pointer which are initialized in eMBInit( ). Depending on the
     * mode (RTU or ASCII) the are set to the correct implementations.
     * Using for Modbus Master,Add by Armink 20130813
     */
    peMBFrameSend peMBMasterFrameSendCur;
    pvMBFrameStart pvMBMasterFrameStartCur;
    pvMBFrameStop pvMBMasterFrameStopCur;
    peMBFrameReceive peMBMasterFrameReceiveCur;
    pvMBFrameClose pvMBMasterFrameCloseCur;

    osSemaphoreId_t xMasterRunRes;
    osEventFlagsId_t xMasterOsEvent;

} MB_M_StackTypeDef, *pMB_M_StackTypeDef;

#define NEW_MB_M_StackTypeDef {.eState = STATE_NOT_INITIALIZED}

#endif
