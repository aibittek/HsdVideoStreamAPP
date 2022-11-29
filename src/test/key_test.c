#include <log.h>
#include <key_core.h>
#include <user_test.h>

void on_key_event(key_event_t event, void* user_data)
{
    switch(event) {
        case KEY_EVENT_RELEASE:
            LOG(EDEBUG, "key is released.");
            break;
        case KEY_EVENT_PRESSED:
            LOG(EDEBUG, "key is pressed.");
            break;
        case KEY_EVENT_LONG_PRESSED:
            LOG(EDEBUG, "key is long pressed.");
            break;
        default:
            LOG(EDEBUG, "key event is error.");
            break;
    }
}

void key_test()
{
    // 经过消抖的按键按下、抬起、长按功能测试
    key.init();
    key.event_register(on_key_event, NULL);
}
