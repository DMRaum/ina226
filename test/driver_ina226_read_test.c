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
 * @file      driver_ina226_read_test.c
 * @brief     ina226 读取测试源文件
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

#include "driver_ina226_read_test.h"

static ina226_handle_t gs_handle;        /**< ina226 句柄 */

/**
 * @brief     读取测试
 * @param[in] addr_pin IIC 设备地址
 * @param[in] r 外部电阻
 * @param[in] times 测试次数
 * @return    状态码
 *            - 0 成功
 *            - 1 测试失败
 * @note      无
 */
uint8_t ina226_read_test(ina226_address_t addr_pin, double r, uint32_t times)
{
    uint8_t res;
    uint32_t i;
    uint16_t calibration;
    ina226_info_t info;
    
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
    
    /* 开始读取测试 */
    ina226_interface_debug_print("ina226: 开始读取测试。\n");
    
    /* 设置地址引脚 */
    res = ina226_set_addr_pin(&gs_handle, addr_pin);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置地址引脚失败。\n");
       
        return 1;
    }

    /* 设置电阻 */
    res = ina226_set_resistance(&gs_handle, r);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置电阻失败。\n");
       
        return 1;
    }
    
    /* 初始化 */
    res = ina226_init(&gs_handle);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 初始化失败。\n");
       
        return 1;
    }
    
    /* 设置平均模式 16 */
    res = ina226_set_average_mode(&gs_handle, INA226_AVG_16);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置平均模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 设置总线电压转换时间 1.1ms */
    res = ina226_set_bus_voltage_conversion_time(&gs_handle, INA226_CONVERSION_TIME_1P1_MS);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置总线电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 设置分流电压转换时间 1.1ms */
    res = ina226_set_shunt_voltage_conversion_time(&gs_handle, INA226_CONVERSION_TIME_1P1_MS);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置分流电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 计算校准值 */
    res = ina226_calculate_calibration(&gs_handle, (uint16_t *)&calibration);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 计算校准值失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    res = ina226_set_calibration(&gs_handle, calibration);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置校准值失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 设置分流总线电压连续模式 */
    res = ina226_set_mode(&gs_handle, INA226_MODE_SHUNT_BUS_VOLTAGE_CONTINUOUS);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 分流总线电压连续测试 */
    ina226_interface_debug_print("ina226: 分流总线电压连续测试。\n");
    
    /* delay 1000ms */
    ina226_interface_delay_ms(1000);
    
    for (i = 0; i < times; i++)
    {
        int16_t s_raw;
        uint16_t u_raw;
        float m;
        
        /* 读取分流电压 */
        res = ina226_read_shunt_voltage(&gs_handle, (int16_t *)&s_raw, (float *)&m);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 读取分流电压失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        ina226_interface_debug_print("ina226: 分流电压是 %0.3fmV。\n", m);
        
        /* 读取总线电压 */
        res = ina226_read_bus_voltage(&gs_handle, (uint16_t *)&u_raw, (float *)&m);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 读取总线电压失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        ina226_interface_debug_print("ina226: 总线电压是 %0.3fmV。\n", m);
        
        /* 读取电流 */
        res = ina226_read_current(&gs_handle, (int16_t *)&s_raw, (float *)&m);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 读取电流失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        ina226_interface_debug_print("ina226: 电流是 %0.3fmA。\n", m);
        
        /* 读取功率 */
        res = ina226_read_power(&gs_handle, (uint16_t *)&u_raw, (float *)&m);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 读取功率失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        ina226_interface_debug_print("ina226: 功率是 %0.3fmW。\n", m);
        
        /* 延时 1000ms */
        ina226_interface_delay_ms(1000);
    }
    
    /* 设置电源关闭 */
    res = ina226_set_mode(&gs_handle, INA226_MODE_POWER_DOWN);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 分流和总线触发测试 */
    ina226_interface_debug_print("ina226: 分流和总线触发测试。\n");
    
    /* delay 1000ms */
    ina226_interface_delay_ms(1000);
    
    for (i = 0; i < times; i++)
    {
        int16_t s_raw;
        uint16_t u_raw;
        float m;
        
        /* 设置分流和总线触发 */
        res = ina226_set_mode(&gs_handle, INA226_MODE_SHUNT_BUS_VOLTAGE_TRIGGERED);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置模式失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 读取分流电压 */
        res = ina226_read_shunt_voltage(&gs_handle, (int16_t *)&s_raw, (float *)&m);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 读取分流电压失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        ina226_interface_debug_print("ina226: 分流电压是 %0.3fmV。\n", m);
        
        /* 读取总线电压 */
        res = ina226_read_bus_voltage(&gs_handle, (uint16_t *)&u_raw, (float *)&m);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 读取总线电压失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        ina226_interface_debug_print("ina226: 总线电压是 %0.3fmV。\n", m);
        
        /* 读取电流 */
        res = ina226_read_current(&gs_handle, (int16_t *)&s_raw, (float *)&m);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 读取电流失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        ina226_interface_debug_print("ina226: 电流是 %0.3fmA。\n", m);
        
        /* 读取功率 */
        res = ina226_read_power(&gs_handle, (uint16_t *)&u_raw, (float *)&m);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 读取功率失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        ina226_interface_debug_print("ina226: 功率是 %0.3fmW。\n", m);
        
        /* 延时 1000ms */
        ina226_interface_delay_ms(1000);
    }
    
    /* 完成读取测试 */
    (void)ina226_deinit(&gs_handle);
    ina226_interface_debug_print("ina226: 完成读取测试。\n");
    
    return 0;
}
