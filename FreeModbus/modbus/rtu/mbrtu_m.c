/*
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2013 China Beijing Armink <armink.ztl@gmail.com>
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
 * File: $Id: mbrtu_m.c,v 1.60 2013/08/17 11:42:56 Armink Add Master Functions $
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "mbrtu.h"
#include "mbframe.h"

#include "mbcrc.h"
#include "mbport.h"

#include "mb_m_stack.h"

#if MB_MASTER_RTU_ENABLED > 0
/* ----------------------- Defines ------------------------------------------*/

/* ----------------------- Type definitions ---------------------------------*/

/* ----------------------- Static variables ---------------------------------*/

/* ----------------------- Start implementation -----------------------------*/
eMBErrorCode
eMBMasterRTUInit(void * this, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    ULONG           usTimerT35_50us;
    pMB_M_StackTypeDef p = (pMB_M_StackTypeDef)this;

    ENTER_CRITICAL_SECTION(  );

    /* Modbus RTU uses 8 Databits. */
    if( xMBMasterPortSerialInit( (void *)&(p->hardware.max485), ucPort, ulBaudRate, 8, eParity ) != TRUE )
    {
        eStatus = MB_EPORTERR;
    }
    else
    {
        /* If baudrate > 19200 then we should use the fixed timer values
         * t35 = 1750us. Otherwise t35 must be 3.5 times the character time.
         */
        if( ulBaudRate > 19200 )
        {
            usTimerT35_50us = 35;       /* 1800us. */
        }
        else
        {
            /* The timer reload value for a character is given by:
             *
             * ChTimeValue = Ticks_per_1s / ( Baudrate / 11 )
             *             = 11 * Ticks_per_1s / Baudrate
             *             = 220000 / Baudrate
             * The reload for t3.5 is 1.5 times this value and similary
             * for t3.5.
             */
            usTimerT35_50us = ( 7UL * 220000UL ) / ( 2UL * ulBaudRate );
        }
        if( xMBMasterPortTimersInit( this, ( USHORT ) usTimerT35_50us ) != TRUE )
        {
            eStatus = MB_EPORTERR;
        }
    }
    EXIT_CRITICAL_SECTION(  );

    return eStatus;
}

void
eMBMasterRTUStart( void * this )
{
    pMB_M_StackTypeDef p = (pMB_M_StackTypeDef)this;
    ENTER_CRITICAL_SECTION(  );
    /* Initially the receiver is in the state STATE_M_RX_INIT. we start
     * the timer and if no character is received within t3.5 we change
     * to STATE_M_RX_IDLE. This makes sure that we delay startup of the
     * modbus protocol stack until the bus is free.
     */
    p->eRcvState = STATE_M_RX_INIT;
    vMBMasterPortSerialEnable( (void *)&(p->hardware.max485), TRUE, FALSE );
    vMBMasterPortTimersT35Enable( this );

    EXIT_CRITICAL_SECTION(  );
}

void
eMBMasterRTUStop( void * this )
{
    pMB_M_StackTypeDef p = (pMB_M_StackTypeDef)this;
    ENTER_CRITICAL_SECTION(  );
    vMBMasterPortSerialEnable( (void*)&(p->hardware.max485), FALSE, FALSE );
    vMBMasterPortTimersDisable( (void *)(p->hardware.phtim) );
    EXIT_CRITICAL_SECTION(  );
}

eMBErrorCode
eMBMasterRTUReceive( void * this, UCHAR * pucRcvAddress, UCHAR ** pucFrame, USHORT * pusLength )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    pMB_M_StackTypeDef p = (pMB_M_StackTypeDef)this;

    ENTER_CRITICAL_SECTION(  );
    assert_param( p->usMasterRcvBufferPos < MB_SER_PDU_SIZE_MAX );

    /* Length and CRC check */
    if( ( p->usMasterRcvBufferPos >= MB_SER_PDU_SIZE_MIN )
        && ( usMBCRC16( ( UCHAR * ) p->ucMasterRTURcvBuf, p->usMasterRcvBufferPos ) == 0 ) )
    {
        /* Save the address field. All frames are passed to the upper layed
         * and the decision if a frame is used is done there.
         */
        *pucRcvAddress = p->ucMasterRTURcvBuf[MB_SER_PDU_ADDR_OFF];

        /* Total length of Modbus-PDU is Modbus-Serial-Line-PDU minus
         * size of address field and CRC checksum.
         */
        *pusLength = ( USHORT )( p->usMasterRcvBufferPos - MB_SER_PDU_PDU_OFF - MB_SER_PDU_SIZE_CRC );

        /* Return the start of the Modbus PDU to the caller. */
        *pucFrame = ( UCHAR * ) & p->ucMasterRTURcvBuf[MB_SER_PDU_PDU_OFF];
    }
    else
    {
        eStatus = MB_EIO;
    }

    EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

