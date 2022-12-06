/*
 * @Author: AIoTMaker kuili@iflytek.com
 * @Date: 2022-12-05 21:44:15
 * @LastEditors: AIoTMaker kuili@iflytek.com
 * @LastEditTime: 2022-12-06 10:52:18
 * @FilePath: \csk6_apps\src\main.c
 * @Description: 
 * 
 * Copyright (c) 2022 by AIoTMaker , All Rights Reserved. 
 */
/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/zephyr.h>
#include <delay.h>
#include <user_test.h>

void main(void)
{
#if 0
    // 按键测试
    key_test();
    // 自定义shell命令测试
    shell_test();
    // flash读写测试
    flash_test();
    // wifi连接热点测试
    wifi_test();
    // udp服务端连接通信测试
    udp_test();
#endif
    // 日志测试
    log_test();
    // LED测试
    led_test();
    // 舵机测试
    servo_test();

    while(1){ delay_ms(100); }
}
