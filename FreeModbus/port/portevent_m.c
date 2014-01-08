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
 * File: $Id: portevent_m.c v 1.60 2013/08/13 15:07:05 Armink add Master Functions$
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "port.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
/* ----------------------- Variables ----------------------------------------*/
static eMBMasterEventType  eMasterQueuedEvent;
static BOOL                xMasterEventInQueue;
static struct rt_semaphore xMasterRunRes;

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBMasterPortEventInit( void )
{
    xMasterEventInQueue = FALSE;
    return TRUE;
}

BOOL
xMBMasterPortEventPost( eMBMasterEventType eEvent )
{
    xMasterEventInQueue = TRUE;
    eMasterQueuedEvent = eEvent;
    return TRUE;
}

BOOL
xMBMasterPortEventGet( eMBMasterEventType * eEvent )
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

/**
 * This function is initialize the Mobus Master running resource .
 * Note:The resource is define by Operating System.If you not use Opearting System this function can be empty.
 *
 */
void vMBasterRunResInit( void )
{
	rt_sem_init(&xMasterRunRes, "master res", 0x01 , RT_IPC_FLAG_PRIO);
}

/**
 * This function is take Mobus Master running resource.
 * Note:The resource is define by Operating System.If you not use Opearting System this function can be just return TRUE.
 *
 * @param lTimeOut the waiting time.
 *
 * @return resource taked result
 */
BOOL xMBasterRunResTake( LONG lTimeOut )
{
	/*If waiting time is -1 .It will wait forever */
	return rt_sem_take(&xMasterRunRes, lTimeOut) ? FALSE : TRUE ;
}

/**
 * This function is release Mobus Master running resource.
 * Note:The resource is define by Operating System.If you not use Opearting System this function can be empty.
 *
 */
void vMBasterRunResRelease( void )
{
	/* Clear up resource when need release resource. */
	rt_sem_control(&xMasterRunRes, RT_IPC_CMD_RESET, 0);
	rt_sem_release(&xMasterRunRes);
}

#endif
