## Wi-Fi自动联网并上报头肩识别到视频流服务器



| 日期       | 修订号 | 内容                                                         | 修订人 |
| ---------- | ------ | ------------------------------------------------------------ | ------ |
| 2022-11-27 | v1.0   | 1. 长按用户按键3秒后，Wi-Fi进入配网模式；<br />2. Wi-Fi通过SoftAP提供HTTP Server，手机连接设备热点配置设备连接的热点和服务端信息；<br />3. 设备连接上Wi-Fi热点后，保存Wi-Fi账号和密码、服务端连接信息；<br />4. 设备连接成功后，打开摄像头并尝试连接服务器，并发送识别数据到服务器；<br />5. 服务端解析数据并绘制识别结果； | 李奎   |
|            |        |                                                              |        |



[TOC]

## 一、案例演示

### 1.1 案例演示

实现了哪些功能；

提供演示视频或gif：如果视频使用B站，如果gif演示案例最终呈现的效果；



### 1.2 环境搭建

#### 1.2.1 服务器搭建

#### 1.2.2 服务器插件下载

#### 1.2.3 服务器部署

#### 1.2.4 设备源码下载

#### 1.2.5 设备源码编译

#### 1.2.6 设备源码烧录

#### 1.2.7 设备配网说明

#### 1.2.8 联调验证



## 二、整体框架



## 三、功能列表及说明

本方案涉及到的技术方案主要分为两部分。

- **设备端：**Key、LED、cJson、Wi-Fi、HTTP、Websocket
- **服务端：**Nodejs、Node-RED



### 3.1 简要说明

| 功能列表  | 实现功能                                                     | 案例说明                                                     |
| --------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| LOG       | 实现了**日志分级别管理**，实现简单；                         | LOG(EDEBUG, "hello world");                                  |
| Key       | 实现**短按、长按**检测功能；                                 | key_event_register(key_handle, event_cb, user_data);         |
| LED       | 实现**亮、灭、闪烁、呼吸**四个功能；                         | led.on(RED_LED);<br />led.off(RED_LED);<br />led.blink(RED_LED, 500, 5000);<br />led.breathing(RED_LED, 1000, 0); |
| cJson     | 实现**cJson序列化和反序列化**的封装；                        |                                                              |
| Wi-Fi     | 实现**AP、Sta**、连接TCP的基本功能；                         | wifi_ble.set_wifi_mode();<br />wifi_ble.apconnect(ssid, password); |
| HTTP      | 实现**HTTP服务端**，提供网页功能支持;<br />实现**HTTP客户端**，支持Get和Post； | http_server.start();<br />http_client.get(url, head, head_len, response);<br />http_client.post(url, head, head_len, post, post_len, response); |
| Websocket | 实现**Websocket客户端**数据流通信；                          | ws_client.start(url, head, head_len, ws_cb, user_data);      |



### 3.2 案例演示

对于封装的功能模块以下提供了测试案例，这些案例的代码放在了`src/test`目录下。



#### 3.2.1 LOG应用案例

参考log_test.c文件，内容如下。

```c
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

```

打印显示内容：

```shell
*** Booting Zephyr OS build v1.1.1-alpha.2  ***
[log_test.c module1:15][EDEBUG][module1] log
[log_test.c module2:19][EINFO][module2] log
[log_test.c module3:23][EERROR][module3] log
[log_test.c module2:19][EINFO][module2] log
[log_test.c module3:23][EERROR][module3] log
[log_test.c module3:23][EERROR][module3] log
```



#### 3.2.1 Key应用案例

通过以下按键原理图分析，硬件本身没办法完全做到按键消抖，查看案例没有增加这块，为了能够正常产品中使用按键的功能，增加按键消抖，并支持长按的功能（默认3秒长按触发），用户可按照自己需求修改。

