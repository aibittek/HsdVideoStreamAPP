#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <log.h>
#include <zephyr/zephyr.h>
#include <zephyr/shell/shell_types.h>
#include <zephyr/shell/shell.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/usb/usb_device.h>
#include <shell_command.h>
#include <zephyr/fs/fs.h>
#include <log.h>
#include <flash_core.h>
#include <cJSON_user_define.h>
#include <wifi_core.h>

#if defined CONFIG_SHELL_GETOPT
#include <getopt.h>
#endif


/* init 指令处理函数 */
// static int cmd_wifi_init(const struct shell *shell)
// {
//     LOG(EDEBUG, "cmd_wifi_init\n");
//     return 0;
// }
/* scan 指令处理函数 */
// static int cmd_wifi_scan(const struct shell *shell)
// {
//     LOG(EDEBUG, "cmd_wifi_scan\n");
//     return 0;
// }


static int cmd_write_to_flash(const char* pathname, const char* data)
{
    int rc;
    ssize_t size;
    struct fs_file_t file = {0};    /*!< 初始化结构体 */    

    // 初始化flash
    user_flash_init();

    // 写入文件
    rc = fs_open(&file, pathname, FS_O_CREATE | FS_O_WRITE);
    if (rc < 0) {
        LOG(EERROR, "FAIL: open %s: %d", pathname, rc);
        return -1;
    }

    /* 写入文件 */
    size = fs_write(&file, data, strlen(data));
    if (size < 0) {
        LOG(EERROR, "FAIL: write %s: %d", pathname, rc);
        goto out;
    }

    LOG(EDEBUG, "write %s to file %s successed", data, pathname);

out:
    fs_close(&file);
    return rc;
}


/* connect 指令处理函数，附带wifi连接的参数 */
static int cmd_wifi_connect(const struct shell *shell, 
                            size_t argc, char **argv)
{
    int rc = 0;
    char json_data[100] = {0};
    int data_len = sizeof(json_data);

    LOG(EDEBUG, "cmd_wifi_connect ssid: %s psw: %s save: %d \n", 
        argv[1], argv[2], atoi(argv[3]));

    // 判断是否已经初始化Wi-Fi模块
    if (!wifi.connect_status) {
        if (WIFI_OK != wifi.init()) {
            LOG(EERROR, "wifi init failed");
            return -1;
        }
        // 连接AP热点
        if (WIFI_OK != wifi.connect_ap(argv[1], argv[2])) {
            LOG(EERROR, "wifi connect ap ssid:%s failed", argv[1]);
            return -1;
        }
    }

    // 判断是否需要把SSID和Password保存到Flash中
    if (atoi(argv[3]) == 1) {
        // 构造json数据，格式为
        // {
        //     "ssid": "xxxxxx",
        //     "passwd": "xxxxxx"
        // }
        LOG(EDEBUG, "start json");
        JSON_SERIALIZE_CREATE_OBJECT_START(json_root_obj);
        JSON_SERIALIZE_ADD_STRING_TO_OBJECT(json_root_obj, "ssid", argv[1]);
        JSON_SERIALIZE_ADD_STRING_TO_OBJECT(json_root_obj, "passwd", argv[2]);
        JSON_SERIALIZE_STRING(json_root_obj, json_data, data_len);
        JSON_SERIALIZE_CREATE_END(json_root_obj);
        LOG(EDEBUG, "json data:%s", json_data);

        rc = cmd_write_to_flash(SHELL_COMMAND_WIFI_AP_INFO_PATHNAME, 
                                json_data);
        if (0 != rc) {
            LOG(EERROR, "can not write %s to %s", 
                json_data, SHELL_COMMAND_WIFI_AP_INFO_PATHNAME);
        }
    }

    return rc;
}

/// @brief 添加wifi命令的子命令(init、scan、connect)
SHELL_STATIC_SUBCMD_SET_CREATE(init_deinit_command,
// SHELL_CMD(init, NULL, "init wifi", cmd_wifi_init),
// SHELL_CMD(scan, NULL, "scan wifi ap", cmd_wifi_scan),
SHELL_CMD(connect, NULL, 
    "<ssid> <pwd> <save>, example:mimi 12345678 1", 
    cmd_wifi_connect),
SHELL_SUBCMD_SET_END /* Array terminated. */
);

/// @brief 添加WiFi命令集的根命令
SHELL_CMD_REGISTER(wifi, &init_deinit_command, "wifi command", NULL);

void shell_command_init(void)
{
#if defined(CONFIG_USB_UART_CONSOLE)
    const struct device *dev;
    uint32_t dtr = 0;
    
    /* 获取Shell设备实例 */
    dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_shell_uart));
    if (!device_is_ready(dev) || usb_enable(NULL)) {
        LOG(EDEBUG, "device is not ready");
        return;
    }

    while (!dtr) {
        uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
        k_sleep(K_MSEC(100));
    }
#endif
}
