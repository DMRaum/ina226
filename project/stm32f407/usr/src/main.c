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
 * @file      main.c
 * @brief     主源文件
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

#include "driver_ina226_shot.h"
#include "driver_ina226_basic.h"
#include "driver_ina226_alert.h"
#include "driver_ina226_register_test.h"
#include "driver_ina226_read_test.h"
#include "driver_ina226_alert_test.h"
#include "shell.h"
#include "clock.h"
#include "delay.h"
#include "uart.h"
#include "gpio.h"
#include "getopt.h"
#include <stdlib.h>

/**
 * @brief 全局变量定义
 */
uint8_t g_buf[256];                        /**< 串口缓冲区 */
uint8_t (*g_gpio_irq)(void) = NULL;        /**< GPIO中断回调函数指针 */
volatile uint16_t g_len;                   /**< 串口缓冲区长度 */
static volatile uint16_t gs_flag;          /**< 中断标志位 */

/**
 * @brief 外部中断0中断服务函数
 * @note  无
 */
void EXTI0_IRQHandler(void)
{
    /* 处理GPIO_PIN_0的外部中断 */
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

/**
 * @brief     GPIO外部中断回调函数
 * @param[in] pin GPIO引脚号
 * @note      无
 */
void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
    /* 判断是否为PIN_0触发 */
    if (pin == GPIO_PIN_0)
    {
        /* 若已注册中断回调则执行 */
        if (g_gpio_irq != NULL)
        {
            g_gpio_irq();
        }
    }
}

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
            /* 置位第0位，表示并联电压过压 */
            gs_flag |= 1 << 0;
            ina226_interface_debug_print("ina226: 中断-并联电压过压。\n");
            
            break;
        }
        case INA226_STATUS_SHUNT_VOLTAGE_UNDER_VOLTAGE :
        {
            /* 置位第1位，表示并联电压欠压 */
            gs_flag |= 1 << 1;
            ina226_interface_debug_print("ina226: 中断-并联电压欠压。\n");
            
            break;
        }
        case INA226_STATUS_BUS_VOLTAGE_OVER_VOLTAGE :
        {
            /* 置位第2位，表示总线电压过压 */
            gs_flag |= 1 << 2;
            ina226_interface_debug_print("ina226: 中断-总线电压过压。\n");
            
            break;
        }
        case INA226_STATUS_BUS_VOLTAGE_UNDER_VOLTAGE :
        {
            /* 置位第3位，表示总线电压欠压 */
            gs_flag |= 1 << 3;
            ina226_interface_debug_print("ina226: 中断-总线电压欠压。\n");
            
            break;
        }
        case INA226_STATUS_POWER_OVER_LIMIT :
        {
            /* 置位第4位，表示功率超限 */
            gs_flag |= 1 << 4;
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

/**
 * @brief     INA226全功能命令处理函数
 * @param[in] argc 参数个数
 * @param[in] **argv 参数地址
 * @return    状态码
 *            - 0 成功
 *            - 1 运行失败
 *            - 5 参数无效
 * @note      无
 */
uint8_t ina226(uint8_t argc, char **argv)
{
    int c;
    int longindex = 0;
    const char short_options[] = "hipe:t:";                 /* 短选项字符串 */
    const struct option long_options[] =
    {
        {"help", no_argument, NULL, 'h'},                    /* 帮助 */
        {"information", no_argument, NULL, 'i'},             /* 芯片信息 */
        {"port", no_argument, NULL, 'p'},                    /* 引脚连接 */
        {"example", required_argument, NULL, 'e'},           /* 运行示例 */
        {"test", required_argument, NULL, 't'},              /* 运行测试 */
        {"addr", required_argument, NULL, 1},                /* 设置地址 */
        {"resistance", required_argument, NULL, 2},          /* 设置采样电阻 */
        {"times", required_argument, NULL, 3},               /* 设置运行次数 */
        {"type", required_argument, NULL, 4},                /* 设置告警类型 */
        {"threshold", required_argument, NULL, 5},           /* 设置告警阈值 */
        {NULL, 0, NULL, 0},
    };
    char type[33] = "unknown";                               /* 命令类型，默认unknown */
    uint32_t times = 3;                                      /* 运行次数，默认3次 */
    double r = 0.1;                                          /* 采样电阻值，默认0.1欧姆 */
    float threshold = 3300.0f;                               /* 告警阈值，默认3300.0 */
    ina226_mask_t mask = INA226_MASK_BUS_VOLTAGE_OVER_VOLTAGE;  /* 告警掩码，默认总线电压过压 */
    ina226_address_t addr = INA226_ADDRESS_0;                /* IIC设备地址，默认地址0 */
    
    /* 若无参数 */
    if (argc == 1)
    {
        /* 跳转到帮助 */
        goto help;
    }
    
    /* 初始化optind为0 */
    optind = 0;
    
    /* 解析参数 */
    do
    {
        /* 解析命令行参数 */
        c = getopt_long(argc, argv, short_options, long_options, &longindex);
        
        /* 判断解析结果 */
        switch (c)
        {
            /* 帮助 */
            case 'h' :
            {
                /* 设置类型为h */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "h");
                
                break;
            }
            
            /* 信息 */
            case 'i' :
            {
                /* 设置类型为i */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "i");
                
                break;
            }
            
            /* 端口 */
            case 'p' :
            {
                /* 设置类型为p */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "p");
                
                break;
            }
            
            /* 示例 */
            case 'e' :
            {
                /* 设置类型为e_参数值 */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "e_%s", optarg);
                
                break;
            }
            
            /* 测试 */
            case 't' :
            {
                /* 设置类型为t_参数值 */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "t_%s", optarg);
                
                break;
            }
            
            /* 地址参数 */
            case 1 :
            {
                /* 设置地址引脚 */
                if (strcmp("0", optarg) == 0)
                {
                    addr = INA226_ADDRESS_0;
                }
                else if (strcmp("1", optarg) == 0)
                {
                    addr = INA226_ADDRESS_1;
                }
                else if (strcmp("2", optarg) == 0)
                {
                    addr = INA226_ADDRESS_2;
                }
                else if (strcmp("3", optarg) == 0)
                {
                    addr = INA226_ADDRESS_3;
                }
                else if (strcmp("4", optarg) == 0)
                {
                    addr = INA226_ADDRESS_4;
                }
                else if (strcmp("5", optarg) == 0)
                {
                    addr = INA226_ADDRESS_5;
                }
                else if (strcmp("6", optarg) == 0)
                {
                    addr = INA226_ADDRESS_6;
                }
                else if (strcmp("7", optarg) == 0)
                {
                    addr = INA226_ADDRESS_7;
                }
                else if (strcmp("8", optarg) == 0)
                {
                    addr = INA226_ADDRESS_8;
                }
                else if (strcmp("9", optarg) == 0)
                {
                    addr = INA226_ADDRESS_9;
                }
                else if (strcmp("A", optarg) == 0)
                {
                    addr = INA226_ADDRESS_A;
                }
                else if (strcmp("B", optarg) == 0)
                {
                    addr = INA226_ADDRESS_B;
                }
                else if (strcmp("C", optarg) == 0)
                {
                    addr = INA226_ADDRESS_C;
                }
                else if (strcmp("D", optarg) == 0)
                {
                    addr = INA226_ADDRESS_D;
                }
                else if (strcmp("E", optarg) == 0)
                {
                    addr = INA226_ADDRESS_E;
                }
                else if (strcmp("F", optarg) == 0)
                {
                    addr = INA226_ADDRESS_F;
                }
                else
                {
                    /* 地址参数无效 */
                    return 5;
                }
                
                break;
            }
            
            /* 采样电阻参数 */
            case 2 :
            {
                /* 设置采样电阻值 */
                r = atof(optarg);
                
                break;
            }

            /* 运行次数参数 */
            case 3 :
            {
                /* 设置运行次数 */
                times = atol(optarg);
                
                break;
            } 
            
            /* 告警类型参数 */
            case 4 :
            {
                uint8_t t;
                
                /* 设置告警类型 */
                t = (uint8_t)atol(optarg);
                if (t == 0)
                {
                    /* 0: 并联电压过压 */
                    mask = INA226_MASK_SHUNT_VOLTAGE_OVER_VOLTAGE;
                }
                else if (t == 1)
                {
                    /* 1: 并联电压欠压 */
                    mask = INA226_MASK_SHUNT_VOLTAGE_UNDER_VOLTAGE;
                }
                else if (t == 2)
                {
                    /* 2: 总线电压过压 */
                    mask = INA226_MASK_BUS_VOLTAGE_OVER_VOLTAGE;
                }
                else if (t == 3)
                {
                    /* 3: 总线电压欠压 */
                    mask = INA226_MASK_BUS_VOLTAGE_UNDER_VOLTAGE;
                }
                else if (t == 4)
                {
                    /* 4: 功率超限 */
                    mask = INA226_MASK_POWER_OVER_LIMIT;
                }
                else
                {
                    /* 告警类型参数无效 */
                    return 5;
                }
                
                break;
            } 
            
            /* 告警阈值参数 */
            case 5 :
            {
                /* 设置告警阈值 */
                threshold = (float)atof(optarg);
                
                break;
            } 
            
            /* 解析结束 */
            case -1 :
            {
                break;
            }
            
            /* 其他情况 */
            default :
            {
                return 5;
            }
        }
    } while (c != -1);

    /* 根据类型运行对应功能 */
    if (strcmp("t_reg", type) == 0)
    {
        uint8_t res;
        
        /* 运行寄存器测试 */
        res = ina226_register_test(addr);
        if (res != 0)
        {
            /* 测试失败 */
            return 1;
        }
        else
        {
            /* 测试成功 */
            return 0;
        }
    }
    else if (strcmp("t_read", type) == 0)
    {
        uint8_t res;
        
        /* 运行读取测试 */
        res = ina226_read_test(addr, r, times);
        if (res != 0)
        {
            /* 测试失败 */
            return 1;
        }
        else
        {
            /* 测试成功 */
            return 0;
        }
    }
    else if (strcmp("t_alert", type) == 0)
    {
        uint8_t res;
        
        /* GPIO中断初始化 */
        res = gpio_interrupt_init();
        if (res != 0)
        {
            return 1;
        }
        /* 注册告警测试中断处理函数 */
        g_gpio_irq = ina226_alert_test_irq_handler;
        
        /* 运行告警测试 */
        res = ina226_alert_test(addr, r, mask, threshold, 5000);
        if (res != 0)
        {
            /* 测试失败，清理资源 */
            g_gpio_irq = NULL;
            (void)gpio_interrupt_deinit();
            
            return 1;
        }
        else
        {
            /* 测试成功，清理资源 */
            g_gpio_irq = NULL;
            (void)gpio_interrupt_deinit();
            
            return 0;
        }
    }
    else if (strcmp("e_read", type) == 0)
    {
        uint8_t res;
        uint32_t i;
        
        /* 基础初始化 */
        res = ina226_basic_init(addr, r);
        if (res != 0)
        {
            return 1;
        }
        
        /* 延时1000ms */
        ina226_interface_delay_ms(1000);
        
        /* 循环读取数据 */
        for (i = 0; i < times; i++)
        {
            float mV;
            float mA;
            float mW;
            
            /* 读取数据 */
            res = ina226_basic_read(&mV, &mA, &mW);
            if (res != 0)
            {
                /* 读取失败，反初始化并返回 */
                (void)ina226_basic_deinit();
                
                return 1;
            }
            
            /* 输出数据 */
            ina226_interface_debug_print("ina226: %d/%d。\n", i + 1, times);
            ina226_interface_debug_print("ina226: 总线电压为 %0.3fmV。\n", mV);
            ina226_interface_debug_print("ina226: 电流为 %0.3fmA。\n", mA);
            ina226_interface_debug_print("ina226: 功率为 %0.3fmW。\n", mW);
            ina226_interface_delay_ms(1000);
        }
        
        /* 反初始化 */
        (void)ina226_basic_deinit();
        
        return 0;
    }
    else if (strcmp("e_shot", type) == 0)
    {
        uint8_t res;
        uint32_t i;
        
        /* 单次模式初始化 */
        res = ina226_shot_init(addr, r);
        if (res != 0)
        {
            return 1;
        }
        
        /* 延时1000ms */
        ina226_interface_delay_ms(1000);

        /* 循环读取数据 */
        for (i = 0; i < times; i++)
        {
            float mV;
            float mA;
            float mW;
            
            /* 读取数据 */
            res = ina226_shot_read(&mV, &mA, &mW);
            if (res != 0)
            {
                /* 读取失败，反初始化并返回 */
                (void)ina226_shot_deinit();
                
                return 1;
            }
            
            /* 输出数据 */
            ina226_interface_debug_print("ina226: %d/%d。\n", i + 1, times);
            ina226_interface_debug_print("ina226: 总线电压为 %0.3fmV。\n", mV);
            ina226_interface_debug_print("ina226: 电流为 %0.3fmA。\n", mA);
            ina226_interface_debug_print("ina226: 功率为 %0.3fmW。\n", mW);
            ina226_interface_delay_ms(1000);
        }
        
        /* 单次模式反初始化 */
        (void)ina226_shot_deinit();
        
        return 0;
    }
    else if (strcmp("e_alert", type) == 0)
    {
        uint8_t res;
        uint8_t flag;
        uint32_t timeout = 5000;                               /* 超时时间，单位ms */
        
        /* GPIO中断初始化 */
        res = gpio_interrupt_init();
        if (res != 0)
        {
            return 1;
        }
        /* 注册告警中断处理函数 */
        g_gpio_irq = ina226_alert_irq_handler;
        
        /* 清除标志位 */
        gs_flag = 0;
        if (mask == INA226_MASK_SHUNT_VOLTAGE_OVER_VOLTAGE)
        {
            /* 设置并联电压过压阈值 */
            ina226_interface_debug_print("ina226: 设置并联电压过压阈值 %0.3fmV。\n", threshold);
            
            flag = 1 << 0;
        }
        else if (mask == INA226_MASK_SHUNT_VOLTAGE_UNDER_VOLTAGE)
        {
            /* 设置并联电压欠压阈值 */
            ina226_interface_debug_print("ina226: 设置并联电压欠压阈值 %0.3fmV。\n", threshold);
            
            flag = 1 << 1;
        }
        else if (mask == INA226_MASK_BUS_VOLTAGE_OVER_VOLTAGE)
        {
            /* 设置总线电压过压阈值 */
            ina226_interface_debug_print("ina226: 设置总线电压过压阈值 %0.3fmV。\n", threshold);
            
            flag = 1 << 2;
        }
        else if (mask == INA226_MASK_BUS_VOLTAGE_UNDER_VOLTAGE)
        {
            /* 设置总线电压欠压阈值 */
            ina226_interface_debug_print("ina226: 设置总线电压欠压阈值 %0.3fmV。\n", threshold);
            
            flag = 1 << 3;
        }
        else
        {
            /* 设置功率超限阈值 */
            ina226_interface_debug_print("ina226: 设置功率超限阈值 %0.3fmV。\n", threshold);
            
            flag = 1 << 4;
        }
        
        /* 告警初始化 */
        res = ina226_alert_init(addr, r, mask, threshold, a_receive_callback);
        if (res != 0)
        {
            /* 初始化失败，清理资源 */
            g_gpio_irq = NULL;
            (void)gpio_interrupt_deinit();
            
            return 1;
        }
        
        /* 等待中断触发 */
        while (timeout != 0)
        {
            if ((gs_flag & flag) != 0)
            {
                /* 中断已触发，退出等待 */
                break;
            }
            timeout--;
            ina226_interface_delay_ms(1);
        }
        
        /* 检查是否超时 */
        if (timeout == 0)
        {
            ina226_interface_debug_print("ina226: 告警超时。\n");
            g_gpio_irq = NULL;
            (void)gpio_interrupt_deinit();
            ina226_alert_deinit();
            
            return 1;
        }
        
        /* 告警反初始化，清理资源 */
        g_gpio_irq = NULL;
        (void)gpio_interrupt_deinit();
        (void)ina226_alert_deinit();
        
        return 0;
    }
    else if (strcmp("h", type) == 0)
    {
        help:
        /* 打印用法说明 */
        ina226_interface_debug_print("用法:\n");
        ina226_interface_debug_print("  ina226 (-i | --information)\n");
        ina226_interface_debug_print("  ina226 (-h | --help)\n");
        ina226_interface_debug_print("  ina226 (-p | --port)\n");
        ina226_interface_debug_print("  ina226 (-t reg | --test=reg) [--addr=<0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B | C | D | E | F>]\n");
        ina226_interface_debug_print("  ina226 (-t read | --test=read) [--addr=<0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B | C | D | E | F>]\n");
        ina226_interface_debug_print("         [--resistance=<r>] [--times=<num>]\n");
        ina226_interface_debug_print("  ina226 (-t alert | --test=alert) [--addr=<0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B | C | D | E | F>]\n");
        ina226_interface_debug_print("         [--resistance=<r>] [--type=<type>][--threshold=<th>]\n");
        ina226_interface_debug_print("  ina226 (-e read | --example=read) [--addr=<0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B | C | D | E | F>]\n");
        ina226_interface_debug_print("         [--resistance=<r>] [--times=<num>]\n");
        ina226_interface_debug_print("  ina226 (-e shot | --example=shot) [--addr=<0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B | C | D | E | F>]\n");
        ina226_interface_debug_print("         [--resistance=<r>] [--times=<num>]\n");
        ina226_interface_debug_print("  ina226 (-e alert | --example=alert) [--addr=<0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B | C | D | E | F>]\n");
        ina226_interface_debug_print("         [--resistance=<r>] [--type=<type>][--threshold=<th>]\n");
        ina226_interface_debug_print("\n");
        ina226_interface_debug_print("选项:\n");
        ina226_interface_debug_print("      --addr=<0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B | C | D | E | F>\n");
        ina226_interface_debug_print("                                 设置地址引脚。([默认: 0])\n");
        ina226_interface_debug_print("  -e <read | shot | alert>, --example=<read | shot | alert>\n");
        ina226_interface_debug_print("                                 运行驱动示例。\n");
        ina226_interface_debug_print("  -h, --help                     显示帮助信息。\n");
        ina226_interface_debug_print("  -i, --information              显示芯片信息。\n");
        ina226_interface_debug_print("  -p, --port                     显示当前板子的引脚连接。\n");
        ina226_interface_debug_print("      --resistance=<r>           设置采样电阻。([默认: 0.1])\n");
        ina226_interface_debug_print("  -t <reg | read | alert>, --test=<reg | read | alert>\n");
        ina226_interface_debug_print("                                 运行驱动测试。\n");
        ina226_interface_debug_print("      --times=<num>              设置运行次数。([默认: 3])\n");
        ina226_interface_debug_print("      --type=<type>              设置告警类型,\n");
        ina226_interface_debug_print("                                 0: 并联电压过压;\n");
        ina226_interface_debug_print("                                 1: 并联电压欠压;\n");
        ina226_interface_debug_print("                                 2: 总线电压过压;\n");
        ina226_interface_debug_print("                                 3: 总线电压欠压;\n");
        ina226_interface_debug_print("                                 4: 功率超限。([默认: 2])\n");
        ina226_interface_debug_print("      --threshold=<th>           设置告警阈值。([默认: 3300.0f])\n");
          
        return 0;
    }
    else if (strcmp("i", type) == 0)
    {
        ina226_info_t info;
        
        /* 打印INA226芯片信息 */
        ina226_info(&info);
        ina226_interface_debug_print("ina226: 芯片为 %s。\n", info.chip_name);
        ina226_interface_debug_print("ina226: 制造商为 %s。\n", info.manufacturer_name);
        ina226_interface_debug_print("ina226: 接口为 %s。\n", info.interface);
        ina226_interface_debug_print("ina226: 驱动版本为 %d.%d。\n", info.driver_version / 1000, (info.driver_version % 1000) / 100);
        ina226_interface_debug_print("ina226: 最小供电电压为 %0.1fV。\n", info.supply_voltage_min_v);
        ina226_interface_debug_print("ina226: 最大供电电压为 %0.1fV。\n", info.supply_voltage_max_v);
        ina226_interface_debug_print("ina226: 最大电流为 %0.2fmA。\n", info.max_current_ma);
        ina226_interface_debug_print("ina226: 最高温度为 %0.1fC。\n", info.temperature_max);
        ina226_interface_debug_print("ina226: 最低温度为 %0.1fC。\n", info.temperature_min);
        
        return 0;
    }
    else if (strcmp("p", type) == 0)
    {
        /* 打印引脚连接信息 */
        ina226_interface_debug_print("ina226: SCL连接到GPIOB PIN8。\n");
        ina226_interface_debug_print("ina226: SDA连接到GPIOB PIN9。\n");
        ina226_interface_debug_print("ina226: INT连接到GPIOB PIN0。\n");
        
        return 0;
    }
    else
    {
        /* 未知命令类型 */
        return 5;
    }
}