![image-20221129115122244](https://cdn.jsdelivr.net/gh/aibittek/ImageHost/img/20221129115200.png)

参考key_test.c文件的使用案例，内容如下，实现了按键**定时器消抖，短按、长按**的功能实现。

```c
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

```

打印显示内容：

```shell
[key_test.c on_key_event:12][EDEBUG]key is pressed.
[key_test.c on_key_event:15][EDEBUG]key is long pressed.
[key_test.c on_key_event:9][EDEBUG]key is released.
```



#### 3.2.2 Led应用案例

通过以下的LED原理图分析，硬件本身没办法完全做到按键消抖，查看案例没有增加这块，为了能够正常产品中使用按键的功能，增加按键消抖，并支持长按的功能（默认3秒长按触发），用户可按照自己需求修改。



#### 3.2.3 自定义Shell命令案例





#### 3.2.4 Flash应用案例

烧录

```shell
lisa zep exec cskburn -s \\.\COMx -C 6 0x700000 littlefs_imge.bin -b 748800
```

显示内容

```shell
Partition 1: 0x00700000 (2048.00 KB) - littlefs_imge.bin
Waiting for device...
Entering update mode...
Detected flash size: 16 MB
Burning partition 1/1... (0x00700000, 2048.00 KB)
2048.00 KB / 2048.00 KB (100.00%)
Finished

√ exec exit
```

代码运行结果

```shell
[flash_core.c littlefs_flash_erase:26][EDEBUG]Area 0 at 0x700000 on FLASH_CTRL for 1048576 bytes

[flash_core.c littlefs_mount:81][EDEBUG]/lfs1 automounted

[flash_core.c user_flash_init:172][EDEBUG]/lfs1: bsize = 16 ; frsize = 4096 ; blocks = 256 ; bfree = 253
[flash_core.c lsdir:125][EDEBUG]Listing dir /lfs1 ...
[flash_core.c lsdir:141][EDEBUG][FILE] boot_count (size = 1)
[flash_core.c lsdir:141][EDEBUG][FILE] pattern.bin (size = 547)
[flash_core.c lsdir:141][EDEBUG][FILE] test.txt (size = 36)
[flash_test.c flash_test:75][EDEBUG]read from /lfs1/test.txt, data:this is a flash read/write test data
```



#### 3.2.5 Wi-Fi Sta模式案例

应用层测试代码

```c
#include <wifi_core.h>
#include <log.h>

void wifi_test(void)
{
    // 初始化Wi-Fi模块
    wifi.init();

    // 连接AP热点
    wifi.connect_ap("mimi", "xxxxxxxx");
}

```

代码运行结果

```c
[wifi_core.c esp32_c3_wifi_init:94][EDEBUG]mac address:10:91:a8:3e:9b:54
[wifi_core.c esp32_c3_wifi_connect_ap:129][EDEBUG]connecting to wifi: mimi ...
[wifi_core.c esp32_c3_wifi_connect_ap:150][EDEBUG]--------------------------Current AP info-------------------------------
[wifi_core.c esp32_c3_wifi_connect_ap:151][EDEBUG]ssid: mimi  pwd: xxxxxxxx  bssid: 52:fc:57:12:03:ec  channel: 1  rssi: -48
[wifi_core.c esp32_c3_wifi_connect_ap:154][EDEBUG]------------------------------------------------------------------------
[wifi_core.c handler_cb:56][EDEBUG]Your address: 192.168.43.96,Subnet: 255.255.255.0,Router: 192.168.43.1
```

shell联网并保存到flash中，修改shell命令的实现，实现了把连接成功的Wi-Fi信息保存到了Flash中，下次设备上电时可以根据保存的Wi-Fi联网信息自动联网。

```c
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
```

串口打印结果

```shell
uart:~$
  clear    device   devmem   flash    help     history  kernel   pwm
  resize   shell    wifi
uart:~$ wifi connect mimi 12348765 1
[shell_command.c cmd_wifi_connect:75][EDEBUG]cmd_wifi_connect ssid: mimi psw: 12348765 save: 1

[shell_command.c cmd_wifi_connect:98][EDEBUG]start json
[shell_command.c cmd_wifi_connect:104][EDEBUG]json data:{"ssid":"mimi","passwd":"xxxxxxxx"}
[shell_command.c cmd_write_to_flash:59][EDEBUG]write {"ssid":"mimi","passwd":"xxxxxxxx"} to file /lfs/wifi.conf successed
```



#### 3.2.6 UDP网络通信案例

udp_test.c文件内容

```c
#include <string.h>
#include <net/net_ip.h>
#include <net/socket.h>
#include <delay.h>
#include <log.h>
#include <wifi_core.h>

void udp_test()
{
    const char* send_msg = "this is a udp test message.";
    const char* server_ip = "192.168.43.66";
    const short server_port = 8888;
    struct sockaddr_in addr = {0};

    while(!wifi.connect_status) { delay_ms(100); }

    // 初始化本地socket，获取套接字描述符
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        LOG(EERROR, "socket init failed");
        return ;
    }
    
    // 配置UDP服务端IP和端口号
    net_sin((struct sockaddr *)&addr)->sin_family = AF_INET;
    net_sin((struct sockaddr *)&addr)->sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &net_sin((struct sockaddr *)&addr)->sin_addr);

    // 循环发送数据到服务端
    for (;;) {
        ssize_t len = sendto(fd, send_msg, strlen(send_msg), 0, 
                            (struct sockaddr*)&addr, sizeof(addr));
        if (len < 0) {
            LOG(EERROR, "socket send failed");
        }
        delay_ms(1000);
    }
}
```

服务端接收内容

<img src="https://cdn.jsdelivr.net/gh/aibittek/ImageHost/img/20221201194641.png" alt="image-20221201194622868" style="zoom: 67%;" />



#### 3.2.7 头肩识别案例

移植



## 四、功能开发流程

### 4.1 新建基本工程

基于hello world创建功能，工程命名为HsdVideoStreamApp

```
lisa zep create
```



## 五、API接口文档

由于本案例实现的代码遵循了`Doxygen`的注释规范，已经自动生成了注释文档，文档参考链接：



## 六、源码链接

本案例的源代码已经上传到了`github`，有兴趣的可以下载体验：

```shell
git clone https://github.com/aibittek/HsdVideoStreamAPP.git
```



## 七、问题

【文档错误】在开发实践->外设驱动->PWM的使用示例的原理图下面，控制引脚是GPIOA_06改为GPIOB_06

【文档错误】在开发实践->系统服务->网络->WIFI连接 编译和烧录->编译中使用了lisa zep build -b csk6002_9s_nano版型，实际通过验证lisa zep build -b csk6011a_c3_nano编译通过

【网络问题】网络通信模块等待固定30秒才能进入main函数，试了tcp、websocket_client原始代码都是这样



## 八、疑问

【网络疑问】wifi_ap的demo怎么开dhcp服务给连接上来的设备提供IP，是否有相关案例，最好有ap配网的案例；



## 九、期待



## 十、引脚使用

```
I2C0：
B0
B1

DVP：
A4
A5
A6
A7
A8
A9
A12
A13
A14
B7
B8
B9

gc032a：A6

WIFI:
SPI1:
PA10
PA11

UART0:
PA2
PA3

PWM2:

KEY:B5
LED:B6
PA16

SPI0-flash:
A17
A18
A19
A20

SERVO:B2
```

