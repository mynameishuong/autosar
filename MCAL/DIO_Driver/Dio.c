/***************************************************************************
 * @file    Dio.C
 * @brief   Định nghĩa các hàm và cấu trúc liên quan đến điều khiển GPIO
 * @details File này định nghĩa các hàm chức năng có thể sử dụng để cấu hình các Port
 * @version 1.0
 * @date    18-06-2025
 ***************************************************************************/

#include "Dio.h"
#include "Det.h"  // Dùng để báo lỗi DET (nếu bật)
#include "stm32f10x.h"

/**
 * @brief      Đọc mức logic của kênh DIO được chỉ định.
 * @details    Hàm này đọc trạng thái (STD_HIGH hoặc STD_LOW) của một chân DIO.
 *
 * @param[in]  ChannelId  ID của kênh DIO cần đọc.
 *
 * @return     STD_HIGH hoặc STD_LOW tùy theo trạng thái của chân.
 *
 * @note       Hàm giả định rằng chân đã được cấu hình đúng (input hoặc output).
 */
Dio_LevelType Dio_ReadChannel(Dio_ChannelType ChannelId)
{
    Dio_LevelType retVal = STD_LOW;
    GPIO_TypeDef *GET_PORT = NULL_PTR;
    uint16_t GET_PIN;

    // Ánh xạ ChannelId thành Port và Pin vật lý
    GET_PORT = DIO_GET_PORT_ID(ChannelId);
    GET_PIN  = DIO_GET_PIN_NUM(ChannelId);

    // Đọc trạng thái chân và chuyển về STD_HIGH hoặc STD_LOW
    if (GPIO_ReadInputDataBit(GET_PORT, GET_PIN) == Bit_SET)
    {
        retVal = STD_HIGH;
    }
    else
    {
        retVal = STD_LOW;
    }

    return retVal;
}

/**
 * @brief      Ghi mức logic vào một kênh DIO.
 * @details    Thiết lập chân DIO được chỉ định thành mức logic cao hoặc thấp.
 *
 * @param[in]  ChannelId  ID của kênh cần ghi.
 * @param[in]  Level      Mức logic cần ghi (STD_HIGH hoặc STD_LOW).
 *
 * @note       Chân phải được cấu hình là output thì mới có tác dụng.
 */
void Dio_WriteChannel(Dio_ChannelType ChannelId, Dio_LevelType Level)
{
    GPIO_TypeDef *GET_PORT = NULL_PTR;
    uint16_t GET_PIN;

    GET_PORT = DIO_GET_PORT_ID(ChannelId);
    GET_PIN  = DIO_GET_PIN_NUM(ChannelId);

    switch (Level)
    {
        case STD_HIGH:
            GPIO_WriteBit(GET_PORT, GET_PIN, Bit_SET);
            break;
        case STD_LOW:
            GPIO_WriteBit(GET_PORT, GET_PIN, Bit_RESET);
            break;
        default:
            break;
    }
}

/**
 * @brief      Đảo trạng thái logic của một chân DIO.
 * @details    Đọc trạng thái hiện tại, sau đó ghi giá trị ngược lại.
 *
 * @param[in]  ChannelId  ID của kênh cần đảo trạng thái.
 *
 * @return     Trạng thái mới sau khi được đảo.
 *
 * @note       Chân phải ở chế độ output. Hàm không đảm bảo tính nguyên tử.
 */
Dio_LevelType Dio_FlipChannel(Dio_ChannelType ChannelId)
{
    Dio_LevelType val = STD_LOW;
    Dio_LevelType new_reval = STD_LOW;

    val = Dio_ReadChannel(ChannelId);

    switch (val)
    {
        case STD_LOW:
            Dio_WriteChannel(ChannelId, STD_HIGH);
            new_reval = STD_HIGH;
            break;
        case STD_HIGH:
            Dio_WriteChannel(ChannelId, STD_LOW);
            new_reval = STD_LOW;
            break;
        default:
            new_reval = val;
            break;
    }

    return new_reval;
}

/**
 * @brief      Đọc toàn bộ trạng thái logic của một port.
 * @details    Trả về giá trị mức logic của tất cả các chân trong port.
 *
 * @param[in]  PortId  ID của port cần đọc (VD: DIO_GPIO_PORT_A...)
 *
 * @return     Trạng thái bitwise của toàn bộ port.
 */
Dio_PortLevelType Dio_ReadPort(Dio_PortType PortId)
{
    Dio_PortLevelType retVal = STD_LOW;
    GPIO_TypeDef *GET_PORT = NULL_PTR;

#if (DIO_DEV_ERROR_DETECT == STD_ON)
    if (PortId >= MAX_DIO_PORT)
    {
        Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID,
                        DIO_E_PARAM_INVALID_PORT_ID,
                        DIO_E_PARAM_INVALID_PORT_ID);
        return STD_LOW;
    }
#endif

    switch (PortId)
    {
        case 0: GET_PORT = GPIOA; break;
        case 1: GET_PORT = GPIOB; break;
        case 2: GET_PORT = GPIOC; break;
        case 3: GET_PORT = GPIOD; break;
    }

    retVal = (Dio_PortLevelType)(GPIO_ReadOutputData(GET_PORT));
    return retVal;
}

