/*
 * FreeModbus Libary: user callback functions and buffer define in master mode
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
 * File: $Id: user_mb_app_m.c,v 1.60 2013/11/23 11:49:05 Armink $
 */
#include "user_mb_app.h"

/*-----------------------Master mode use these variables----------------------*/
#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
//Master mode:DiscreteInputs variables
USHORT   usMDiscInStart                             = M_DISCRETE_INPUT_START;
#if      M_DISCRETE_INPUT_NDISCRETES%8
UCHAR    ucMDiscInBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_DISCRETE_INPUT_NDISCRETES/8+1];
#else
UCHAR    ucMDiscInBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_DISCRETE_INPUT_NDISCRETES/8];
#endif
//Master mode:Coils variables
USHORT   usMCoilStart                               = M_COIL_START;
#if      M_COIL_NCOILS%8
UCHAR    ucMCoilBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_COIL_NCOILS/8+1];
#else
UCHAR    ucMCoilBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_COIL_NCOILS/8];
#endif
//Master mode:InputRegister variables
USHORT   usMRegInStart                              = M_REG_INPUT_START;
USHORT   usMRegInBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_INPUT_NREGS];
//Master mode:HoldingRegister variables
USHORT   usMRegHoldStart                            = M_REG_HOLDING_START;
USHORT   usMRegHoldBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_HOLDING_NREGS];

