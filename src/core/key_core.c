#include <zephyr/zephyr.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <log.h>

#include <key_core.h>

#define SW0_NODE	DT_ALIAS(sw0)
#if !DT_NODE_HAS_STATUS(SW0_NODE, okay)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios,
                                     {0});
static struct gpio_callback button_cb_data;

/// 接口实现声明
__weak bool csk6011a_nano_key_init(void);
__weak void csk6011a_nano_key_uninit(void);
__weak bool csk6011a_nano_key_event_register(on_key_event_t on_key_event, 
                                            void* user_data);
__weak void csk6011a_nano_key_event_unregister(void);

/// @brief 按键单例初始化
user_key_t key = {
    .event              = KEY_EVENT_RELEASE,    /*!< 初始化引脚为抬起状态 */
    .init               = csk6011a_nano_key_init,
    .uninit             = csk6011a_nano_key_uninit,
    .event_register     = csk6011a_nano_key_event_register,
    .event_unregister   = csk6011a_nano_key_event_unregister,
    .gpio_handle        = (void*)&button,       /*!< 初始化引脚配置的句柄 */
};


static void dejitter_timer_expiry(struct k_timer *timer)
{
    /* 更新电平状态 */
    int level = gpio_pin_get_dt(&button);
    if (level == 0) key.event = KEY_EVENT_RELEASE;
    else if (level == 1) key.event = KEY_EVENT_PRESSED;

    /* 上报用户按键事件 */
    if (key.on_event) {
        key.on_event(key.event, key.user_data);
    }

    // LOG(EDEBUG, "Button pressed at %u, level:%d", 
    //         k_cycle_get_32(), gpio_pin_get_dt(&button));
}


static void dejitter_timer_stop(struct k_timer *timer)
{
    // LOG(EDEBUG, "dejitter_timer_stop");
}


static void long_pressed_timer_expiry(struct k_timer *timer)
{
    /* 上报用户按键事件 */
    key.event = KEY_EVENT_LONG_PRESSED;
    if (key.on_event) {
        key.on_event(key.event, key.user_data);
    }
}


static void long_pressed_timer_stop(struct k_timer *timer)
{
    // LOG(EDEBUG, "long_pressed_timer_stop");
}


static void button_pressed(const struct device *dev, struct gpio_callback *cb,
        uint32_t pins)
{
    /* 长按判断逻辑，按键按下启动长按定时器，抬起关闭 */
    int level = gpio_pin_get_dt(&button);
    if (level == 0) {
        k_timer_stop((struct k_timer *)key.long_pressed_timer);
    }
    else if (level == 1) {
        k_timer_start((struct k_timer *)key.long_pressed_timer, K_MSEC(KYE_LONG_PRESSED_TIME), K_MSEC(0));
    }
    
    /* 触发定时器, 使用10ms间隔进行按键消抖，定时器只触发一次 */
    k_timer_start((struct k_timer *)key.dejitter_timer, K_MSEC(10), K_MSEC(0));
}


__weak bool csk6011a_nano_key_init()
{
    int32_t ret;
    
    /* 检查硬件设备是否就绪 */
    if (!device_is_ready(button.port)) {
        LOG(EERROR, "Error: key %s is not ready", button.port->name);
        return false;
    }

    /* 配置引脚功能为输入模式 */
    ret = gpio_pin_configure_dt(&button, GPIO_INPUT | GPIO_PULL_DOWN);
    if (ret != 0) {
        LOG(EERROR, "Error %d: failed to configure %s pin %d",
            ret, button.port->name, button.pin);
        return false;
    }

    /* 配置引脚中断模式，支持上升沿和下降沿触发 */
    ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_BOTH);
    if (ret != 0) {
        LOG(EERROR, "Error %d: failed to configure interrupt on %s pin %d",
            ret, button.port->name, button.pin);
        return false;
    }
    
    /* 配置按键定时器 */
    static struct k_timer dejitter_timer;
    k_timer_init(&dejitter_timer, dejitter_timer_expiry, dejitter_timer_stop);
    key.dejitter_timer = (void*)&dejitter_timer;

    /* 配置按键定时器 */
    static struct k_timer long_pressed_timer;
    k_timer_init(&long_pressed_timer, long_pressed_timer_expiry, long_pressed_timer_stop);
    key.long_pressed_timer = (void*)&long_pressed_timer;

    /* 根据原理图设置默认电平状态，保存按键设备树操作接口 */
    key.event = 
        (gpio_pin_get_dt(&button) == 1)?KEY_EVENT_PRESSED:KEY_EVENT_RELEASE;
    key.gpio_handle = (void*)&button;

    return true;
}


__weak void csk6011a_nano_key_uninit()
{
    k_timer_stop((struct k_timer *)key.dejitter_timer);
    k_timer_stop((struct k_timer *)key.long_pressed_timer);
    key.init                = NULL;
    key.uninit              = NULL;
    key.event_register      = NULL;
    key.event_unregister    = NULL;
    key.gpio_handle         = NULL;
    key.event               = KEY_EVENT_RELEASE;
}


__weak bool csk6011a_nano_key_event_register(on_key_event_t on_key_event, 
                                    void* user_data)
{
    int32_t ret;

    /* 初始化中断回调 */
    gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
    /* 注册回调事件 */
    ret = gpio_add_callback(button.port, &button_cb_data);
    LOG(EDEBUG, "Set up button at %s pin %d", button.port->name, button.pin);

    key.on_event = on_key_event;
    key.user_data = user_data;

    return (ret==0)?true:false;
}


__weak void csk6011a_nano_key_event_unregister()
{
    gpio_remove_callback(button.port, &button_cb_data);
}
