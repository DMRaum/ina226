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
 * @file      driver_ina226_alert_test.c
 * @brief     ina226 告警测试源文件
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

#ifndef DRIVER_INA226_ALERT_TEST_H
#define DRIVER_INA226_ALERT_TEST_H

#include "driver_ina226_interface.h"

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @addtogroup ina226_test_driver
 * @{
 */

/**
 * @brief  告警测试中断处理函数
 * @return 状态码
 *         - 0 成功
 *         - 1 运行失败
 * @note   无
 */
uint8_t ina226_alert_test_irq_handler(void);

/**
 * @brief     告警测试
 * @param[in] addr_pin IIC 设备地址
 * @param[in] r 外部电阻
 * @param[in] mask 设置掩码
 * @param[in] threshold 设置阈值
 * @param[in] timeout_ms 超时时间（毫秒）
 * @return    状态码
 *            - 0 成功
 *            - 1 测试失败
 * @note      无
 */
uint8_t ina226_alert_test(ina226_address_t addr_pin, double r, ina226_mask_t mask, float threshold, uint32_t timeout_ms);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
