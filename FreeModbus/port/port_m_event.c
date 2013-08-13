/*
 * FreeModbus Libary: STM32 Port
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
 * File: $Id: port_m_event.c add Master Functions,v 1.1 2013/08/13 15:07:05 Armink Exp $
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Variables ----------------------------------------*/
static eMBEventType eMasterQueuedEvent;
static BOOL     xMasterEventInQueue;

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortMasterEventInit( void )
{
    xMasterEventInQueue = FALSE;
    return TRUE;
}

BOOL
xMBPortMasterEventPost( eMBEventType eEvent )
{
    xMasterEventInQueue = TRUE;
    eMasterQueuedEvent = eEvent;
    return TRUE;
}

BOOL
xMBPortMasterEventGet( eMBEventType * eEvent )
{
    BOOL            xEventHappened = FALSE;

    if( xMasterEventInQueue )
    {
        *eEvent = eMasterQueuedEvent;
        xMasterEventInQueue = FALSE;
        xEventHappened = TRUE;
    }
    return xEventHappened;
}
