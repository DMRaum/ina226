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

#include "driver_ina226_alert_test.h"

static ina226_handle_t gs_handle;      /**< ina226 句柄 */
static volatile uint16_t gs_flag;      /**< 标志 */

/**
 * @brief     接口接收回调函数
 * @param[in] type 中断类型
 * @note      无
 */
static void a_receive_callback(uint8_t type)
{
    switch (type)
    {
        case INA226_STATUS_SHUNT_VOLTAGE_OVER_VOLTAGE :
        {
            gs_flag |= 1 << 0;
            ina226_interface_debug_print("ina226: 中断 分流电压过压。\n");
            
            break;
        }
        case INA226_STATUS_SHUNT_VOLTAGE_UNDER_VOLTAGE :
        {
            gs_flag |= 1 << 1;
            ina226_interface_debug_print("ina226: 中断 分流电压欠压。\n");
            
            break;
        }
        case INA226_STATUS_BUS_VOLTAGE_OVER_VOLTAGE :
        {
            gs_flag |= 1 << 2;
            ina226_interface_debug_print("ina226: 中断 总线电压过压。\n");
            
            break;
        }
        case INA226_STATUS_BUS_VOLTAGE_UNDER_VOLTAGE :
        {
            gs_flag |= 1 << 3;
            ina226_interface_debug_print("ina226: 中断 总线电压欠压。\n");
            
            break;
        }
        case INA226_STATUS_POWER_OVER_LIMIT :
        {
            gs_flag |= 1 << 4;
            ina226_interface_debug_print("ina226: 中断 功率超限。\n");
            
            break;
        }
        default :
        {
            ina226_interface_debug_print("ina226: 未知代码。\n");
            
            break;
        }
    }
}

/**
 * @brief  告警测试中断处理函数
 * @return 状态码
 *         - 0 成功
 *         - 1 运行失败
 * @note   无
 */
