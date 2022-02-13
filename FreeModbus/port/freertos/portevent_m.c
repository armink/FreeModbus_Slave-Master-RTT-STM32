/*
 * FreeModbus Libary: RT-Thread Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portevent_m.c v 1.60 2013/08/13 15:07:05 Armink add Master Functions$
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "mbport.h"
#include "port.h"
#include "mb_m_stack.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
/* ----------------------- Defines ------------------------------------------*/
/* ----------------------- Variables ----------------------------------------*/
/* ----------------------- Start implementation -----------------------------*/
BOOL xMBMasterPortEventInit(void *this)
{
    osEventFlagsAttr_t flagAttr_t = {.cb_mem = NULL, .cb_size = 0};
    *((osEventFlagsId_t *)this) = osEventFlagsNew(&flagAttr_t);
    return TRUE;
}

BOOL xMBMasterPortEventPost(void *this, eMBMasterEventType eEvent)
{
    return (osOK == osEventFlagsSet((osEventFlagsId_t)this, eEvent));
}

BOOL xMBMasterPortEventGet(void *this, eMBMasterEventType *eEvent)
{
    uint32_t recvedEvent;
    /* waiting forever OS event */
    recvedEvent = osEventFlagsWait((osEventFlagsId_t)this,
                                   EV_MASTER_READY | EV_MASTER_FRAME_RECEIVED | EV_MASTER_EXECUTE |
                                       EV_MASTER_FRAME_SENT | EV_MASTER_ERROR_PROCESS,
                                   osFlagsWaitAny, osWaitForever);
    /* the enum type couldn't convert to int type */
    switch (recvedEvent)
    {
    case EV_MASTER_READY:
        *eEvent = EV_MASTER_READY;
        break;
    case EV_MASTER_FRAME_RECEIVED:
        *eEvent = EV_MASTER_FRAME_RECEIVED;
        break;
    case EV_MASTER_EXECUTE:
        *eEvent = EV_MASTER_EXECUTE;
        break;
    case EV_MASTER_FRAME_SENT:
        *eEvent = EV_MASTER_FRAME_SENT;
        break;
    case EV_MASTER_ERROR_PROCESS:
        *eEvent = EV_MASTER_ERROR_PROCESS;
        break;
    }
    return TRUE;
}
/**
 * This function is initialize the OS resource for modbus master.
 * Note:The resource is define by OS.If you not use OS this function can be empty.
 *
 */
void vMBMasterOsResInit(void *this)
{
    osSemaphoreAttr_t semAttr_t = {.cb_mem = NULL, .cb_size = 0};
    *((osSemaphoreId_t *)this) = osSemaphoreNew(0xffffffff, 0x01, &semAttr_t);
}

/**
 * This function is take Mobus Master running resource.
 * Note:The resource is define by Operating System.If you not use OS this function can be just return TRUE.
 *
 * @param lTimeOut the waiting time.
 *
 * @return resource taked result
 */
BOOL xMBMasterRunResTake(void *this, LONG lTimeOut)
{
    /*If waiting time is -1 .It will wait forever */
    return osSemaphoreAcquire((osSemaphoreId_t)this, lTimeOut == -1 ? osWaitForever : lTimeOut) ? FALSE : TRUE;
}

/**
 * This function is release Mobus Master running resource.
 * Note:The resource is define by Operating System.If you not use OS this function can be empty.
 *
 */
void vMBMasterRunResRelease(void *this)
{
    /* release resource */
    osSemaphoreRelease((osSemaphoreId_t)this);
}

/**
 * This is modbus master respond timeout error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 * @param ucDestAddress destination salve address
 * @param pucPDUData PDU buffer data
 * @param ucPDULength PDU buffer length
 *
 */
void vMBMasterErrorCBRespondTimeout(void *this)
{
    /**
     * @note This code is use OS's event mechanism for modbus master protocol stack.
     * If you don't use OS, you can change it.
     */
    osEventFlagsSet((osEventFlagsId_t)this, EV_MASTER_ERROR_RESPOND_TIMEOUT);

    /* You can add your code under here. */
}

/**
 * This is modbus master receive data error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 * @param ucDestAddress destination salve address
 * @param pucPDUData PDU buffer data
 * @param ucPDULength PDU buffer length
 *
 */
void vMBMasterErrorCBReceiveData(void *this)
{
    /**
     * @note This code is use OS's event mechanism for modbus master protocol stack.
     * If you don't use OS, you can change it.
     */
    osEventFlagsSet((osEventFlagsId_t)this, EV_MASTER_ERROR_RECEIVE_DATA);

    /* You can add your code under here. */
}

/**
 * This is modbus master execute function error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 * @param ucDestAddress destination salve address
 * @param pucPDUData PDU buffer data
 * @param ucPDULength PDU buffer length
 *
 */
void vMBMasterErrorCBExecuteFunction(void *this)
{
    /**
     * @note This code is use OS's event mechanism for modbus master protocol stack.
     * If you don't use OS, you can change it.
     */
    osEventFlagsSet((osEventFlagsId_t)this, EV_MASTER_ERROR_EXECUTE_FUNCTION);

    /* You can add your code under here. */
}

/**
 * This is modbus master request process success callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 */
void vMBMasterCBRequestScuuess(void *this)
{
    /**
     * @note This code is use OS's event mechanism for modbus master protocol stack.
     * If you don't use OS, you can change it.
     */
    osEventFlagsSet((osEventFlagsId_t)this, EV_MASTER_PROCESS_SUCESS);

    /* You can add your code under here. */
}

/**
 * This function is wait for modbus master request finish and return result.
 * Waiting result include request process success, request respond timeout,
 * receive data error and execute function error.You can use the above callback function.
 * @note If you are use OS, you can use OS's event mechanism. Otherwise you have to run
 * much user custom delay for waiting.
 *
 * @return request error code
 */
eMBMasterReqErrCode eMBMasterWaitRequestFinish(void *this)
{
    eMBMasterReqErrCode eErrStatus = MB_MRE_NO_ERR;
    uint32_t recvedEvent;
    /* waiting for OS event */
    recvedEvent = osEventFlagsWait((osEventFlagsId_t)this,
                                   EV_MASTER_PROCESS_SUCESS | EV_MASTER_ERROR_RESPOND_TIMEOUT |
                                       EV_MASTER_ERROR_RECEIVE_DATA | EV_MASTER_ERROR_EXECUTE_FUNCTION,
                                   osFlagsWaitAny, osWaitForever);
    switch (recvedEvent)
    {
    case EV_MASTER_PROCESS_SUCESS:
        break;
    case EV_MASTER_ERROR_RESPOND_TIMEOUT:
    {
        eErrStatus = MB_MRE_TIMEDOUT;
        break;
    }
    case EV_MASTER_ERROR_RECEIVE_DATA:
    {
        eErrStatus = MB_MRE_REV_DATA;
        break;
    }
    case EV_MASTER_ERROR_EXECUTE_FUNCTION:
    {
        eErrStatus = MB_MRE_EXE_FUN;
        break;
    }
    }
    return eErrStatus;
}

#endif
