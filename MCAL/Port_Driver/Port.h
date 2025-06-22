/***************************************************************************
 * @file    Port.h
 * @brief   Giao diện cấu hình và điều khiển GPIO theo chuẩn AUTOSAR cho STM32F1
 * @details Cung cấp các định nghĩa, kiểu dữ liệu và API để cấu hình các chân I/O
 *          bao gồm chế độ hoạt động, hướng dữ liệu, tốc độ, pull-up/down,...
 * @version 1.0
 * @date    18-06-2025
 ***************************************************************************/

#ifndef PORT_H
#define PORT_H

#include "Std_Type.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"

/// @name GPIO Pull configuration
/// @{
#define PULL_UP     0x1u    ///< Kéo điện trở lên Vcc (GPIO_Mode_IPU)
#define PULL_DOWN   0x0u    ///< Kéo điện trở xuống GND (GPIO_Mode_IPD hoặc Open Drain)
/// @}

/// @name GPIO Output Level
/// @{
#define PORT_PIN_LEVEL_HIGH     0x1u    ///< Mức logic cao (3.3V)
#define PORT_PIN_LEVEL_LOW      0x0u    ///< Mức logic thấp (0V)
/// @}

/// @brief Kiểu dữ liệu đại diện cho mã số chân (0–15)
typedef uint16 Port_PinType;

/// @brief Các chế độ hoạt động của chân GPIO
typedef enum {
    PORT_PIN_MODE_DIO = 0x00,   ///< Chế độ Digital Input/Output
    PORT_PIN_MODE_ADC = 0x01,   ///< Chế độ Analog (ví dụ dùng cho ADC)
    PORT_PIN_MODE_PWM = 0x02    ///< Chế độ PWM (Alternate Function)
} Port_PinModeType;

/// @brief Hướng dữ liệu của chân GPIO
typedef enum {
    PORT_PIN_IN  = 0x01,    ///< Hướng Input
    PORT_PIN_OUT = 0x00     ///< Hướng Output
} Port_PinDirectionType;

/// @brief Cấu hình cho một chân GPIO
typedef struct
{
    uint8 PortID;                           ///< ID của Port: A = 0, B = 1,...
    Port_PinType PinID;                     ///< Số chân trong Port: 0–15
    Port_PinModeType PinMode;              ///< Chế độ hoạt động: DIO/ADC/PWM
    Port_PinDirectionType Direction;       ///< Hướng chân: Input/Output
    uint8 Speed;                            ///< Tốc độ: GPIO_Speed_10MHz, 2MHz, 50MHz
    uint8 Pull;                             ///< Kiểu kéo: PULL_UP hoặc PULL_DOWN
    uint8 Level;                            ///< Mức logic mặc định nếu là Output
    uint8 DirectionChangeable;             ///< Cho phép thay đổi hướng trong runtime
    uint8 ModeChangeable;                  ///< Cho phép thay đổi chế độ trong runtime
} Port_PinConfigType;

/// @brief Cấu trúc cấu hình tổng cho nhiều chân GPIO
typedef struct
{
    const Port_PinConfigType *PinCfgType;   ///< Mảng chứa cấu hình cho từng chân
    uint16 PortCfg_PinsCount;              ///< Tổng số chân được cấu hình
} Port_ConfigType;

/// @name Định danh các Port
/// @{
#define PORT_ID_A  0   ///< GPIOA
#define PORT_ID_B  1   ///< GPIOB
#define PORT_ID_C  2   ///< GPIOC
#define PORT_ID_D  3   ///< GPIOD
/// @}

/// @name Định nghĩa là Driver version
#define PORT_VENDOR_ID    1001u
#define PORT_MODULE_ID    120u
#define PORT_SW_MAJOR_VERSION 1u
#define PORT_SW_MINOR_VERSION 0u
#define PORT_SW_PATCH_VERSION 0u


/// @brief Macro lấy con trỏ GPIOx tương ứng từ PortID
#define PORT_GET_ID(PortID)        (((PortID) == PORT_ID_A) ? GPIOA : \
                                    ((PortID) == PORT_ID_B) ? GPIOB : \
                                    ((PortID) == PORT_ID_C) ? GPIOC : \
                                    ((PortID) == PORT_ID_D) ? GPIOD : \
                                    NULL_PTR)

/// @brief Macro lấy giá trị bit tương ứng với chân GPIO (ví dụ 1 << 8 với chân số 8)
#define PORT_GET_PIN_NUM(Pin)      (1 << ((Pin) % 16))

//==============================================================================
//                              API FUNCTIONS
//==============================================================================

/**
 * @brief Khởi tạo một chân GPIO theo thông số cấu hình
 * @param Portconf Con trỏ đến cấu trúc cấu hình pin
 */
void Port_Deploy_pin(const Port_PinConfigType *Portconf);

/**
 * @brief Khởi tạo tất cả các chân theo cấu hình toàn cục
 * @param ConfigPtr Con trỏ đến cấu trúc tổng chứa toàn bộ cấu hình chân
 */
void Port_Init(const Port_ConfigType* ConfigPtr);

/**
 * @brief Thay đổi hướng chân (input/output) tại runtime nếu được phép
 * @param Pin Số hiệu toàn cục của chân (theo mảng cấu hình)
 * @param Direction Hướng mới (PORT_PIN_IN hoặc PORT_PIN_OUT)
 */
void Port_SetPinDirection (Port_PinType Pin, Port_PinDirectionType Direction);


/**
 * @brief Làm mới lại hướng của tất cả các chân GPIO
 *
 * @details Hàm này sẽ thiết lập lại hướng (Direction) cho tất cả các chân
 *          đã được cấu hình ban đầu trong `Port_Init`, dùng trong trường hợp
 *          phần mềm có thể đã thay đổi hướng của các chân trong runtime.
 *
 * @note Chỉ những chân không cho phép đổi hướng (`DirectionChangeable == 0`)
 *       mới được refresh lại.
 */
void Port_RefreshPortDirection(void);

/**
 * @brief Lấy thông tin version của module Port
 *
 * @param[in,out] VersionInfo Con trỏ đến biến chứa thông tin version.
 *                            (vendor ID, module ID, major, minor, patch)
 *
 * @note Nếu `VersionInfo == NULL_PTR` thì hàm sẽ không thực hiện gì.
 */
void Port_GetVersionInfo(Std_VersionInfoType* VersionInfo);

/**
 * @brief Thay đổi mode (chế độ hoạt động) của 1 chân tại thời điểm runtime
 *
 * @param[in] Pin  Chân cần thay đổi (theo chỉ số trong mảng cấu hình)
 * @param[in] Mode Chế độ mới muốn chuyển sang (DIO/ADC/PWM,...)
 *
 * @details Chỉ áp dụng được nếu `ModeChangeable` trong cấu hình là TRUE.
 *
 * @note Nếu không được phép thay đổi mode, hàm sẽ bỏ qua và không thực hiện.
 */
void Port_SetPinMode(Port_PinType Pin, Port_PinModeType Mode);

#endif /* PORT_H */
