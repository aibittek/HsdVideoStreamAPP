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
//  0.5ms--------------0度；
//  1.0ms------------45度；
//  1.5ms------------90度；
//  2.0ms-----------135度；
//  2.5ms-----------180度；
// 根据以上推到公式ccr = 0.0111*angle + 0.5
void servo_angle(int angle)
{
    // 规范角度值为0-180度之间
    if (angle > 180) angle = 180;
    if (angle < 0) angle = 0;

    int32_t ccr = (int32_t)(0.0111*angle + 0.5);

    /* 50Hz:20ms */
    pwm_set_dt(&pwm_servo0, PWM_SEC(50U), PWM_SEC(ccr));  
}


void stop_servo(void)
{
    
}
