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
 * File: $Id: portevent.c,v 1.60 2013/08/13 15:07:05 Armink $
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "mb_stack.h"

/* ----------------------- Variables ----------------------------------------*/
/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortEventInit(void * this)
{
    osEventFlagsAttr_t flagAttr_t = {.cb_mem = NULL, .cb_size = 0};
    *((osEventFlagsId_t *)this) = osEventFlagsNew(&flagAttr_t);
    return TRUE;
}

BOOL xMBPortEventPost(void * this,eMBEventType eEvent)
{
    return (osOK == osEventFlagsSet((osEventFlagsId_t)this, eEvent));
}

BOOL xMBPortEventGet(void * this,eMBEventType *eEvent)
{
    uint32_t recvedEvent;
    /* waiting forever OS event */
    recvedEvent = osEventFlagsWait((osEventFlagsId_t)this,
                                   EV_READY | EV_FRAME_RECEIVED | EV_EXECUTE | EV_FRAME_SENT,
                                   osFlagsWaitAny, osWaitForever);
    switch (recvedEvent)
    {
    case EV_READY:
        *eEvent = EV_READY;
        break;
    case EV_FRAME_RECEIVED:
        *eEvent = EV_FRAME_RECEIVED;
        break;
    case EV_EXECUTE:
        *eEvent = EV_EXECUTE;
        break;
    case EV_FRAME_SENT:
        *eEvent = EV_FRAME_SENT;
        break;
    }
    return TRUE;
}
