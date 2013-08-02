/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : otgd_fs_pcd.h
* Author             : MCD Application Team
* Version            : V3.2.1
* Date               : 07/05/2010
* Description        : Header file of the High Layer device mode interface and 
*                      wrapping layer
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#ifndef __USB_OTG_PCD_H__
#define __USB_OTG_PCD_H__

#include "otgd_fs_regs.h"

#define MAX_EP0_SIZE                    0x40
#define MAX_PACKET_SIZE                 0x400


#define USB_ENDPOINT_XFER_CONTROL       0
#define USB_ENDPOINT_XFER_ISOC          1
#define USB_ENDPOINT_XFER_BULK          2
#define USB_ENDPOINT_XFER_INT           3
#define USB_ENDPOINT_XFERTYPE_MASK      3


/********************************************************************************
                              ENUMERATION TYPE
********************************************************************************/
enum usb_device_speed {
  USB_SPEED_UNKNOWN = 0,
  USB_SPEED_LOW, USB_SPEED_FULL,
  USB_SPEED_HIGH
};
/********************************************************************************
                              Data structure type
********************************************************************************/
typedef struct usb_ep_descriptor
{
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bEndpointAddress;
  uint8_t  bmAttributes;
  uint16_t wMaxPacketSize;
  uint8_t  bInterval;
}
EP_DESCRIPTOR , *PEP_DESCRIPTOR;
/********************************************************************************
                     USBF LAYER UNION AND STRUCTURES
********************************************************************************/
typedef struct USB_OTG_USBF
{

  USB_OTG_EP ep0;
  USB_OTG_EP in_ep[ NUM_TX_FIFOS - 1];
  USB_OTG_EP out_ep[ NUM_TX_FIFOS - 1];
}
USB_OTG_PCD_DEV , *USB_OTG_PCD_PDEV;
/********************************************************************************
                     EXPORTED FUNCTION FROM THE USB_OTG LAYER
********************************************************************************/
void        PCD_Init(void);
void        PCD_DevConnect (void);
void        PCD_DevDisconnect (void);
void        PCD_EP_SetAddress (uint8_t address);
uint32_t    PCD_EP_Open(EP_DESCRIPTOR *epdesc);
uint32_t    PCD_EP_Close  ( uint8_t  ep_addr);
uint32_t    PCD_EP_Read  ( uint8_t  ep_addr, 
                           uint8_t  *pbuf, 
                           uint32_t   buf_len);
uint32_t    PCD_EP_Write ( uint8_t  ep_addr, 
                           uint8_t  *pbuf, 
                           uint32_t   buf_len);
uint32_t    PCD_EP_Stall (uint8_t   epnum);
uint32_t    PCD_EP_ClrStall (uint8_t epnum);
uint32_t    PCD_EP_Flush (uint8_t epnum);
uint32_t    PCD_Handle_ISR(void);

USB_OTG_EP* PCD_GetOutEP(uint32_t ep_num) ;
USB_OTG_EP* PCD_GetInEP(uint32_t ep_num);
void        PCD_EP0_OutStart(void);

#endif
/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
