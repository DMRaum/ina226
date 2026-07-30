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
 * @file      driver_ina226_basic.c
 * @brief     ina226 基本驱动源文件
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

#ifndef DRIVER_INA226_BASIC_H
#define DRIVER_INA226_BASIC_H

#include "driver_ina226_interface.h"

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @defgroup ina226_example_driver ina226 示例驱动功能
 * @brief    ina226 示例驱动模块
 * @ingroup  ina226_driver
 * @{
 */

/**
 * @brief ina226 基本示例默认定义
 */
#define INA226_BASIC_DEFAULT_AVG_MODE                             INA226_AVG_16                        /**< 16 次平均 */
#define INA226_BASIC_DEFAULT_BUS_VOLTAGE_CONVERSION_TIME          INA226_CONVERSION_TIME_1P1_MS        /**< 总线电压转换时间 1.1 ms */
#define INA226_BASIC_DEFAULT_SHUNT_VOLTAGE_CONVERSION_TIME        INA226_CONVERSION_TIME_1P1_MS        /**< 分流电压转换时间 1.1 ms */

/**
 * @brief     基本示例初始化
 * @param[in] addr_pin IIC 地址引脚
 * @param[in] r 参考电阻值
 * @return    状态码
 *            - 0 成功
 *            - 1 初始化失败
 * @note      无
 */
uint8_t ina226_basic_init(ina226_address_t addr_pin, double r);

/**
 * @brief  基本示例反初始化
 * @return 状态码
 *         - 0 成功
 *         - 1 反初始化失败
 * @note   无
 */
uint8_t ina226_basic_deinit(void);

/**
 * @brief      基本示例读取
 * @param[out] *mV 指向 mV 缓冲区的指针
 * @param[out] *mA 指向 mA 缓冲区的指针
 * @param[out] *mW 指向 mW 缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 读取失败
 * @note       无
 */
uint8_t ina226_basic_read(float *mV, float *mA, float *mW);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