//******************************输入寄存器回调函数**********************************
//函数定义: eMBErrorCode eMBMasterRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
//描    述：输入寄存器相关的功能（读、连续读）
//入口参数：pucRegBuffer : 回调函数将Modbus寄存器的当前值写入的缓冲区
//			usAddress    : 寄存器的起始地址，输入寄存器的地址范围是1-65535。
//			usNRegs      : 寄存器数量
//出口参数：eMBErrorCode : 这个函数将返回的错误码
//备    注：Editor：Armink 2013-11-25    Company: BXXJS
//**********************************************************************************
eMBErrorCode
eMBMasterRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
    USHORT *        pusRegInputBuf;
    UCHAR           REG_INPUT_START;
    UCHAR           REG_INPUT_NREGS;
    UCHAR           usRegInStart;

	pusRegInputBuf = usMRegInBuf[ucMBMasterGetDestAddress()];
	REG_INPUT_START = M_REG_INPUT_START;
	REG_INPUT_NREGS = M_REG_INPUT_NREGS;
	usRegInStart = usMRegInStart;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInStart );
        while( usNRegs > 0 )
        {
			pusRegInputBuf[iRegIndex] = *pucRegBuffer++ << 8;
			pusRegInputBuf[iRegIndex] |= *pucRegBuffer++;
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}
//******************************保持寄存器回调函数**********************************
//函数定义: eMBErrorCode eMBMasterRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
//描    述：保持寄存器相关的功能（读、连续读、写、连续写）
//入口参数：pucRegBuffer : 如果需要更新用户寄存器数值，这个缓冲区必须指向新的寄存器数值。
//                         如果协议栈想知道当前的数值，回调函数必须将当前值写入这个缓冲区
//			usAddress    : 寄存器的起始地址。
//			usNRegs      : 寄存器数量
//          eMode        : 如果该参数为eMBRegisterMode::MB_REG_WRITE，用户的应用数值将从pucRegBuffer中得到更新。
//                         如果该参数为eMBRegisterMode::MB_REG_READ，用户需要将当前的应用数据存储在pucRegBuffer中
//出口参数：eMBErrorCode : 这个函数将返回的错误码
//备    注：Editor：Armink 2013-11-25    Company: BXXJS
//**********************************************************************************
eMBErrorCode
eMBMasterRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
    USHORT *        pusRegHoldingBuf;
    UCHAR           REG_HOLDING_START;
    UCHAR           REG_HOLDING_NREGS;
    UCHAR           usRegHoldStart;

	pusRegHoldingBuf = usMRegHoldBuf[ucMBMasterGetDestAddress()];
	REG_HOLDING_START = M_REG_HOLDING_START;
	REG_HOLDING_NREGS = M_REG_HOLDING_NREGS;
	usRegHoldStart = usMRegHoldStart;
	//If mode is read,the master will wirte the received date to bufffer.
	eMode = MB_REG_WRITE;

    if( ( usAddress >= REG_HOLDING_START ) &&
        ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegHoldStart );
        switch ( eMode )
        {
            /* Pass current register values to the protocol stack. */
        case MB_REG_READ:
            while( usNRegs > 0 )
            {
				*pucRegBuffer++ = ( unsigned char )( pusRegHoldingBuf[iRegIndex] >> 8 );
				*pucRegBuffer++ = ( unsigned char )( pusRegHoldingBuf[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;

            /* Update current register values with new values from the
             * protocol stack. */
        case MB_REG_WRITE:
            while( usNRegs > 0 )
            {
                pusRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                pusRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
            }
            break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}
//****************************线圈状态寄存器回调函数********************************
//函数定义: eMBErrorCode eMBMasterRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
//描    述：线圈状态寄存器相关的功能（读、连续读、写、连续写）
//入口参数：pucRegBuffer : 位组成一个字节，起始寄存器对应的位处于该字节pucRegBuffer的最低位LSB。
//                         如果回调函数要写这个缓冲区，没有用到的线圈（例如不是8个一组的线圈状态）对应的位的数值必须设置位0。
//			usAddress    : 第一个线圈地址。
//			usNCoils     : 请求的线圈个数
//          eMode        ；如果该参数为eMBRegisterMode::MB_REG_WRITE，用户的应用数值将从pucRegBuffer中得到更新。
//                         如果该参数为eMBRegisterMode::MB_REG_READ，用户需要将当前的应用数据存储在pucRegBuffer中
//出口参数：eMBErrorCode : 这个函数将返回的错误码
//备    注：Editor：Armink 2013-11-25    Company: BXXJS
//**********************************************************************************
eMBErrorCode
eMBMasterRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex , iRegBitIndex , iNReg;
	UCHAR *         pucCoilBuf;
    UCHAR           COIL_START;
    UCHAR           COIL_NCOILS;
    UCHAR           usCoilStart;
    iNReg =  usNCoils / 8 + 1;        //占用寄存器数量

	pucCoilBuf = ucMCoilBuf[ucMBMasterGetDestAddress()];
	COIL_START = M_COIL_START;
	COIL_NCOILS = M_COIL_NCOILS;
	usCoilStart = usMCoilStart;
	//If mode is read,the master will wirte the received date to bufffer.
	eMode = MB_REG_WRITE;

	if( ( usAddress >= COIL_START ) &&
        ( usAddress + usNCoils <= COIL_START + COIL_NCOILS ) )
    {
		usAddress-=usCoilStart;  //计算绝对地址
        iRegIndex    = ( int )( usAddress - usCoilStart ) / 8 ;    //每个寄存器存8个
		iRegBitIndex = ( int )( usAddress - usCoilStart ) % 8 ;	   //相对于寄存器内部的位地址
        switch ( eMode )
        {
            /* Pass current coil values to the protocol stack. */
        case MB_REG_READ:
            while( iNReg > 0 )
            {
				*pucRegBuffer++ = xMBUtilGetBits(&pucCoilBuf[iRegIndex++] , iRegBitIndex , 8);
                iNReg --;
            }
			pucRegBuffer --;
			usNCoils = usNCoils % 8;                        //余下的线圈数	
			*pucRegBuffer = *pucRegBuffer <<(8 - usNCoils); //高位补零
			*pucRegBuffer = *pucRegBuffer >>(8 - usNCoils);
            break;

            /* Update current coil values with new values from the
             * protocol stack. */
        case MB_REG_WRITE:
            while(iNReg > 1)									 //最后面余下来的数单独算
            {
				xMBUtilSetBits(&pucCoilBuf[iRegIndex++] , iRegBitIndex  , 8 , *pucRegBuffer++);
                iNReg--;
            }
			usNCoils = usNCoils % 8;                            //余下的线圈数
			if (usNCoils != 0)                                  //xMBUtilSetBits方法 在操作位数量为0时存在bug
			{
				xMBUtilSetBits(&pucCoilBuf[iRegIndex++], iRegBitIndex, usNCoils,
						*pucRegBuffer++);
			}
			break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}
//****************************离散输入寄存器回调函数********************************
//函数定义: eMBErrorCode eMBMasterRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
//描    述：离散输入寄存器相关的功能（读、连续读）
//入口参数：pucRegBuffer : 用当前的线圈数据更新这个寄存器，起始寄存器对应的位处于该字节pucRegBuffer的最低位LSB。
//                         如果回调函数要写这个缓冲区，没有用到的线圈（例如不是8个一组的线圈状态）对应的位的数值必须设置为0。
//			usAddress    : 离散输入的起始地址
//			usNDiscrete  : 离散输入点数量
//出口参数：eMBErrorCode : 这个函数将返回的错误码
//备    注：Editor：Armink 2013-11-25    Company: BXXJS
//**********************************************************************************
eMBErrorCode
eMBMasterRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    eMBErrorCode    eStatus = MB_ENOERR;
	int             iRegIndex , iRegBitIndex , iNReg;
	UCHAR *         pucDiscreteInputBuf;
    UCHAR           DISCRETE_INPUT_START;
    UCHAR           DISCRETE_INPUT_NDISCRETES;
    UCHAR           usDiscreteInputStart;
	iNReg =  usNDiscrete / 8 + 1;        //占用寄存器数量

	pucDiscreteInputBuf = ucMDiscInBuf[ucMBMasterGetDestAddress()];
	DISCRETE_INPUT_START = M_DISCRETE_INPUT_START;
	DISCRETE_INPUT_NDISCRETES = M_DISCRETE_INPUT_NDISCRETES;
	usDiscreteInputStart = usMDiscInStart;

    if( ( usAddress >= DISCRETE_INPUT_START )
        && ( usAddress + usNDiscrete <= DISCRETE_INPUT_START + DISCRETE_INPUT_NDISCRETES ) )
    {
    	usAddress-=usDiscreteInputStart;  //计算绝对地址
        iRegIndex    = ( int )( usAddress - usDiscreteInputStart ) / 8 ;    //每个寄存器存8个
		iRegBitIndex = ( int )( usAddress - usDiscreteInputStart ) % 8 ;	   //相对于寄存器内部的位地址

		/* Update current coil values with new values from the
		 * protocol stack. */
		while (iNReg > 1) //最后面余下来的数单独算
		{
			xMBUtilSetBits(&pucDiscreteInputBuf[iRegIndex++], iRegBitIndex, 8,
					*pucRegBuffer++);
			iNReg--;
		}
		usNDiscrete = usNDiscrete % 8; //余下的线圈数
		if (usNDiscrete != 0) //xMBUtilSetBits方法 在操作位数量为0时存在bug
		{
			xMBUtilSetBits(&pucDiscreteInputBuf[iRegIndex++], iRegBitIndex,
					usNDiscrete, *pucRegBuffer++);
		}
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}
#endif