/**
 * @brief      Ghi toàn bộ mức logic ra một port.
 * @details    Ghi giá trị logic cho toàn bộ các chân của port.
 *             Các chân cấu hình input sẽ không bị ảnh hưởng.
 *
 * @param[in]  PortId  ID của port (VD: 0 cho GPIOA, 1 cho GPIOB, ...)
 * @param[in]  Level   Giá trị logic bitwise cần ghi.
 */
void Dio_WritePort(Dio_PortType PortId, Dio_PortLevelType Level)
{
    GPIO_TypeDef *GET_PORT = NULL_PTR;

    switch (PortId)
    {
        case 0: GET_PORT = GPIOA; break;
        case 1: GET_PORT = GPIOB; break;
        case 2: GET_PORT = GPIOC; break;
        case 3: GET_PORT = GPIOD; break;
        default: return;
    }

    GPIO_Write(GET_PORT, Level);
}

/**
 * @brief      Đọc trạng thái của một nhóm kênh DIO liền kề.
 * @details    Trả về giá trị nhóm sau khi đã dịch offset về bit thấp nhất.
 *
 * @param[in]  ChannelGroupIdPtr  Con trỏ tới cấu trúc nhóm kênh.
 *
 * @return     Giá trị mức logic của nhóm (sau khi đã dịch).
 */
Dio_PortLevelType Dio_ReadChannelGroup(const Dio_ChannelGroupType* ChannelGroupIdPtr)
{
    GPIO_TypeDef *GET_PORT = NULL_PTR;

    if (ChannelGroupIdPtr == NULL_PTR) return STD_LOW;

    GET_PORT = DIO_GET_PORT_ID(ChannelGroupIdPtr->port);
    if (GET_PORT == NULL_PTR) return STD_LOW;

    uint16_t value = GPIO_ReadOutputData(GET_PORT);
    uint16_t group_value = (value & ChannelGroupIdPtr->mask) >> ChannelGroupIdPtr->offset;

    return (Dio_PortLevelType)group_value;
}

/**
 * @brief      Ghi mức logic cho nhóm kênh DIO.
 * @details    Chỉ những bit nằm trong mask mới bị thay đổi.
 *
 * @param[in]  ChannelGroupIdPtr  Con trỏ đến cấu hình nhóm kênh.
 * @param[in]  Level              Giá trị logic cần ghi (bit thấp nhất ứng với offset).
 */
void Dio_WriteChannelGroup(const Dio_ChannelGroupType* ChannelGroupIdPtr, Dio_PortLevelType Level)
{
    GPIO_TypeDef *GET_PORT = NULL_PTR;

    if (ChannelGroupIdPtr == NULL_PTR) return;

    GET_PORT = DIO_GET_PORT_ID(ChannelGroupIdPtr->port);
    if (GET_PORT == NULL_PTR) return;

    uint16_t port_value = GPIO_ReadOutputData(GET_PORT);

    port_value &= ~(ChannelGroupIdPtr->mask);
    port_value |= ((Level << ChannelGroupIdPtr->offset) & ChannelGroupIdPtr->mask);

    GPIO_Write(GET_PORT, port_value);
}

/**
 * @brief      Trả về thông tin phiên bản của module.
 */
void Dio_GetVersionInfo(Std_VersionInfoType* VersionInfo)
{
    if (VersionInfo == NULL_PTR) return;

    VersionInfo->vendorID = PORT_VENDOR_ID;
    VersionInfo->moduleID = PORT_MODULE_ID;
    VersionInfo->sw_major_version = PORT_SW_MAJOR_VERSION;
    VersionInfo->sw_minor_version = PORT_SW_MINOR_VERSION;
    VersionInfo->sw_patch_version = PORT_SW_PATCH_VERSION;
}

/**
 * @brief      Ghi dữ liệu có mặt nạ lên port.
 * @details    Chỉ những bit được chỉ định bởi mask sẽ bị ghi đè. Các bit còn lại giữ nguyên.
 *
 * @param[in]  PortId  ID của port (VD: DIO_GPIO_PORT_A, B, C, D)
 * @param[in]  Level   Giá trị cần ghi (bit phải đúng vị trí mask)
 * @param[in]  Mask    Mặt nạ để xác định các bit cần ghi
 */
void Dio_MaskedWritePort(Dio_PortType PortId, Dio_PortLevelType Level, Dio_PortLevelType Mask)
{
    GPIO_TypeDef *GET_PORT = NULL_PTR;

    switch (PortId)
    {
        case 0: GET_PORT = GPIOA; break;
        case 1: GET_PORT = GPIOB; break;
        case 2: GET_PORT = GPIOC; break;
        case 3: GET_PORT = GPIOD; break;
        default: return;
    }

    uint16_t port_val = GPIO_ReadOutputData(GET_PORT);
    port_val = (port_val & ~Mask) | (Level & Mask);
    GPIO_Write(GET_PORT, port_val);
}
