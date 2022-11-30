/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/zephyr.h>
#include <user_test.h>

void main(void)
{
    // 日志测试
    log_test();
    // 按键测试
    key_test();
    // LED测试
    led_test();
    // 自定义shell命令测试
    shell_test();

    while(1){ delay_ms(100); }
}
