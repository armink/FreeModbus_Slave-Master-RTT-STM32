# FreeModbus V1.6 主机使用说明

----------

## 一、简述

FreeModbus是一款开源的Modbus协议栈，但是只有从机开源，主机源码是需要**收费**的。同时网上也没有发现比较好的开源的Modbus主机协议栈，所以才开发这款支持主机模式的FreeModbus协议栈。本版FreeModbus版本号更改为V1.6，特性如下：

 - 新增加的主机源码与原有从机的风格及接口保持一致；
 - 支持主机与从机在同一协议栈运行；
 - 支持实时操作系统及裸机移植；
 - 为应用提供多种请求模式，用户可以选择阻塞还是非阻塞模式，自定义超时时间等，方便应用层灵活调用；
 - 支持所有常用的Modbus方法。

### 1.1、文件结构

|源文件                                        |描述   |
|:------------------------------               |:----- |
|FreeModbus\modbus\mb.c                        |给应用层提供Modbus从机设置及轮询相关接口|
|FreeModbus\modbus\mb_m.c                      |给应用层提供Modbus主机设置及轮询相关接口|
|FreeModbus\modbus\ascii\mbascii.c             |ASCII模式设置及其状态机|
|FreeModbus\modbus\functions\mbfunccoils.c     |从机线圈相关功能|
|FreeModbus\modbus\functions\mbfunccoils_m.c   |主机线圈相关功能|
|FreeModbus\modbus\functions\mbfuncdisc.c      |从机离散输入相关功能|
|FreeModbus\modbus\functions\mbfuncdisc_m.c    |主机离散输入相关功能|
|FreeModbus\modbus\functions\mbfuncholding.c   |从机保持寄存器相关功能|
|FreeModbus\modbus\functions\mbfuncholding_m.c |主机保持寄存器相关功能|
|FreeModbus\modbus\functions\mbfuncinput.c     |从机输入寄存器相关功能|
|FreeModbus\modbus\functions\mbfuncinput_m.c   |主机输入寄存器相关功能|
|FreeModbus\modbus\functions\mbfuncother.c     |其余Modbus功能|
|FreeModbus\modbus\functions\mbutils.c         |一些协议栈中需要用到的小工具|
|FreeModbus\modbus\rtu\mbcrc.c                 |CRC校验功能|
|FreeModbus\modbus\rtu\mbrtu.c                 |从机RTU模式设置及其状态机|
|FreeModbus\modbus\rtu\mbrtu_m.c               |主机RTU模式设置及其状态机|
|FreeModbus\modbus\tcp\mbtcp.c                 |TCP模式设置及其状态机|
|FreeModbus\port\port.c                        |实现硬件移植部分接口|
|FreeModbus\port\portevent.c                   |实现从机事件移植接口|
|FreeModbus\port\portevent_m.c                 |实现主机事件及错误处理移植接口|
|FreeModbus\port\portserial.c                  |从机串口移植|
|FreeModbus\port\portserial_m.c                |主机串口移植|
|FreeModbus\port\porttimer.c                   |从机定时器移植|
|FreeModbus\port\porttimer_m.c                 |主机定时器移植|
|FreeModbus\port\user_mb_app.c                 |定义从机数据缓冲区，实现从机Modbus功能的回调接口|
|FreeModbus\port\user_mb_app_m.c               |定义主机数据缓冲区，实现主机Modbus功能的回调接口|

> 注：所有带_m后缀的文件为主机模式下必须使用的文件，如使用从机模式则无需这些文件。

----------

## 二、移植

该项目底层代码有Stm32CubeMX自动生成，如果芯片选择STM32也强烈建议您使用Cube生成代码，免去配置接口不必要的麻烦。该项目没有使用任何IDE，Cube生成带Makefile的代码，然后直接编译运行，本人使用的VSC+Cortex Debug插件+make+arm-none-eabi-gcc+jlink的开发方式（**环境搭建请自行搜索**）。 STM32配置详情见demo.ioc，下面介绍的移植过程全部是基于Cube自动生成代码的。

