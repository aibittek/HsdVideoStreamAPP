/*
 * @Author: AIoTMaker kuili@iflytek.com
 * @Date: 2022-11-30 17:35:46
 * @LastEditors: AIoTMaker kuili@iflytek.com
 * @LastEditTime: 2022-11-30 17:57:39
 * @FilePath: \HsdVideoStreamAPP\src\core\flash_core.h
 * @Description: 
 * 
 * Copyright (c) 2022 by AIoTMaker kuili@iflytek.com, All Rights Reserved. 
 */
#ifndef _FLASH_CORE_H
#define _FLASH_CORE_H

/**
 * @brief flash文件系统初始化，根目录为/lfs1，初始化之后，可以使用zephyr的文件系统API接口访问文件
 * @return 0成功，其他值失败
 */
int user_flash_init(void);

/**
 * @brief 注销flash文件系统
 * @return 0成功，其他值失败
 */
int user_flash_uninit(void);

#endif
