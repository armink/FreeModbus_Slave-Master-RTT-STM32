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
 * File: $Id: mbfuncholding_m.c,v 1.60 2013/09/02 14:13:40 Armink Add Master Functions  Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbconfig.h"

/* ----------------------- Defines ------------------------------------------*/
#define MB_PDU_FUNC_READ_ADDR_OFF               ( MB_PDU_DATA_OFF + 0)
#define MB_PDU_FUNC_READ_REGCNT_OFF             ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_READ_SIZE                   ( 4 )
#define MB_PDU_FUNC_READ_REGCNT_MAX             ( 0x007D )

#define MB_PDU_FUNC_WRITE_ADDR_OFF              ( MB_PDU_DATA_OFF + 0)
#define MB_PDU_FUNC_WRITE_VALUE_OFF             ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_WRITE_SIZE                  ( 4 )

#define MB_PDU_FUNC_WRITE_MUL_ADDR_OFF          ( MB_PDU_DATA_OFF + 0 )
#define MB_PDU_FUNC_WRITE_MUL_REGCNT_OFF        ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF       ( MB_PDU_DATA_OFF + 4 )
#define MB_PDU_FUNC_WRITE_MUL_VALUES_OFF        ( MB_PDU_DATA_OFF + 5 )
#define MB_PDU_FUNC_WRITE_MUL_SIZE_MIN          ( 5 )
#define MB_PDU_FUNC_WRITE_MUL_REGCNT_MAX        ( 0x0078 )

#define MB_PDU_FUNC_READWRITE_READ_ADDR_OFF     ( MB_PDU_DATA_OFF + 0 )
#define MB_PDU_FUNC_READWRITE_READ_REGCNT_OFF   ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_READWRITE_WRITE_ADDR_OFF    ( MB_PDU_DATA_OFF + 4 )
#define MB_PDU_FUNC_READWRITE_WRITE_REGCNT_OFF  ( MB_PDU_DATA_OFF + 6 )
#define MB_PDU_FUNC_READWRITE_BYTECNT_OFF       ( MB_PDU_DATA_OFF + 8 )
#define MB_PDU_FUNC_READWRITE_WRITE_VALUES_OFF  ( MB_PDU_DATA_OFF + 9 )
#define MB_PDU_FUNC_READWRITE_SIZE_MIN          ( 9 )

/* ----------------------- Static functions ---------------------------------*/

/* ----------------------- Start implementation -----------------------------*/
#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
#if MB_FUNC_WRITE_HOLDING_ENABLED > 0

eMBMasterReqErrCode
eMBMasterReqWriteHoldingRegister( UCHAR ucSndAddr, USHORT * pusDataBuffer, USHORT usRegAddr )
{
    UCHAR                 *ucMBFrame;
    eMBMasterReqErrCode    eErrStatus = MB_MRE_NO_ERR;

    if ( xMBMasterGetIsBusy() ) eErrStatus = MB_MRE_MASTER_BUSY;
    else if ( ucSndAddr > MB_MASTER_TOTAL_SLAVE_NUM ) eErrStatus = MB_MRE_ILL_ARG;
    else
    {
		vMBMasterGetPDUSndBuf(&ucMBFrame);
		vMBMasterSetDestAddress(ucSndAddr);
		ucMBFrame[MB_PDU_FUNC_OFF]                 = MB_FUNC_WRITE_REGISTER;
		ucMBFrame[MB_PDU_FUNC_WRITE_ADDR_OFF]      = usRegAddr >> 8;
		ucMBFrame[MB_PDU_FUNC_WRITE_ADDR_OFF + 1]  = usRegAddr;
		ucMBFrame[MB_PDU_FUNC_WRITE_VALUE_OFF]     = pusDataBuffer[0] >> 8;
		ucMBFrame[MB_PDU_FUNC_WRITE_VALUE_OFF + 1] = pusDataBuffer[0] ;
		vMBMasterSetPDUSndLength( MB_PDU_SIZE_MIN + MB_PDU_FUNC_WRITE_SIZE );
		( void ) xMBMasterPortEventPost( EV_MASTER_FRAME_SENT );
    }
    return eErrStatus;
}
#endif

#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0

eMBMasterReqErrCode
eMBMasterReqWriteMultipleHoldingRegister( UCHAR ucSndAddr,USHORT * pusDataBuffer, USHORT usRegAddr, USHORT usNRegs )
{
    UCHAR                 *ucMBFrame;
    USHORT                 usRegIndex = 0;
    eMBMasterReqErrCode    eErrStatus = MB_MRE_NO_ERR;

    if ( xMBMasterGetIsBusy() ) eErrStatus = MB_MRE_MASTER_BUSY;
    else if ( ucSndAddr > MB_MASTER_TOTAL_SLAVE_NUM ) eErrStatus = MB_MRE_ILL_ARG;
    else
    {
		vMBMasterGetPDUSndBuf(&ucMBFrame);
		vMBMasterSetDestAddress(ucSndAddr);
		ucMBFrame[MB_PDU_FUNC_OFF]                      = MB_FUNC_WRITE_MULTIPLE_REGISTERS;
		ucMBFrame[MB_PDU_FUNC_WRITE_MUL_ADDR_OFF]       = usRegAddr >> 8;
		ucMBFrame[MB_PDU_FUNC_WRITE_MUL_ADDR_OFF + 1]   = usRegAddr;
		ucMBFrame[MB_PDU_FUNC_WRITE_MUL_REGCNT_OFF]     = usNRegs >> 8;
		ucMBFrame[MB_PDU_FUNC_WRITE_MUL_REGCNT_OFF + 1] = usNRegs ;
		ucMBFrame[MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF]    = usNRegs * 2;
		ucMBFrame += MB_PDU_FUNC_WRITE_MUL_VALUES_OFF;
		while( usNRegs > usRegIndex)
		{
			*ucMBFrame++ = pusDataBuffer[usRegIndex] >> 8;
			*ucMBFrame++ = pusDataBuffer[usRegIndex++] ;
		}
		vMBMasterSetPDUSndLength( MB_PDU_SIZE_MIN + MB_PDU_FUNC_WRITE_MUL_SIZE_MIN + 2*usNRegs );
		( void ) xMBMasterPortEventPost( EV_MASTER_FRAME_SENT );
    }
    return eErrStatus;
}
#endif

