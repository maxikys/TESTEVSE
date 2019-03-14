/*****************************************************************************
*
* Author       : Õîìåíêî Ñ.Ï.
* Created      : 12.03.2015
* $Rev:: 101    $:  Revision of last commit
* $Date:: 2015-03-24 13:22:09#$:  Date of last commit
* Target MCU   : STM32F042C6     
* Compiler     : Keil 5.14
* Editor Tabs  : 2
*
* NOTE: 
******************************************************************************/ 

/* Includes ------------------------------------------------------------------*/

#include "usbd_desc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USBD_VID                     0xFFFE
#define USBD_PID                     0x0100
//#define USBD_VID                     0x0483
//#define USBD_PID                     0x5710

#define USBD_LANGID_STRING            0x409
#define USBD_MANUFACTURER_STRING      "NPP «Contactmodule» Ltd."
//#define USBD_MANUFACTURER_STRING      "STMicroelectronics"

#define USBD_PRODUCT_FS_STRING        "USB Keyboard"
//#define USBD_PRODUCT_FS_STRING        "Joystick in FS Mode"

#define USBD_CONFIGURATION_FS_STRING  "HID Config"
#define USBD_INTERFACE_FS_STRING      "HID Interface"

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
char          USBD_SERIALNUMBER_FS_STRING[26];

USBD_DEVICE   USR_desc                    =
                                          {
  USBD_USR_DeviceDescriptor,
                                            USBD_USR_LangIDStrDescriptor,
                                            USBD_USR_ManufacturerStrDescriptor,
                                            USBD_USR_ProductStrDescriptor,
                                            USBD_USR_SerialStrDescriptor,
                                            USBD_USR_ConfigStrDescriptor,
                                            USBD_USR_InterfaceStrDescriptor,

                                          };
// Chicony
// äåñêðåïòîð óñòðîéñòâà
//12 01 00 02 00 00 00 08 F2 04 33 08 30 02 01 02 00 01

/* USB Standard Device Descriptor */
const uint8_t USBD_DeviceDesc[USB_SIZ_DEVICE_DESC]             =
            {
              0x12,                                               /*bLength */
              USB_DEVICE_DESCRIPTOR_TYPE,                         /*bDescriptorType*/
              0x00,                                               /*bcdUSB */
              0x02,
              0x00,                                               /*bDeviceClass*/
              0x00,                                               /*bDeviceSubClass*/
              0x00,                                               /*bDeviceProtocol*/
              USB_MAX_EP0_SIZE,                                   /*bMaxPacketSize*/
              LOBYTE( USBD_VID ),                                 /*idVendor*/
              HIBYTE( USBD_VID ),                                 /*idVendor*/
              LOBYTE( USBD_PID ),                                 /*idVendor*/
              HIBYTE( USBD_PID ),                                 /*idVendor*/
              0x00,                                               /*bcdDevice rel. 2.00*/
              0x01,
              USBD_IDX_MFC_STR,                                   /*Index of manufacturer  string*/
              USBD_IDX_PRODUCT_STR,                               /*Index of product string*/
              0/*USBD_IDX_SERIAL_STR*/,                           /*Index of serial number string*/
              USBD_CFG_MAX_NUM                                    /*bNumConfigurations*/
            };                                                    /* USB_DeviceDescriptor */

/* USB Standard Device Descriptor */
const uint8_t USBD_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC]    =
   {
  USB_LEN_DEV_QUALIFIER_DESC,
     USB_DESC_TYPE_DEVICE_QUALIFIER,
     0x00,
     0x02,
     0x00,
     0x00,
     0x00,
     0x40,
     0x01,
     0x00,
   };

/* USB Standard Device Descriptor */
const uint8_t USBD_LangIDDesc[USB_SIZ_STRING_LANGID]             =
            {
  USB_SIZ_STRING_LANGID,
              USB_DESC_TYPE_STRING,
              LOBYTE( USBD_LANGID_STRING ),
              HIBYTE( USBD_LANGID_STRING ),
            };
uint8_t       USBD_StringSerial[USB_SIZ_STRING_SERIAL]           =
{
  USB_SIZ_STRING_SERIAL,                              /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                         /* bDescriptorType */
};

/* Private function prototypes -----------------------------------------------*/
static void IntToUnicode( uint32_t value, uint8_t *pbuf, uint8_t len );

