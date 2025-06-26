/***********************************************************
 *  @file    Port_Cfg.h
 *  @brief   Port Driver Configuration Header File
 *  @details File này chứa các định nghĩa cấu hình các chân Port
 *           cho driver Port theo chuẩn AUTOSAR, dùng trên
 *           STM32F103 với thư viện SPL.
 ***********************************************************/

#ifndef PORT_CFG_H
#define PORT_CFG_H

#include "Port.h"  /* Bao gồm các kiểu dữ liệu chuẩn của Port Driver */

/***********************************************************
 * Số lượng chân Port được cấu hình (tùy chỉnh theo dự án)
 ***********************************************************/
#define Pincount     64     // Tổng số chân của STM32 có trong 4 port

/***********************************************************
 * Mảng cấu hình chi tiết cho từng chân GPIO
 * (khai báo extern, định nghĩa cụ thể ở port_cfg.c)
 ***********************************************************/
extern const Port_PinConfigType PortCfg_Pins[Pincount];

#endif /* PORT_CFG_H */
