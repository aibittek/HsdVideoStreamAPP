#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/printk.h>
#include <sys/sys_heap.h>

#include <net/net_if.h>
#include <net/net_core.h>
#include <net/net_context.h>
#include <net/net_mgmt.h>

#include "csk6/csk_wifi.h"

#include <log.h>
#include <wifi_core.h>

__weak wifi_status_t esp32_c3_wifi_init(void);
__weak wifi_status_t esp32_c3_wifi_uninit(void);
__weak wifi_status_t esp32_c3_wifi_connect_ap(const char* ssid, 
                                            const char* passwd);
__weak wifi_status_t esp32_c3_wifi_disconnect_ap(void);
// __weak wifi_status_t esp32_c3_wifi_scan_ap(void);
__weak bool esp32_c3_wifi_is_connect(void);

wifi_t wifi = {
    .init           = esp32_c3_wifi_init,
    .uninit         = esp32_c3_wifi_uninit,
    .connect_ap     = esp32_c3_wifi_connect_ap,
    .disconnect_ap  = esp32_c3_wifi_disconnect_ap,
    // .scan_ap        = esp32_c3_wifi_scan_ap,
    .is_connect     = esp32_c3_wifi_is_connect,
    .connect_status = false,
};


static csk_wifi_event_cb_t wifi_event_cb;
static csk_wifi_result_t wifi_result;
static struct net_mgmt_event_callback dhcp_cb;
static void handler_cb(struct net_mgmt_event_callback *cb, 
                    uint32_t mgmt_event, struct net_if *iface)
{
    if (mgmt_event != NET_EVENT_IPV4_DHCP_BOUND) {
        return;
    }

    char buf[NET_IPV4_ADDR_LEN];

    snprintf(wifi.ip, sizeof(wifi.ip), "%s", 
        net_addr_ntop(AF_INET, &iface->config.dhcpv4.requested_ip, 
                    buf, sizeof(buf)));
    snprintf(wifi.netmask, sizeof(wifi.netmask), "%s", 
        net_addr_ntop(AF_INET, &iface->config.ip.ipv4->netmask, 
                    buf, sizeof(buf)));
    snprintf(wifi.gw, sizeof(wifi.gw), "%s", 
        net_addr_ntop(AF_INET, &iface->config.ip.ipv4->gw, 
                    buf, sizeof(buf)));

    LOG(EDEBUG, "Your address: %s,Subnet: %s,Router: %s", 
        wifi.ip, wifi.netmask, wifi.gw);
    
    wifi.connect_status = true;
}


static void wifi_event_handler(csk_wifi_event_t events, void *event_data,
                                uint32_t data_len, void *arg)
{
    if (events & CSK_WIFI_EVT_STA_CONNECTED) {
        // wifi.connect_status = true;
        LOG(EDEBUG, "[WiFi sta] connected");
    } else if (events & CSK_WIFI_EVT_STA_DISCONNECTED) {
        wifi.connect_status = false;
        LOG(EDEBUG, "[WiFi sta] disconnected");
    } else {
        abort();
    }
}


__weak wifi_status_t esp32_c3_wifi_init(void)
{
    uint8_t mac_addr[6] = {0};

    /* CSK WiFi 驱动初始化 */
    int rc = csk_wifi_init();
    if (rc != 0) {
        LOG(EERROR, "wifi get mac failed, ret: %d\n", rc);
        return WIFI_ERROR;
    }
    rc = csk_wifi_get_mac(CSK_WIFI_MODE_STA, mac_addr);
    if (rc != 0) {
        LOG(EERROR, "wifi get mac failed, ret: %d\n", rc);
        return WIFI_ERROR;
    }
    snprintf(wifi.mac, sizeof(wifi.mac), "%x:%x:%x:%x:%x:%x", 
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3],
            mac_addr[4], mac_addr[5]);
    LOG(EDEBUG, "mac address:%s", wifi.mac);

    return (rc == 0)?WIFI_OK:WIFI_ERROR;
}


__weak wifi_status_t esp32_c3_wifi_uninit(void)
{
    /* CSK WiFi 驱动初始化 */
    int rc = csk_wifi_deinit();

    return (rc == 0)?WIFI_OK:WIFI_ERROR;
}


__weak wifi_status_t esp32_c3_wifi_connect_ap(const char* ssid, 
                                                const char* passwd)
{
    /* 配置WiFi回调事件参数 */
    wifi_event_cb.handler = &wifi_event_handler;
    wifi_event_cb.events = 
        CSK_WIFI_EVT_STA_CONNECTED | CSK_WIFI_EVT_STA_DISCONNECTED;
    wifi_event_cb.arg = NULL;

    /* 注册WiFi回调事件 */
    csk_wifi_add_callback(&wifi_event_cb);

    /* WiFi参数配置 */
    csk_wifi_sta_config_t sta_config = {0};
    snprintf(sta_config.ssid, sizeof(sta_config.ssid), "%s", ssid);
    snprintf(sta_config.pwd, sizeof(sta_config.pwd), "%s", passwd);
    sta_config.encryption_mode = CSK_WIFI_AUTH_WPA2_PSK;

    int retry_count = 0;
    do {
        LOG(EDEBUG, "connecting to wifi: %s ...", sta_config.ssid);
        /* 连接WiFi */
        int ret = csk_wifi_sta_connect(&sta_config, &wifi_result, K_FOREVER);
        if (ret == 0) {
            snprintf(wifi.ssid, sizeof(wifi.ssid), "%s", ssid);
            snprintf(wifi.passwd, sizeof(wifi.passwd), "%s", passwd);
            break;
        } else {
            if (wifi_result == CSK_WIFI_ERR_STA_FAILED) {
                retry_count++;
                LOG(EERROR, "retry to connecting wifi ... %d", retry_count);
            } else {
                LOG(EERROR, "AP not found or invalid password");
                return WIFI_ERROR;
            }
        }
    } while (retry_count < 10);

    if (retry_count >= 10) return WIFI_ERROR;
    
    /* 打印已连接WiFi信息 */
    LOG(EDEBUG, "--------------------------Current AP info-------------------------------");
    LOG(EDEBUG, "ssid: %s  pwd: %s  bssid: %s  channel: %d  rssi: %d\n",
        sta_config.ssid, sta_config.pwd, sta_config.bssid, sta_config.channel,
        sta_config.rssi);
    LOG(EDEBUG, "------------------------------------------------------------------------");
    /* 初始化并注册 DHCP BOUND 事件，设备获取 ipv4 地址后产生回调 */
    net_mgmt_init_event_callback(&dhcp_cb, handler_cb, NET_EVENT_IPV4_DHCP_BOUND);
    net_mgmt_add_event_callback(&dhcp_cb);
    struct net_if *iface = net_if_get_default();
    if (!iface) {
        LOG(EDEBUG, "wifi interface not available");
        return WIFI_ERROR;
    }
    /* 开启dhcp client，DHCP 用来分配 IP */
    net_dhcpv4_start(iface);

    return WIFI_OK;
}


__weak wifi_status_t esp32_c3_wifi_disconnect_ap(void)
{
    int rc = csk_wifi_sta_disconnect(&wifi_result, K_FOREVER);

    return (rc == 0)?WIFI_OK:WIFI_ERROR;
}


__weak bool esp32_c3_wifi_is_connect(void)
{
    return wifi.connect_status;
}