### 2.1、硬件配置

#### 2.1.1、选择串口

打开demo.ioc您会看到配置了两个串口，USART1和USART2，并各配置了两个方向引脚。配置串口时需要配置串口中断，modbus通过中断接收数据。

#### 2.1.2、选择定时器

打开demo.ioc您会看到配置了两个定时器，TIM3和TIM4，该modbusRTU协议栈需要50us的定时中断。

### 2.2、软件配置

修改串口中断函数，文件位于`Core\Src\stm32f4xx_it.c`.

```C
/* USER CODE BEGIN Includes */
#include "mb_m_stack.h"
#include "mb_stack.h"
/* USER CODE END Includes */

/* USER CODE BEGIN EV */
extern MB_M_StackTypeDef mbMasterStack;
extern MB_StackTypeDef mbStack;
/* USER CODE END EV */

```
> 添加协议栈头文件和协议栈句柄。

```C
/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  uint32_t isrflags = READ_REG(huart1.Instance->SR);
  uint32_t cr1its = READ_REG(huart1.Instance->CR1);

  uint32_t errorflags = 0x00U;
  /* If no error occurs */
  errorflags = (isrflags & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));
  if (errorflags == RESET)
  {
    /* UART in mode Receiver -------------------------------------------------*/
    if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
    {
      mbMasterStack.peMBMasterFrameCBByteReceivedCur((void *)&mbMasterStack);
      return;
    }
    if (((isrflags & USART_SR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
    {
      mbMasterStack.peMBMasterFrameCBTransmitterEmptyCur((void *)&mbMasterStack);
      return;
    }
  }
  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

```
> USART1为主机接口。

```C
/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

  uint32_t isrflags = READ_REG(huart2.Instance->SR);
  uint32_t cr1its = READ_REG(huart2.Instance->CR1);

  uint32_t errorflags = 0x00U;
  /* If no error occurs */
  errorflags = (isrflags & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));
  if (errorflags == RESET)
  {
    /* UART in mode Receiver -------------------------------------------------*/
    if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
    {
      mbStack.peMBFrameCBByteReceivedCur((void *)&mbStack);
      return;
    }
    if (((isrflags & USART_SR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
    {
      mbStack.peMBFrameCBTransmitterEmptyCur((void *)&mbStack);
      return;
    }
  }
  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}
```
> USART2为从机接口。

修改定时器中断回调函数，文件位于`Core\Src\main.c`.
```C
/* USER CODE BEGIN Includes */
#include "mb_stack.h"
#include "mb_m_stack.h"
/* USER CODE END Includes */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */
  extern MB_M_StackTypeDef mbMasterStack;
  extern MB_StackTypeDef mbStack;
  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM5) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  if (htim == &htim3)
  {
    mbStack.peMBPortCBTimerExpiredCur(&mbStack);
  }
  if (htim == &htim4)
  {
    mbMasterStack.peMBMasterPortCBTimerExpiredCur(&mbMasterStack);
  }
  /* USER CODE END Callback 1 */
}
```
> TIM3为从机定时器，TIM4为主机定时器。

关联协议栈与底层接口，文件位于 `APP\*.c`.
```C
#include "mb.h"
#include "mb_m.h"
#include "mb_m_stack.h"

MB_M_StackTypeDef mbMasterStack;

void Mb_m_Task(void *argument)
{
    mbMasterStack.hardware.max485.phuart = &huart1;
    mbMasterStack.hardware.max485.dirPin = USART1_DIR_Pin;
    mbMasterStack.hardware.max485.dirPort = USART1_DIR_GPIO_Port;
    mbMasterStack.hardware.phtim = &htim4;
    eMBMasterInit(&mbMasterStack, MB_RTU, 2, 115200, MB_PAR_NONE);
    eMBMasterEnable(&mbMasterStack);
    while (1)
    {
        eMBMasterPoll(&mbMasterStack);
    }
}
```

