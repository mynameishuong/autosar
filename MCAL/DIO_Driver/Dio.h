/***************************************************************************
 * @file    Dio.h
 * @brief   Khai báo các hàm và cấu trúc liên quan đến diều khiển GPIO
 * @details File này sẽ khởi tạo các hàm có thể sử dụng để cfg các Port
 * @version 1.0
 * @date     18-06-2025
 ***************************************************************************/
#ifndef DIO_H
#define DIO_H

#include "Std_Type.h"
/*--------------------------------------------------
 * Dio_ChannelType Definition
 *--------------------------------------------------*/
typedef uint8 Dio_ChannelType;  // Use uint8 if < 256 channels, else uint16 (48 channels (pins))

typedef uint8 Dio_PortType;  // Được sử dụng để chỉ định cụ thể loại port A,B,C,D

/*--------------------------------------------------
 * Dio_ChannelGroupType Definition
 * @brief
 * @details Type for the definition of a channel group, which consists of several adjoining channels within a port.
 *--------------------------------------------------*/
typedef struct
{
    uint8 mask;         //This element mask which defines the positions of the channel group.
    uint8 offset;       //This element shall be the position of the Channel Group on the port,counted from the LSB.
    Dio_PortType port;
} Dio_ChannelGroupType; //This shall be the port on which the Channel group is defined

/*--------------------------------------------------
 * Dio_LevelType Definition
 * @details Là mức điện áp của GPIO
 *--------------------------------------------------*/
typedef uint8 Dio_LevelType;

/*--------------------------------------------------
 * Dio_PortLevelType Definition
 * @details Sẽ in ra tất các giá trị của 1 groupt A,B,C,D dưới dạng 0 1,và phải kiểu dữ liệu phải cover groupt lớn nhất
 *--------------------------------------------------*/
typedef uint16 Dio_PortLevelType;
/*--------------------------------------------------
 * Giá trị hợp lệ cho Dio_LevelType (Range)
 *--------------------------------------------------*/
#define STD_LOW     0x00U  // Mức điện áp 0V (Logic 0)
#define STD_HIGH    0x01U  // Mức điện áp 5V/3.3V (Logic 1)

/*--------------------------------------------------
 * Mô tả (Description):
 * - Dio_LevelType đại diện cho trạng thái vật lý của chân DIO (input/output).
 * - STD_LOW  = 0V (hoặc GND)
 * - STD_HIGH = 5V/3.3V (tùy MCU)
 *--------------------------------------------------*/

#define PORT_VENDOR_ID    1001u
#define PORT_MODULE_ID    120u
#define PORT_SW_MAJOR_VERSION 1u
#define PORT_SW_MINOR_VERSION 0u
#define PORT_SW_PATCH_VERSION 0u
 /*--------------------------------------------------
 * Function Dio_ReadChannel
 *--------------------------------------------------*/
Dio_LevelType Dio_ReadChannel(Dio_ChannelType ChannelId);
/* Macro 4 port của STM32*/
#define GPIO_PORT_A 0
#define GPIO_PORT_B 1
#define GPIO_PORT_C 2
#define GPIO_PORT_D 3
/*Lấy port của ChanelID*/
#define DIO_GET_PORT_ID(ChannelId) (((ChannelId) < 16) ? GPIOA : \
                                    ((ChannelId) < 32) ? GPIOB : \
                                    ((ChannelId) < 48) ? GPIOC : \
                                    ((ChannelId) < 64) ? GPIOD : \
                                    NULL_PTR)


/*Lấy pin của ChanelID*/
#define DIO_GET_PIN_NUM(ChannelId)  (1 << ((ChannelId) % 16))
 /*--------------------------------------------------
 * Function Dio_WriteChannel
 *--------------------------------------------------*/
void Dio_WriteChannel (Dio_ChannelType ChannelId, Dio_LevelType Level);
 /*--------------------------------------------------
 * Function Dio_FlipChannel
 *--------------------------------------------------*/
Dio_LevelType Dio_FlipChannel (Dio_ChannelType ChannelId);
 /*--------------------------------------------------
 * Function Dio_ReadPort
 *--------------------------------------------------*/
Dio_PortLevelType Dio_ReadPort (Dio_PortType PortId);
 /*--------------------------------------------------
 * Function Dio_WritePort
 *--------------------------------------------------*/
void Dio_WritePort (Dio_PortType PortId,Dio_PortLevelType Level);
 /*--------------------------------------------------
 * Function Dio_ReadChannelGroup
 *--------------------------------------------------*/
Dio_PortLevelType Dio_ReadChannelGroup (const Dio_ChannelGroupType* ChannelGroupIdPtr);
 /*--------------------------------------------------
 * Function Dio_WriteChannelGroup
 *--------------------------------------------------*/
void Dio_WriteChannelGroup (const Dio_ChannelGroupType* ChannelGroupIdPtr,Dio_PortLevelType Level);

 /*--------------------------------------------------
 * Function Dio_WriteChannelGroup
 *--------------------------------------------------*/
void Dio_GetVersionInfo (Std_VersionInfoType* VersionInfo);
 /*--------------------------------------------------
 * Function Dio_MaskedWritePort
 *--------------------------------------------------*/
void Dio_MaskedWritePort (Dio_PortType PortId,Dio_PortLevelType Level,Dio_PortLevelType Mask);
#endif /* DIO_H */