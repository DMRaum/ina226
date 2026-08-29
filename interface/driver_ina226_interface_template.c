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
 * @file      driver_ina226_interface_template.c
 * @brief     INA226驱动接口模板源文件
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2025-01-29
 *
 * <h3>历史</h3>
 * <table>
 * <tr><th>日期        <th>版本    <th>作者        <th>描述
 * <tr><td>2025/01/29  <td>1.0     <td>Shifeng Li  <td>首次上传
 * </table>
 */

#include "driver_ina226_interface.h"

/**
 * @brief  接口IIC总线初始化
 * @return 状态码
 *         - 0 成功
 *         - 1 IIC初始化失败
 * @note   无
 */
uint8_t ina226_interface_iic_init(void)
{
    return 0;
}

/**
 * @brief  接口IIC总线反初始化
 * @return 状态码
 *         - 0 成功
 *         - 1 IIC反初始化失败
 * @note   无
 */
uint8_t ina226_interface_iic_deinit(void)
{
    return 0;
}

/**
 * @brief      接口IIC总线读取
 * @param[in]  addr IIC设备写地址
 * @param[in]  reg IIC寄存器地址
 * @param[out] *buf 指向数据缓冲区的指针
 * @param[in]  len 数据缓冲区长度
 * @return     状态码
 *             - 0 成功
 *             - 1 读取失败
 * @note       无
 */
uint8_t ina226_interface_iic_read(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    return 0;
}

/**
 * @brief     接口IIC总线写入
 * @param[in] addr IIC设备写地址
 * @param[in] reg IIC寄存器地址
 * @param[in] *buf 指向数据缓冲区的指针
 * @param[in] len 数据缓冲区长度
 * @return    状态码
 *            - 0 成功
 *            - 1 写入失败
 * @note      无
 */
uint8_t ina226_interface_iic_write(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    return 0;
}

/**
 * @brief     接口毫秒级延时
 * @param[in] ms 时间
 * @note      无
 */
void ina226_interface_delay_ms(uint32_t ms)
{

}

/**
 * @brief     接口打印格式化数据
 * @param[in] fmt 格式化数据
 * @note      无
 */
void ina226_interface_debug_print(const char *const fmt, ...)
{
    
}

/**
 * @brief     接口接收回调函数
 * @param[in] type 中断类型
 * @note      无
 */
void ina226_interface_receive_callback(uint8_t type)
{
    switch (type)
    {
        case INA226_STATUS_SHUNT_VOLTAGE_OVER_VOLTAGE :
        {
            ina226_interface_debug_print("ina226: 中断-并联电压过压。\n");
            
            break;
        }
        case INA226_STATUS_SHUNT_VOLTAGE_UNDER_VOLTAGE :
        {
            ina226_interface_debug_print("ina226: 中断-并联电压欠压。\n");
            
            break;
        }
        case INA226_STATUS_BUS_VOLTAGE_OVER_VOLTAGE :
        {
            ina226_interface_debug_print("ina226: 中断-总线电压过压。\n");
            
            break;
        }
        case INA226_STATUS_BUS_VOLTAGE_UNDER_VOLTAGE :
        {
            ina226_interface_debug_print("ina226: 中断-总线电压欠压。\n");
            
            break;
        }
        case INA226_STATUS_POWER_OVER_LIMIT :
        {
            ina226_interface_debug_print("ina226: 中断-功率超限。\n");
            
            break;
        }
        default :
        {
            ina226_interface_debug_print("ina226: 未知代码。\n");
            
            break;
        }
    }
}
