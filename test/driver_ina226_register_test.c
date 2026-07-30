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
 * @file      driver_ina226_register_test.c
 * @brief     ina226 寄存器测试源文件
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

#include "driver_ina226_register_test.h"
#include <stdlib.h>

static ina226_handle_t gs_handle;        /**< ina226 句柄 */

/**
 * @brief     寄存器测试
 * @param[in] addr_pin IIC 设备地址
 * @return    状态码
 *            - 0 成功
 *            - 1 测试失败
 * @note      无
 */
uint8_t ina226_register_test(ina226_address_t addr_pin)
{
    uint8_t res;
    uint8_t die_revision_id;
    float f;
    float f_check;
    double r;
    double r_check;
    uint16_t data;
    uint16_t data_check;
    uint16_t device_id;
    ina226_info_t info;
    ina226_address_t addr;
    ina226_avg_t mode;
    ina226_conversion_time_t t;
    ina226_mode_t chip_mode;
    ina226_bool_t enable;
    ina226_alert_polarity_t pin;
    
    /* 链接接口函数 */
    DRIVER_INA226_LINK_INIT(&gs_handle, ina226_handle_t);
    DRIVER_INA226_LINK_IIC_INIT(&gs_handle, ina226_interface_iic_init);
    DRIVER_INA226_LINK_IIC_DEINIT(&gs_handle, ina226_interface_iic_deinit);
    DRIVER_INA226_LINK_IIC_READ(&gs_handle, ina226_interface_iic_read);
    DRIVER_INA226_LINK_IIC_WRITE(&gs_handle, ina226_interface_iic_write);
    DRIVER_INA226_LINK_DELAY_MS(&gs_handle, ina226_interface_delay_ms);
    DRIVER_INA226_LINK_DEBUG_PRINT(&gs_handle, ina226_interface_debug_print);
    DRIVER_INA226_LINK_RECEIVE_CALLBACK(&gs_handle, ina226_interface_receive_callback);
    
    /* 获取信息 */
    res = ina226_info(&info);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取信息失败。\n");
       
        return 1;
    }
    else
    {
        /* 打印芯片信息 */
        ina226_interface_debug_print("ina226: 芯片是 %s。\n", info.chip_name);
        ina226_interface_debug_print("ina226: 制造商是 %s。\n", info.manufacturer_name);
        ina226_interface_debug_print("ina226: 接口是 %s。\n", info.interface);
        ina226_interface_debug_print("ina226: 驱动版本是 %d.%d。\n", info.driver_version / 1000, (info.driver_version % 1000) / 100);
        ina226_interface_debug_print("ina226: 最小电源电压是 %0.1fV。\n", info.supply_voltage_min_v);
        ina226_interface_debug_print("ina226: 最大电源电压是 %0.1fV。\n", info.supply_voltage_max_v);
        ina226_interface_debug_print("ina226: 最大电流是 %0.2fmA。\n", info.max_current_ma);
        ina226_interface_debug_print("ina226: 最高温度是 %0.1fC。\n", info.temperature_max);
        ina226_interface_debug_print("ina226: 最低温度是 %0.1fC。\n", info.temperature_min);
    }
    
    /* 开始寄存器测试 */
    ina226_interface_debug_print("ina226: 开始寄存器测试。\n");
    
    /* ina226_set_addr_pin/ina226_get_addr_pin 测试 */
    ina226_interface_debug_print("ina226: ina226_set_addr_pin/ina226_get_addr_pin 测试。\n");
    
    /* 设置地址 0 */
    res = ina226_set_addr_pin(&gs_handle, INA226_ADDRESS_0);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置地址引脚 0。\n");
    res = ina226_get_addr_pin(&gs_handle, &addr);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查地址引脚 %s。\n", addr == INA226_ADDRESS_0 ? "ok" : "error");
    
    /* 设置地址 1 */
    res = ina226_set_addr_pin(&gs_handle, INA226_ADDRESS_1);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置地址引脚 1。\n");
    res = ina226_get_addr_pin(&gs_handle, &addr);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查地址引脚 %s。\n", addr == INA226_ADDRESS_1 ? "ok" : "error");
    
    /* 设置地址 2 */
    res = ina226_set_addr_pin(&gs_handle, INA226_ADDRESS_2);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置地址引脚 2。\n");
    res = ina226_get_addr_pin(&gs_handle, &addr);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查地址引脚 %s。\n", addr == INA226_ADDRESS_2 ? "ok" : "error");
    
    /* 设置地址 3 */
    res = ina226_set_addr_pin(&gs_handle, INA226_ADDRESS_3);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置地址引脚 3。\n");
    res = ina226_get_addr_pin(&gs_handle, &addr);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查地址引脚 %s。\n", addr == INA226_ADDRESS_3 ? "ok" : "error");
    
    /* 设置地址 4 */
    res = ina226_set_addr_pin(&gs_handle, INA226_ADDRESS_4);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置地址引脚 4。\n");
    res = ina226_get_addr_pin(&gs_handle, &addr);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查地址引脚 %s。\n", addr == INA226_ADDRESS_4 ? "ok" : "error");
    
    /* 设置地址 5 */
    res = ina226_set_addr_pin(&gs_handle, INA226_ADDRESS_5);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置地址引脚 5。\n");
    res = ina226_get_addr_pin(&gs_handle, &addr);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查地址引脚 %s。\n", addr == INA226_ADDRESS_5 ? "ok" : "error");
    
    /* 设置地址 6 */
    res = ina226_set_addr_pin(&gs_handle, INA226_ADDRESS_6);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置地址引脚 6。\n");
    res = ina226_get_addr_pin(&gs_handle, &addr);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查地址引脚 %s。\n", addr == INA226_ADDRESS_6 ? "ok" : "error");
    
    /* 设置地址 7 */
    res = ina226_set_addr_pin(&gs_handle, INA226_ADDRESS_7);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置地址引脚 7。\n");
    res = ina226_get_addr_pin(&gs_handle, &addr);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查地址引脚 %s。\n", addr == INA226_ADDRESS_7 ? "ok" : "error");
    
    /* 设置地址 8 */
    res = ina226_set_addr_pin(&gs_handle, INA226_ADDRESS_8);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置地址引脚 8。\n");
    res = ina226_get_addr_pin(&gs_handle, &addr);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查地址引脚 %s。\n", addr == INA226_ADDRESS_8 ? "ok" : "error");
    
    /* 设置地址 9 */
    res = ina226_set_addr_pin(&gs_handle, INA226_ADDRESS_9);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置地址引脚 9。\n");
    res = ina226_get_addr_pin(&gs_handle, &addr);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查地址引脚 %s。\n", addr == INA226_ADDRESS_9 ? "ok" : "error");
    
    /* 设置地址 10 */
    res = ina226_set_addr_pin(&gs_handle, INA226_ADDRESS_A);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置地址引脚 10。\n");
    res = ina226_get_addr_pin(&gs_handle, &addr);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查地址引脚 %s。\n", addr == INA226_ADDRESS_A ? "ok" : "error");
    
    /* 设置地址 11 */
    res = ina226_set_addr_pin(&gs_handle, INA226_ADDRESS_B);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置地址引脚 11。\n");
    res = ina226_get_addr_pin(&gs_handle, &addr);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查地址引脚 %s。\n", addr == INA226_ADDRESS_B ? "ok" : "error");
    
    /* 设置地址 12 */
    res = ina226_set_addr_pin(&gs_handle, INA226_ADDRESS_C);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置地址引脚 12。\n");
    res = ina226_get_addr_pin(&gs_handle, &addr);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查地址引脚 %s。\n", addr == INA226_ADDRESS_C ? "ok" : "error");
    
    /* 设置地址 13 */
    res = ina226_set_addr_pin(&gs_handle, INA226_ADDRESS_D);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置地址引脚 13。\n");
    res = ina226_get_addr_pin(&gs_handle, &addr);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查地址引脚 %s。\n", addr == INA226_ADDRESS_D ? "ok" : "error");
    
    /* 设置地址 14 */
    res = ina226_set_addr_pin(&gs_handle, INA226_ADDRESS_E);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置地址引脚 14。\n");
    res = ina226_get_addr_pin(&gs_handle, &addr);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查地址引脚 %s。\n", addr == INA226_ADDRESS_E ? "ok" : "error");
    
    /* 设置地址 15 */
    res = ina226_set_addr_pin(&gs_handle, INA226_ADDRESS_F);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置地址引脚 15。\n");
    res = ina226_get_addr_pin(&gs_handle, &addr);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取地址引脚失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查地址引脚 %s。\n", addr == INA226_ADDRESS_F ? "ok" : "error");
    
    /* ina226_set_resistance/ina226_get_resistance 测试 */
    ina226_interface_debug_print("ina226: ina226_set_resistance/ina226_get_resistance 测试。\n");
    
    /* 生成电阻值 */
    r = (double)(rand() % 100) / 1000.0;
    res = ina226_set_resistance(&gs_handle, r);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置电阻失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置电阻 %f。\n", r);
    res = ina226_get_resistance(&gs_handle, (double *)&r_check);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取电阻失败。\n");
       
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查电阻 %f。\n", r_check);
    
    /* 设置地址引脚 */
    res = ina226_set_addr_pin(&gs_handle, addr_pin);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置地址引脚失败。\n");
       
        return 1;
    }
    
    /* 初始化 */
    res = ina226_init(&gs_handle);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 初始化失败。\n");
       
        return 1;
    }
    
    /* ina226_set_average_mode/ina226_get_average_mode 测试 */
    ina226_interface_debug_print("ina226: ina226_set_average_mode/ina226_get_average_mode 测试。\n");
    
    /* 设置平均 1 */
    res = ina226_set_average_mode(&gs_handle, INA226_AVG_1);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置平均模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置平均 1。\n");
    res = ina226_get_average_mode(&gs_handle, &mode);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取平均模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查平均模式 %s。\n", mode == INA226_AVG_1 ? "ok" : "error");
    
    /* 设置平均 4 */
    res = ina226_set_average_mode(&gs_handle, INA226_AVG_4);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置平均模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置平均 4。\n");
    res = ina226_get_average_mode(&gs_handle, &mode);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取平均模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查平均模式 %s。\n", mode == INA226_AVG_4 ? "ok" : "error");
    
    /* 设置平均 16 */
    res = ina226_set_average_mode(&gs_handle, INA226_AVG_16);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置平均模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置平均 16。\n");
    res = ina226_get_average_mode(&gs_handle, &mode);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取平均模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查平均模式 %s。\n", mode == INA226_AVG_16 ? "ok" : "error");
    
    /* 设置平均 64 */
    res = ina226_set_average_mode(&gs_handle, INA226_AVG_64);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置平均模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置平均 64。\n");
    res = ina226_get_average_mode(&gs_handle, &mode);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取平均模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查平均模式 %s。\n", mode == INA226_AVG_64 ? "ok" : "error");
    
    /* 设置平均 128 */
    res = ina226_set_average_mode(&gs_handle, INA226_AVG_128);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置平均模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置平均 128。\n");
    res = ina226_get_average_mode(&gs_handle, &mode);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取平均模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查平均模式 %s。\n", mode == INA226_AVG_128 ? "ok" : "error");
    
    /* 设置平均 256 */
    res = ina226_set_average_mode(&gs_handle, INA226_AVG_256);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置平均模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置平均 256。\n");
    res = ina226_get_average_mode(&gs_handle, &mode);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取平均模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查平均模式 %s。\n", mode == INA226_AVG_256 ? "ok" : "error");
    
    /* 设置平均 512 */
    res = ina226_set_average_mode(&gs_handle, INA226_AVG_512);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置平均模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置平均 512。\n");
    res = ina226_get_average_mode(&gs_handle, &mode);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取平均模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查平均模式 %s。\n", mode == INA226_AVG_512 ? "ok" : "error");
    
    /* 设置平均 1024 */
    res = ina226_set_average_mode(&gs_handle, INA226_AVG_1024);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置平均模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置平均 1024。\n");
    res = ina226_get_average_mode(&gs_handle, &mode);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取平均模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查平均模式 %s。\n", mode == INA226_AVG_1024 ? "ok" : "error");
    
    /* ina226_set_bus_voltage_conversion_time/ina226_get_bus_voltage_conversion_time 测试 */
    ina226_interface_debug_print("ina226: ina226_set_bus_voltage_conversion_time/ina226_get_bus_voltage_conversion_time 测试。\n");
    
    /* 设置总线电压转换时间 140us */
    res = ina226_set_bus_voltage_conversion_time(&gs_handle, INA226_CONVERSION_TIME_140_US);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置总线电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置总线电压转换时间 140us。\n");
    res = ina226_get_bus_voltage_conversion_time(&gs_handle, &t);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取总线电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查总线电压转换时间 %s。\n", t == INA226_CONVERSION_TIME_140_US ? "ok" : "error");
    
    /* 设置总线电压转换时间 204us */
    res = ina226_set_bus_voltage_conversion_time(&gs_handle, INA226_CONVERSION_TIME_204_US);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置总线电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置总线电压转换时间 204us。\n");
    res = ina226_get_bus_voltage_conversion_time(&gs_handle, &t);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取总线电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查总线电压转换时间 %s。\n", t == INA226_CONVERSION_TIME_204_US ? "ok" : "error");
    
    /* 设置总线电压转换时间 332us */
    res = ina226_set_bus_voltage_conversion_time(&gs_handle, INA226_CONVERSION_TIME_332_US);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置总线电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置总线电压转换时间 332us。\n");
    res = ina226_get_bus_voltage_conversion_time(&gs_handle, &t);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取总线电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查总线电压转换时间 %s。\n", t == INA226_CONVERSION_TIME_332_US ? "ok" : "error");
    
    /* 设置总线电压转换时间 588us */
    res = ina226_set_bus_voltage_conversion_time(&gs_handle, INA226_CONVERSION_TIME_588_US);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置总线电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置总线电压转换时间 588us。\n");
    res = ina226_get_bus_voltage_conversion_time(&gs_handle, &t);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取总线电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查总线电压转换时间 %s。\n", t == INA226_CONVERSION_TIME_588_US ? "ok" : "error");
    
    /* 设置总线电压转换时间 1.1ms */
    res = ina226_set_bus_voltage_conversion_time(&gs_handle, INA226_CONVERSION_TIME_1P1_MS);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置总线电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置总线电压转换时间 1.1ms。\n");
    res = ina226_get_bus_voltage_conversion_time(&gs_handle, &t);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取总线电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查总线电压转换时间 %s。\n", t == INA226_CONVERSION_TIME_1P1_MS ? "ok" : "error");
    
    /* 设置总线电压转换时间 2.116ms */
    res = ina226_set_bus_voltage_conversion_time(&gs_handle, INA226_CONVERSION_TIME_2P116_MS);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置总线电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置总线电压转换时间 2.116ms。\n");
    res = ina226_get_bus_voltage_conversion_time(&gs_handle, &t);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取总线电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查总线电压转换时间 %s。\n", t == INA226_CONVERSION_TIME_2P116_MS ? "ok" : "error");
    
    /* 设置总线电压转换时间 4.156ms */
    res = ina226_set_bus_voltage_conversion_time(&gs_handle, INA226_CONVERSION_TIME_4P156_MS);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置总线电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置总线电压转换时间 4.156ms。\n");
    res = ina226_get_bus_voltage_conversion_time(&gs_handle, &t);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取总线电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查总线电压转换时间 %s。\n", t == INA226_CONVERSION_TIME_4P156_MS ? "ok" : "error");
    
    /* 设置总线电压转换时间 8.244ms */
    res = ina226_set_bus_voltage_conversion_time(&gs_handle, INA226_CONVERSION_TIME_8P244_MS);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置总线电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置总线电压转换时间 8.244ms。\n");
    res = ina226_get_bus_voltage_conversion_time(&gs_handle, &t);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取总线电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查总线电压转换时间 %s。\n", t == INA226_CONVERSION_TIME_8P244_MS ? "ok" : "error");
    
    /* ina226_set_shunt_voltage_conversion_time/ina226_get_shunt_voltage_conversion_time 测试 */
    ina226_interface_debug_print("ina226: ina226_set_shunt_voltage_conversion_time/ina226_get_shunt_voltage_conversion_time 测试。\n");
    
    /* 设置分流电压转换时间 140us */
    res = ina226_set_shunt_voltage_conversion_time(&gs_handle, INA226_CONVERSION_TIME_140_US);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置分流电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置分流电压转换时间 140us。\n");
    res = ina226_get_shunt_voltage_conversion_time(&gs_handle, &t);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取分流电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查分流电压转换时间 %s。\n", t == INA226_CONVERSION_TIME_140_US ? "ok" : "error");
    
    /* 设置分流电压转换时间 204us */
    res = ina226_set_shunt_voltage_conversion_time(&gs_handle, INA226_CONVERSION_TIME_204_US);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置分流电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置分流电压转换时间 204us。\n");
    res = ina226_get_shunt_voltage_conversion_time(&gs_handle, &t);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取分流电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查分流电压转换时间 %s。\n", t == INA226_CONVERSION_TIME_204_US ? "ok" : "error");
    
    /* 设置分流电压转换时间 332us */
    res = ina226_set_shunt_voltage_conversion_time(&gs_handle, INA226_CONVERSION_TIME_332_US);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置分流电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置分流电压转换时间 332us。\n");
    res = ina226_get_shunt_voltage_conversion_time(&gs_handle, &t);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取分流电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查分流电压转换时间 %s。\n", t == INA226_CONVERSION_TIME_332_US ? "ok" : "error");
    
    /* 设置分流电压转换时间 588us */
    res = ina226_set_shunt_voltage_conversion_time(&gs_handle, INA226_CONVERSION_TIME_588_US);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置分流电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置分流电压转换时间 588us。\n");
    res = ina226_get_shunt_voltage_conversion_time(&gs_handle, &t);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取分流电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查分流电压转换时间 %s。\n", t == INA226_CONVERSION_TIME_588_US ? "ok" : "error");
    
    /* 设置分流电压转换时间 1.1ms */
    res = ina226_set_shunt_voltage_conversion_time(&gs_handle, INA226_CONVERSION_TIME_1P1_MS);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置分流电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置分流电压转换时间 1.1ms。\n");
    res = ina226_get_shunt_voltage_conversion_time(&gs_handle, &t);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取分流电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查分流电压转换时间 %s。\n", t == INA226_CONVERSION_TIME_1P1_MS ? "ok" : "error");
    
    /* 设置分流电压转换时间 2.116ms */
    res = ina226_set_shunt_voltage_conversion_time(&gs_handle, INA226_CONVERSION_TIME_2P116_MS);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置分流电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置分流电压转换时间 2.116ms。\n");
    res = ina226_get_shunt_voltage_conversion_time(&gs_handle, &t);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取分流电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查分流电压转换时间 %s。\n", t == INA226_CONVERSION_TIME_2P116_MS ? "ok" : "error");
    
    /* 设置分流电压转换时间 4.156ms */
    res = ina226_set_shunt_voltage_conversion_time(&gs_handle, INA226_CONVERSION_TIME_4P156_MS);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置分流电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置分流电压转换时间 4.156ms。\n");
    res = ina226_get_shunt_voltage_conversion_time(&gs_handle, &t);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取分流电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查分流电压转换时间 %s。\n", t == INA226_CONVERSION_TIME_4P156_MS ? "ok" : "error");
    
    /* 设置分流电压转换时间 8.244ms */
    res = ina226_set_shunt_voltage_conversion_time(&gs_handle, INA226_CONVERSION_TIME_8P244_MS);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置分流电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置分流电压转换时间 8.244ms。\n");
    res = ina226_get_shunt_voltage_conversion_time(&gs_handle, &t);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取分流电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查分流电压转换时间 %s。\n", t == INA226_CONVERSION_TIME_8P244_MS ? "ok" : "error");
    
    /* ina226_set_mode/ina226_get_mode 测试 */
    ina226_interface_debug_print("ina226: ina226_set_mode/ina226_get_mode 测试。\n");
    
    /* 设置电源关闭 */
    res = ina226_set_mode(&gs_handle, INA226_MODE_POWER_DOWN);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置模式电源关闭。\n");
    res = ina226_get_mode(&gs_handle, &chip_mode);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查模式 %s。\n", chip_mode == INA226_MODE_POWER_DOWN ? "ok" : "error");
    
    /* 设置关机 */
    res = ina226_set_mode(&gs_handle, INA226_MODE_SHUTDOWN);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置模式关机。\n");
    res = ina226_get_mode(&gs_handle, &chip_mode);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查模式 %s。\n", chip_mode == INA226_MODE_SHUTDOWN ? "ok" : "error");
    
    /* 设置分流电压连续模式 */
    res = ina226_set_mode(&gs_handle, INA226_MODE_SHUNT_VOLTAGE_CONTINUOUS);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置模式分流电压连续。\n");
    res = ina226_get_mode(&gs_handle, &chip_mode);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查模式 %s。\n", chip_mode == INA226_MODE_SHUNT_VOLTAGE_CONTINUOUS ? "ok" : "error");
    
    /* 设置总线电压连续模式 */
    res = ina226_set_mode(&gs_handle, INA226_MODE_BUS_VOLTAGE_CONTINUOUS);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置模式总线电压连续。\n");
    res = ina226_get_mode(&gs_handle, &chip_mode);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查模式 %s。\n", chip_mode == INA226_MODE_BUS_VOLTAGE_CONTINUOUS ? "ok" : "error");
    
    /* 设置分流和总线电压连续模式 */
    res = ina226_set_mode(&gs_handle, INA226_MODE_SHUNT_BUS_VOLTAGE_CONTINUOUS);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置模式分流和总线电压连续。\n");
    res = ina226_get_mode(&gs_handle, &chip_mode);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查模式 %s。\n", chip_mode == INA226_MODE_SHUNT_BUS_VOLTAGE_CONTINUOUS ? "ok" : "error");
    
    /* ina226_set_calibration/ina226_get_calibration 测试 */
    ina226_interface_debug_print("ina226: ina226_set_calibration/ina226_get_calibration 测试。\n");
    
    data = rand() % 0x7FFFU;
    res = ina226_set_calibration(&gs_handle, data);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置校准值失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置校准值 0x%04X。\n", data);
    res = ina226_get_calibration(&gs_handle, &data_check);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取校准值失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查校准值 %s。\n", data == data_check ? "ok" : "error");
    
    /* ina226_calculate_calibration 测试 */
    ina226_interface_debug_print("ina226: ina226_calculate_calibration 测试。\n");
    
    /* 计算校准值 */
    res = ina226_calculate_calibration(&gs_handle, &data_check);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 计算校准值失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 计算校准值为 0x%04X。\n", data_check);
    
    /* ina226_set_mask/ina226_get_mask 测试 */
    ina226_interface_debug_print("ina226: ina226_set_mask/ina226_get_mask 测试。\n");
    
    /* 使能分流电压过压 */
    res = ina226_set_mask(&gs_handle, INA226_MASK_SHUNT_VOLTAGE_OVER_VOLTAGE, INA226_BOOL_TRUE);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置掩码失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 使能分流电压过压。\n");
    res = ina226_get_mask(&gs_handle, INA226_MASK_SHUNT_VOLTAGE_OVER_VOLTAGE, &enable);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取掩码失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查掩码 %s。\n", enable == INA226_BOOL_TRUE ? "ok" : "error");
    
    /* 禁用分流电压过压 */
    res = ina226_set_mask(&gs_handle, INA226_MASK_SHUNT_VOLTAGE_OVER_VOLTAGE, INA226_BOOL_FALSE);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置掩码失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 禁用分流电压过压。\n");
    res = ina226_get_mask(&gs_handle, INA226_MASK_SHUNT_VOLTAGE_OVER_VOLTAGE, &enable);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取掩码失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查掩码 %s。\n", enable == INA226_BOOL_FALSE ? "ok" : "error");
    
    /* 使能分流电压欠压 */
    res = ina226_set_mask(&gs_handle, INA226_MASK_SHUNT_VOLTAGE_UNDER_VOLTAGE, INA226_BOOL_TRUE);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置掩码失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 使能分流电压欠压。\n");
    res = ina226_get_mask(&gs_handle, INA226_MASK_SHUNT_VOLTAGE_UNDER_VOLTAGE, &enable);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取掩码失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查掩码 %s。\n", enable == INA226_BOOL_TRUE ? "ok" : "error");
    
    /* 禁用分流电压欠压 */
    res = ina226_set_mask(&gs_handle, INA226_MASK_SHUNT_VOLTAGE_UNDER_VOLTAGE, INA226_BOOL_FALSE);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置掩码失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 禁用分流电压欠压。\n");
    res = ina226_get_mask(&gs_handle, INA226_MASK_SHUNT_VOLTAGE_UNDER_VOLTAGE, &enable);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取掩码失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查掩码 %s。\n", enable == INA226_BOOL_FALSE ? "ok" : "error");
    
    /* 使能总线电压过压 */
    res = ina226_set_mask(&gs_handle, INA226_MASK_BUS_VOLTAGE_OVER_VOLTAGE, INA226_BOOL_TRUE);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置掩码失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 使能总线电压过压。\n");
    res = ina226_get_mask(&gs_handle, INA226_MASK_BUS_VOLTAGE_OVER_VOLTAGE, &enable);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取掩码失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查掩码 %s。\n", enable == INA226_BOOL_TRUE ? "ok" : "error");
    
    /* 禁用总线电压过压 */
    res = ina226_set_mask(&gs_handle, INA226_MASK_BUS_VOLTAGE_OVER_VOLTAGE, INA226_BOOL_FALSE);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置掩码失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 禁用总线电压过压。\n");
    res = ina226_get_mask(&gs_handle, INA226_MASK_BUS_VOLTAGE_OVER_VOLTAGE, &enable);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取掩码失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查掩码 %s。\n", enable == INA226_BOOL_FALSE ? "ok" : "error");
    
    /* 使能总线电压欠压 */
    res = ina226_set_mask(&gs_handle, INA226_MASK_BUS_VOLTAGE_UNDER_VOLTAGE, INA226_BOOL_TRUE);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置掩码失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 使能总线电压欠压。\n");
    res = ina226_get_mask(&gs_handle, INA226_MASK_BUS_VOLTAGE_UNDER_VOLTAGE, &enable);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取掩码失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查掩码 %s。\n", enable == INA226_BOOL_TRUE ? "ok" : "error");
    
    /* 禁用总线电压欠压 */
    res = ina226_set_mask(&gs_handle, INA226_MASK_BUS_VOLTAGE_UNDER_VOLTAGE, INA226_BOOL_FALSE);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置掩码失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 禁用总线电压欠压。\n");
    res = ina226_get_mask(&gs_handle, INA226_MASK_BUS_VOLTAGE_UNDER_VOLTAGE, &enable);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取掩码失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查掩码 %s。\n", enable == INA226_BOOL_FALSE ? "ok" : "error");
    
    /* 使能功率超限 */
    res = ina226_set_mask(&gs_handle, INA226_MASK_POWER_OVER_LIMIT, INA226_BOOL_TRUE);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置掩码失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 使能功率超限。\n");
    res = ina226_get_mask(&gs_handle, INA226_MASK_POWER_OVER_LIMIT, &enable);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取掩码失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查掩码 %s。\n", enable == INA226_BOOL_TRUE ? "ok" : "error");
    
    /* 禁用功率超限 */
    res = ina226_set_mask(&gs_handle, INA226_MASK_POWER_OVER_LIMIT, INA226_BOOL_FALSE);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置掩码失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 禁用功率超限。\n");
    res = ina226_get_mask(&gs_handle, INA226_MASK_POWER_OVER_LIMIT, &enable);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取掩码失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查掩码 %s。\n", enable == INA226_BOOL_FALSE ? "ok" : "error");
    
    /* ina226_set_conversion_ready_alert_pin/ina226_get_conversion_ready_alert_pin 测试 */
    ina226_interface_debug_print("ina226: ina226_set_conversion_ready_alert_pin/ina226_get_conversion_ready_alert_pin 测试。\n");
    
    /* 使能转换就绪告警引脚 */
    res = ina226_set_conversion_ready_alert_pin(&gs_handle, INA226_BOOL_TRUE);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置转换就绪告警引脚失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 使能转换就绪告警引脚。\n");
    res = ina226_get_conversion_ready_alert_pin(&gs_handle, &enable);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取转换就绪告警引脚失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查转换就绪告警引脚 %s。\n", enable == INA226_BOOL_TRUE ? "ok" : "error");
    
    /* 禁用转换就绪告警引脚 */
    res = ina226_set_conversion_ready_alert_pin(&gs_handle, INA226_BOOL_FALSE);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置转换就绪告警引脚失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 禁用转换就绪告警引脚。\n");
    res = ina226_get_conversion_ready_alert_pin(&gs_handle, &enable);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取转换就绪告警引脚失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查转换就绪告警引脚 %s。\n", enable == INA226_BOOL_FALSE ? "ok" : "error");
    
    /* ina226_set_alert_polarity_pin/ina226_get_alert_polarity_pin 测试 */
    ina226_interface_debug_print("ina226: ina226_set_alert_polarity_pin/ina226_get_alert_polarity_pin 测试。\n");
    
    /* 设置告警极性引脚为正常 */
    res = ina226_set_alert_polarity_pin(&gs_handle, INA226_ALERT_POLARITY_NORMAL);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置告警极性引脚失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置告警极性引脚正常。\n");
    res = ina226_get_alert_polarity_pin(&gs_handle, &pin);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取告警极性引脚失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查告警极性引脚 %s。\n", pin == INA226_ALERT_POLARITY_NORMAL ? "ok" : "error");
    
    /* 设置告警极性引脚为反向 */
    res = ina226_set_alert_polarity_pin(&gs_handle, INA226_ALERT_POLARITY_INVERTED);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置告警极性引脚失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置告警极性引脚反向。\n");
    res = ina226_get_alert_polarity_pin(&gs_handle, &pin);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取告警极性引脚失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查告警极性引脚 %s。\n", pin == INA226_ALERT_POLARITY_INVERTED ? "ok" : "error");
    
    /* ina226_set_alert_latch/ina226_get_alert_latch 测试 */
    ina226_interface_debug_print("ina226: ina226_set_alert_latch/ina226_get_alert_latch 测试。\n");
    
    /* 使能告警锁存 */
    res = ina226_set_alert_latch(&gs_handle, INA226_BOOL_TRUE);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置告警锁存失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 使能告警锁存。\n");
    res = ina226_get_alert_latch(&gs_handle, &enable);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取告警锁存失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查告警锁存 %s。\n", enable == INA226_BOOL_TRUE ? "ok" : "error");
    
    /* 禁用告警锁存 */
    res = ina226_set_alert_latch(&gs_handle, INA226_BOOL_FALSE);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置告警锁存失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 禁用告警锁存。\n");
    res = ina226_get_alert_latch(&gs_handle, &enable);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取告警锁存失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查告警锁存 %s。\n", enable == INA226_BOOL_FALSE ? "ok" : "error");
    
    /* ina226_set_alert_limit/ina226_get_alert_limit 测试 */
    ina226_interface_debug_print("ina226: ina226_set_alert_limit/ina226_get_alert_limit 测试。\n");
    
    data = rand() % 0xFFFFU;
    res = ina226_set_alert_limit(&gs_handle, data);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置告警阈值失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置告警阈值 0x%04X。\n", data);
    res = ina226_get_alert_limit(&gs_handle, &data_check);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取告警阈值失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查告警阈值 %s。\n", data == data_check ? "ok" : "error");
    
    /* ina226_shunt_voltage_convert_to_register/ina226_shunt_voltage_convert_to_data 测试 */
    ina226_interface_debug_print("ina226: ina226_shunt_voltage_convert_to_register/ina226_shunt_voltage_convert_to_data 测试。\n");
    
    f = (float)(rand() % 1000) / 100.0f;
    res = ina226_shunt_voltage_convert_to_register(&gs_handle, f, &data);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 分流电压转换为寄存器失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置分流电压 %0.2fmV。\n", f);
    res = ina226_shunt_voltage_convert_to_data(&gs_handle, data, &f_check);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 分流电压转换为数据失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查分流电压 %0.2fmV。\n", f_check);
    
    /* ina226_bus_voltage_convert_to_register/ina226_bus_voltage_convert_to_data 测试 */
    ina226_interface_debug_print("ina226: ina226_bus_voltage_convert_to_register/ina226_bus_voltage_convert_to_data 测试。\n");
    
    f = (float)(rand() % 1000) / 100.0f;
    res = ina226_bus_voltage_convert_to_register(&gs_handle, f, &data);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 总线电压转换为寄存器失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置总线电压 %0.2fmV。\n", f);
    res = ina226_bus_voltage_convert_to_data(&gs_handle, data, &f_check);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 总线电压转换为数据失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查总线电压 %0.2fmV。\n", f_check);
    
    /* ina226_power_convert_to_register/ina226_power_convert_to_data 测试 */
    ina226_interface_debug_print("ina226: ina226_power_convert_to_register/ina226_power_convert_to_data 测试。\n");
    
    f = (float)(rand() % 1000) / 100.0f;
    res = ina226_power_convert_to_register(&gs_handle, f, &data);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 功率转换为寄存器失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 设置功率 %0.2fmW。\n", f);
    res = ina226_power_convert_to_data(&gs_handle, data, &f_check);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 功率转换为数据失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    ina226_interface_debug_print("ina226: 检查功率 %0.2fmW。\n", f_check);
    
    /* ina226_get_die_id 测试 */
    ina226_interface_debug_print("ina226: ina226_get_die_id 测试。\n");
    
    /* 获取芯片ID */
    res = ina226_get_die_id(&gs_handle, &device_id, &die_revision_id);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 获取芯片ID失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 输出 */
    ina226_interface_debug_print("ina226: 设备ID是 0x%04X。\n", device_id);
    
    /* 输出 */
    ina226_interface_debug_print("ina226: 芯片版本ID是 0x%02X。\n", die_revision_id);
    
    /* ina226_soft_reset 测试 */
    ina226_interface_debug_print("ina226: ina226_soft_reset 测试。\n");
    
    /* 软复位 */
    res = ina226_soft_reset(&gs_handle);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 软复位失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 完成寄存器测试 */
    (void)ina226_deinit(&gs_handle);
    ina226_interface_debug_print("ina226: 完成寄存器测试。\n");
    
    return 0;
}
