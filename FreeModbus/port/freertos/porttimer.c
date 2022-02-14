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
 * File: $Id: porttimer.c,v 1.60 2013/08/13 15:07:05 Armink $
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "tim.h"
#include "mb_stack.h"
/* ----------------------- static functions ---------------------------------*/

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortTimersInit(void * this,USHORT usTim1Timerout50us)
{
    pMB_StackTypeDef p = (pMB_StackTypeDef)this;

    __HAL_TIM_SET_COUNTER(p->hardware.phtim, 0);

    __HAL_TIM_SET_AUTORELOAD(p->hardware.phtim, usTim1Timerout50us - 1);

    return TRUE;
}

void vMBPortTimersEnable(void * this)
{
    HAL_TIM_Base_Start_IT((TIM_HandleTypeDef *)this);
}

void vMBPortTimersDisable(void * this)
{
    HAL_TIM_Base_Stop_IT((TIM_HandleTypeDef *)this);
}

