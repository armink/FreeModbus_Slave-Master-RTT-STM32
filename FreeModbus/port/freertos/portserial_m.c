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
 * File: $Id: portserial_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions $
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "gpio.h"
#include "usart.h"
#include "mb_m_stack.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
/* ----------------------- Static variables ---------------------------------*/

/* ----------------------- Defines ------------------------------------------*/

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBMasterPortSerialInit(void *this, UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
                             eMBParity eParity)
{
    pMax485TypeDef p = (pMax485TypeDef)this;
    /**
     * set 485 mode receive and transmit control IO
     * @note MODBUS_MASTER_RT_CONTROL_PIN_INDEX need be defined by user
     */
    HAL_GPIO_WritePin(p->dirPort, p->dirPin, GPIO_PIN_RESET);
    /* set serial configure parameter */
    HAL_UART_DeInit(p->phuart);
    p->phuart->Init.BaudRate = ulBaudRate;
    p->phuart->Init.WordLength = UART_WORDLENGTH_8B;
    p->phuart->Init.StopBits = UART_STOPBITS_1;
    p->phuart->Init.Parity = UART_PARITY_NONE;
    p->phuart->Init.Mode = UART_MODE_TX_RX;
    p->phuart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    p->phuart->Init.OverSampling = UART_OVERSAMPLING_16;

    switch (eParity)
    {
    case MB_PAR_NONE:
    {
        p->phuart->Init.WordLength = UART_WORDLENGTH_8B;
        p->phuart->Init.Parity = UART_PARITY_NONE;
        break;
    }
    case MB_PAR_ODD:
    {
        p->phuart->Init.WordLength = UART_WORDLENGTH_9B;
        p->phuart->Init.Parity = UART_PARITY_ODD;
        break;
    }
    case MB_PAR_EVEN:
    {
        p->phuart->Init.WordLength = UART_WORDLENGTH_9B;
        p->phuart->Init.Parity = UART_PARITY_EVEN;
        break;
    }
    }
    /* set serial configure */
    if (HAL_UART_Init(p->phuart) != HAL_OK)
    {
        return FALSE;
    }

    return TRUE;
}

void vMBMasterPortSerialEnable(void *this, BOOL xRxEnable, BOOL xTxEnable)
{
    pMax485TypeDef p = (pMax485TypeDef)this;
    if (xRxEnable)
    {
        /* enable RX interrupt */
        __HAL_UART_ENABLE_IT(p->phuart, UART_IT_RXNE);
    }
    else
    {
        /* disable RX interrupt */
        __HAL_UART_DISABLE_IT(p->phuart, UART_IT_RXNE);
    }
    if (xTxEnable)
    {
        /* switch 485 to transmit mode */
        HAL_GPIO_WritePin(p->dirPort, p->dirPin, GPIO_PIN_SET);
        /* start serial transmit */
        __HAL_UART_ENABLE_IT(p->phuart, UART_IT_TXE);
    }
    else
    {
        /* stop serial transmit */
        __HAL_UART_DISABLE_IT(p->phuart, UART_IT_TXE);
        while (!(__HAL_UART_GET_FLAG(p->phuart, UART_FLAG_TC)))
            ;
        /* switch 485 to receive mode */
        HAL_GPIO_WritePin(p->dirPort, p->dirPin, GPIO_PIN_RESET);
    }
}

void vMBMasterPortClose(void *this)
{
    HAL_UART_DeInit(((pMax485TypeDef)this)->phuart);
}

#endif
