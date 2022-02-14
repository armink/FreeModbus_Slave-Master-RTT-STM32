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
 * File: $Id: porttimer_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions$
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "mbport.h"
#include "tim.h"
#include "mb_m_stack.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
/* ----------------------- Variables ----------------------------------------*/

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBMasterPortTimersInit(void * this, USHORT usTimeOut50us)
{
    pMB_M_StackTypeDef p = (pMB_M_StackTypeDef)this;
    /* backup T35 ticks */
    p->usT35TimeOut50us = usTimeOut50us - 1;

    __HAL_TIM_SET_COUNTER(p->hardware.phtim, 0);

    __HAL_TIM_SET_AUTORELOAD(p->hardware.phtim, p->usT35TimeOut50us);

    return TRUE;
}

void vMBMasterPortTimersT35Enable(void * this)
{
    pMB_M_StackTypeDef p = (pMB_M_StackTypeDef)this;
    /* Set current timer mode, don't change it.*/
    p->eMasterCurTimerMode = (MB_TMODE_T35);

    __HAL_TIM_SET_COUNTER(p->hardware.phtim, 0);

    __HAL_TIM_SET_AUTORELOAD(p->hardware.phtim, p->usT35TimeOut50us);

    HAL_TIM_Base_Start_IT(p->hardware.phtim);
}

void vMBMasterPortTimersConvertDelayEnable(void * this)
{
    pMB_M_StackTypeDef p = (pMB_M_StackTypeDef)this;
    uint32_t timer_tick = MB_MASTER_DELAY_MS_CONVERT * 1000 / 50 - 1;

    /* Set current timer mode, don't change it.*/
    p->eMasterCurTimerMode = (MB_TMODE_CONVERT_DELAY);

    __HAL_TIM_SET_COUNTER(p->hardware.phtim, 0);

    __HAL_TIM_SET_AUTORELOAD(p->hardware.phtim, timer_tick);

    HAL_TIM_Base_Start_IT(p->hardware.phtim);
}

void vMBMasterPortTimersRespondTimeoutEnable(void * this)
{
    pMB_M_StackTypeDef p = (pMB_M_StackTypeDef)this;
    uint32_t timer_tick = MB_MASTER_TIMEOUT_MS_RESPOND * 1000 / 50 - 1;

    /* Set current timer mode, don't change it.*/
    p->eMasterCurTimerMode = (MB_TMODE_RESPOND_TIMEOUT);

    __HAL_TIM_SET_COUNTER(p->hardware.phtim, 0);

    __HAL_TIM_SET_AUTORELOAD(p->hardware.phtim, timer_tick);

    HAL_TIM_Base_Start_IT(p->hardware.phtim);
}

void vMBMasterPortTimersDisable(void * this)
{
    HAL_TIM_Base_Stop_IT((TIM_HandleTypeDef *)this);
}

#endif
