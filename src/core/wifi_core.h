/*
 * @Author: AIoTMaker kuili@iflytek.com
 * @Date: 2022-11-30 21:28:10
 * @LastEditors: AIoTMaker kuili@iflytek.com
 * @LastEditTime: 2022-12-01 12:39:46
 * @FilePath: \HsdVideoStreamAPP\src\core\wifi_core.h
 * @Description: 
 * 
 * Copyright (c) 2022 by AIoTMaker kuili@iflytek.com, All Rights Reserved. 
 */
#ifndef _WIFI_BLE_H
#define _WIFI_BLE_H

#include <stdbool.h>

/// \brief Wi-Fi模块的返回状态
typedef enum {
    WIFI_OK,
    WIFI_ERROR
}wifi_status_t;

/// \brief Wi-Fi模块的当前模式
typedef enum {
    WIFI_NONE_MODE      = 0,
    WIFI_STA_MODE       = 1,
    WIFI_AP_MODE        = 2,
    WIFI_AP_STA_MODE    = 3,
}wifi_mode_t;

/**
 * @brief
 *  封装了STA相关的接口，后续增加AP和其他相关接口功能
 */
typedef struct {
    char mac[24];           /*!< MAC地址 */
    char ip[16];            /*!< 设备IP */
    char netmask[16];       /*!< 子网掩码 */
    char gw[16];            /*!< 网关 */
    char ssid[32];          /*!< 连接热点名称 */
    char passwd[32];        /*!< 连接热点密码 */

    /**
    * @brief 初始化wifi模块
    *
    * @return wifi_status_t 返回状态
    */
    wifi_status_t (*init)(void);

    /**
    * @brief 反初始化wifi模块
    *
    * @return wifi_status_t 返回状态
    */
    wifi_status_t (*uninit)(void);

    /**
    * @brief 连接热点
    *
    * @return wifi_status_t 返回状态
    */
    wifi_status_t (*connect_ap)(const char* ssid, const char* passwd);

    /**
     * @brief: 断开热点
     * @return wifi_status_t 返回状态
     */    
    wifi_status_t (*disconnect_ap)(void);

    // 
    /**
     * @brief: 扫描可用热点
     * @return wifi_status_t 返回状态
     */    
    // wifi_status_t (*scan_ap)(void);

    /**
     * @brief: 检查Wi-Fi模块是否工作正常
     * @return wifi_status_t 返回状态
     */
    bool (*is_connect)(void);
    /*!< Wi-Fi当前连接状态 */
    bool connect_status;
    
}wifi_t;

///< wifi单例
///< ESP32-C3产品说明：https://www.espressif.com.cn/zh-hans/products/socs/esp32-c3
extern wifi_t wifi;

#endif
