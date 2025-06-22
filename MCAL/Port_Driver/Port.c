#include "Port.h"
#include "Dio.h"
#include "Port_Cfg.h"

// Biến trạng thái xác định xem Port đã được khởi tạo hay chưa
static uint8 PortInitState = 0;

/**
 * @brief Hàm triển khai cấu hình cho từng chân GPIO theo cấu hình đã định nghĩa
 *
 * @param Portconf Con trỏ tới cấu hình một chân GPIO
 */
void Port_Deploy_pin(const Port_PinConfigType *Portconf)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // Gán số chân (Pin number)
    GPIO_InitStruct.GPIO_Pin = PORT_GET_PIN_NUM(Portconf -> PinID);

    // Gán tốc độ (speed)
    GPIO_InitStruct.GPIO_Speed = Portconf->Speed;

    // Bật xung clock cho Port tương ứng (A, B, C, D)
    switch (Portconf->PortID)
    {
        case 0:
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
            break;
        case 1:
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
            break;
        case 2:
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
            break;
        case 3:
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
            break;
        default:
            return; // Port không hợp lệ
    }

    // Cấu hình mode cho chân DIO
    if (Portconf->PinMode == PORT_PIN_MODE_DIO)
    {
        if (Portconf->Direction == PORT_PIN_IN)
        {
            // Input: có thể kéo lên/kéo xuống
            if (Portconf->Pull == PULL_UP)
            {
                GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
            }
            else if (Portconf->Pull == PULL_DOWN)
            {
                GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
            }
        }
        else if (Portconf->Direction == PORT_PIN_OUT)
        {
            // Output: có thể là Push-Pull hoặc Open-Drain
            if (Portconf->Pull == PULL_UP)
            {
                GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
            }
            else if (Portconf->Pull == PULL_DOWN)
            {
                GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
            }
        }
    }
    else if (Portconf->PinMode == PORT_PIN_MODE_ADC)
    {
        // do not thing
    }
    else if (Portconf->PinMode == PORT_PIN_MODE_PWM)
    {
        // do not thing
    }

    // Khởi tạo chân GPIO với thông số cấu hình
    GPIO_Init(PORT_GET_ID(Portconf->PortID), &GPIO_InitStruct);

    // Nếu là chân output, cấu hình trạng thái mặc định (level)
    if (Portconf->Direction == PORT_PIN_OUT)
    {
        if (Portconf->Level == PORT_PIN_LEVEL_HIGH)
            GPIO_WriteBit(PORT_GET_ID(Portconf->PortID), GPIO_InitStruct.GPIO_Pin, PORT_PIN_LEVEL_HIGH);
        else
            GPIO_WriteBit(PORT_GET_ID(Portconf->PortID), GPIO_InitStruct.GPIO_Pin, PORT_PIN_LEVEL_LOW);
    }
}

/**
 * @brief Khởi tạo tất cả các chân GPIO theo cấu hình đầu vào
 *
 * @param ConfigPtr Con trỏ tới cấu trúc cấu hình tổng của Port
 */
void Port_Init(const Port_ConfigType* ConfigPtr)
{
    if (ConfigPtr == NULL_PTR) return;

    for (uint16_t i = 0; i < ConfigPtr->PortCfg_PinsCount; i++)
    {
        // Gọi hàm cấu hình từng chân GPIO
        Port_Deploy_pin(&ConfigPtr->PinCfgType[i]);
    }

    // Đánh dấu đã khởi tạo
    PortInitState = 1;
}

/**
 * @brief Cập nhật lại hướng (direction) của một chân tại runtime nếu được phép
 *
 * @param Pin Mã số chân (toàn cục)
 * @param Direction Hướng mong muốn: IN hoặc OUT
 */
void Port_SetPinDirection(Port_PinType Pin, Port_PinDirectionType Direction)
{
    // Nếu chưa khởi tạo Port thì không làm gì
    if (!PortInitState) return;

    // Nếu số chân không hợp lệ
    if (Pin >= Pincount) return;

    // Nếu chân không cho phép thay đổi hướng trong runtime
    if (PortCfg_Pins[Pin].DirectionChangeable == 0) return;

    // Tạo một bản sao của cấu hình pin (vì cấu hình gốc là const)
    Port_PinConfigType pinCfg = PortCfg_Pins[Pin];

    // Cập nhật lại hướng mới
    pinCfg.Direction = Direction;

    // Áp dụng lại cấu hình mới cho chân
    Port_Deploy_pin(&pinCfg);
}

void Port_RefreshPortDirection(void)
{
    // Nếu chưa khởi tạo Port thì không làm gì
    if (!PortInitState) return;
    for (uint8_t i = 0; i < Pincount; i++)
    {
        if (PortCfg_Pins[Pincount].DirectionChangeable == 0)
                Port_Deploy_pin(&PortCfg_Pins[i]);
    }
}
void Port_GetVersionInfo(Std_VersionInfoType* VersionInfo)
{
    if (VersionInfo == NULL_PTR) return;

    VersionInfo->vendorID = PORT_VENDOR_ID;
    VersionInfo->moduleID = PORT_MODULE_ID;
    VersionInfo->sw_major_version = PORT_SW_MAJOR_VERSION;
    VersionInfo->sw_minor_version = PORT_SW_MINOR_VERSION;
    VersionInfo->sw_patch_version = PORT_SW_PATCH_VERSION;
}
void Port_SetPinMode(Port_PinType Pin, Port_PinModeType Mode)
{
// Nếu chưa khởi tạo Port thì không làm gì
    if (!PortInitState) return;

    // Nếu số chân không hợp lệ
    if (Pin >= Pincount) return;

    // Nếu chân không cho phép thay đổi hướng trong runtime
    if (PortCfg_Pins[Pin].PinMode == 0) return;

    // Tạo một bản sao của cấu hình pin (vì cấu hình gốc là const)
    Port_PinConfigType pinCfg = PortCfg_Pins[Pin];

    // Cập nhật lại hướng mới
    pinCfg.PinMode = Mode;

    // Áp dụng lại cấu hình mới cho chân
    Port_Deploy_pin(&pinCfg);
}
