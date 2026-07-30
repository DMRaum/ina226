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
 * @file      driver_ina226.c
 * @brief     ina226 驱动源文件
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

#include "driver_ina226.h" 
#include <math.h>

/**
 * @brief 芯片信息定义
 */
#define CHIP_NAME                 "Texas Instruments INA226"        /**< 芯片名称 */
#define MANUFACTURER_NAME         "Texas Instruments"               /**< 制造商名称 */
#define SUPPLY_VOLTAGE_MIN        2.7f                              /**< 芯片最小供电电压 */
#define SUPPLY_VOLTAGE_MAX        5.5f                              /**< 芯片最大供电电压 */
#define MAX_CURRENT               0.33f                             /**< 芯片最大电流 */
#define TEMPERATURE_MIN           -40.0f                            /**< 芯片最小工作温度 */
#define TEMPERATURE_MAX           125.0f                            /**< 芯片最大工作温度 */
#define DRIVER_VERSION            1000                              /**< 驱动版本 */

/**
 * @brief 芯片寄存器定义
 */
#define INA226_REG_CONF                 0x00        /**< 配置寄存器 */
#define INA226_REG_SHUNT_VOLTAGE        0x01        /**< 分流电压寄存器 */
#define INA226_REG_BUS_VOLTAGE          0x02        /**< 总线电压寄存器 */
#define INA226_REG_POWER                0x03        /**< 功率寄存器 */
#define INA226_REG_CURRENT              0x04        /**< 电流寄存器 */
#define INA226_REG_CALIBRATION          0x05        /**< 校准寄存器 */
#define INA226_REG_MASK                 0x06        /**< 掩码寄存器 */
#define INA226_REG_ALERT_LIMIT          0x07        /**< 告警阈值寄存器 */
#define INA226_REG_MANUFACTURER         0xFE        /**< 制造商 ID 寄存器 */
#define INA226_REG_DIE                  0xFF        /**< 芯片 ID 寄存器 */

/**
 * @brief      IIC 接口读取字节
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[in]  reg IIC 寄存器地址
 * @param[out] *data 指向数据缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 读取失败
 * @note       无
 */
static uint8_t a_ina226_iic_read(ina226_handle_t *handle, uint8_t reg, uint16_t *data)
{
    uint8_t buf[2];
    
    memset(buf, 0, sizeof(uint8_t) * 2);                                        /* 清空缓冲区 */
    if (handle->iic_read(handle->iic_addr, reg, (uint8_t *)buf, 2) != 0)        /* 读取数据 */
    {
        return 1;                                                               /* 返回错误 */
    }
    else
    {
        *data = (uint16_t)buf[0] << 8 | buf[1];                                 /* 获取数据 */
        
        return 0;                                                               /* 成功返回 0 */
    }
}

/**
 * @brief     IIC 接口写入字节
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] reg IIC 寄存器地址
 * @param[in] data 写入的数据
 * @return    状态码
 *            - 0 成功
 *            - 1 写入失败
 * @note      无
 */
static uint8_t a_ina226_iic_write(ina226_handle_t *handle, uint8_t reg, uint16_t data)
{
    uint8_t buf[2];
    
    buf[0] = (uint8_t)((data >> 8) & 0xFF);                                      /* 获取高字节 */
    buf[1] = (uint8_t)((data >> 0) & 0xFF);                                      /* 获取低字节 */
    if (handle->iic_write(handle->iic_addr, reg, (uint8_t *)buf, 2) != 0)        /* 写入数据 */
    {
        return 1;                                                                /* 返回错误 */
    }
    else
    {
        return 0;                                                                /* 成功返回 0 */
    }
}

/**
 * @brief     设置电阻值
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] resistance 电流采样电阻值
 * @return    状态码
 *            - 0 成功
 *            - 2 句柄为空
 * @note      无
 */
uint8_t ina226_set_resistance(ina226_handle_t *handle, double resistance)
{
    if (handle == NULL)            /* 检查句柄 */
    {
        return 2;                  /* 返回错误 */
    }
    
    handle->r = resistance;        /* 设置电阻 */
    
    return 0;                      /* 成功返回 0 */
}

/**
 * @brief      获取电阻值
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *resistance 指向电流采样电阻值缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 句柄为空
 * @note       无
 */
uint8_t ina226_get_resistance(ina226_handle_t *handle, double *resistance)
{
    if (handle == NULL)            /* 检查句柄 */
    {
        return 2;                  /* 返回错误 */
    }
    
    *resistance = handle->r;       /* 获取电阻 */
    
    return 0;                      /* 成功返回 0 */
}

/**
 * @brief     设置 IIC 地址引脚
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] addr_pin 地址引脚
 * @return    状态码
 *            - 0 成功
 *            - 2 句柄为空
 * @note      无
 */
uint8_t ina226_set_addr_pin(ina226_handle_t *handle, ina226_address_t addr_pin)
{
    if (handle == NULL)                          /* 检查句柄 */
    {
        return 2;                                /* 返回错误 */
    }
    
    handle->iic_addr = (uint8_t)addr_pin;        /* 设置引脚 */
    
    return 0;                                    /* 成功返回 0 */
}

/**
 * @brief      获取 IIC 地址引脚
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *addr_pin 指向地址引脚缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 句柄为空
 * @note       无
 */
uint8_t ina226_get_addr_pin(ina226_handle_t *handle, ina226_address_t *addr_pin)
{
    if (handle == NULL)                                      /* 检查句柄 */
    {
        return 2;                                            /* 返回错误 */
    }
    
    *addr_pin = (ina226_address_t)(handle->iic_addr);        /* 获取引脚 */
    
    return 0;                                                /* 成功返回 0 */
}

/**
 * @brief     芯片软复位
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @return    状态码
 *            - 0 成功
 *            - 1 软复位失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 * @note      无
 */
