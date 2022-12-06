/*
 * @Author: AIoTMaker kuili@iflytek.com
 * @Date: 2022-12-05 21:47:35
 * @LastEditors: AIoTMaker kuili@iflytek.com
 * @LastEditTime: 2022-12-06 23:25:14
 * @FilePath: \csk6_apps\src\core\servo_core.c
 * @Description: 
 * 
 * Copyright (c) 2022 by AIoTMaker kuili@iflytek.com, All Rights Reserved. 
 */
#include <zephyr/zephyr.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <log.h>
#include <delay.h>
#include <servo_core.h>

// #if !DT_NODE_EXISTS(DT_NODELABEL(pwm_servo0))
// #error "pwm_servo0 is not define in dts."
// #endif

// 需要再

/* 获取设备树配置 */
static const struct pwm_dt_spec pwm_servo0 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_servo0));


void start_servo(void)
{
    static bool is_init = false;
    if (!is_init) {
        if (!device_is_ready(pwm_servo0.dev)) {
            LOG(EERROR, "Error: PWM device %s is not ready", pwm_servo0.dev->name);
            return ;
        }
        is_init = true;
    }
}

//舵机的控制一般需要一个20ms左右的时基脉冲，
//该脉冲的高电平部分一般为0.5ms~2.5ms范围内的角度控制脉冲部分。
//以180度角度伺服为例，那么对应的控制关系是这样的：
//  0.5ms-------------0度；
//  1.0ms------------45度；
//  1.5ms------------90度；
//  2.0ms-----------135度；
//  2.5ms-----------180度；
// 根据以上推导公式ccr = 0.0111*angle + 0.5
void servo_angle(int angle)
{
    // 规范角度值为0-180度之间
    if (angle > 180) angle = 180;
    if (angle < 0) angle = 0;

    double fccr = 0.0111*angle + 0.5;
    LOG(EDEBUG, "ccr:%.2lf", fccr);
    uint32_t ccr = (uint32_t)(fccr*1000*1000);

    pwm_set_dt(&pwm_servo0, PWM_MSEC(20U), ccr);  
}


void stop_servo(void)
{
    // pwm_pin_disable_capture(pwm_servo0.dev, 6);
}