#if MB_FUNC_READ_HOLDING_ENABLED > 0

eMBMasterReqErrCode
eMBMasterReqReadHoldingRegister( UCHAR ucSndAddr, USHORT usRegAddr, USHORT usNRegs )
{
    UCHAR                 *ucMBFrame;
    eMBMasterReqErrCode    eErrStatus = MB_MRE_NO_ERR;

    if ( xMBMasterGetIsBusy() ) eErrStatus = MB_MRE_MASTER_BUSY;
    else if ( ucSndAddr > MB_MASTER_TOTAL_SLAVE_NUM ) eErrStatus = MB_MRE_ILL_ARG;
    else
    {
		vMBMasterGetPDUSndBuf(&ucMBFrame);
		vMBMasterSetDestAddress(ucSndAddr);
		ucMBFrame[MB_PDU_FUNC_OFF]                 = MB_FUNC_READ_HOLDING_REGISTER;
		ucMBFrame[MB_PDU_FUNC_READ_ADDR_OFF]       = usRegAddr >> 8;
		ucMBFrame[MB_PDU_FUNC_READ_ADDR_OFF + 1]   = usRegAddr;
		ucMBFrame[MB_PDU_FUNC_READ_REGCNT_OFF]     = usNRegs >> 8;
		ucMBFrame[MB_PDU_FUNC_READ_REGCNT_OFF + 1] = usNRegs;
		vMBMasterSetPDUSndLength( MB_PDU_SIZE_MIN + MB_PDU_FUNC_READ_SIZE );
		( void ) xMBMasterPortEventPost( EV_MASTER_FRAME_SENT );
    }
    return eErrStatus;
}

#endif

#if MB_FUNC_READWRITE_HOLDING_ENABLED > 0

eMBMasterReqErrCode
eMBMasterReqReadWriteMultipleHoldingRegister( UCHAR ucSndAddr,USHORT * pusDataBuffer, USHORT usReadRegAddr, USHORT usNReadRegs ,
		USHORT usWriteRegAddr, USHORT usNWriteRegs)
{
    UCHAR                 *ucMBFrame;
    USHORT                 usRegIndex = 0;
    eMBMasterReqErrCode    eErrStatus = MB_MRE_NO_ERR;

    if ( xMBMasterGetIsBusy() ) eErrStatus = MB_MRE_MASTER_BUSY;
    else if ( ucSndAddr > MB_MASTER_TOTAL_SLAVE_NUM ) eErrStatus = MB_MRE_ILL_ARG;
    else
    {
		vMBMasterGetPDUSndBuf(&ucMBFrame);
		vMBMasterSetDestAddress(ucSndAddr);
		ucMBFrame[MB_PDU_FUNC_OFF]                            = MB_FUNC_READWRITE_MULTIPLE_REGISTERS;
		ucMBFrame[MB_PDU_FUNC_READWRITE_READ_ADDR_OFF]        = usReadRegAddr >> 8;
		ucMBFrame[MB_PDU_FUNC_READWRITE_READ_ADDR_OFF + 1]    = usReadRegAddr;
		ucMBFrame[MB_PDU_FUNC_READWRITE_READ_REGCNT_OFF]      = usNReadRegs >> 8;
		ucMBFrame[MB_PDU_FUNC_READWRITE_READ_REGCNT_OFF + 1]  = usNReadRegs ;
		ucMBFrame[MB_PDU_FUNC_READWRITE_WRITE_ADDR_OFF]       = usWriteRegAddr >> 8;
		ucMBFrame[MB_PDU_FUNC_READWRITE_WRITE_ADDR_OFF + 1]   = usWriteRegAddr;
		ucMBFrame[MB_PDU_FUNC_READWRITE_WRITE_REGCNT_OFF]     = usNWriteRegs >> 8;
		ucMBFrame[MB_PDU_FUNC_READWRITE_WRITE_REGCNT_OFF + 1] = usNWriteRegs ;
		ucMBFrame[MB_PDU_FUNC_READWRITE_BYTECNT_OFF]          = usNWriteRegs * 2;
		ucMBFrame += MB_PDU_FUNC_READWRITE_WRITE_VALUES_OFF;
		while( usNWriteRegs > usRegIndex)
		{
			*ucMBFrame++ = pusDataBuffer[usRegIndex] >> 8;
			*ucMBFrame++ = pusDataBuffer[usRegIndex++] ;
		}
		vMBMasterSetPDUSndLength( MB_PDU_SIZE_MIN + MB_PDU_FUNC_READWRITE_SIZE_MIN + 2*usNWriteRegs );
		( void ) xMBMasterPortEventPost( EV_MASTER_FRAME_SENT );
    }
    return eErrStatus;
}

#endif
#endif

