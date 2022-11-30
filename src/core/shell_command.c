#include <string.h>
#include <stdlib.h>
#include <log.h>
#include <zephyr/zephyr.h>
#include <zephyr/shell/shell_types.h>
#include <zephyr/shell/shell.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/usb/usb_device.h>
#include <shell_command.h>

#if defined CONFIG_SHELL_GETOPT
#include <getopt.h>
#endif

/* init 指令处理函数 */
static int cmd_wifi_init(const struct shell *shell)
{
    printk("cmd_wifi_init\n");
    return 0;
}
/* scan 指令处理函数 */
static int cmd_wifi_scan(const struct shell *shell)
{
    printk("cmd_wifi_scan\n");
    return 0;
}

/* connect 指令处理函数，附带wifi连接的参数 */
static int cmd_wifi_connect(const struct shell *shell, 
                            size_t argc, char **argv)
{
    printk("cmd_wifi_connect ssid: %s psw: %s retry: %d \n", 
        argv[1], argv[2], atoi(argv[3]));

    return 0;
}

/// @brief 添加wifi命令的子命令(init、scan、connect)
SHELL_STATIC_SUBCMD_SET_CREATE(init_deinit_test,
SHELL_CMD(init, NULL, "init wifi", cmd_wifi_init),
SHELL_CMD(scan, NULL, "scan wifi ap", cmd_wifi_scan),
SHELL_CMD(connect, NULL, 
    "<ssid> <pwd> <save>, example:mimi 12345678 1", 
    cmd_wifi_connect),
SHELL_SUBCMD_SET_END /* Array terminated. */
);

/// @brief 添加WiFi命令集的根命令
SHELL_CMD_REGISTER(wifi, &init_deinit_test, "wifi test", NULL);

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