/* Private functions ---------------------------------------------------------*/
/**
  * @brief return the device descriptor
  * @param  speed : current device speed
  * @param  length : pointer to data length variable
  * @retval pointer to descriptor buffer
  */
uint8_t*  USBD_USR_DeviceDescriptor( uint8_t speed, uint16_t *length )
{
  *length = sizeof(USBD_DeviceDesc);
  return ( uint8_t * )USBD_DeviceDesc;
}

/**
  * @brief  return the LangID string descriptor
  * @param  speed : current device speed
  * @param  length : pointer to data length variable
  * @retval pointer to descriptor buffer
  */
uint8_t*  USBD_USR_LangIDStrDescriptor( uint8_t speed, uint16_t *length )
{
  *length =  sizeof(USBD_LangIDDesc);
  return ( uint8_t * )USBD_LangIDDesc;
}


/**
  * @brief  return the product string descriptor
  * @param  speed : current device speed
  * @param  length : pointer to data length variable
  * @retval pointer to descriptor buffer
  */
uint8_t*  USBD_USR_ProductStrDescriptor( uint8_t speed, uint16_t *length )
{
  USBD_GetString( ( uint8_t * )USBD_PRODUCT_FS_STRING, USBD_StrDesc, length );
  return USBD_StrDesc;
}

/**
  * @brief  return the manufacturer string descriptor
  * @param  speed : current device speed
  * @param  length : pointer to data length variable
  * @retval pointer to descriptor buffer
  */
uint8_t*  USBD_USR_ManufacturerStrDescriptor( uint8_t speed, uint16_t *length )
{
  USBD_GetString( ( uint8_t * )USBD_MANUFACTURER_STRING, USBD_StrDesc, length );
  return USBD_StrDesc;
}

/**
  * @brief  return the serial number string descriptor
  * @param  speed : current device speed
  * @param  length : pointer to data length variable
  * @retval pointer to descriptor buffer
  */
uint8_t*  USBD_USR_SerialStrDescriptor( uint8_t speed, uint16_t *length )
{
  *length = USB_SIZ_STRING_SERIAL;
  return USBD_StringSerial;
}

/**
  * @brief return the configuration string descriptor
  * @param  speed : current device speed
  * @param  length : pointer to data length variable
  * @retval pointer to descriptor buffer
  */
uint8_t*  USBD_USR_ConfigStrDescriptor( uint8_t speed, uint16_t *length )
{
  USBD_GetString( ( uint8_t * )USBD_CONFIGURATION_FS_STRING, USBD_StrDesc, length );
  return USBD_StrDesc;
}

/**
  * @brief  return the interface string descriptor
  * @param  speed : current device speed
  * @param  length : pointer to data length variable
  * @retval pointer to descriptor buffer
  */
uint8_t*  USBD_USR_InterfaceStrDescriptor( uint8_t speed, uint16_t *length )
{
  USBD_GetString( ( uint8_t * )USBD_INTERFACE_FS_STRING, USBD_StrDesc, length );
  return USBD_StrDesc;
}

/**
  * @brief  Create the serial number string descriptor 
  * @param  None 
  * @retval None
  */
void Get_SerialNum( void )
{
  uint32_t Device_Serial0,
           Device_Serial1,
           Device_Serial2;

  Device_Serial0 = *( uint32_t * )Device1_Identifier;
  Device_Serial1 = *( uint32_t * )Device2_Identifier;
  Device_Serial2 = *( uint32_t * )Device3_Identifier;

  Device_Serial0 += Device_Serial2;

  if( Device_Serial0 != 0 )
  {
    IntToUnicode( Device_Serial0, &USBD_StringSerial[ 2 ], 8 );
    IntToUnicode( Device_Serial1, &USBD_StringSerial[ 18 ], 4 );
  }
}

/**
  * @brief  Convert Hex 32Bits value into char 
  * @param  value: value to convert
  * @param  pbuf: pointer to the buffer 
  * @param  len: buffer length
  * @retval None
  */
static void IntToUnicode( uint32_t value, uint8_t *pbuf, uint8_t len )
{
  uint8_t idx = 0;

  for( idx = 0; idx < len; idx++ )
  {
    if( ((value >> 28)) < 0xA )
    {
      pbuf[ 2 * idx ] = (value >> 28) + '0';
    }
    else
    {
      pbuf[ 2 * idx ] = (value >> 28) + 'A' - 10;
    }

    value = value << 4;

    pbuf[ 2 * idx + 1 ] = 0;
  }
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
