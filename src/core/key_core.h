#ifndef KEY_CORE_H
#define KEY_CORE_H

#include <stdbool.h>

/// 按键消抖延迟事件，根据实际硬件情况更改，机械按键推荐5-10毫秒
#define KEY_DEJITTER_TIME       10
/// 长按事件的时间间隔
#define KYE_LONG_PRESSED_TIME   3000

/// \brief 支持的按键事件
typedef enum {
    KEY_EVENT_RELEASE,
    KEY_EVENT_PRESSED,
    KEY_EVENT_LONG_PRESSED,
} key_event_t;

/// 定义按键的事件回调函数指针
typedef void (*on_key_event_t)(key_event_t event, void* user_data);

/**
 * @brief 按键用户核心层接口
 * 
 * @details
 *  实现了按键的引脚配置，使用init完成；
 *  实现了按键的事件注册，支持的事件
 */
typedef struct key_ {

    /// 记录按键当前的状态
    key_event_t         event;

    /// 引脚配置句柄
    void*               gpio_handle;

    /// 按键消抖定时器
    void*               dejitter_timer;
    /// 长按判断定时器
    void*               long_pressed_timer;

    /// 按键事件回调函数指针
    on_key_event_t      on_event;
    void*               user_data;   

    /**
     * @brief 按键初始化
     * 
     * @return  成功返回true, 失败返回false
     */
    bool (*init)(void);

    /**
     * @brief 按键逆初始化
     * 
     */
    void (*uninit)(void);

    /**
     * @brief: 注册按键事件
     * 
     * @param on_key_event 按键事件
     * @param user_data 向按键回调提供的用户私有数据
     * 
     * @return 成功返回true，识别返回false
     */
    bool (*event_register)(on_key_event_t on_key_event, void* user_data);

    /**
     * @brief: 反注册按键事件
     * 
     */
    void (*event_unregister)(void);

} user_key_t;

/*!< 按键单例 */
extern user_key_t key;

#endif