```C
#include "mb.h"
#include "mb_stack.h"

MB_StackTypeDef mbStack;

void Mb_Task(void *argument)
{
    mbStack.hardware.max485.phuart = &huart2;
    mbStack.hardware.max485.dirPin = USART2_DIR_Pin;
    mbStack.hardware.max485.dirPort = USART2_DIR_GPIO_Port;
    mbStack.hardware.phtim = &htim3;
    eMBInit(&mbStack, MB_RTU, 0x01, 1, 115200, MB_PAR_NONE);
    eMBEnable(&mbStack);
    while (1)
    {
        eMBPoll(&mbStack);
    }
}
```

### 2.3、数据缓冲区

数据缓冲区定义的位置位于 `FreeModbus\port\user_mb_app_m.c` 文件顶部，共计 **4种** 数据类型。
FreeModbus从机默认使用 **一维数组** 作为缓存区数据结构，主机可以存储所有网内从机的数据，所以主机采用 **二维数组** 对所有从机节点数据进行存储。二维数组的列号代表寄存器、线圈及离散量地址，行号代表从机节点ID，但需要做减一处理，例如`usMRegHoldBuf[2][1]`代表从机ID为 3，保持寄存器地址为 1 的从机数据。

### 2.4、Modbus数据处理回调接口

Modbus 一共有4种不同的数据类型，所有的 Modbus 功能都围绕这些数据类型进行操作。由于不同的用户数据缓冲区结构可能有所不同，那么对应的 Modbus 数据处理方式也就存在差异，所以用户需要把每种数据类型对应的操作，按照自己的数据缓冲区结构进行定制实现。
所有的 Modbus 数据处理回调接口如下：

|接口                            |功能描述|
|:-----                          |:----|
|eMBMasterRegInputCB             |输入寄存器回调接口|
|eMBMasterRegHoldingCB           |保持寄存器回调接口|
|eMBMasterRegCoilsCB             |线圈回调接口|
|eMBMasterRegDiscreteCB          |离散输入回调接口|

