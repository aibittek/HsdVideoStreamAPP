/*
 * @Author: AIoTMaker kuili@iflytek.com
 * @Date: 2022-11-27 18:46:19
 * @LastEditors: AIoTMaker kuili@iflytek.com
 * @LastEditTime: 2022-11-27 19:24:07
 * @FilePath: \HsdVideoStreamAPP\src\Test\log_test.c
 * @Description: 
 * 
 * Copyright (c) 2022 by AIoTMaker kuili@iflytek.com, All Rights Reserved. 
 */
#include <log.h>
#include <user_test.h>

void module1(void) {
    LOG(EDEBUG, "[module1] log");
}

void module2(void) {
    LOG(EINFO, "[module2] log");
}

void module3(void) {
    LOG(EERROR, "[module3] log");
}

void log_test()
{
    /// 由于默认是EDEBUG级别，默认都会输出日志
    module1();
    module2();
    module3();

    /// 设置日志等级为EINFO，只有module2和module3会输出日志
    set_log_level(EINFO);
    module1();
    module2();
    module3();

    /// 设置日志等级为EINFO，只有module3会输出日志
    set_log_level(EERROR);
    module1();
    module2();
    module3();
}
