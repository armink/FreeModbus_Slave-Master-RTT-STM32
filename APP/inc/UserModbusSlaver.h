#ifndef	USER_MODBUS_SLAVER_H
#define USER_MODBUS_SLAVER_H
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
/* ----------------------- Defines ------------------------------------------*/
#define DISCRETE_INPUT_START        1
#define DISCRETE_INPUT_NDISCRETES   16
#define COIL_START                  1
#define COIL_NCOILS                 64
#define REG_INPUT_START             1
#define REG_INPUT_NREGS             100
#define REG_HOLDING_START           1
#define REG_HOLDING_NREGS           100

//===========================在保持寄存器中，各个地址对应的功能定义========================
#define          HD_RESERVE                     0		  //保留
#define          HD_CPU_USAGE_MAJOR             1         //当前CPU利用率的整数位
#define          HD_CPU_USAGE_MINOR             2         //当前CPU利用率的小数位

//===========================在输入寄存器中，各个地址对应的功能定义========================
#define          IN_RESERVE                     0		  //保留

//===========================在线圈中，各个地址对应的功能定义============================
#define          CO_RESERVE                     2		  //保留

//===========================在离散输入中，各个地址对应的功能定义=========================
#define          DI_RESERVE                     1		  //保留

/*--------------------------Extern Functions------------------------------------*/
extern UCHAR xMBUtilGetBits( UCHAR * ucByteBuf, USHORT usBitOffset, UCHAR ucNBits );
extern void  xMBUtilSetBits( UCHAR * ucByteBuf, USHORT usBitOffset, UCHAR ucNBits,UCHAR ucValue );

#endif