> 对于数组形式的数据缓冲区结构，源码中已经做好了移植，直接使用即可。也可以使用 [EasyDataManager](https://github.com/armink/EasyDataManager) 库，采用链表作为缓冲区，该库还支持事件驱动，做到数据变化自动通知应用层。

## 三、API

Modbus 主机使用过程中与从机有很大不同，从机是需要被动等待主机请求，而主机则是主动发出请求，并接收处理从机响应。在主机发送广播请求的时候，从机不需要返回响应，所以广播请求适合主机的写从机数据命令，不适合读从机数据命令。
主机请求API中的所有方法的返回值格式都相同，返回值意义如下。

|返回值                               |描述|
|:-----                               |:----|
|MB_MRE_NO_ERR                        |正常，没错误|
|MB_MRE_NO_REG                        |寄存器、线圈或离散输入地址出错|
|MB_MRE_ILL_ARG                       |入参格式有误|
|MB_MRE_REV_DATA                      |接收数据出错|
|MB_MRE_TIMEDOUT                      |响应超时。主机在设定的时间内未收到从机响应。|
|MB_MRE_MASTER_BUSY                   |主机忙。在设定的时间内，没有请求没有被发送。|
|MB_MRE_EXE_FUN                       |主机收到响应后，执行Modbus方法（功能）出错。|

> 所有的主机请求方法都是 **线程安全** 的也是 **阻塞模式** 的。在使用过程中，只要在设定的超时时间内没有得到主机资源，就会返回主机忙；如果在设定的超时时间内得到主机资源，那么必须等待得到请求结果后才会返回。

### 3.1、写单个保持寄存器

往从机某个保持寄存器中写入数据

```C
eMBMasterReqErrCode eMBMasterReqWriteHoldingRegister( UCHAR ucSndAddr, 
                                                      USHORT usRegAddr,
                                                      USHORT usRegData,
                                                      LONG lTimeOut );
```

|参数                                    |描述|
|:-----                                  |:----|
|ucSndAddr                               |请求的从机地址，0代表广播。|
|usRegAddr                               |写寄存器的地址|
|usRegData                               |写寄存器的数据|
|lTimeOut                                |请求超时时间。支持永久等待，使用操作系统的永久等待参数即可。|

### 3.2、写多个保持寄存器

往从机多个保持寄存器中写入数据。

```C
eMBMasterReqErrCode eMBMasterReqWriteMultipleHoldingRegister( UCHAR ucSndAddr,
                                                              USHORT usRegAddr,
                                                              USHORT usNRegs, 
                                                              USHORT * pusDataBuffer,
                                                              LONG lTimeOut )
```

|参数                                    |描述|
|:-----                                  |:----|
|ucSndAddr                               |请求的从机地址，0代表广播。|
|usRegAddr                               |写寄存器的起始地址|
|usNRegs                                 |写寄存器的总数|
|pusDataBuffer                           |写寄存器的数据|
|lTimeOut                                |请求超时时间。支持永久等待，使用操作系统的永久等待参数即可。|

### 3.3、读多个保持寄存器

读取多个保持寄存器中的数据

```C
eMBMasterReqErrCode eMBMasterReqReadHoldingRegister( UCHAR ucSndAddr, 
                                                     USHORT usRegAddr,
                                                     USHORT usNRegs,
                                                     LONG lTimeOut );
```

|参数                                    |描述|
|:-----                                  |:----|
|ucSndAddr                               |请求的从机地址，0代表广播。|
|usRegAddr                               |读寄存器的地址|
|usRegData                               |读寄存器的数量|
|lTimeOut                                |请求超时时间。支持永久等待，使用操作系统的永久等待参数即可。|

### 3.4、读写多个保持寄存器

先读多个寄存器，然后再写多个寄存器。

```C
eMBMasterReqErrCode eMBMasterReqReadWriteMultipleHoldingRegister( UCHAR ucSndAddr,
                                                                  USHORT usReadRegAddr,
                                                                  USHORT usNReadRegs,
                                                                  USHORT * pusDataBuffer,
                                                                  USHORT usWriteRegAddr,
                                                                  USHORT usNWriteRegs,
                                                                  LONG lTimeOut )
```

|参数                                    |描述|
|:-----                                  |:----|
|ucSndAddr                               |请求的从机地址，0代表广播。|
|usReadRegAddr                           |读寄存器的地址|
|usNReadRegs                             |读寄存器的数量|
|pusDataBuffer                           |写寄存器的数据|
|usWriteRegAddr                          |写寄存器的地址|
|usNWriteRegs                            |写寄存器的数量|
|lTimeOut                                |请求超时时间。支持永久等待，使用操作系统的永久等待参数即可。|

### 3.5、读多个输入寄存器

读取多个输入寄存器中的数据

```C
eMBMasterReqErrCode eMBMasterReqReadInputRegister( UCHAR ucSndAddr, 
                                                   USHORT usRegAddr,
                                                   USHORT usNRegs,
                                                   LONG lTimeOut );
```

|参数                                    |描述|
|:-----                                  |:----|
|ucSndAddr                               |请求的从机地址，0代表广播。|
|usRegAddr                               |读寄存器的地址|
|usRegData                               |读寄存器的数量|
|lTimeOut                                |请求超时时间。支持永久等待，使用操作系统的永久等待参数即可。|

### 3.6、写单个线圈

往从机某个线圈中写入数据

```C
eMBMasterReqErrCode eMBMasterReqWriteCoil( UCHAR ucSndAddr,
                                           USHORT usCoilAddr,
                                           USHORT usCoilData,
                                           LONG lTimeOut )
```

|参数                                    |描述|
|:-----                                  |:----|
|ucSndAddr                               |请求的从机地址，0代表广播。|
|usCoilAddr                              |写线圈的地址|
|usCoilData                              |写线圈的数量|
|lTimeOut                                |请求超时时间。支持永久等待，使用操作系统的永久等待参数即可。|

### 3.7、写多个线圈

往从机多个线圈中写入数据。

```C
eMBMasterReqErrCode eMBMasterReqWriteMultipleCoils( UCHAR ucSndAddr,
                                                    USHORT usCoilAddr,
                                                    USHORT usNCoils,
                                                    UCHAR * pucDataBuffer,
                                                    LONG lTimeOut)
```

|参数                                    |描述|
|:-----                                  |:----|
|ucSndAddr                               |请求的从机地址，0代表广播。|
|usCoilAddr                              |写线圈的起始地址|
|usNCoils                                |写线圈的总数|
|pucDataBuffer                           |写线圈的数据|
|lTimeOut                                |请求超时时间。支持永久等待，使用操作系统的永久等待参数即可。|

### 3.8、读多个线圈

读取多个线圈中的数据

```C
eMBMasterReqErrCode eMBMasterReqReadCoils( UCHAR ucSndAddr,
                                           USHORT usCoilAddr,
                                           USHORT usNCoils ,
                                           LONG lTimeOut )
```

|参数                                    |描述|
|:-----                                  |:----|
|ucSndAddr                               |请求的从机地址，0代表广播。|
|usCoilAddr                              |读线圈的地址|
|usNCoils                                |读线圈的数量|
|lTimeOut                                |请求超时时间。支持永久等待，使用操作系统的永久等待参数即可。|

### 3.9、读多个离散输入

读取多个离散输入中的数据

```C
eMBMasterReqErrCode eMBMasterReqReadDiscreteInputs( UCHAR ucSndAddr,
                                                    USHORT usDiscreteAddr,
                                                    USHORT usNDiscreteIn,
                                                    LONG lTimeOut )
```

|参数                                    |描述|
|:-----                                  |:----|
|ucSndAddr                               |请求的从机地址，0代表广播。|
|usDiscreteAddr                          |读离散输入的地址|
|usNDiscreteIn                           |读离散输入的数量|
|lTimeOut                                |请求超时时间。支持永久等待，使用操作系统的永久等待参数即可。|

## 四、流程

### 4.1、初始化配置流程

本协议栈所有配置参数都位于`FreeModbus\modbus\include\mbconfig.h`，目前协议栈支持主机及从机两种模式，并且支持**两种模式同时开启**。从机支持Modbus RTU 、Modbus ASCII 及Modbus TCP 3种模式，主机现在只支持常用的**Modbus RTU**模式。在使用主机的过程中，用户需要对广播的转换延时时间、命令响应超时时间及从机数量做以配置。需要注意的是，目前协议栈只支持**从机地址连续**，并且起始地址**从1开始**。

### 4.2、正常使用流程

这里只介绍主机的正常使用流程，在使用主机前，需要先把协议栈移植到自己的项目中去，包括上述的软件及硬件部分，移植完成后的使用流程如下

- 1、调用`eMBMasterInit`方法初始化Modbus主机协议栈，主机涉及到的一些硬件就在这个时候做了初始化
- 2、调用`eMBMasterEnable`方法启动Modbus主机
- 3、通过在线程或者定时器轮询调用`eMBMasterPoll`方法，轮询周期决定了命令的响应时间。
- 4、调用主机请求API方法，设定一定的请求超时时间，直到方法有结果后才会返回。如果方法执行成功并且命令是读命令，可以通过查看Modbus主机的数据缓冲区，获取最新从机数据。

### 4.3、异常处理流程

异常处理主要出现在主机正常使用过程中，所有的主机请求API的错误码都在第三章开头已经做以描述，针对的这些错误码，用户需要根据自己的产品特征去完成不同的动作。建议用户自己封装实现主机请求方法的重发机制，这样实现方式比较灵活，一般是在接收到帧数据出错及命令响应超时的错误码时需要重发，重发次数自动加一，如果重发次数超过设定值则认为从机掉线，以后所有只要是发给这个从机命令都被提前拦截掉；如果第二次重发命令响应成功，则**自动清零**该从机重发次数。
上述所有功能可以利用主机请求方法或者使用`FreeModbus\port\portevent_m.c`中的回调接口来实现，用户可以根据自己的需求灵活选择。

## 五、许可

基于 BSD 协议，详细内容请查看 LICENSE 文件。

[1]: http://www.rt-thread.org/