uint8_t ina226_soft_reset(ina226_handle_t *handle)
{
    uint8_t res;
    uint16_t prev;
   
    if (handle == NULL)                                                        /* 检查句柄 */
    {
        return 2;                                                              /* 返回错误 */
    }
    if (handle->inited != 1)                                                   /* 检查句柄初始化 */
    {
        return 3;                                                              /* 返回错误 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_CONF, (uint16_t *)&prev);       /* 读取配置 */
    if (res != 0)                                                              /* 检查结果 */
    {
        handle->debug_print("ina226: 读取配置寄存器失败。\n");           /* 读取配置寄存器失败 */
       
        return 1;                                                              /* 返回错误 */
    }
    prev &= ~(1 << 15);                                                        /* 清除软复位 */
    prev |= 1 << 15;                                                           /* 设置软复位 */

    return a_ina226_iic_write(handle, INA226_REG_CONF, (uint16_t )prev);       /* 写入配置 */
}

/**
 * @brief     设置平均模式
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] mode 平均模式
 * @return    状态码
 *            - 0 成功
 *            - 1 设置平均模式失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 * @note      无
 */
uint8_t ina226_set_average_mode(ina226_handle_t *handle, ina226_avg_t mode)
{
    uint8_t res;
    uint16_t prev;
   
    if (handle == NULL)                                                        /* 检查句柄 */
    {
        return 2;                                                              /* 返回错误 */
    }
    if (handle->inited != 1)                                                   /* 检查句柄初始化 */
    {
        return 3;                                                              /* 返回错误 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_CONF, (uint16_t *)&prev);       /* 读取配置 */
    if (res != 0)                                                              /* 检查结果 */
    {
        handle->debug_print("ina226: 读取配置寄存器失败。\n");           /* 读取配置寄存器失败 */
       
        return 1;                                                              /* 返回错误 */
    }
    prev &= ~(0x7 << 9);                                                       /* 清除模式位 */
    prev |= mode << 9;                                                         /* 设置模式位 */

    return a_ina226_iic_write(handle, INA226_REG_CONF, (uint16_t )prev);       /* 写入配置 */
}

/**
 * @brief      获取平均模式
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *mode 指向平均模式缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取平均模式失败
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_get_average_mode(ina226_handle_t *handle, ina226_avg_t *mode)
{
    uint8_t res;
    uint16_t prev;
   
    if (handle == NULL)                                                        /* 检查句柄 */
    {
        return 2;                                                              /* 返回错误 */
    }
    if (handle->inited != 1)                                                   /* 检查句柄初始化 */
    {
        return 3;                                                              /* 返回错误 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_CONF, (uint16_t *)&prev);       /* 读取配置 */
    if (res != 0)                                                              /* 检查结果 */
    {
        handle->debug_print("ina226: 读取配置寄存器失败。\n");           /* 读取配置寄存器失败 */
       
        return 1;                                                              /* 返回错误 */
    }
    *mode = (ina226_avg_t)((prev >> 9) & 0x07);                                /* 获取模式 */
    
    return 0;                                                                  /* 成功返回 0 */
}

/**
 * @brief     设置总线电压转换时间
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] t 转换时间
 * @return    状态码
 *            - 0 成功
 *            - 1 设置总线电压转换时间失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 * @note      无
 */
uint8_t ina226_set_bus_voltage_conversion_time(ina226_handle_t *handle, ina226_conversion_time_t t)
{
    uint8_t res;
    uint16_t prev;
   
    if (handle == NULL)                                                        /* 检查句柄 */
    {
        return 2;                                                              /* 返回错误 */
    }
    if (handle->inited != 1)                                                   /* 检查句柄初始化 */
    {
        return 3;                                                              /* 返回错误 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_CONF, (uint16_t *)&prev);       /* 读取配置 */
    if (res != 0)                                                              /* 检查结果 */
    {
        handle->debug_print("ina226: 读取配置寄存器失败。\n");           /* 读取配置寄存器失败 */
       
        return 1;                                                              /* 返回错误 */
    }
    prev &= ~(7 << 6);                                                         /* 清除时间位 */
    prev |= t << 6;                                                            /* 设置时间位 */

    return a_ina226_iic_write(handle, INA226_REG_CONF, (uint16_t )prev);       /* 写入配置 */
}

/**
 * @brief      获取总线电压转换时间
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *t 指向转换时间缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取总线电压转换时间失败
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_get_bus_voltage_conversion_time(ina226_handle_t *handle, ina226_conversion_time_t *t)
{
    uint8_t res;
    uint16_t prev;
   
    if (handle == NULL)                                                        /* 检查句柄 */
    {
        return 2;                                                              /* 返回错误 */
    }
    if (handle->inited != 1)                                                   /* 检查句柄初始化 */
    {
        return 3;                                                              /* 返回错误 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_CONF, (uint16_t *)&prev);       /* 读取配置 */
    if (res != 0)                                                              /* 检查结果 */
    {
        handle->debug_print("ina226: 读取配置寄存器失败。\n");           /* 读取配置寄存器失败 */
       
        return 1;                                                              /* 返回错误 */
    }
    *t = (ina226_conversion_time_t)((prev >> 6) & 0x07);                       /* 获取时间 */
    
    return 0;                                                                  /* 成功返回 0 */
}

/**
 * @brief     设置分流电压转换时间
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] t 转换时间
 * @return    状态码
 *            - 0 成功
 *            - 1 设置分流电压转换时间失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 * @note      无
 */
uint8_t ina226_set_shunt_voltage_conversion_time(ina226_handle_t *handle, ina226_conversion_time_t t)
{
    uint8_t res;
    uint16_t prev;
   
    if (handle == NULL)                                                        /* 检查句柄 */
    {
        return 2;                                                              /* 返回错误 */
    }
    if (handle->inited != 1)                                                   /* 检查句柄初始化 */
    {
        return 3;                                                              /* 返回错误 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_CONF, (uint16_t *)&prev);       /* 读取配置 */
    if (res != 0)                                                              /* 检查结果 */
    {
        handle->debug_print("ina226: 读取配置寄存器失败。\n");           /* 读取配置寄存器失败 */
       
        return 1;                                                              /* 返回错误 */
    }
    prev &= ~(7 << 3);                                                         /* 清除时间位 */
    prev |= t << 3;                                                            /* 设置时间位 */

    return a_ina226_iic_write(handle, INA226_REG_CONF, (uint16_t )prev);       /* 写入配置 */
}

/**
 * @brief      获取分流电压转换时间
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *t 指向转换时间缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取分流电压转换时间失败
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_get_shunt_voltage_conversion_time(ina226_handle_t *handle, ina226_conversion_time_t *t)
{
    uint8_t res;
    uint16_t prev;
   
    if (handle == NULL)                                                        /* 检查句柄 */
    {
        return 2;                                                              /* 返回错误 */
    }
    if (handle->inited != 1)                                                   /* 检查句柄初始化 */
    {
        return 3;                                                              /* 返回错误 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_CONF, (uint16_t *)&prev);       /* 读取配置 */
    if (res != 0)                                                              /* 检查结果 */
    {
        handle->debug_print("ina226: 读取配置寄存器失败。\n");           /* 读取配置寄存器失败 */
       
        return 1;                                                              /* 返回错误 */
    }
    *t = (ina226_conversion_time_t)((prev >> 3) & 0x07);                       /* 获取时间 */
    
    return 0;                                                                  /* 成功返回 0 */
}

/**
 * @brief     设置模式
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] mode 芯片模式
 * @return    状态码
 *            - 0 成功
 *            - 1 设置模式失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 * @note      无
 */
uint8_t ina226_set_mode(ina226_handle_t *handle, ina226_mode_t mode)
{
    uint8_t res;
    uint16_t prev;
   
    if (handle == NULL)                                                        /* 检查句柄 */
    {
        return 2;                                                              /* 返回错误 */
    }
    if (handle->inited != 1)                                                   /* 检查句柄初始化 */
    {
        return 3;                                                              /* 返回错误 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_CONF, (uint16_t *)&prev);       /* 读取配置 */
    if (res != 0)                                                              /* 检查结果 */
    {
        handle->debug_print("ina226: 读取配置寄存器失败。\n");           /* 读取配置寄存器失败 */
       
        return 1;                                                              /* 返回错误 */
    }
    prev &= ~(0x7 << 0);                                                       /* 清除模式位 */
    prev |= mode << 0;                                                         /* 设置模式位 */
    res = a_ina226_iic_write(handle, INA226_REG_CONF, (uint16_t )prev);        /* 写入配置 */
    if (res != 0)                                                              /* 检查结果 */
    {
        handle->debug_print("ina226: 写入配置寄存器失败。\n");          /* 写入配置寄存器失败 */
       
        return 1;                                                              /* 返回错误 */
    }
    if ((mode == INA226_MODE_SHUNT_VOLTAGE_TRIGGERED) ||
        (mode == INA226_MODE_BUS_VOLTAGE_TRIGGERED) ||
        (mode == INA226_MODE_SHUNT_BUS_VOLTAGE_TRIGGERED))                     /* 检查触发模式 */
    {
        handle->trigger = 1;                                                   /* 设置 1 */
    }
    else
    {
        handle->trigger = 0;                                                   /* 设置 0 */
    }
    
    return 0;                                                                  /* 成功返回 0 */
}

/**
 * @brief      获取模式
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *mode 指向芯片模式缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取模式失败
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_get_mode(ina226_handle_t *handle, ina226_mode_t *mode)
{
    uint8_t res;
    uint16_t prev;
   
    if (handle == NULL)                                                        /* 检查句柄 */
    {
        return 2;                                                              /* 返回错误 */
    }
    if (handle->inited != 1)                                                   /* 检查句柄初始化 */
    {
        return 3;                                                              /* 返回错误 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_CONF, (uint16_t *)&prev);       /* 读取配置 */
    if (res != 0)                                                              /* 检查结果 */
    {
        handle->debug_print("ina226: 读取配置寄存器失败。\n");           /* 读取配置寄存器失败 */
       
        return 1;                                                              /* 返回错误 */
    }
    *mode = (ina226_mode_t)((prev >> 0) & 0x7);                                /* 获取模式 */
    
    return 0;                                                                  /* 成功返回 0 */
}

/**
 * @brief      获取芯片 ID
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *device_id 指向设备 ID 缓冲区的指针
 * @param[out] *die_revision_id 指向芯片修订 ID 缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取芯片 ID 失败
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_get_die_id(ina226_handle_t *handle, uint16_t *device_id, uint8_t *die_revision_id)
{
    uint8_t res;
    uint16_t prev;
   
    if (handle == NULL)                                                       /* 检查句柄 */
    {
        return 2;                                                             /* 返回错误 */
    }
    if (handle->inited != 1)                                                  /* 检查句柄初始化 */
    {
        return 3;                                                             /* 返回错误 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_DIE, (uint16_t *)&prev);       /* 读取配置 */
    if (res != 0)                                                             /* 检查结果 */
    {
        handle->debug_print("ina226: 读取芯片 ID 寄存器失败。\n");            /* 读取芯片 ID 寄存器失败 */
       
        return 1;                                                             /* 返回错误 */
    }
    *device_id = (prev >> 4) & 0xFFF;                                         /* 获取设备 ID */
    *die_revision_id = prev & 0xF;                                            /* 获取芯片修订 ID */
    
    return 0;                                                                 /* 成功返回 0 */
}

/**
 * @brief      读取分流电压
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *raw 指向原始数据缓冲区的指针
 * @param[out] *mV 指向转换后数据缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 读取分流电压失败
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 *             - 4 数学溢出
 *             - 5 读取超时
 * @note       无
 */
uint8_t ina226_read_shunt_voltage(ina226_handle_t *handle, int16_t *raw, float *mV)
{
    uint8_t res;
    union
    {
        uint16_t u;
        int16_t s;
    } u;
    uint16_t prev;
    
    if (handle == NULL)                                                                 /* 检查句柄 */
    {
        return 2;                                                                       /* 返回错误 */
    }
    if (handle->inited != 1)                                                            /* 检查句柄初始化 */
    {
        return 3;                                                                       /* 返回错误 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_MASK, (uint16_t *)&prev);                /* 读取掩码 */
    if (res != 0)                                                                       /* 检查结果 */
    {
        handle->debug_print("ina226: 读取掩码寄存器失败。\n");                    /* 读取掩码寄存器失败 */
       
        return 1;                                                                       /* 返回错误 */
    }
    if ((prev & (1 << 2)) != 0)                                                         /* 检查数学溢出 */
    {
        handle->debug_print("ina226: 数学溢出。\n");                                /* 数学溢出 */
       
        return 4;                                                                       /* 返回错误 */
    }
    if (handle->trigger != 0)                                                           /* 触发模式 */
    {
        uint16_t i;
        uint16_t timeout;
        
        if ((prev & (1 << 3)) == 0)                                                     /* 检查上次掩码转换就绪标志 */
        {
            timeout = INA226_READ_TIMEOUT;                                              /* 设置超时 */
            for (i = 0; i< timeout; i++)                                                /* 循环所有 */
            {
                res = a_ina226_iic_read(handle, INA226_REG_MASK, (uint16_t *)&prev);    /* 读取掩码 */
                if (res != 0)                                                           /* 检查结果 */
                {
                    handle->debug_print("ina226: 读取掩码寄存器失败。\n");        /* 读取掩码寄存器失败 */
                   
                    return 1;                                                           /* 返回错误 */
                }
                if ((prev & (1 << 3)) != 0)                                             /* 检查转换就绪标志 */
                {
                    break;                                                              /* 跳出 */
                }
                handle->delay_ms(1);                                                    /* 延时 1ms */
            }
            if (i >= timeout)                                                           /* 检查超时 */
            {
                handle->debug_print("ina226: 读取超时。\n");                         /* 超时 */
               
                return 5;                                                               /* 返回错误 */
            }
        }
        handle->trigger = 0;                                                            /* 设置 0 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_SHUNT_VOLTAGE, (uint16_t *)&u.u);        /* 读取分流电压 */
    if (res != 0)                                                                       /* 检查结果 */
    {
        handle->debug_print("ina226: 读取分流电压寄存器失败。\n");                      /* 读取分流电压寄存器失败 */
       
        return 1;                                                                       /* 返回错误 */
    }
    *raw = u.s;                                                                         /* 设置原始数据 */
    *mV = (float)(*raw) / 400.0f;                                                       /* 设置转换后数据 */
    
    return 0;                                                                           /* 成功返回 0 */
}

/**
 * @brief      读取总线电压
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *raw 指向原始数据缓冲区的指针
 * @param[out] *mV 指向转换后数据缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 读取总线电压失败
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 *             - 4 数学溢出
 *             - 5 读取超时
 * @note       无
 */
uint8_t ina226_read_bus_voltage(ina226_handle_t *handle, uint16_t *raw, float *mV)
{
    uint8_t res;
    uint16_t prev;
   
    if (handle == NULL)                                                                 /* 检查句柄 */
    {
        return 2;                                                                       /* 返回错误 */
    }
    if (handle->inited != 1)                                                            /* 检查句柄初始化 */
    {
        return 3;                                                                       /* 返回错误 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_MASK, (uint16_t *)&prev);                /* 读取掩码 */
    if (res != 0)                                                                       /* 检查结果 */
    {
        handle->debug_print("ina226: 读取掩码寄存器失败。\n");                    /* 读取掩码寄存器失败 */
       
        return 1;                                                                       /* 返回错误 */
    }
    if ((prev & (1 << 2)) != 0)                                                         /* 检查数学溢出 */
    {
        handle->debug_print("ina226: 数学溢出。\n");                                /* 数学溢出 */
       
        return 4;                                                                       /* 返回错误 */
    }
    if (handle->trigger != 0)                                                           /* 触发模式 */
    {
        uint16_t i;
        uint16_t timeout;
        
        if ((prev & (1 << 3)) == 0)                                                     /* 检查上次掩码转换就绪标志 */
        {
            timeout = INA226_READ_TIMEOUT;                                              /* 设置超时 */
            for (i = 0; i< timeout; i++)                                                /* 循环所有 */
            {
                res = a_ina226_iic_read(handle, INA226_REG_MASK, (uint16_t *)&prev);    /* 读取掩码 */
                if (res != 0)                                                           /* 检查结果 */
                {
                    handle->debug_print("ina226: 读取掩码寄存器失败。\n");        /* 读取掩码寄存器失败 */
                   
                    return 1;                                                           /* 返回错误 */
                }
                if ((prev & (1 << 3)) != 0)                                             /* 检查转换就绪标志 */
                {
                    break;                                                              /* 跳出 */
                }
                handle->delay_ms(1);                                                    /* 延时 1ms */
            }
            if (i >= timeout)                                                           /* 检查超时 */
            {
                handle->debug_print("ina226: 读取超时。\n");                         /* 超时 */
               
                return 5;                                                               /* 返回错误 */
            }
        }
        handle->trigger = 0;                                                            /* 设置 0 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_BUS_VOLTAGE, (uint16_t *)raw);           /* 读取总线电压 */
    if (res != 0)                                                                       /* 检查结果 */
    {
        handle->debug_print("ina226: 读取总线电压寄存器失败。\n");                      /* 读取总线电压寄存器失败 */
       
        return 1;                                                                       /* 返回错误 */
    }
    *mV = (float)(*raw) * 1.25f;                                                        /* 设置转换后数据 */
    
    return 0;                                                                           /* 成功返回 0 */
}

/**
 * @brief      读取电流
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *raw 指向原始数据缓冲区的指针
 * @param[out] *mA 指向转换后数据缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 读取电流失败
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 *             - 4 数学溢出
 *             - 5 读取超时
 * @note       无
 */
uint8_t ina226_read_current(ina226_handle_t *handle, int16_t *raw, float *mA)
{
    uint8_t res;
    union
    {
        uint16_t u;
        int16_t s;
    } u;
    uint16_t prev;
   
    if (handle == NULL)                                                                 /* 检查句柄 */
    {
        return 2;                                                                       /* 返回错误 */
    }
    if (handle->inited != 1)                                                            /* 检查句柄初始化 */
    {
        return 3;                                                                       /* 返回错误 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_MASK, (uint16_t *)&prev);                /* 读取掩码 */
    if (res != 0)                                                                       /* 检查结果 */
    {
        handle->debug_print("ina226: 读取掩码寄存器失败。\n");                    /* 读取掩码寄存器失败 */
       
        return 1;                                                                       /* 返回错误 */
    }
    if ((prev & (1 << 2)) != 0)                                                         /* 检查数学溢出 */
    {
        handle->debug_print("ina226: 数学溢出。\n");                                /* 数学溢出 */
       
        return 4;                                                                       /* 返回错误 */
    }
    if (handle->trigger != 0)                                                           /* 触发模式 */
    {
        uint16_t i;
        uint16_t timeout;
        
        if ((prev & (1 << 3)) == 0)                                                     /* 检查上次掩码转换就绪标志 */
        {
            timeout = INA226_READ_TIMEOUT;                                              /* 设置超时 */
            for (i = 0; i< timeout; i++)                                                /* 循环所有 */
            {
                res = a_ina226_iic_read(handle, INA226_REG_MASK, (uint16_t *)&prev);    /* 读取掩码 */
                if (res != 0)                                                           /* 检查结果 */
                {
                    handle->debug_print("ina226: 读取掩码寄存器失败。\n");        /* 读取掩码寄存器失败 */
                   
                    return 1;                                                           /* 返回错误 */
                }
                if ((prev & (1 << 3)) != 0)                                             /* 检查转换就绪标志 */
                {
                    break;                                                              /* 跳出 */
                }
                handle->delay_ms(1);                                                    /* 延时 1ms */
            }
            if (i >= timeout)                                                           /* 检查超时 */
            {
                handle->debug_print("ina226: 读取超时。\n");                         /* 超时 */
               
                return 5;                                                               /* 返回错误 */
            }
        }
        handle->trigger = 0;                                                            /* 设置 0 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_CURRENT, (uint16_t *)&u.u);              /* 读取电流 */
    if (res != 0)                                                                       /* 检查结果 */
    {
        handle->debug_print("ina226: 读取电流寄存器失败。\n");                          /* 读取电流寄存器失败 */
       
        return 1;                                                                       /* 返回错误 */
    }
    *raw = u.s;                                                                         /* 设置原始数据 */
    *mA = (float)((double)(*raw) * handle->current_lsb * 1000);                         /* 设置转换后数据 */
    
    return 0;                                                                           /* 成功返回 0 */
}

/**
 * @brief      读取功率
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *raw 指向原始数据缓冲区的指针
 * @param[out] *mW 指向转换后数据缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 读取功率失败
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 *             - 4 数学溢出
 *             - 5 读取超时
 * @note       无
 */
uint8_t ina226_read_power(ina226_handle_t *handle, uint16_t *raw, float *mW)
{
    uint8_t res;
    uint16_t prev;
   
    if (handle == NULL)                                                                 /* 检查句柄 */
    {
        return 2;                                                                       /* 返回错误 */
    }
    if (handle->inited != 1)                                                            /* 检查句柄初始化 */
    {
        return 3;                                                                       /* 返回错误 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_MASK, (uint16_t *)&prev);                /* 读取掩码 */
    if (res != 0)                                                                       /* 检查结果 */
    {
        handle->debug_print("ina226: 读取掩码寄存器失败。\n");                    /* 读取掩码寄存器失败 */
       
        return 1;                                                                       /* 返回错误 */
    }
    if ((prev & (1 << 2)) != 0)                                                         /* 检查数学溢出 */
    {
        handle->debug_print("ina226: 数学溢出。\n");                                /* 数学溢出 */
       
        return 4;                                                                       /* 返回错误 */
    }
    if (handle->trigger != 0)                                                           /* 触发模式 */
    {
        uint16_t i;
        uint16_t timeout;
        
        if ((prev & (1 << 3)) == 0)                                                     /* 检查上次掩码转换就绪标志 */
        {
            timeout = INA226_READ_TIMEOUT;                                              /* 设置超时 */
            for (i = 0; i< timeout; i++)                                                /* 循环所有 */
            {
                res = a_ina226_iic_read(handle, INA226_REG_MASK, (uint16_t *)&prev);    /* 读取掩码 */
                if (res != 0)                                                           /* 检查结果 */
                {
                    handle->debug_print("ina226: 读取掩码寄存器失败。\n");        /* 读取掩码寄存器失败 */
                   
                    return 1;                                                           /* 返回错误 */
                }
                if ((prev & (1 << 3)) != 0)                                             /* 检查转换就绪标志 */
                {
                    break;                                                              /* 跳出 */
                }
                handle->delay_ms(1);                                                    /* 延时 1ms */
            }
            if (i >= timeout)                                                           /* 检查超时 */
            {
                handle->debug_print("ina226: 读取超时。\n");                         /* 超时 */
               
                return 5;                                                               /* 返回错误 */
            }
        }
        handle->trigger = 0;                                                            /* 设置 0 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_POWER, (uint16_t *)raw);                 /* 读取功率 */
    if (res != 0)                                                                       /* 检查结果 */
    {
        handle->debug_print("ina226: 读取功率寄存器失败。\n");                   /* 读取功率寄存器失败 */
       
        return 1;                                                                       /* 返回错误 */
    }
    *mW = (float)((double)(*raw) * handle->current_lsb * 25.0 * 1000.0);                /* 设置转换后数据 */
    
    return 0;                                                                           /* 成功返回 0 */
}

/**
 * @brief      获取校准值
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *data 指向校准数据缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取校准值失败
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_get_calibration(ina226_handle_t *handle, uint16_t *data)
{
    uint8_t res;
   
    if (handle == NULL)                                                              /* 检查句柄 */
    {
        return 2;                                                                    /* 返回错误 */
    }
    if (handle->inited != 1)                                                         /* 检查句柄初始化 */
    {
        return 3;                                                                    /* 返回错误 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_CALIBRATION, (uint16_t *)data);       /* 读取校准 */
    if (res != 0)                                                                    /* 检查结果 */
    {
        handle->debug_print("ina226: 读取校准寄存器失败。\n");          /* 读取校准寄存器失败 */
       
        return 1;                                                                    /* 返回错误 */
    }
    
    return 0;                                                                        /* 成功返回 0 */
}

/**
 * @brief      计算校准值
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *calibration 指向校准数据缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 计算校准值失败
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 *             - 4 电阻值不能为零
 * @note       无
 */
uint8_t ina226_calculate_calibration(ina226_handle_t *handle, uint16_t *calibration)
{
    double v;
   
    if (handle == NULL)                                                /* 检查句柄 */
    {
        return 2;                                                      /* 返回错误 */
    }
    if (handle->inited != 1)                                           /* 检查句柄初始化 */
    {
        return 3;                                                      /* 返回错误 */
    }
    if ((handle->r >= -0.000001f)  && (handle->r <= 0.000001f))        /* 检查电阻值 */
    {
        handle->debug_print("ina226: 电阻值不能为零。\n");             /* 电阻值不能为零 */
       
        return 4;                                                      /* 返回错误 */
    }
    
    v = 0.08192;                                                       /* 设置最大范围 */
    handle->current_lsb = v / handle->r / pow(2.0, 15.0);              /* 电流最低有效位 */
    *calibration = (uint16_t)(0.00512 / (v / pow(2.0, 15.0)));         /* 设置校准值 */
    
    return 0;                                                          /* 成功返回 0 */
}

/**
 * @brief     设置校准值
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] data 校准数据
 * @return    状态码
 *            - 0 成功
 *            - 1 设置校准值失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 * @note      无
 */
uint8_t ina226_set_calibration(ina226_handle_t *handle, uint16_t data)
{
    uint8_t res;
   
    if (handle == NULL)                                                             /* 检查句柄 */
    {
        return 2;                                                                   /* 返回错误 */
    }
    if (handle->inited != 1)                                                        /* 检查句柄初始化 */
    {
        return 3;                                                                   /* 返回错误 */
    }
    
    res = a_ina226_iic_write(handle, INA226_REG_CALIBRATION, data);                 /* 写入校准 */
    if (res != 0)                                                                   /* 检查结果 */
    {
        handle->debug_print("ina226: 写入校准寄存器失败。\n");        /* 写入校准寄存器失败 */
       
        return 1;                                                                   /* 返回错误 */
    }
    
    return 0;                                                                       /* 成功返回 0 */
}

/**
 * @brief     启用或禁用掩码
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] mask 设置掩码
 * @param[in] enable 布尔值
 * @return    状态码
 *            - 0 成功
 *            - 1 设置掩码失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 * @note      无
 */
uint8_t ina226_set_mask(ina226_handle_t *handle, ina226_mask_t mask, ina226_bool_t enable)
{
    uint8_t res;
    uint16_t prev;
   
    if (handle == NULL)                                                         /* 检查句柄 */
    {
        return 2;                                                               /* 返回错误 */
    }
    if (handle->inited != 1)                                                    /* 检查句柄初始化 */
    {
        return 3;                                                               /* 返回错误 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_MASK, (uint16_t *)&prev);        /* 读取掩码 */
    if (res != 0)                                                               /* 检查结果 */
    {
        handle->debug_print("ina226: 读取掩码寄存器失败。\n");            /* 读取掩码寄存器失败 */
       
        return 1;                                                               /* 返回错误 */
    }
    prev &= ~(1 << mask);                                                       /* 清除设置 */
    prev |= (enable << mask);                                                   /* 设置掩码 */
    res = a_ina226_iic_write(handle, INA226_REG_MASK, prev);                    /* 写入掩码 */
    if (res != 0)                                                               /* 检查结果 */
    {
        handle->debug_print("ina226: 写入掩码寄存器失败。\n");           /* 写入掩码寄存器失败 */
       
        return 1;                                                               /* 返回错误 */
    }
    
    return 0;                                                                   /* 成功返回 0 */
}

/**
 * @brief      获取掩码
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[in]  mask 设置掩码
 * @param[out] *enable 指向布尔值缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取掩码失败
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_get_mask(ina226_handle_t *handle, ina226_mask_t mask, ina226_bool_t *enable)
{
    uint8_t res;
    uint16_t prev;
   
    if (handle == NULL)                                                         /* 检查句柄 */
    {
        return 2;                                                               /* 返回错误 */
    }
    if (handle->inited != 1)                                                    /* 检查句柄初始化 */
    {
        return 3;                                                               /* 返回错误 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_MASK, (uint16_t *)&prev);        /* 读取掩码 */
    if (res != 0)                                                               /* 检查结果 */
    {
        handle->debug_print("ina226: 读取掩码寄存器失败。\n");            /* 读取掩码寄存器失败 */
       
        return 1;                                                               /* 返回错误 */
    }
    *enable = (ina226_bool_t)((prev >> mask) & 0x01);                           /* 获取布尔值 */
    
    return 0;                                                                   /* 成功返回 0 */
}

/**
 * @brief     启用或禁用转换就绪告警引脚
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] enable 布尔值
 * @return    状态码
 *            - 0 成功
 *            - 1 设置转换就绪告警引脚失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 * @note      无
 */
uint8_t ina226_set_conversion_ready_alert_pin(ina226_handle_t *handle, ina226_bool_t enable)
{
    uint8_t res;
    uint16_t prev;
   
    if (handle == NULL)                                                         /* 检查句柄 */
    {
        return 2;                                                               /* 返回错误 */
    }
    if (handle->inited != 1)                                                    /* 检查句柄初始化 */
    {
        return 3;                                                               /* 返回错误 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_MASK, (uint16_t *)&prev);        /* 读取掩码 */
    if (res != 0)                                                               /* 检查结果 */
    {
        handle->debug_print("ina226: 读取掩码寄存器失败。\n");            /* 读取掩码寄存器失败 */
       
        return 1;                                                               /* 返回错误 */
    }
    prev &= ~(1 << 10);                                                         /* 清除设置 */
    prev |= (enable << 10);                                                     /* 设置掩码 */
    res = a_ina226_iic_write(handle, INA226_REG_MASK, prev);                    /* 写入掩码 */
    if (res != 0)                                                               /* 检查结果 */
    {
        handle->debug_print("ina226: 写入掩码寄存器失败。\n");           /* 写入掩码寄存器失败 */
       
        return 1;                                                               /* 返回错误 */
    }
    
    return 0;                                                                   /* 成功返回 0 */
}

/**
 * @brief      获取转换就绪告警引脚状态
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *enable 指向布尔值缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取转换就绪告警引脚失败
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_get_conversion_ready_alert_pin(ina226_handle_t *handle, ina226_bool_t *enable)
{
    uint8_t res;
    uint16_t prev;
   
    if (handle == NULL)                                                         /* 检查句柄 */
    {
        return 2;                                                               /* 返回错误 */
    }
    if (handle->inited != 1)                                                    /* 检查句柄初始化 */
    {
        return 3;                                                               /* 返回错误 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_MASK, (uint16_t *)&prev);        /* 读取掩码 */
    if (res != 0)                                                               /* 检查结果 */
    {
        handle->debug_print("ina226: 读取掩码寄存器失败。\n");            /* 读取掩码寄存器失败 */
       
        return 1;                                                               /* 返回错误 */
    }
    *enable = (ina226_bool_t)((prev >> 10) & 0x01);                             /* 获取布尔值 */
    
    return 0;                                                                   /* 成功返回 0 */
}

/**
 * @brief     设置告警极性引脚
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] pin 告警极性引脚
 * @return    状态码
 *            - 0 成功
 *            - 1 设置告警极性引脚失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 * @note      无
 */
uint8_t ina226_set_alert_polarity_pin(ina226_handle_t *handle, ina226_alert_polarity_t pin)
{
    uint8_t res;
    uint16_t prev;
   
    if (handle == NULL)                                                         /* 检查句柄 */
    {
        return 2;                                                               /* 返回错误 */
    }
    if (handle->inited != 1)                                                    /* 检查句柄初始化 */
    {
        return 3;                                                               /* 返回错误 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_MASK, (uint16_t *)&prev);        /* 读取掩码 */
    if (res != 0)                                                               /* 检查结果 */
    {
        handle->debug_print("ina226: 读取掩码寄存器失败。\n");            /* 读取掩码寄存器失败 */
       
        return 1;                                                               /* 返回错误 */
    }
    prev &= ~(1 << 1);                                                          /* 清除设置 */
    prev |= (pin << 1);                                                         /* 设置掩码 */
    res = a_ina226_iic_write(handle, INA226_REG_MASK, prev);                    /* 写入掩码 */
    if (res != 0)                                                               /* 检查结果 */
    {
        handle->debug_print("ina226: 写入掩码寄存器失败。\n");           /* 写入掩码寄存器失败 */
       
        return 1;                                                               /* 返回错误 */
    }
    
    return 0;                                                                   /* 成功返回 0 */
}

/**
 * @brief      获取告警极性引脚
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *pin 指向告警极性引脚缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取告警极性引脚失败
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_get_alert_polarity_pin(ina226_handle_t *handle, ina226_alert_polarity_t *pin)
{
    uint8_t res;
    uint16_t prev;
   
    if (handle == NULL)                                                         /* 检查句柄 */
    {
        return 2;                                                               /* 返回错误 */
    }
    if (handle->inited != 1)                                                    /* 检查句柄初始化 */
    {
        return 3;                                                               /* 返回错误 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_MASK, (uint16_t *)&prev);        /* 读取掩码 */
    if (res != 0)                                                               /* 检查结果 */
    {
        handle->debug_print("ina226: 读取掩码寄存器失败。\n");            /* 读取掩码寄存器失败 */
       
        return 1;                                                               /* 返回错误 */
    }
    *pin = (ina226_alert_polarity_t)((prev >> 1) & 0x01);                       /* 获取告警极性引脚 */
    
    return 0;                                                                   /* 成功返回 0 */
}

/**
 * @brief     启用或禁用告警锁存
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] enable 布尔值
 * @return    状态码
 *            - 0 成功
 *            - 1 设置告警锁存失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 * @note      无
 */
uint8_t ina226_set_alert_latch(ina226_handle_t *handle, ina226_bool_t enable)
{
    uint8_t res;
    uint16_t prev;
   
    if (handle == NULL)                                                         /* 检查句柄 */
    {
        return 2;                                                               /* 返回错误 */
    }
    if (handle->inited != 1)                                                    /* 检查句柄初始化 */
    {
        return 3;                                                               /* 返回错误 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_MASK, (uint16_t *)&prev);        /* 读取掩码 */
    if (res != 0)                                                               /* 检查结果 */
    {
        handle->debug_print("ina226: 读取掩码寄存器失败。\n");            /* 读取掩码寄存器失败 */
       
        return 1;                                                               /* 返回错误 */
    }
    prev &= ~(1 << 0);                                                          /* 清除设置 */
    prev |= (enable << 0);                                                      /* 设置掩码 */
    res = a_ina226_iic_write(handle, INA226_REG_MASK, prev);                    /* 写入掩码 */
    if (res != 0)                                                               /* 检查结果 */
    {
        handle->debug_print("ina226: 写入掩码寄存器失败。\n");           /* 写入掩码寄存器失败 */
       
        return 1;                                                               /* 返回错误 */
    }
    
    return 0;                                                                   /* 成功返回 0 */
}

/**
 * @brief      获取告警锁存状态
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *enable 指向布尔值缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取告警锁存失败
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_get_alert_latch(ina226_handle_t *handle, ina226_bool_t *enable)
{
    uint8_t res;
    uint16_t prev;
   
    if (handle == NULL)                                                         /* 检查句柄 */
    {
        return 2;                                                               /* 返回错误 */
    }
    if (handle->inited != 1)                                                    /* 检查句柄初始化 */
    {
        return 3;                                                               /* 返回错误 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_MASK, (uint16_t *)&prev);        /* 读取掩码 */
    if (res != 0)                                                               /* 检查结果 */
    {
        handle->debug_print("ina226: 读取掩码寄存器失败。\n");            /* 读取掩码寄存器失败 */
       
        return 1;                                                               /* 返回错误 */
    }
    *enable = (ina226_bool_t)((prev >> 0) & 0x01);                              /* 获取布尔值 */
    
    return 0;                                                                   /* 成功返回 0 */
}

/**
 * @brief     设置告警阈值
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] reg 设置寄存器
 * @return    状态码
 *            - 0 成功
 *            - 1 设置告警阈值失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 * @note      无
 */
uint8_t ina226_set_alert_limit(ina226_handle_t *handle, uint16_t reg)
{
    uint8_t res;
   
    if (handle == NULL)                                                             /* 检查句柄 */
    {
        return 2;                                                                   /* 返回错误 */
    }
    if (handle->inited != 1)                                                        /* 检查句柄初始化 */
    {
        return 3;                                                                   /* 返回错误 */
    }
    
    res = a_ina226_iic_write(handle, INA226_REG_ALERT_LIMIT, reg);                  /* 写入配置 */
    if (res != 0)                                                                   /* 检查结果 */
    {
        handle->debug_print("ina226: 写入告警阈值寄存器失败。\n");        /* 写入告警阈值寄存器失败 */
       
        return 1;                                                                   /* 返回错误 */
    }
    
    return 0;                                                                       /* 成功返回 0 */
}

/**
 * @brief      获取告警阈值
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[out] *reg 指向寄存器缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 获取告警阈值失败
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_get_alert_limit(ina226_handle_t *handle, uint16_t *reg)
{
    uint8_t res;
   
    if (handle == NULL)                                                            /* 检查句柄 */
    {
        return 2;                                                                  /* 返回错误 */
    }
    if (handle->inited != 1)                                                       /* 检查句柄初始化 */
    {
        return 3;                                                                  /* 返回错误 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_ALERT_LIMIT, reg);                  /* 读取配置 */
    if (res != 0)                                                                  /* 检查结果 */
    {
        handle->debug_print("ina226: 读取告警阈值寄存器失败。\n");        /* 读取告警阈值寄存器失败 */
       
        return 1;                                                                  /* 返回错误 */
    }
    
    return 0;                                                                      /* 成功返回 0 */
}

/**
 * @brief      将分流电压转换为寄存器原始数据
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[in]  mV 毫伏
 * @param[out] *reg 指向寄存器原始缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_shunt_voltage_convert_to_register(ina226_handle_t *handle, float mV, uint16_t *reg)
{
    if (handle == NULL)                  /* 检查句柄 */
    {
        return 2;                        /* 返回错误 */
    }
    if (handle->inited != 1)             /* 检查句柄初始化 */
    {
        return 3;                        /* 返回错误 */
    }
    
    *reg = (uint16_t)(mV * 400.0f);      /* 将实际数据转换为寄存器数据 */
    
    return 0;                            /* 成功返回 0 */
}

/**
 * @brief      将寄存器原始数据转换为分流电压
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[in]  reg 寄存器原始数据
 * @param[out] *mV 指向毫伏缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_shunt_voltage_convert_to_data(ina226_handle_t *handle, uint16_t reg, float *mV)
{
    if (handle == NULL)               /* 检查句柄 */
    {
        return 2;                     /* 返回错误 */
    }
    if (handle->inited != 1)          /* 检查句柄初始化 */
    {
        return 3;                     /* 返回错误 */
    }
    
    *mV = (float)(reg) / 400.0f;      /* 将寄存器数据转换为实际数据 */
    
    return 0;                         /* 成功返回 0 */
}

/**
 * @brief      将总线电压转换为寄存器原始数据
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[in]  mV 毫伏
 * @param[out] *reg 指向寄存器原始缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_bus_voltage_convert_to_register(ina226_handle_t *handle, float mV, uint16_t *reg)
{
    if (handle == NULL)                   /* 检查句柄 */
    {
        return 2;                         /* 返回错误 */
    }
    if (handle->inited != 1)              /* 检查句柄初始化 */
    {
        return 3;                         /* 返回错误 */
    }
    
    *reg = (uint16_t)(mV / 1.25f);        /* 将实际数据转换为寄存器数据 */
    
    return 0;                             /* 成功返回 0 */
}

/**
 * @brief      将寄存器原始数据转换为总线电压
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[in]  reg 寄存器原始数据
 * @param[out] *mV 指向毫伏缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_bus_voltage_convert_to_data(ina226_handle_t *handle, uint16_t reg, float *mV)
{
    if (handle == NULL)                /* 检查句柄 */
    {
        return 2;                      /* 返回错误 */
    }
    if (handle->inited != 1)           /* 检查句柄初始化 */
    {
        return 3;                      /* 返回错误 */
    }
    
    *mV = (float)(reg) * 1.25f;        /* 将寄存器数据转换为实际数据 */
    
    return 0;                          /* 成功返回 0 */
}

/**
 * @brief      将功率转换为寄存器原始数据
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[in]  mW 毫瓦
 * @param[out] *reg 指向寄存器原始缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_power_convert_to_register(ina226_handle_t *handle, float mW, uint16_t *reg)
{
    if (handle == NULL)                                                             /* 检查句柄 */
    {
        return 2;                                                                   /* 返回错误 */
    }
    if (handle->inited != 1)                                                        /* 检查句柄初始化 */
    {
        return 3;                                                                   /* 返回错误 */
    }
    
    *reg = (uint16_t)((double)(mW) / (handle->current_lsb * 25.0 * 1000.0));        /* 设置转换后的数据 */
    
    return 0;                                                                       /* 成功返回 0 */
}

/**
 * @brief      将寄存器原始数据转换为功率
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[in]  reg 寄存器原始数据
 * @param[out] *mW 指向毫瓦缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_power_convert_to_data(ina226_handle_t *handle, uint16_t reg, float *mW)
{
    if (handle == NULL)                                                        /* 检查句柄 */
    {
        return 2;                                                              /* 返回错误 */
    }
    if (handle->inited != 1)                                                   /* 检查句柄初始化 */
    {
        return 3;                                                              /* 返回错误 */
    }
    
    *mW = (float)((double)(reg) * handle->current_lsb * 25.0 * 1000.0);        /* 设置转换后的数据 */
    
    return 0;                                                                  /* 成功返回 0 */
}

/**
 * @brief     中断处理函数
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @return    状态码
 *            - 0 成功
 *            - 1 运行失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 * @note      无
 */
uint8_t ina226_irq_handler(ina226_handle_t *handle)
{
    uint8_t res;
    uint16_t prev;
   
    if (handle == NULL)                                                                     /* 检查句柄 */
    {
        return 2;                                                                           /* 返回错误 */
    }
    if (handle->inited != 1)                                                                /* 检查句柄初始化 */
    {
        return 3;                                                                           /* 返回错误 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_MASK, (uint16_t *)&prev);                    /* 读取掩码 */
    if (res != 0)                                                                           /* 检查结果 */
    {
        handle->debug_print("ina226: 读取掩码寄存器失败。\n");                        /* 读取掩码寄存器失败 */
       
        return 1;                                                                           /* 返回错误 */
    }
    if ((prev & (1 << 4)) != 0)                                                             /* 检查告警标志 */
    {
        if ((prev & (1 << 15)) != 0)                                                        /* 分流电压过压 */
        {
            if (handle->receive_callback != NULL)                                           /* 检查非空 */
            {
                handle->receive_callback(INA226_STATUS_SHUNT_VOLTAGE_OVER_VOLTAGE);         /* 运行回调 */
            }
        }
        else if ((prev & (1 << 14)) != 0)                                                   /* 分流电压欠压 */
        {
            if (handle->receive_callback != NULL)                                           /* 检查非空 */
            {
                handle->receive_callback(INA226_STATUS_SHUNT_VOLTAGE_UNDER_VOLTAGE);        /* 运行回调 */
            }
        }
        else if ((prev & (1 << 13)) != 0)                                                   /* 总线电压过压 */
        {
            if (handle->receive_callback != NULL)                                           /* 检查非空 */
            {
                handle->receive_callback(INA226_STATUS_BUS_VOLTAGE_OVER_VOLTAGE);           /* 运行回调 */
            }
        }
        else if ((prev & (1 << 12)) != 0)                                                   /* 总线电压欠压 */
        {
            if (handle->receive_callback != NULL)                                           /* 检查非空 */
            {
                handle->receive_callback(INA226_STATUS_BUS_VOLTAGE_UNDER_VOLTAGE);          /* 运行回调 */
            }
        }
        else if ((prev & (1 << 11)) != 0)                                                   /* 功率超限 */
        {
            if (handle->receive_callback != NULL)                                           /* 检查非空 */
            {
                handle->receive_callback(INA226_STATUS_POWER_OVER_LIMIT);                   /* 运行回调 */
            }
        }
        else
        {
                                                                                            /* 无操作 */
        }
    }
    
    return 0;                                                                               /* 成功返回 0 */
}

/**
 * @brief     初始化芯片
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @return    状态码
 *            - 0 成功
 *            - 1 IIC 初始化失败
 *            - 2 句柄为空
 *            - 3 链接函数为空
 *            - 4 ID 无效
 *            - 5 软复位失败
 * @note      无
 */
uint8_t ina226_init(ina226_handle_t *handle)
{
    uint8_t res;
    uint16_t prev;
    
    if (handle == NULL)                                                                /* 检查句柄 */
    {
        return 2;                                                                      /* 返回错误 */
    }
    if (handle->debug_print == NULL)                                                   /* 检查 debug_print */
    {
        return 3;                                                                      /* 返回错误 */
    }
    if (handle->iic_init == NULL)                                                      /* 检查 iic_init */
    {
        handle->debug_print("ina226: iic_init 为空。\n");                            /* iic_init 为空 */
        
        return 3;                                                                      /* 返回错误 */
    }
    if (handle->iic_deinit == NULL)                                                    /* 检查 iic_deinit */
    {
        handle->debug_print("ina226: iic_deinit 为空。\n");                          /* iic_deinit 为空 */
        
        return 3;                                                                      /* 返回错误 */
    }
    if (handle->iic_read == NULL)                                                      /* 检查 iic_read */
    {
        handle->debug_print("ina226: iic_read 为空。\n");                            /* iic_read 为空 */
        
        return 3;                                                                      /* 返回错误 */
    }
    if (handle->iic_write == NULL)                                                     /* 检查 iic_write */
    {
        handle->debug_print("ina226: iic_write 为空。\n");                           /* iic_write 为空 */
        
        return 3;                                                                      /* 返回错误 */
    }
    if (handle->delay_ms == NULL)                                                      /* 检查 delay_ms */
    {
        handle->debug_print("ina226: delay_ms 为空。\n");                            /* delay_ms 为空 */
        
        return 3;                                                                      /* 返回错误 */
    }
    if (handle->receive_callback == NULL)                                              /* 检查 receive_callback */
    {
        handle->debug_print("ina226: receive_callback 为空。\n");                    /* receive_callback 为空 */
        
        return 3;                                                                      /* 返回错误 */
    }
    
    if (handle->iic_init() != 0)                                                       /* IIC 初始化 */
    {
        handle->debug_print("ina226: IIC 初始化失败。\n");                             /* IIC 初始化失败 */
        
        return 1;                                                                      /* 返回错误 */
    }
    res = a_ina226_iic_read(handle, INA226_REG_MANUFACTURER, (uint16_t *)&prev);       /* 读取制造商 */
    if (res != 0)                                                                      /* 检查结果 */
    {
        handle->debug_print("ina226: 读取制造商 ID 失败。\n");                    /* 读取制造商失败 */
        (void)handle->iic_deinit();                                                    /* IIC 反初始化 */
       
        return 4;                                                                      /* 返回错误 */
    }
    if (prev != 0x5449)                                                                /* 检查 ID */
    {
        handle->debug_print("ina226: ID 无效。\n");                               /* ID 无效 */
        (void)handle->iic_deinit();                                                    /* IIC 反初始化 */
       
        return 4;                                                                      /* 返回错误 */
    }
    res = a_ina226_iic_read(handle, INA226_REG_CONF, (uint16_t *)&prev);               /* 读取配置 */
    if (res != 0)                                                                      /* 检查结果 */
    {
        handle->debug_print("ina226: 读取配置寄存器失败。\n");                   /* 读取配置寄存器失败 */
        (void)handle->iic_deinit();                                                    /* IIC 反初始化 */
       
        return 5;                                                                      /* 返回错误 */
    }
    prev |= 1 << 15;                                                                   /* 设置位 */
    res = a_ina226_iic_write(handle, INA226_REG_CONF, prev);                           /* 写入配置 */
    if (res != 0)                                                                      /* 检查结果 */
    {
        handle->debug_print("ina226: 写入配置寄存器失败。\n");                  /* 写入配置寄存器失败 */
        (void)handle->iic_deinit();                                                    /* IIC 反初始化 */
        
        return 5;                                                                      /* 返回错误 */
    }
    handle->delay_ms(10);                                                              /* 延时 10ms */
    res = a_ina226_iic_read(handle, INA226_REG_CONF, (uint16_t *)&prev);               /* 读取配置 */
    if (res != 0)                                                                      /* 检查结果 */
    {
        handle->debug_print("ina226: 读取配置寄存器失败。\n");                   /* 读取配置寄存器失败 */
        (void)handle->iic_deinit();                                                    /* IIC 反初始化 */
        
        return 5;                                                                      /* 返回错误 */
    }
    if ((prev & (1 << 15)) != 0)                                                       /* 检查结果 */
    {
        handle->debug_print("ina226: 软复位失败。\n");                           /* 软复位失败 */
        (void)handle->iic_deinit();                                                    /* IIC 反初始化 */
        
        return 5;                                                                      /* 返回错误 */
    }
    handle->trigger = 0;                                                               /* 无 */
    handle->inited = 1;                                                                /* 标记已初始化 */
    
    return 0;                                                                          /* 成功返回 0 */
}

/**
 * @brief     关闭芯片
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @return    状态码
 *            - 0 成功
 *            - 1 IIC 反初始化失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 *            - 4 断电失败
 * @note      无
 */
uint8_t ina226_deinit(ina226_handle_t *handle)
{
    uint8_t res;
    uint16_t prev;
   
    if (handle == NULL)                                                        /* 检查句柄 */
    {
        return 2;                                                              /* 返回错误 */
    }
    if (handle->inited != 1)                                                   /* 检查句柄初始化 */
    {
        return 3;                                                              /* 返回错误 */
    }
    
    res = a_ina226_iic_read(handle, INA226_REG_CONF, (uint16_t *)&prev);       /* 读取配置 */
    if (res != 0)                                                              /* 检查结果 */
    {
        handle->debug_print("ina226: 读取配置寄存器失败。\n");           /* 读取配置寄存器失败 */
       
        return 4;                                                              /* 返回错误 */
    }
    prev &= ~(0x07);                                                           /* 清除模式 */
    res = a_ina226_iic_write(handle, INA226_REG_CONF, (uint16_t )prev);        /* 写入配置 */
    if (res != 0)                                                              /* 检查结果 */
    {
        handle->debug_print("ina226: 写入配置寄存器失败。\n");          /* 写入配置寄存器失败 */
       
        return 4;                                                              /* 返回错误 */
    }
    res = handle->iic_deinit();                                                /* IIC 反初始化 */
    if (res != 0)                                                              /* 检查结果 */
    {
        handle->debug_print("ina226: IIC 反初始化失败。\n");                   /* IIC 反初始化失败 */
       
        return 1;                                                              /* 返回错误 */
    }
    
    return 0;                                                                  /* 成功返回 0 */
}

/**
 * @brief     设置芯片寄存器
 * @param[in] *handle 指向 ina226 句柄结构的指针
 * @param[in] reg 寄存器地址
 * @param[in] data 写入的数据
 * @return    状态码
 *            - 0 成功
 *            - 1 写入失败
 *            - 2 句柄为空
 *            - 3 句柄未初始化
 * @note      无
 */
uint8_t ina226_set_reg(ina226_handle_t *handle, uint8_t reg, uint16_t data)
{
    if (handle == NULL)                                 /* 检查句柄 */
    {
        return 2;                                       /* 返回错误 */
    }
    if (handle->inited != 1)                            /* 检查句柄初始化 */
    {
        return 3;                                       /* 返回错误 */
    }
    
    return a_ina226_iic_write(handle, reg, data);       /* 写入数据 */
}

/**
 * @brief      获取芯片寄存器
 * @param[in]  *handle 指向 ina226 句柄结构的指针
 * @param[in]  reg 寄存器地址
 * @param[out] *data 指向数据缓冲区的指针
 * @return     状态码
 *             - 0 成功
 *             - 1 读取失败
 *             - 2 句柄为空
 *             - 3 句柄未初始化
 * @note       无
 */
uint8_t ina226_get_reg(ina226_handle_t *handle, uint8_t reg, uint16_t *data)
{
    if (handle == NULL)                                /* 检查句柄 */
    {
        return 2;                                      /* 返回错误 */
    }
    if (handle->inited != 1)                           /* 检查句柄初始化 */
    {
        return 3;                                      /* 返回错误 */
    }
    
    return a_ina226_iic_read(handle, reg, data);       /* 读取数据 */
}

/**
 * @brief      获取芯片信息
 * @param[out] *info 指向 ina226 信息结构的指针
 * @return     状态码
 *             - 0 成功
 *             - 2 句柄为空
 * @note       无
 */
uint8_t ina226_info(ina226_info_t *info)
{
    if (info == NULL)                                               /* 检查句柄 */
    {
        return 2;                                                   /* 返回错误 */
    }
    
    memset(info, 0, sizeof(ina226_info_t));                         /* 初始化 ina226 信息结构体 */
    strncpy(info->chip_name, CHIP_NAME, 32);                        /* 复制芯片名称 */
    strncpy(info->manufacturer_name, MANUFACTURER_NAME, 32);        /* 复制制造商名称 */
    strncpy(info->interface, "IIC", 8);                             /* 复制接口名称 */
    info->supply_voltage_min_v = SUPPLY_VOLTAGE_MIN;                /* 设置最小供电电压 */
    info->supply_voltage_max_v = SUPPLY_VOLTAGE_MAX;                /* 设置最大供电电压 */
    info->max_current_ma = MAX_CURRENT;                             /* 设置最大电流 */
    info->temperature_max = TEMPERATURE_MAX;                        /* 设置最小温度 */
    info->temperature_min = TEMPERATURE_MIN;                        /* 设置最大温度 */
    info->driver_version = DRIVER_VERSION;                          /* 设置驱动版本 */
    
    return 0;                                                       /* 成功返回 0 */
}
