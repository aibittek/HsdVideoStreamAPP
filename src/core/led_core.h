/*
 * @Author: AIoTMaker kuili@iflytek.com
 * @Date: 2022-11-29 18:57:06
 * @LastEditors: AIoTMaker kuili@iflytek.com
 * @LastEditTime: 2022-11-29 20:41:48
 * @FilePath: \HsdVideoStreamAPP\src\core\led_core.h
 * @Description: 
 * 
 * Copyright (c) 2022 by AIoTMaker kuili@iflytek.com, All Rights Reserved. 
 */

#ifndef _LED_SENSOR_H
#define _LED_SENSOR_H


/// \brief LED的类型，目前板载只有绿色LED
typedef enum {
    GREEN_LED,
} led_type_t;


/// \brief LED灯状态，有开关、闪烁和呼吸4个状态
///
/// 其中的绿色LED不支持呼吸灯的效果，使用时需注意
typedef enum {
    LED_OFF         = 1,
    LED_ON          = 2,
    LED_BLINK       = 3,
    LED_BREATHING   = 4,
}led_status_t;


/// \brief LED灯当前信息
typedef struct {
    led_type_t      led_type;
    led_status_t    led_status;
    int             interval;
    int             duration;
    int             current_interval;
    int             current_duration;
    int             pwm_flap;
    int             pwm_cnt;
}led_info_t;


/**
 * @brief
 *  LED灯的亮灭、闪烁、呼吸的控制接口
 */
typedef struct {
    /*!< @brief 绿色LED信息 */
    led_info_t      green_led;

    /*!< @brief LED周期 */
    int             period;

    /// LED闪烁定时器
    void*           blink_timer;
    /// LED呼吸定时器
    void*           breathe_timer;
    
    /**
    * @brief 点亮LED灯
    *
    * @param led 要被点亮的led灯，可以是red_led和green_led
    * @return
    */
    void (*on)(led_type_t led_type);
    
    /**
    * @brief 熄灭LED灯
    *
    * @param led 要被熄灭的led灯，可以是red_led和green_led
    * @return
    */
    void (*off)(led_type_t led_type);
    
    /**
    * @brief LED灯闪烁
    *
    * @param led 闪烁的led灯，可以是red_led和green_led
    * @param interval 闪烁间隔时间，以毫秒为单位
    * @param duration 灯闪烁或呼吸持续的时间，以毫秒为单位，开关LED不关心这个值
    * @return
    */
    void (*blink)(led_type_t led_type, int interval, int duration);
    
    /**
    * @brief LED呼吸灯
    *
    * @param led 呼吸的led灯，注意仅red_led支持呼吸灯
    * @param interval 灯间隔时间，以毫秒为单位，开关LED不关心这个值
    * @param duration 灯闪烁或呼吸持续的时间，以毫秒为单位，开关LED不关心这个值
    * @return
    */
    void (*breathing)(led_type_t led_type, int interval, int duration);

}led_core_t;


/*! @brief led灯单例 */
extern led_core_t led;

#endif