uint8_t ina226_alert_test_irq_handler(void)
{
    if (ina226_irq_handler(&gs_handle) != 0)
    {
        return 1;
    }
    
    return 0;
}

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
uint8_t ina226_alert_test(ina226_address_t addr_pin, double r, ina226_mask_t mask, float threshold, uint32_t timeout_ms)
{
    uint8_t res;
    uint16_t reg;
    uint16_t flag;
    uint32_t timeout;
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
    DRIVER_INA226_LINK_RECEIVE_CALLBACK(&gs_handle, a_receive_callback);
    
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
    
    /* 开始告警测试 */
    ina226_interface_debug_print("ina226: 开始告警测试。\n");
    
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
    
    /* 禁用转换就绪告警引脚 */
    res = ina226_set_conversion_ready_alert_pin(&gs_handle, INA226_BOOL_FALSE);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置转换就绪告警引脚失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 设置告警极性引脚为正常 */
    res = ina226_set_alert_polarity_pin(&gs_handle, INA226_ALERT_POLARITY_NORMAL);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置告警极性引脚失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 禁用告警锁存 */
    res = ina226_set_alert_latch(&gs_handle, INA226_BOOL_FALSE);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置告警锁存失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    
    gs_flag = 0;
    if (mask == INA226_MASK_SHUNT_VOLTAGE_OVER_VOLTAGE)
    {
        /* 分流电压转换为寄存器值 */
        res = ina226_shunt_voltage_convert_to_register(&gs_handle, threshold, &reg);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 分流电压转换为寄存器值失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 设置告警阈值 */
        res = ina226_set_alert_limit(&gs_handle, reg);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置告警阈值失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 使能 */
        res = ina226_set_mask(&gs_handle, INA226_MASK_SHUNT_VOLTAGE_OVER_VOLTAGE, INA226_BOOL_TRUE);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置掩码失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 禁用 */
        res = ina226_set_mask(&gs_handle, INA226_MASK_SHUNT_VOLTAGE_UNDER_VOLTAGE, INA226_BOOL_FALSE);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置掩码失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 禁用 */
        res = ina226_set_mask(&gs_handle, INA226_MASK_BUS_VOLTAGE_OVER_VOLTAGE, INA226_BOOL_FALSE);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置掩码失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 禁用 */
        res = ina226_set_mask(&gs_handle, INA226_MASK_BUS_VOLTAGE_UNDER_VOLTAGE, INA226_BOOL_FALSE);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置掩码失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 禁用 */
        res = ina226_set_mask(&gs_handle, INA226_MASK_POWER_OVER_LIMIT, INA226_BOOL_FALSE);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置掩码失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        flag = 1 << 0;
    }
    else if (mask == INA226_MASK_SHUNT_VOLTAGE_UNDER_VOLTAGE)
    {
        /* 分流电压转换为寄存器值 */
        res = ina226_shunt_voltage_convert_to_register(&gs_handle, threshold, &reg);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 分流电压转换为寄存器值失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 设置告警阈值 */
        res = ina226_set_alert_limit(&gs_handle, reg);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置告警阈值失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 禁用 */
        res = ina226_set_mask(&gs_handle, INA226_MASK_SHUNT_VOLTAGE_OVER_VOLTAGE, INA226_BOOL_FALSE);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置掩码失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 使能 */
        res = ina226_set_mask(&gs_handle, INA226_MASK_SHUNT_VOLTAGE_UNDER_VOLTAGE, INA226_BOOL_TRUE);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置掩码失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 禁用 */
        res = ina226_set_mask(&gs_handle, INA226_MASK_BUS_VOLTAGE_OVER_VOLTAGE, INA226_BOOL_FALSE);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置掩码失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 禁用 */
        res = ina226_set_mask(&gs_handle, INA226_MASK_BUS_VOLTAGE_UNDER_VOLTAGE, INA226_BOOL_FALSE);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置掩码失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 禁用 */
        res = ina226_set_mask(&gs_handle, INA226_MASK_POWER_OVER_LIMIT, INA226_BOOL_FALSE);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置掩码失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        flag = 1 << 1;
    }
    else if (mask == INA226_MASK_BUS_VOLTAGE_OVER_VOLTAGE)
    {
        /* 总线电压转换为寄存器值 */
        res = ina226_bus_voltage_convert_to_register(&gs_handle, threshold, &reg);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 分流电压转换为寄存器值失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 设置告警阈值 */
        res = ina226_set_alert_limit(&gs_handle, reg);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置告警阈值失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 禁用 */
        res = ina226_set_mask(&gs_handle, INA226_MASK_SHUNT_VOLTAGE_OVER_VOLTAGE, INA226_BOOL_FALSE);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置掩码失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 禁用 */
        res = ina226_set_mask(&gs_handle, INA226_MASK_SHUNT_VOLTAGE_UNDER_VOLTAGE, INA226_BOOL_FALSE);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置掩码失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 使能 */
        res = ina226_set_mask(&gs_handle, INA226_MASK_BUS_VOLTAGE_OVER_VOLTAGE, INA226_BOOL_TRUE);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置掩码失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 禁用 */
        res = ina226_set_mask(&gs_handle, INA226_MASK_BUS_VOLTAGE_UNDER_VOLTAGE, INA226_BOOL_FALSE);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置掩码失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 禁用 */
        res = ina226_set_mask(&gs_handle, INA226_MASK_POWER_OVER_LIMIT, INA226_BOOL_FALSE);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置掩码失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        flag = 1 << 2;
    }
    else if (mask == INA226_MASK_BUS_VOLTAGE_UNDER_VOLTAGE)
    {
        /* 总线电压转换为寄存器值 */
        res = ina226_bus_voltage_convert_to_register(&gs_handle, threshold, &reg);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 分流电压转换为寄存器值失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 设置告警阈值 */
        res = ina226_set_alert_limit(&gs_handle, reg);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置告警阈值失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 禁用 */
        res = ina226_set_mask(&gs_handle, INA226_MASK_SHUNT_VOLTAGE_OVER_VOLTAGE, INA226_BOOL_FALSE);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置掩码失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 禁用 */
        res = ina226_set_mask(&gs_handle, INA226_MASK_SHUNT_VOLTAGE_UNDER_VOLTAGE, INA226_BOOL_FALSE);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置掩码失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 禁用 */
        res = ina226_set_mask(&gs_handle, INA226_MASK_BUS_VOLTAGE_OVER_VOLTAGE, INA226_BOOL_FALSE);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置掩码失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 使能 */
        res = ina226_set_mask(&gs_handle, INA226_MASK_BUS_VOLTAGE_UNDER_VOLTAGE, INA226_BOOL_TRUE);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置掩码失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 禁用 */
        res = ina226_set_mask(&gs_handle, INA226_MASK_POWER_OVER_LIMIT, INA226_BOOL_FALSE);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置掩码失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        flag = 1 << 3;
    }
    else
    {
        /* 功率转换为寄存器值 */
        res = ina226_power_convert_to_register(&gs_handle, threshold, &reg);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 分流电压转换为寄存器值失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 设置告警阈值 */
        res = ina226_set_alert_limit(&gs_handle, reg);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置告警阈值失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 禁用 */
        res = ina226_set_mask(&gs_handle, INA226_MASK_SHUNT_VOLTAGE_OVER_VOLTAGE, INA226_BOOL_FALSE);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置掩码失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 禁用 */
        res = ina226_set_mask(&gs_handle, INA226_MASK_SHUNT_VOLTAGE_UNDER_VOLTAGE, INA226_BOOL_FALSE);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置掩码失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 禁用 */
        res = ina226_set_mask(&gs_handle, INA226_MASK_BUS_VOLTAGE_OVER_VOLTAGE, INA226_BOOL_FALSE);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置掩码失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 禁用 */
        res = ina226_set_mask(&gs_handle, INA226_MASK_BUS_VOLTAGE_UNDER_VOLTAGE, INA226_BOOL_FALSE);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置掩码失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 使能 */
        res = ina226_set_mask(&gs_handle, INA226_MASK_POWER_OVER_LIMIT, INA226_BOOL_TRUE);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置掩码失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        flag = 1 << 4;
    }
    
    timeout = timeout_ms;
    while (timeout != 0)
    {
        if ((gs_flag & flag) != 0)
        {
            break;
        }
        timeout--;
        ina226_interface_delay_ms(1);
    }
    
    /* 检查超时 */
    if (timeout == 0)
    {
        ina226_interface_debug_print("ina226: 告警超时。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 完成告警测试 */
    (void)ina226_deinit(&gs_handle);
    ina226_interface_debug_print("ina226: 完成告警测试。\n");
    
    return 0;
}
