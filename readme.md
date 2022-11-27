## Wi-Fi自动联网并上报头肩识别到视频流服务器



| 日期       | 修订号 | 内容                                                         | 修订人 |
| ---------- | ------ | ------------------------------------------------------------ | ------ |
| 2022-11-27 | v1.0   | 1. 长按用户按键3秒后，Wi-Fi进入配网模式；<br />2. Wi-Fi通过SoftAP提供HTTP Server，手机连接设备热点配置设备连接的热点和服务端信息；<br />3. 设备连接上Wi-Fi热点后，保存Wi-Fi账号和密码、服务端连接信息；<br />4. 设备连接成功后，打开摄像头并尝试连接服务器，并发送识别数据到服务器；<br />5. 服务端解析数据并绘制识别结果； | 李奎   |
|            |        |                                                              |        |



## 一、案例演示



## 二、整体框架



## 三、功能列表及说明

本方案涉及到的技术方案主要分为两部分。

- **设备端：**Key、LED、cJson、Wi-Fi、HTTP、Websocket
- **服务端：**Nodejs、Node-RED



### 3.1 简要说明

| 功能列表  | 实现功能                                                     | 案例说明                                                     |
| --------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| Key       | 实现**短按、长按**检测功能；                                 | key_event_register(key_handle, event_cb, user_data);         |
| LED       | 实现**亮、灭、闪烁、呼吸**四个功能；                         | led.on(RED_LED);<br />led.off(RED_LED);<br />led.blink(RED_LED, 500, 5000);<br />led.breathing(RED_LED, 1000, 0); |
| cJson     | 实现**cJson序列化和反序列化**的封装；                        |                                                              |
| Wi-Fi     | 实现**AP、Sta**、连接TCP的基本功能；                         | wifi_ble.set_wifi_mode();<br />wifi_ble.apconnect(ssid, password); |
| HTTP      | 实现**HTTP服务端**，提供网页功能支持;<br />实现**HTTP客户端**，支持Get和Post； | http_server.start();<br />http_client.get(url, head, head_len, response);<br />http_client.post(url, head, head_len, post, post_len, response); |
| Websocket | 实现**Websocket客户端**数据流通信；                          | ws_client.start(url, head, head_len, ws_cb, user_data);      |



### 3.2 案例演示

#### 3.2.1 Key案例

#### 3.2.2 Led案例



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