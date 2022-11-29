/*
 * @Author: AIoTMaker kuili@iflytek.com
 * @Date: 2022-11-29 18:56:58
 * @LastEditors: AIoTMaker kuili@iflytek.com
 * @LastEditTime: 2022-11-30 00:11:21
 * @FilePath: \HsdVideoStreamAPP\src\core\led_core.c
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
#include <led_core.h>

#define LED_DEFAULT_INTERVAL 20U
#define LED_MIN_PERIOD PWM_MSEC(1U)
#define LED_MAX_PERIOD PWM_MSEC(LED_DEFAULT_INTERVAL)

#define LED_TIMER_INIT(timer, timer_expiry, timer_stop) \
    static struct k_timer timer; \
    k_timer_init(&timer, timer_expiry, timer_stop); \
    led.timer = (void*)&timer;

/* 通过别名获取 "led0" 设备树 node id */
#define LED0_NODE DT_ALIAS(led0)

/* 通过 node id 获取 led0 设备树信息 */
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/* 获取设备树配置 */
static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));

/// 接口实现声明
__weak void csk6011a_nano_led_on(led_type_t led_type);
__weak void csk6011a_nano_led_off(led_type_t led_type);
__weak void csk6011a_nano_led_blink(led_type_t led_type, 
                                        int interval, 
                                        int duration);
__weak void csk6011a_nano_led_breathing(led_type_t led_type, 
                                        int interval, 
                                        int duration);

// led单例初始化
led_core_t led = {
    .green_led      = {GREEN_LED, LED_OFF, 0, 0, 0, 0, 0, 0},
    .on             = csk6011a_nano_led_on,
    .off            = csk6011a_nano_led_off,
    .blink          = csk6011a_nano_led_blink,
    .breathing      = csk6011a_nano_led_breathing,
    .period         = LED_MAX_PERIOD,
};


static void blink_timer_expiry(struct k_timer *timer)
{
    /* 关闭LED闪烁 */
    csk6011a_nano_led_off(GREEN_LED);
    LOG(EDEBUG, "blink_timer_expiry called");
}


static void blink_timer_stop(struct k_timer *timer)
{
    LOG(EDEBUG, "blink_timer_stop called");
}


static void breathe_timer_expiry(struct k_timer *timer)
{
    if(led.green_led.pwm_flap)led.green_led.pwm_cnt -= led.green_led.interval;
    else led.green_led.pwm_cnt += led.green_led.interval;
    
    if(led.green_led.pwm_cnt >= led.period) {
        led.green_led.pwm_cnt = led.period - 1;
        led.green_led.pwm_flap = 1;
    }
    if(led.green_led.pwm_cnt <= 0) {
        led.green_led.pwm_cnt = 1;
        led.green_led.pwm_flap = 0;
    }

    pwm_set_dt(&pwm_led0, led.period, led.green_led.pwm_cnt);
    
    if (led.green_led.duration > 0) {
        led.green_led.current_duration += LED_DEFAULT_INTERVAL;
        if (led.green_led.current_duration >= led.green_led.duration) {
            k_timer_stop((struct k_timer *)led.breathe_timer);
        }
    }
}


static void breathe_timer_stop(struct k_timer *timer)
{
    LOG(EDEBUG, "breathe_timer_stop");
}


static void csk6011a_nano_led_init(void)
{
    static bool is_init = false;
    if (!is_init) {
        if (!device_is_ready(led0.port)) {
            LOG(EERROR, "led0 is not ready");
            return ;
        }
        if (!device_is_ready(pwm_led0.dev)) {
            LOG(EERROR, "Error: PWM device %s is not ready",
                pwm_led0.dev->name);
            return ;
        }
        /* 配置LED闪烁定时器 */
        LED_TIMER_INIT(blink_timer, blink_timer_expiry, blink_timer_stop);
        /* 配置LED呼吸灯定时器 */
        LED_TIMER_INIT(breathe_timer, 
            breathe_timer_expiry, breathe_timer_stop);
        is_init = true;
    }
}


__weak void csk6011a_nano_led_on(led_type_t led_type)
{
    csk6011a_nano_led_init();

    if (led_type == GREEN_LED) {
        pwm_set_dt(&pwm_led0, led.period, 1);
        // TODO: 按照以下设置后，无法使用新的周期设置占空比，后续使用pwm_cycles_to_usec重新初始化尝试
        // pwm_set_dt(&pwm_led0, PWM_SEC(1U), 1);
        // gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE);
    }
}


__weak void csk6011a_nano_led_off(led_type_t led_type)
{
    csk6011a_nano_led_init();

    if (led_type == GREEN_LED) {
        pwm_set_dt(&pwm_led0, led.period, led.period-1);
        // pwm_set_dt(&pwm_led0, PWM_SEC(1U), PWM_SEC(1U)-1);
        // gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
    }
}


__weak void csk6011a_nano_led_blink(led_type_t led_type, 
                                    int interval, 
                                    int duration)
{
    csk6011a_nano_led_init();

    if (led_type == GREEN_LED) {
        k_timer_stop((struct k_timer *)led.blink_timer);
        k_timer_stop((struct k_timer *)led.breathe_timer);
        pwm_set_dt(&pwm_led0, PWM_MSEC(interval), PWM_MSEC(interval)/2);
        if (duration > 0) {
            k_timer_start((struct k_timer *)led.blink_timer, 
                K_MSEC(duration), K_MSEC(0));
        }
    }
}


__weak void csk6011a_nano_led_breathing(led_type_t led_type, 
                                        int interval, 
                                        int duration)
{
    csk6011a_nano_led_init();

    if (led_type == GREEN_LED) {
        led.green_led.led_status = LED_BREATHING;
        led.green_led.interval = 
            (int)((1000.0/interval)*(led.period/(1000/LED_DEFAULT_INTERVAL)));
        led.green_led.duration = duration;
        led.green_led.current_duration = 0;
        led.green_led.current_interval = 0;
        led.green_led.pwm_flap = 0;
        led.green_led.pwm_cnt = 0;

        k_timer_stop((struct k_timer *)led.blink_timer);
        k_timer_stop((struct k_timer *)led.breathe_timer);
        k_timer_start((struct k_timer *)led.breathe_timer, 
            K_MSEC(0), K_MSEC(LED_DEFAULT_INTERVAL));
    }
}
