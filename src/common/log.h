/*
 * @Author: AIoTMaker kuili@iflytek.com
 * @Date: 2022-11-27 16:51:54
 * @LastEditors: AIoTMaker kuili@iflytek.com
 * @LastEditTime: 2022-12-01 21:24:09
 * @FilePath: \HsdVideoStreamAPP\src\common\log.h
 * @Description: 
 * 
 * Copyright (c) 2022 by AIoTMaker kuili@iflytek.com, All Rights Reserved. 
 */
#ifndef _LOG_H
#define _LOG_H

#include <stdio.h>
#include <string.h>

/// 日志分等级管理
typedef enum {
    EVERBOSE,			///< 冗长日志
    EDEBUG,				///< 调试日志
    EINFO,				///< 信息日志
    EWARN,				///< 警告日志
    EERROR,				///< 错误日志
    LOG_LEVEL_MAX		///< 日志分级最大值
}log_level_t;

/// 记录当前日志等级的静态全局变量
static log_level_t g_log_level = EDEBUG;

/// 日志等级设置
static inline void set_log_level(log_level_t level)
{
    g_log_level = level;
}

/// 去除文件路径，Keil工程的路径是'\'反斜杠,由于单个'\'是转义字符，因此需要通过'\\'把反斜杠改为普通字符
/// 其他系统中，如linux系统，需要把'\\'改为'/'
#define LOG_FILE_NAME_STRIP(name) strrchr(name, '/')?(strrchr(name, '/')+1):name

/// 输出日志，大于等于当前日志等级的会输出，并显示输出的当前文件、函数和行号信息方便查询 
#define LOG(level, format, ...) \
    { \
        if (level >= g_log_level) { \
            printf("[%s %s:%d]""["#level"]"format"\n", \
				LOG_FILE_NAME_STRIP(__FILE__),__FUNCTION__,__LINE__, ##__VA_ARGS__); \
        } \
    }

#endif