/**
 * @brief 主函数
 * @note  无
 */
int main(void)
{
    uint8_t res;
    
    /* STM32F407时钟初始化和HAL初始化 */
    clock_init();
    
    /* 延时初始化 */
    delay_init();
    
    /* 串口初始化，波特率115200 */
    uart_init(115200);
    
    /* Shell初始化并注册ina226命令 */
    shell_init();
    shell_register("ina226", ina226);
    uart_print("ina226: 欢迎使用 libdriver ina226。\n");
    
    /* 主循环 */
    while (1)
    {
        /* 读取串口数据 */
        g_len = uart_read(g_buf, 256);
        if (g_len != 0)
        {
            /* 解析并执行Shell命令 */
            res = shell_parse((char *)g_buf, g_len);
            if (res == 0)
            {
                /* 执行成功 */
            }
            else if (res == 1)
            {
                uart_print("ina226: 运行失败。\n");
            }
            else if (res == 2)
            {
                uart_print("ina226: 未知命令。\n");
            }
            else if (res == 3)
            {
                uart_print("ina226: 长度过长。\n");
            }
            else if (res == 4)
            {
                uart_print("ina226: 预处理失败。\n");
            }
            else if (res == 5)
            {
                uart_print("ina226: 参数无效。\n");
            }
            else
            {
                uart_print("ina226: 未知状态码。\n");
            }
            /* 刷新串口 */
            uart_flush();
        }
        /* 延时100ms */
        delay_ms(100);
    }
}
