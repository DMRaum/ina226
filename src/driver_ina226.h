/**
 * Copyright (c) 2015 - present LibDriver All rights reserved
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 * @file      driver_ina226.h
 * @brief     ina226 驱动头文件
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2025-01-29
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2025/01/29  <td>1.0      <td>Shifeng Li  <td>首次上传
 * </table>
 */

#ifndef DRIVER_INA226_H
#define DRIVER_INA226_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @defgroup ina226_driver ina226 驱动函数
 * @brief    ina226 驱动模块
 * @{
 */

/**
 * @addtogroup ina226_basic_driver
 * @{
 */

/**
 * @brief ina226 读取超时定义
 */
#ifndef INA226_READ_TIMEOUT
    #define INA226_READ_TIMEOUT    1000        /**< 1000 毫秒 */
#endif

/**
 * @brief ina226 地址枚举定义
 */
typedef enum
{
    INA226_ADDRESS_0 = (0x40 << 1),        /**< A0 = GND, A1 = GND */
    INA226_ADDRESS_1 = (0x41 << 1),        /**< A0 = VS+, A1 = GND */
    INA226_ADDRESS_2 = (0x42 << 1),        /**< A0 = SDA, A1 = GND */
    INA226_ADDRESS_3 = (0x43 << 1),        /**< A0 = SCL, A1 = GND */
    INA226_ADDRESS_4 = (0x44 << 1),        /**< A0 = GND, A1 = VS+ */
    INA226_ADDRESS_5 = (0x45 << 1),        /**< A0 = VS+, A1 = VS+ */
    INA226_ADDRESS_6 = (0x46 << 1),        /**< A0 = SDA, A1 = VS+ */
    INA226_ADDRESS_7 = (0x47 << 1),        /**< A0 = SCL, A1 = VS+ */
    INA226_ADDRESS_8 = (0x48 << 1),        /**< A0 = GND, A1 = SDA */
    INA226_ADDRESS_9 = (0x49 << 1),        /**< A0 = VS+, A1 = SDA */
    INA226_ADDRESS_A = (0x4A << 1),        /**< A0 = SDA, A1 = SDA */
    INA226_ADDRESS_B = (0x4B << 1),        /**< A0 = SCL, A1 = SDA */
    INA226_ADDRESS_C = (0x4C << 1),        /**< A0 = GND, A1 = SCL */
    INA226_ADDRESS_D = (0x4D << 1),        /**< A0 = VS+, A1 = SCL */
    INA226_ADDRESS_E = (0x4E << 1),        /**< A0 = SDA, A1 = SCL */
    INA226_ADDRESS_F = (0x4F << 1)         /**< A0 = SCL, A1 = SCL */
} ina226_address_t;

/**
 * @brief ina226 布尔枚举定义
 */
typedef enum
{
    INA226_BOOL_FALSE = 0x00,        /**< false */
    INA226_BOOL_TRUE  = 0x01,        /**< true */
} ina226_bool_t;

/**
 * @brief ina226 平均采样枚举定义
 */
typedef enum
{
    INA226_AVG_1    = 0,        /**< 1 次平均 */
    INA226_AVG_4    = 1,        /**< 4 次平均 */
    INA226_AVG_16   = 2,        /**< 16 次平均 */
    INA226_AVG_64   = 3,        /**< 64 次平均 */
    INA226_AVG_128  = 4,        /**< 128 次平均 */
    INA226_AVG_256  = 5,        /**< 256 次平均 */
    INA226_AVG_512  = 6,        /**< 512 次平均 */
    INA226_AVG_1024 = 7,        /**< 1024 次平均 */
} ina226_avg_t;

typedef enum
{
    INA226_CONVERSION_TIME_140_US   = 0,        /**< 140 us */
    INA226_CONVERSION_TIME_204_US   = 1,        /**< 204 us */
    INA226_CONVERSION_TIME_332_US   = 2,        /**< 332 us */
    INA226_CONVERSION_TIME_588_US   = 3,        /**< 588 us */
    INA226_CONVERSION_TIME_1P1_MS   = 4,        /**< 1.1 ms */
    INA226_CONVERSION_TIME_2P116_MS = 5,        /**< 2.116 ms */
    INA226_CONVERSION_TIME_4P156_MS = 6,        /**< 4.156 ms */
    INA226_CONVERSION_TIME_8P244_MS = 7,        /**< 8.244 ms */
} ina226_conversion_time_t;

/**
 * @brief ina226 模式枚举定义
 */
typedef enum
{
    INA226_MODE_POWER_DOWN                   = 0x0,        /**< 断电模式 */
    INA226_MODE_SHUNT_VOLTAGE_TRIGGERED      = 0x1,        /**< 分流电压触发模式 */
    INA226_MODE_BUS_VOLTAGE_TRIGGERED        = 0x2,        /**< 总线电压触发模式 */
    INA226_MODE_SHUNT_BUS_VOLTAGE_TRIGGERED  = 0x3,        /**< 分流和总线电压触发模式 */
    INA226_MODE_SHUTDOWN                     = 0x4,        /**< 关闭模式 */
    INA226_MODE_SHUNT_VOLTAGE_CONTINUOUS     = 0x5,        /**< 分流电压连续模式 */
    INA226_MODE_BUS_VOLTAGE_CONTINUOUS       = 0x6,        /**< 总线电压连续模式 */
    INA226_MODE_SHUNT_BUS_VOLTAGE_CONTINUOUS = 0x7,        /**< 分流和总线电压连续模式 */
} ina226_mode_t;

/**
 * @brief ina226 状态枚举定义
 */
typedef enum
{
    INA226_STATUS_SHUNT_VOLTAGE_OVER_VOLTAGE  = 15,        /**< 分流电压过压 */
    INA226_STATUS_SHUNT_VOLTAGE_UNDER_VOLTAGE = 14,        /**< 分流电压欠压 */
    INA226_STATUS_BUS_VOLTAGE_OVER_VOLTAGE    = 13,        /**< 总线电压过压 */
    INA226_STATUS_BUS_VOLTAGE_UNDER_VOLTAGE   = 12,        /**< 总线电压欠压 */
    INA226_STATUS_POWER_OVER_LIMIT            = 11,        /**< 功率超限 */
} ina226_status_t;

/**
 * @brief ina226 掩码枚举定义
 */
typedef enum
{
    INA226_MASK_SHUNT_VOLTAGE_OVER_VOLTAGE  = 15,        /**< 分流电压过压 */
    INA226_MASK_SHUNT_VOLTAGE_UNDER_VOLTAGE = 14,        /**< 分流电压欠压 */
    INA226_MASK_BUS_VOLTAGE_OVER_VOLTAGE    = 13,        /**< 总线电压过压 */
    INA226_MASK_BUS_VOLTAGE_UNDER_VOLTAGE   = 12,        /**< 总线电压欠压 */
    INA226_MASK_POWER_OVER_LIMIT            = 11,        /**< 功率超限 */
} ina226_mask_t;

/**
 * @brief ina226 告警极性枚举定义
 */
typedef enum
{
    INA226_ALERT_POLARITY_NORMAL   = 0,        /**< 低电平有效开漏输出 */
    INA226_ALERT_POLARITY_INVERTED = 1,        /**< 高电平有效开漏输出 */
} ina226_alert_polarity_t;

/**
 * @brief ina226 句柄结构体定义
 */
typedef struct ina226_handle_s
{
    uint8_t iic_addr;                                                                   /**< IIC 设备地址 */
    uint8_t (*iic_init)(void);                                                          /**< 指向 iic_init 函数的地址 */
    uint8_t (*iic_deinit)(void);                                                        /**< 指向 iic_deinit 函数的地址 */
    uint8_t (*iic_read)(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len);         /**< 指向 iic_read 函数的地址 */
    uint8_t (*iic_write)(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len);        /**< 指向 iic_write 函数的地址 */
    void (*delay_ms)(uint32_t ms);                                                      /**< 指向 delay_ms 函数的地址 */
    void (*debug_print)(const char *const fmt, ...);                                    /**< 指向 debug_print 函数的地址 */
    void (*receive_callback)(uint8_t type);                                             /**< 指向 receive_callback 函数的地址 */
    double r;                                                                           /**< 电阻值 */
    double current_lsb;                                                                 /**< 电流最小有效位 */
    uint8_t inited;                                                                     /**< 初始化标志 */
    uint8_t trigger;                                                                    /**< 触发标志 */
} ina226_handle_t;

/**
 * @brief ina226 信息结构体定义
 */
typedef struct ina226_info_s
{
    char chip_name[32];                /**< 芯片名称 */
    char manufacturer_name[32];        /**< 制造商名称 */
    char interface[8];                 /**< 芯片接口名称 */
    float supply_voltage_min_v;        /**< 芯片最小供电电压 */
    float supply_voltage_max_v;        /**< 芯片最大供电电压 */
    float max_current_ma;              /**< 芯片最大电流 */
    float temperature_min;             /**< 芯片最小工作温度 */
    float temperature_max;             /**< 芯片最大工作温度 */
    uint32_t driver_version;           /**< 驱动版本 */
} ina226_info_t;

/**
 * @}
 */

/**
 * @defgroup ina226_link_driver ina226 链接驱动函数
 * @brief    ina226 链接驱动模块
 * @ingroup  ina226_driver
 * @{
 */

/**
 * @brief     初始化 ina226_handle_t 结构体
 * @param[in] HANDLE 指向 ina226 句柄结构的指针
 * @param[in] STRUCTURE ina226_handle_t
 * @note      无
 */
#define DRIVER_INA226_LINK_INIT(HANDLE, STRUCTURE)         memset(HANDLE, 0, sizeof(STRUCTURE))

/**
 * @brief     链接 iic_init 函数
 * @param[in] HANDLE 指向 ina226 句柄结构的指针
 * @param[in] FUC 指向 iic_init 函数的地址
 * @note      无
 */
#define DRIVER_INA226_LINK_IIC_INIT(HANDLE, FUC)          (HANDLE)->iic_init = FUC

/**
 * @brief     链接 iic_deinit 函数
 * @param[in] HANDLE 指向 ina226 句柄结构的指针
 * @param[in] FUC 指向 iic_deinit 函数的地址
 * @note      无
 */
#define DRIVER_INA226_LINK_IIC_DEINIT(HANDLE, FUC)        (HANDLE)->iic_deinit = FUC

/**
 * @brief     链接 iic_read 函数
 * @param[in] HANDLE 指向 ina226 句柄结构的指针
 * @param[in] FUC 指向 iic_read 函数的地址
 * @note      无
 */
#define DRIVER_INA226_LINK_IIC_READ(HANDLE, FUC)          (HANDLE)->iic_read = FUC

/**
 * @brief     链接 iic_write 函数
 * @param[in] HANDLE 指向 ina226 句柄结构的指针
 * @param[in] FUC 指向 iic_write 函数的地址
 * @note      无
 */
#define DRIVER_INA226_LINK_IIC_WRITE(HANDLE, FUC)         (HANDLE)->iic_write = FUC

/**
 * @brief     链接 delay_ms 函数
 * @param[in] HANDLE 指向 ina226 句柄结构的指针
 * @param[in] FUC 指向 delay_ms 函数的地址
 * @note      无
 */
#define DRIVER_INA226_LINK_DELAY_MS(HANDLE, FUC)          (HANDLE)->delay_ms = FUC

/**
 * @brief     链接 debug_print 函数
 * @param[in] HANDLE 指向 ina226 句柄结构的指针
 * @param[in] FUC 指向 debug_print 函数的地址
 * @note      无
 */
#define DRIVER_INA226_LINK_DEBUG_PRINT(HANDLE, FUC)       (HANDLE)->debug_print = FUC

/**
 * @brief     链接 receive_callback 函数
 * @param[in] HANDLE 指向 ina226 句柄结构的指针
 * @param[in] FUC 指向 receive_callback 函数的地址
 * @note      无
 */
#define DRIVER_INA226_LINK_RECEIVE_CALLBACK(HANDLE, FUC)  (HANDLE)->receive_callback = FUC

/**
 * @}
 */

/**
 * @defgroup ina226_basic_driver ina226 基本驱动函数
 * @brief    ina226 基本驱动模块
 * @ingroup  ina226_driver
 * @{
 */

/**
 * @brief      获取芯片信息
 * @param[out] *info 指向 ina226 信息结构的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 句柄为 NULL
 * @note       无
 */
uint8_t ina226_info(ina226_info_t *info);

/**
 * @brief     设置 IIC 地址引脚
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] addr_pin 地址引脚
 * @return    状态码
 *            - 0 成功
 *            - 2 句柄为 NULL
 * @note      无
 */
uint8_t ina226_set_addr_pin(ina226_handle_t *handle, ina226_address_t addr_pin);

/**
 * @brief      获取 IIC 地址引脚
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *addr_pin 指向地址引脚缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 句柄为 NULL
 * @note       无
 */
uint8_t ina226_get_addr_pin(ina226_handle_t *handle, ina226_address_t *addr_pin);

/**
 * @brief     设置电阻值
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] resistance 电流采样电阻值
 * @return    状态码
 *            - 0 成功
 *            - 2 句柄为 NULL
 * @note      无
 */
uint8_t ina226_set_resistance(ina226_handle_t *handle, double resistance);

/**
 * @brief      获取电阻值
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *resistance 指向电流采样电阻值缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 句柄为 NULL
 * @note       无
 */
uint8_t ina226_get_resistance(ina226_handle_t *handle, double *resistance);

/**
 * @brief     中断处理函数
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @return    状态码
 *            - 0 成功
 *            - 1 运行失败
 *            - 2 句柄为 NULL
 *            - 3 句柄未初始化
 * @note      无
 */
uint8_t ina226_irq_handler(ina226_handle_t *handle);

/**
 * @brief     初始化芯片
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @return    状态码
 *            - 0 成功
 *            - 1 IIC 初始化失败
 *            - 2 句柄为 NULL
 *            - 3 链接函数为 NULL
 *            - 4 ID 无效
 *            - 5 软复位失败
 * @note      无
 */
uint8_t ina226_init(ina226_handle_t *handle);

/**
 * @brief     关闭芯片
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @return    状态码
 *            - 0 成功
 *            - 1 IIC 反初始化失败
 *            - 2 句柄为 NULL
 *            - 3 句柄未初始化
 *            - 4 断电失败
 * @note      无
 */
uint8_t ina226_deinit(ina226_handle_t *handle);

/**
 * @brief     软复位芯片
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @return    状态码
 *            - 0 成功
 *            - 1 软复位失败
 *            - 2 句柄为 NULL
 *            - 3 句柄未初始化
 * @note      无
 */
uint8_t ina226_soft_reset(ina226_handle_t *handle);

/**
 * @brief     设置平均采样模式
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] mode 平均采样模式
 * @return    状态码
 *            - 0 成功
 *            - 1 设置平均采样模式失败
 *            - 2 句柄为 NULL
 *            - 3 句柄未初始化
 * @note      无
 */
uint8_t ina226_set_average_mode(ina226_handle_t *handle, ina226_avg_t mode);

/**
 * @brief      获取平均采样模式
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *mode 指向平均采样模式缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取平均采样模式失败
 *             - 2 句柄为 NULL
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_get_average_mode(ina226_handle_t *handle, ina226_avg_t *mode);

/**
 * @brief     设置总线电压转换时间
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] t 转换时间
 * @return    状态码
 *            - 0 成功
 *            - 1 设置总线电压转换时间失败
 *            - 2 句柄为 NULL
 *            - 3 句柄未初始化
 * @note      无
 */
uint8_t ina226_set_bus_voltage_conversion_time(ina226_handle_t *handle, ina226_conversion_time_t t);

/**
 * @brief      获取总线电压转换时间
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *t 指向转换时间缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取总线电压转换时间失败
 *             - 2 句柄为 NULL
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_get_bus_voltage_conversion_time(ina226_handle_t *handle, ina226_conversion_time_t *t);

/**
 * @brief     设置分流电压转换时间
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] t 转换时间
 * @return    状态码
 *            - 0 成功
 *            - 1 设置分流电压转换时间失败
 *            - 2 句柄为 NULL
 *            - 3 句柄未初始化
 * @note      无
 */
uint8_t ina226_set_shunt_voltage_conversion_time(ina226_handle_t *handle, ina226_conversion_time_t t);

/**
 * @brief      获取分流电压转换时间
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *t 指向转换时间缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取分流电压转换时间失败
 *             - 2 句柄为 NULL
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_get_shunt_voltage_conversion_time(ina226_handle_t *handle, ina226_conversion_time_t *t);

/**
 * @brief     设置工作模式
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] mode 芯片工作模式
 * @return    状态码
 *            - 0 成功
 *            - 1 设置模式失败
 *            - 2 句柄为 NULL
 *            - 3 句柄未初始化
 * @note      无
 */
uint8_t ina226_set_mode(ina226_handle_t *handle, ina226_mode_t mode);

/**
 * @brief      获取工作模式
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *mode 指向芯片工作模式缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取模式失败
 *             - 2 句柄为 NULL
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_get_mode(ina226_handle_t *handle, ina226_mode_t *mode);

/**
 * @brief      读取分流电压
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *raw 指向原始数据缓冲区的指针
 * @param[out] *mV 指向转换数据缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 读取分流电压失败
 *             - 2 句柄为 NULL
 *             - 3 句柄未初始化
 *             - 4 数学溢出
 *             - 5 读取超时
 * @note       无
 */
uint8_t ina226_read_shunt_voltage(ina226_handle_t *handle, int16_t *raw, float *mV);

/**
 * @brief      读取总线电压
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *raw 指向原始数据缓冲区的指针
 * @param[out] *mV 指向转换数据缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 读取总线电压失败
 *             - 2 句柄为 NULL
 *             - 3 句柄未初始化
 *             - 4 数学溢出
 *             - 5 读取超时
 * @note       无
 */
uint8_t ina226_read_bus_voltage(ina226_handle_t *handle, uint16_t *raw, float *mV);

/**
 * @brief      读取功率
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *raw 指向原始数据缓冲区的指针
 * @param[out] *mW 指向转换数据缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 读取功率失败
 *             - 2 句柄为 NULL
 *             - 3 句柄未初始化
 *             - 4 数学溢出
 *             - 5 读取超时
 * @note       无
 */
uint8_t ina226_read_power(ina226_handle_t *handle, uint16_t *raw, float *mW);

/**
 * @brief      读取电流
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *raw 指向原始数据缓冲区的指针
 * @param[out] *mA 指向转换数据缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 读取电流失败
 *             - 2 句柄为 NULL
 *             - 3 句柄未初始化
 *             - 4 数学溢出
 *             - 5 读取超时
 * @note       无
 */
uint8_t ina226_read_current(ina226_handle_t *handle, int16_t *raw, float *mA);

/**
 * @brief     设置校准值
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] data 校准数据
 * @return    状态码
 *            - 0 成功
 *            - 1 设置校准值失败
 *            - 2 句柄为 NULL
 *            - 3 句柄未初始化
 * @note      无
 */
uint8_t ina226_set_calibration(ina226_handle_t *handle, uint16_t data);

/**
 * @brief      获取校准值
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *data 指向校准数据缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取校准值失败
 *             - 2 句柄为 NULL
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_get_calibration(ina226_handle_t *handle, uint16_t *data);

/**
 * @brief      计算校准值
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *calibration 指向校准数据缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 计算校准值失败
 *             - 2 句柄为 NULL
 *             - 3 句柄未初始化
 *             - 4 电阻值不能为零
 * @note       无
 */
uint8_t ina226_calculate_calibration(ina226_handle_t *handle, uint16_t *calibration);

/**
 * @brief     启用或禁用掩码
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] mask 设置的掩码
 * @param[in] enable 布尔值
 * @return    状态码
 *            - 0 成功
 *            - 1 设置掩码失败
 *            - 2 句柄为 NULL
 *            - 3 句柄未初始化
 * @note      无
 */
uint8_t ina226_set_mask(ina226_handle_t *handle, ina226_mask_t mask, ina226_bool_t enable);

/**
 * @brief      获取掩码
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[in]  mask 设置的掩码
 * @param[out] *enable 指向布尔值缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取掩码失败
 *             - 2 句柄为 NULL
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_get_mask(ina226_handle_t *handle, ina226_mask_t mask, ina226_bool_t *enable);

/**
 * @brief     启用或禁用转换完成告警引脚
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] enable 布尔值
 * @return    状态码
 *            - 0 成功
 *            - 1 设置转换完成告警引脚失败
 *            - 2 句柄为 NULL
 *            - 3 句柄未初始化
 * @note      无
 */
uint8_t ina226_set_conversion_ready_alert_pin(ina226_handle_t *handle, ina226_bool_t enable);

/**
 * @brief      获取转换完成告警引脚状态
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *enable 指向布尔值缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取转换完成告警引脚失败
 *             - 2 句柄为 NULL
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_get_conversion_ready_alert_pin(ina226_handle_t *handle, ina226_bool_t *enable);

/**
 * @brief     设置告警极性引脚
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] pin 告警极性引脚
 * @return    状态码
 *            - 0 成功
 *            - 1 设置告警极性引脚失败
 *            - 2 句柄为 NULL
 *            - 3 句柄未初始化
 * @note      无
 */
uint8_t ina226_set_alert_polarity_pin(ina226_handle_t *handle, ina226_alert_polarity_t pin);

/**
 * @brief      获取告警极性引脚
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *pin 指向告警极性引脚缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取告警极性引脚失败
 *             - 2 句柄为 NULL
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_get_alert_polarity_pin(ina226_handle_t *handle, ina226_alert_polarity_t *pin);

/**
 * @brief     启用或禁用告警锁存
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] enable 布尔值
 * @return    状态码
 *            - 0 成功
 *            - 1 设置告警锁存失败
 *            - 2 句柄为 NULL
 *            - 3 句柄未初始化
 * @note      无
 */
uint8_t ina226_set_alert_latch(ina226_handle_t *handle, ina226_bool_t enable);

/**
 * @brief      获取告警锁存状态
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *enable 指向布尔值缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取告警锁存失败
 *             - 2 句柄为 NULL
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_get_alert_latch(ina226_handle_t *handle, ina226_bool_t *enable);

/**
 * @brief     设置告警阈值
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] reg 设置的寄存器值
 * @return    状态码
 *            - 0 成功
 *            - 1 设置告警阈值失败
 *            - 2 句柄为 NULL
 *            - 3 句柄未初始化
 * @note      无
 */
uint8_t ina226_set_alert_limit(ina226_handle_t *handle, uint16_t reg);

/**
 * @brief      获取告警阈值
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *reg 指向寄存器缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取告警阈值失败
 *             - 2 句柄为 NULL
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_get_alert_limit(ina226_handle_t *handle, uint16_t *reg);

/**
 * @brief      将分流电压转换为寄存器原始数据
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[in]  mV 毫伏值
 * @param[out] *reg 指向寄存器原始数据缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 句柄为 NULL
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_shunt_voltage_convert_to_register(ina226_handle_t *handle, float mV, uint16_t *reg);

/**
 * @brief      将寄存器原始数据转换为分流电压
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[in]  reg 寄存器原始数据
 * @param[out] *mV 指向毫伏值缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 句柄为 NULL
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_shunt_voltage_convert_to_data(ina226_handle_t *handle, uint16_t reg, float *mV);

/**
 * @brief      将总线电压转换为寄存器原始数据
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[in]  mV 毫伏值
 * @param[out] *reg 指向寄存器原始数据缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 句柄为 NULL
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_bus_voltage_convert_to_register(ina226_handle_t *handle, float mV, uint16_t *reg);

/**
 * @brief      将寄存器原始数据转换为总线电压
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[in]  reg 寄存器原始数据
 * @param[out] *mV 指向毫伏值缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 句柄为 NULL
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_bus_voltage_convert_to_data(ina226_handle_t *handle, uint16_t reg, float *mV);

/**
 * @brief      将功率转换为寄存器原始数据
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[in]  mW 毫瓦值
 * @param[out] *reg 指向寄存器原始数据缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 句柄为 NULL
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_power_convert_to_register(ina226_handle_t *handle, float mW, uint16_t *reg);

/**
 * @brief      将寄存器原始数据转换为功率
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[in]  reg 寄存器原始数据
 * @param[out] *mW 指向毫瓦值缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 句柄为 NULL
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_power_convert_to_data(ina226_handle_t *handle, uint16_t reg, float *mW);

/**
 * @brief      获取芯片 ID
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *device_id 指向设备 ID 缓冲区的指针
 * @param[out] *die_revision_id 指向芯片修订 ID 缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取芯片 ID 失败
 *             - 2 句柄为 NULL
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_get_die_id(ina226_handle_t *handle, uint16_t *device_id, uint8_t *die_revision_id);

/**
 * @}
 */

/**
 * @defgroup ina226_extern_driver ina226 外部驱动函数
 * @brief    ina226 外部驱动模块
 * @ingroup  ina226_driver
 * @{
 */

/**
 * @brief     设置芯片寄存器
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] reg 寄存器地址
 * @param[in] data 写入的数据
 * @return    状态码
 *            - 0 成功
 *            - 1 写入失败
 *            - 2 句柄为 NULL
 *            - 3 句柄未初始化
 * @note      无
 */
uint8_t ina226_set_reg(ina226_handle_t *handle, uint8_t reg, uint16_t data);

/**
 * @brief      获取芯片寄存器
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[in]  reg 寄存器地址
 * @param[out] *data 指向数据缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 读取失败
 *             - 2 句柄为 NULL
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_get_reg(ina226_handle_t *handle, uint8_t reg, uint16_t *data);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
