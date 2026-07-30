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
 * @file      driver_ina226_alert.c
 * @brief     ina226 告警驱动源文件
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

#include "driver_ina226_alert.h"

static ina226_handle_t gs_handle;        /**< ina226 句柄 */

/**
 * @brief  告警中断处理函数
 * @return 状态码
 *         - 0 成功
 *         - 1 运行失败
 * @note   无
 */
uint8_t ina226_alert_irq_handler(void)
{
    if (ina226_irq_handler(&gs_handle) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief     告警示例初始化
 * @param[in] addr_pin IIC 地址引脚
 * @param[in] r 参考电阻值
 * @param[in] mask 设置掩码
 * @param[in] threshold 设置阈值
 * @param[in] *callback 指向函数地址的指针
 * @return    状态码
 *            - 0 成功
 *            - 1 初始化失败
 * @note      无
 */
uint8_t ina226_alert_init(ina226_address_t addr_pin, double r, ina226_mask_t mask, float threshold, void (*callback)(uint8_t type))
{
    uint8_t res;
    uint16_t reg;
    uint16_t calibration;
    
    /* 链接接口函数 */
    DRIVER_INA226_LINK_INIT(&gs_handle, ina226_handle_t);
    DRIVER_INA226_LINK_IIC_INIT(&gs_handle, ina226_interface_iic_init);
    DRIVER_INA226_LINK_IIC_DEINIT(&gs_handle, ina226_interface_iic_deinit);
    DRIVER_INA226_LINK_IIC_READ(&gs_handle, ina226_interface_iic_read);
    DRIVER_INA226_LINK_IIC_WRITE(&gs_handle, ina226_interface_iic_write);
    DRIVER_INA226_LINK_DELAY_MS(&gs_handle, ina226_interface_delay_ms);
    DRIVER_INA226_LINK_DEBUG_PRINT(&gs_handle, ina226_interface_debug_print);
    DRIVER_INA226_LINK_RECEIVE_CALLBACK(&gs_handle, callback);
    
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
    
    /* 设置默认平均模式 */
    res = ina226_set_average_mode(&gs_handle, INA226_ALERT_DEFAULT_AVG_MODE);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置平均模式失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 设置默认总线电压转换时间 */
    res = ina226_set_bus_voltage_conversion_time(&gs_handle, INA226_ALERT_DEFAULT_BUS_VOLTAGE_CONVERSION_TIME);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置总线电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 设置默认分流电压转换时间 */
    res = ina226_set_shunt_voltage_conversion_time(&gs_handle, INA226_ALERT_DEFAULT_SHUNT_VOLTAGE_CONVERSION_TIME);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置分流电压转换时间失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 计算校准 */
    res = ina226_calculate_calibration(&gs_handle, (uint16_t *)&calibration);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 计算校准失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    res = ina226_set_calibration(&gs_handle, calibration);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置校准失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 设置默认转换就绪告警引脚 */
    res = ina226_set_conversion_ready_alert_pin(&gs_handle, INA226_ALERT_DEFAULT_READY_ALERT_PIN);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置转换就绪告警引脚失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 设置默认告警极性引脚 */
    res = ina226_set_alert_polarity_pin(&gs_handle, INA226_ALERT_DEFAULT_ALERT_POLARITY_NORMAL);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置告警极性引脚失败。\n");
        (void)ina226_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 设置默认告警锁存 */
    res = ina226_set_alert_latch(&gs_handle, INA226_ALERT_DEFAULT_ALERT_LATCH);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: 设置告警锁存失败。\n");
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
    
    /* 设置掩码 */
    if (mask == INA226_MASK_SHUNT_VOLTAGE_OVER_VOLTAGE)
    {
        /* 分流电压转换为寄存器值 */
        res = ina226_shunt_voltage_convert_to_register(&gs_handle, threshold, &reg);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 分流电压转换为寄存器失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 设置告警限制 */
        res = ina226_set_alert_limit(&gs_handle, reg);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置告警限制失败。\n");
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
    }
    else if (mask == INA226_MASK_SHUNT_VOLTAGE_UNDER_VOLTAGE)
    {
        /* 分流电压转换为寄存器值 */
        res = ina226_shunt_voltage_convert_to_register(&gs_handle, threshold, &reg);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 分流电压转换为寄存器失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 设置告警限制 */
        res = ina226_set_alert_limit(&gs_handle, reg);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置告警限制失败。\n");
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
    }
    else if (mask == INA226_MASK_BUS_VOLTAGE_OVER_VOLTAGE)
    {
        /* 总线电压转换为寄存器值 */
        res = ina226_bus_voltage_convert_to_register(&gs_handle, threshold, &reg);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 分流电压转换为寄存器失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 设置告警限制 */
        res = ina226_set_alert_limit(&gs_handle, reg);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置告警限制失败。\n");
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
    }
    else if (mask == INA226_MASK_BUS_VOLTAGE_UNDER_VOLTAGE)
    {
        /* 总线电压转换为寄存器值 */
        res = ina226_bus_voltage_convert_to_register(&gs_handle, threshold, &reg);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 分流电压转换为寄存器失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 设置告警限制 */
        res = ina226_set_alert_limit(&gs_handle, reg);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置告警限制失败。\n");
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
    }
    else
    {
        /* 功率转换为寄存器值 */
        res = ina226_power_convert_to_register(&gs_handle, threshold, &reg);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 分流电压转换为寄存器失败。\n");
            (void)ina226_deinit(&gs_handle);
            
            return 1;
        }
        
        /* 设置告警限制 */
        res = ina226_set_alert_limit(&gs_handle, reg);
        if (res != 0)
        {
            ina226_interface_debug_print("ina226: 设置告警限制失败。\n");
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
    }
    
    return 0;
}

/**
 * @brief      告警示例读取
 * @param[out] *mV 指向 mV 缓冲区的指针
 * @param[out] *mA 指向 mA 缓冲区的指针
 * @param[out] *mW 指向 mW 缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 读取失败
 * @note       无
 */
uint8_t ina226_alert_read(float *mV, float *mA, float *mW)
{
    uint8_t res;
    int16_t s_raw;
    uint16_t u_raw;
    
    /* 读取总线电压 */
    res = ina226_read_bus_voltage(&gs_handle, (uint16_t *)&u_raw, mV);
    if (res != 0)
    {
        return 1;
    }

    /* 读取电流 */
    res = ina226_read_current(&gs_handle, (int16_t *)&s_raw, mA);
    if (res != 0)
    {
        return 1;
    }

    /* 读取功率 */
    res = ina226_read_power(&gs_handle, (uint16_t *)&u_raw, mW);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief  告警示例反初始化
 * @return 状态码
 *         - 0 成功
 *         - 1 反初始化失败
 * @note   无
 */
uint8_t ina226_alert_deinit(void)
{
    uint8_t res;
    
    res = ina226_deinit(&gs_handle);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}
