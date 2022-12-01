#include <wifi_core.h>
#include <log.h>

void wifi_test(void)
{
    // 初始化Wi-Fi模块
    wifi.init();

    // 连接AP热点
    wifi.connect_ap("mimi", "12348765");
}