eMBErrorCode
eMBMasterRTUSend( void * this, UCHAR ucSlaveAddress, const UCHAR * pucFrame, USHORT usLength )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          usCRC16;
    pMB_M_StackTypeDef p = (pMB_M_StackTypeDef)this;

    if ( ucSlaveAddress > MB_MASTER_TOTAL_SLAVE_NUM ) return MB_EINVAL;

    ENTER_CRITICAL_SECTION(  );

    /* Check if the receiver is still in idle state. If not we where to
     * slow with processing the received frame and the master sent another
     * frame on the network. We have to abort sending the frame.
     */
    if( p->eRcvState == STATE_M_RX_IDLE )
    {
        /* First byte before the Modbus-PDU is the slave address. */
        p->pucMasterSndBufferCur = ( UCHAR * ) pucFrame - 1;
        p->usMasterSndBufferCount = 1;

        /* Now copy the Modbus-PDU into the Modbus-Serial-Line-PDU. */
        p->pucMasterSndBufferCur[MB_SER_PDU_ADDR_OFF] = ucSlaveAddress;
        p->usMasterSndBufferCount += usLength;

        /* Calculate CRC16 checksum for Modbus-Serial-Line-PDU. */
        usCRC16 = usMBCRC16( ( UCHAR * ) p->pucMasterSndBufferCur, p->usMasterSndBufferCount );
        p->ucMasterRTUSndBuf[p->usMasterSndBufferCount++] = ( UCHAR )( usCRC16 & 0xFF );
        p->ucMasterRTUSndBuf[p->usMasterSndBufferCount++] = ( UCHAR )( usCRC16 >> 8 );

        /* Activate the transmitter. */
        p->eSndState = STATE_M_TX_XMIT;
        vMBMasterPortSerialEnable( (void*)&(p->hardware.max485), FALSE, TRUE );
    }
    else
    {
        eStatus = MB_EIO;
    }
    EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

BOOL
xMBMasterRTUReceiveFSM( void * this )
{
    BOOL            xTaskNeedSwitch = FALSE;
    UCHAR           ucByte;
    pMB_M_StackTypeDef p = (pMB_M_StackTypeDef)this;

    assert_param(( p->eSndState == STATE_M_TX_IDLE ) || ( p->eSndState == STATE_M_TX_XFWR ));

    /* Always read the character. */
    ucByte = (UCHAR)(p->hardware.max485.phuart->Instance->DR);

    switch ( p->eRcvState )
    {
        /* If we have received a character in the init state we have to
         * wait until the frame is finished.
         */
    case STATE_M_RX_INIT:
        vMBMasterPortTimersT35Enable( this );
        break;

        /* In the error state we wait until all characters in the
         * damaged frame are transmitted.
         */
    case STATE_M_RX_ERROR:
        vMBMasterPortTimersT35Enable( this );
        break;

        /* In the idle state we wait for a new character. If a character
         * is received the t1.5 and t3.5 timers are started and the
         * receiver is in the state STATE_RX_RECEIVCE and disable early
         * the timer of respond timeout .
         */
    case STATE_M_RX_IDLE:
        /* In time of respond timeout,the receiver receive a frame.
         * Disable timer of respond timeout and change the transmiter state to idle.
         */
        vMBMasterPortTimersDisable( (void *)(p->hardware.phtim) );
        p->eSndState = STATE_M_TX_IDLE;

        p->usMasterRcvBufferPos = 0;
        p->ucMasterRTURcvBuf[p->usMasterRcvBufferPos++] = ucByte;
        p->eRcvState = STATE_M_RX_RCV;

        /* Enable t3.5 timers. */
        vMBMasterPortTimersT35Enable( this );
        break;

        /* We are currently receiving a frame. Reset the timer after
         * every character received. If more than the maximum possible
         * number of bytes in a modbus frame is received the frame is
         * ignored.
         */
    case STATE_M_RX_RCV:
        if( p->usMasterRcvBufferPos < MB_SER_PDU_SIZE_MAX )
        {
            p->ucMasterRTURcvBuf[p->usMasterRcvBufferPos++] = ucByte;
        }
        else
        {
            p->eRcvState = STATE_M_RX_ERROR;
        }
        vMBMasterPortTimersT35Enable( this );
        break;
    }
    return xTaskNeedSwitch;
}

