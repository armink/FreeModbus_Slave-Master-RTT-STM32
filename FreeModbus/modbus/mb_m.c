/*
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: mbrtu_m.c,v 1.60 2013/08/20 11:18:10 Armink Add Master Functions $
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/

#include "mb.h"
#include "mb_m.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbfunc.h"

#include "mbport.h"
#if MB_MASTER_RTU_ENABLED == 1
#include "mbrtu.h"
#endif
#if MB_MASTER_ASCII_ENABLED == 1
#include "mbascii.h"
#endif
#if MB_MASTER_TCP_ENABLED == 1
#include "mbtcp.h"
#endif

#include "mb_m_stack.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0

#ifndef MB_PORT_HAS_CLOSE
#define MB_PORT_HAS_CLOSE 0
#endif

/* ----------------------- Static variables ---------------------------------*/

/* An array of Modbus functions handlers which associates Modbus function
 * codes with implementing functions.
 */
static xMBFunctionHandler xMasterFuncHandlers[MB_FUNC_HANDLERS_MAX] = {
#if MB_FUNC_OTHER_REP_SLAVEID_ENABLED > 0
    //TODO Add Master function define
    {MB_FUNC_OTHER_REPORT_SLAVEID, eMBFuncReportSlaveID},
#endif
#if MB_FUNC_READ_INPUT_ENABLED > 0
    {MB_FUNC_READ_INPUT_REGISTER, eMBMasterFuncReadInputRegister},
#endif
#if MB_FUNC_READ_HOLDING_ENABLED > 0
    {MB_FUNC_READ_HOLDING_REGISTER, eMBMasterFuncReadHoldingRegister},
#endif
#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_REGISTERS, eMBMasterFuncWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_WRITE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_REGISTER, eMBMasterFuncWriteHoldingRegister},
#endif
#if MB_FUNC_READWRITE_HOLDING_ENABLED > 0
    {MB_FUNC_READWRITE_MULTIPLE_REGISTERS, eMBMasterFuncReadWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_READ_COILS_ENABLED > 0
    {MB_FUNC_READ_COILS, eMBMasterFuncReadCoils},
#endif
#if MB_FUNC_WRITE_COIL_ENABLED > 0
    {MB_FUNC_WRITE_SINGLE_COIL, eMBMasterFuncWriteCoil},
#endif
#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_COILS, eMBMasterFuncWriteMultipleCoils},
#endif
#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0
    {MB_FUNC_READ_DISCRETE_INPUTS, eMBMasterFuncReadDiscreteInputs},
#endif
};

/* ----------------------- Start implementation -----------------------------*/
eMBErrorCode
eMBMasterInit( void * this, eMBMode eMode, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity)
{
    eMBErrorCode eStatus = MB_ENOERR;
    pMB_M_StackTypeDef p = (pMB_M_StackTypeDef)this;

    switch (eMode)
    {
#if MB_MASTER_RTU_ENABLED > 0
    case MB_RTU:
        p->pvMBMasterFrameStartCur = eMBMasterRTUStart;
        p->pvMBMasterFrameStopCur = eMBMasterRTUStop;
        p->peMBMasterFrameSendCur = eMBMasterRTUSend;
        p->peMBMasterFrameReceiveCur = eMBMasterRTUReceive;
        p->pvMBMasterFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBMasterPortClose : NULL;
        p->peMBMasterFrameCBByteReceivedCur = xMBMasterRTUReceiveFSM;
        p->peMBMasterFrameCBTransmitterEmptyCur = xMBMasterRTUTransmitFSM;
        p->peMBMasterPortCBTimerExpiredCur = xMBMasterRTUTimerExpired;
        eStatus = eMBMasterRTUInit((void *)this, ucPort, ulBaudRate, eParity);
        break;
#endif
#if MB_MASTER_ASCII_ENABLED > 0
    case MB_ASCII:
        p->pvMBMasterFrameStartCur = eMBMasterASCIIStart;
        p->pvMBMasterFrameStopCur = eMBMasterASCIIStop;
        p->peMBMasterFrameSendCur = eMBMasterASCIISend;
        p->peMBMasterFrameReceiveCur = eMBMasterASCIIReceive;
        p->p->pvMBMasterFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBMasterPortClose : NULL;
        p->pxMBMasterFrameCBByteReceived = xMBMasterASCIIReceiveFSM;
        p->pxMBMasterFrameCBTransmitterEmpty = xMBMasterASCIITransmitFSM;
        p->pxMBMasterPortCBTimerExpired = xMBMasterASCIITimerT1SExpired;

        eStatus = eMBMasterASCIIInit(ucPort, ulBaudRate, eParity);
        break;
#endif
    default:
        eStatus = MB_EINVAL;
        break;
    }

    if (eStatus == MB_ENOERR)
    {
        if (!xMBMasterPortEventInit((void *)&(p->xMasterOsEvent)))
        {
            /* port dependent event module initalization failed. */
            eStatus = MB_EPORTERR;
        }
        else
        {
            p->eState = STATE_DISABLED;
        }
        /* initialize the OS resource for modbus master. */
        vMBMasterOsResInit((void *)&(p->xMasterRunRes));
    }
    return eStatus;
}

eMBErrorCode
eMBMasterClose(void * this)
{
    eMBErrorCode eStatus = MB_ENOERR;
    pMB_M_StackTypeDef p = (pMB_M_StackTypeDef)this;

    if (p->eState == STATE_DISABLED)
    {
        if (p->pvMBMasterFrameCloseCur != NULL)
        {
            p->pvMBMasterFrameCloseCur((void *)&(p->hardware.max485));
        }
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

eMBErrorCode
eMBMasterEnable(void * this)
{
    eMBErrorCode eStatus = MB_ENOERR;
    pMB_M_StackTypeDef p = (pMB_M_StackTypeDef)this;

    if (p->eState == STATE_DISABLED)
    {
        /* Activate the protocol stack. */
        p->pvMBMasterFrameStartCur(this);
        p->eState = STATE_ENABLED;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

eMBErrorCode
eMBMasterDisable(void * this)
{
    eMBErrorCode eStatus;
    pMB_M_StackTypeDef p = (pMB_M_StackTypeDef)this;

    if ((p->eState == STATE_ENABLED) || (p->eState == STATE_ESTABLISHED))
    {
        p->pvMBMasterFrameStopCur(this);
        p->eState = STATE_DISABLED;
        eStatus = MB_ENOERR;
    }
    else if (p->eState == STATE_DISABLED)
    {
        eStatus = MB_ENOERR;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

BOOL eMBMasterIsEstablished(void * this)
{
    pMB_M_StackTypeDef p = (pMB_M_StackTypeDef)this;
    if (p->eState == STATE_ESTABLISHED)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

eMBErrorCode
eMBMasterPoll(void * this)
{
    UCHAR *ucMBFrame;
    UCHAR ucRcvAddress;
    UCHAR ucFunctionCode;
    USHORT usLength;
    eMBException eException;

    int i, j;
    eMBErrorCode eStatus = MB_ENOERR;
    eMBMasterEventType eEvent;
    
    pMB_M_StackTypeDef p = (pMB_M_StackTypeDef)this;

    /* Check if the protocol stack is ready. */
    if ((p->eState != STATE_ENABLED) && (p->eState != STATE_ESTABLISHED))
    {
        return MB_EILLSTATE;
    }

    /* Check if there is a event available. If not return control to caller.
     * Otherwise we will handle the event. */
    if (xMBMasterPortEventGet((void *)(p->xMasterOsEvent), &eEvent) == TRUE)
    {
        switch (eEvent)
        {
        case EV_MASTER_READY:
            p->eState = STATE_ESTABLISHED;
            break;

        case EV_MASTER_FRAME_RECEIVED:
            eStatus = p->peMBMasterFrameReceiveCur(this, &ucRcvAddress, &ucMBFrame, &usLength);
            /* Check if the frame is for us. If not ,send an error process event. */
            if ((eStatus == MB_ENOERR) && (ucRcvAddress == p->ucMBMasterDestAddress))
            {
                goto EV_MASTER_EXECUTE_LABLE;
            }
            else
            {
                p->eMBMasterCurErrorType = EV_ERROR_RECEIVE_DATA;
                goto EV_MASTER_ERROR_PROCESS_LABLE;
            }
            break;

        case EV_MASTER_EXECUTE:
        EV_MASTER_EXECUTE_LABLE:
            ucMBFrame = (UCHAR *)&(p->ucMasterRTUSndBuf[MB_SER_PDU_PDU_OFF]);
            ucFunctionCode = ucMBFrame[MB_PDU_FUNC_OFF];
            eException = MB_EX_ILLEGAL_FUNCTION;
            /* If receive frame has exception .The receive function code highest bit is 1.*/
            if (ucFunctionCode >> 7)
            {
                eException = (eMBException)ucMBFrame[MB_PDU_DATA_OFF];
            }
            else
            {
                for (i = 0; i < MB_FUNC_HANDLERS_MAX; i++)
                {
                    /* No more function handlers registered. Abort. */
                    if (xMasterFuncHandlers[i].ucFunctionCode == 0)
                    {
                        break;
                    }
                    else if (xMasterFuncHandlers[i].ucFunctionCode == ucFunctionCode)
                    {
                        /* If master request is broadcast,
                         * the master need execute function for all slave.
                         */
                        if (p->xFrameIsBroadcast)
                        {
                            usLength = p->usMasterSendPDULength;
                            for (j = 1; j <= MB_MASTER_TOTAL_SLAVE_NUM; j++)
                            {
                                p->ucMBMasterDestAddress = j;
                                eException = xMasterFuncHandlers[i].pxHandler(this, ucMBFrame, &usLength);
                            }
                        }
                        else
                        {
                            eException = xMasterFuncHandlers[i].pxHandler(this, ucMBFrame, &usLength);
                        }
                        break;
                    }
                }
            }
            /* If master has exception ,Master will send error process.Otherwise the Master is idle.*/
            if (eException != MB_EX_NONE)
            {
                p->eMBMasterCurErrorType = EV_ERROR_EXECUTE_FUNCTION;
                goto EV_MASTER_ERROR_PROCESS_LABLE;
            }
            else
            {
                vMBMasterCBRequestScuuess((void *)(p->xMasterOsEvent));
                vMBMasterRunResRelease((void *)(p->xMasterRunRes));
            }
            break;

        case EV_MASTER_FRAME_SENT:
            /* Master is busy now. */
            p->peMBMasterFrameSendCur(this, p->ucMBMasterDestAddress, (UCHAR *)&(p->ucMasterRTUSndBuf[MB_SER_PDU_PDU_OFF]), p->usMasterSendPDULength);
            break;

        case EV_MASTER_ERROR_PROCESS:
        EV_MASTER_ERROR_PROCESS_LABLE:
            /* Execute specified error process callback function. */
            switch (p->eMBMasterCurErrorType)
            {
            case EV_ERROR_RESPOND_TIMEOUT:
                vMBMasterErrorCBRespondTimeout((void *)(p->xMasterOsEvent));
                break;
            case EV_ERROR_RECEIVE_DATA:
                vMBMasterErrorCBReceiveData((void *)(p->xMasterOsEvent));
                break;
            case EV_ERROR_EXECUTE_FUNCTION:
                vMBMasterErrorCBExecuteFunction((void *)(p->xMasterOsEvent));
                break;
            }
            vMBMasterRunResRelease((void *)(p->xMasterRunRes));
            break;

        default:
            break;
        }
    }
    return MB_ENOERR;
}

#endif
