/*
 * @Author: AIoTMaker kuili@iflytek.com
 * @Date: 2022-12-05 21:47:49
 * @LastEditors: AIoTMaker kuili@iflytek.com
 * @LastEditTime: 2022-12-05 23:04:51
 * @FilePath: \csk6_apps\src\core\servo_core.h
 * @Description: 
 * 
 * Copyright (c) 2022 by AIoTMaker kuili@iflytek.com, All Rights Reserved. 
 */
#ifndef _SERVO_CORE_H
#define _SERVO_CORE_H

/**
* @brief 使能舵机
*
* @retval
*/
void start_servo(void);


/**
* @brief 设置舵机的角度
*
* @param angle 舵机的角度，按照0-180度设置
* @retval
*/
void servo_angle(int angle);


/**
* @brief 停止舵机
*
* @retval
*/
void stop_servo(void);

#endif
