FreeModbus_Slave&Master + RT-Thread1.1.1 + STM32F103
----------
# 一、	背景描述 #
目前操作系统在嵌入式软件行业非常流行，在工控组网方面，可能以后会经常会用到Modbus主机+操作系统、Modbus从机+操作系统甚至Modbus主机+Modbus从机+操作系统。但是操作系统缺乏对Modbus的支持；同时市面上也有很好的开源Modbus协议栈，例如FreeModbus，但是缺点就是该协议栈只支持从机，网上缺乏它和操作系统结合的案例。

操作系统方面，出于对国人的支持，公司主导使用RT-Thread这个开源的国人操作系统，并且公司有其许可证，其的稳定性已经得到充分验证。

目前还有一个难题就是关于FreeModbus主机的支持，笔者初步打算修改FreeModbus源码，增加Modbus主机功能，这个工作放在第二步。操作系统和协议栈都有了，接下来的工作就容易了。

该项目软件将采用Keil+IAR+Eclipse三大IDE同步开发，Keil方便调试，IAR作为Eclipse编译STM32的工具，Eclipse作为最主要的编码平台，方便灵活快捷。更重要的是Eclipse支持强大的版本管理插件，在该项目中将采用GIT作为默认的版本管理软件。
(备注：如果您需要把这个项目商用，请及时联系RT-Thread给您授权)
# 二、	项目目标 #
硬件平台选择STM32F103RET6，软件平台：Keil+IAR+Eclipse+GIT。目标共有4个阶段，由于笔者在公司工作繁忙，没法把全部精力放在上面。初步估计一个月时间完成

    1、	RT-Thread+STM32                                 2013.08.01--2013.08.02
    2、	FreeModbus+RT-Thread+STM32                      2013.08.02--2013.08.05
    3、	FreeModbusMaster+RT-Thread+STM32                2013.08.06--2013.08.20   （备注：FreeModbusMaster为自己修改FreeModbus后增加主机功能的协议栈）
    4、	FreeModbusMaster+ FreeModbus +RT-Thread+STM32   2013.08.20--2013.08.30
# 三、	项目准备 #
软件平台：

    序号  	名称	     版本	     主要功能	                          备注
    1	     Keil MDK	4.12	  调试	
    2	     IAR EWARM	6.50	  给Eclipse提供开发STM32插件	
    3	     Eclipse	Indigo	  编码、版本管理	                    安装CDT、EGIT、IAR插件
硬件平台：
准备一块具有两个串口的STM32F103系列开发板即可，RAM及Flash至少20K以上


