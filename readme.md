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

shell联网并保存到flash中

#### 3.2.6 Websocket案例

websocket和服务端通信

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