BOOL
xMBMasterRTUTransmitFSM( void * this )
{
    BOOL            xNeedPoll = FALSE;
    pMB_M_StackTypeDef p = (pMB_M_StackTypeDef)this;

    assert_param( p->eRcvState == STATE_M_RX_IDLE );

    switch ( p->eSndState )
    {
        /* We should not get a transmitter event if the transmitter is in
         * idle state.  */
    case STATE_M_TX_IDLE:
        /* enable receiver/disable transmitter. */
        vMBMasterPortSerialEnable( (void*)&(p->hardware.max485), TRUE, FALSE );
        break;

    case STATE_M_TX_XMIT:
        /* check if we are finished. */
        if( p->usMasterSndBufferCount != 0 )
        {
            p->hardware.max485.phuart->Instance->DR = (CHAR)*(p->pucMasterSndBufferCur);
            p->pucMasterSndBufferCur++;  /* next byte in sendbuffer. */
            p->usMasterSndBufferCount--;
        }
        else
        {
            p->xFrameIsBroadcast = ( p->ucMasterRTUSndBuf[MB_SER_PDU_ADDR_OFF] == MB_ADDRESS_BROADCAST ) ? TRUE : FALSE;
            /* Disable transmitter. This prevents another transmit buffer
             * empty interrupt. */
            vMBMasterPortSerialEnable( (void*)&(p->hardware.max485), TRUE, FALSE );
            p->eSndState = STATE_M_TX_XFWR;
            /* If the frame is broadcast ,master will enable timer of convert delay,
             * else master will enable timer of respond timeout. */
            if ( p->xFrameIsBroadcast == TRUE )
            {
                vMBMasterPortTimersConvertDelayEnable( this );
            }
            else
            {
                vMBMasterPortTimersRespondTimeoutEnable( this );
            }
        }
        break;

    default:
        break;
    }

    return xNeedPoll;
}

BOOL
xMBMasterRTUTimerExpired( void * this )
{
    BOOL xNeedPoll = FALSE;
    pMB_M_StackTypeDef p = (pMB_M_StackTypeDef)this;

    switch (p->eRcvState)
    {
        /* Timer t35 expired. Startup phase is finished. */
    case STATE_M_RX_INIT:
        xNeedPoll = xMBMasterPortEventPost( (void *)(p->xMasterOsEvent), EV_MASTER_READY);
        break;

        /* A frame was received and t35 expired. Notify the listener that
         * a new frame was received. */
    case STATE_M_RX_RCV:
        xNeedPoll = xMBMasterPortEventPost( (void *)(p->xMasterOsEvent), EV_MASTER_FRAME_RECEIVED);
        break;

        /* An error occured while receiving the frame. */
    case STATE_M_RX_ERROR:
        p->eMBMasterCurErrorType = EV_ERROR_RECEIVE_DATA;
        xNeedPoll = xMBMasterPortEventPost( (void *)(p->xMasterOsEvent),  EV_MASTER_ERROR_PROCESS );
        break;

        /* Function called in an illegal state. */
    default:
        assert_param(
                ( p->eRcvState == STATE_M_RX_INIT ) || ( p->eRcvState == STATE_M_RX_RCV ) ||
                ( p->eRcvState == STATE_M_RX_ERROR ) || ( p->eRcvState == STATE_M_RX_IDLE ));
        break;
    }
    p->eRcvState = STATE_M_RX_IDLE;

    switch (p->eSndState)
    {
        /* A frame was send finish and convert delay or respond timeout expired.
         * If the frame is broadcast,The master will idle,and if the frame is not
         * broadcast.Notify the listener process error.*/
    case STATE_M_TX_XFWR:
        if ( p->xFrameIsBroadcast == FALSE ) {
            p->eMBMasterCurErrorType = EV_ERROR_RESPOND_TIMEOUT;
            xNeedPoll = xMBMasterPortEventPost( (void *)(p->xMasterOsEvent), EV_MASTER_ERROR_PROCESS);
        }
        break;
        /* Function called in an illegal state. */
    default:
        assert_param(
                ( p->eSndState == STATE_M_TX_XFWR ) || ( p->eSndState == STATE_M_TX_IDLE ));
        break;
    }
    p->eSndState = STATE_M_TX_IDLE;

    vMBMasterPortTimersDisable( (void *)(p->hardware.phtim) );
    /* If timer mode is convert delay, the master event then turns EV_MASTER_EXECUTE status. */
    if (p->eMasterCurTimerMode == MB_TMODE_CONVERT_DELAY) {
        xNeedPoll = xMBMasterPortEventPost( (void *)(p->xMasterOsEvent),  EV_MASTER_EXECUTE );
    }

    return xNeedPoll;
}

#endif

