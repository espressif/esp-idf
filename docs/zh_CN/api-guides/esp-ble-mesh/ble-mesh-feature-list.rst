ESP-BLE-MESH 功能列表
=====================

:link_to_translation:`en:[English]`

支持的功能
----------

Mesh 核心
"""""""""

* 入网
    * PB-ADV，PB-GATT，PB-Remote
    * OOB 验证
    * 基于证书的入网
    * 远程入网
    * 增强入网验证

* 网络
    * 中继
    * 分包和重组
    * 密钥更新程序
    * IV 更新程序
    * 朋友节点
    * 低功耗节点
    * 代理服务器
    * 代理客户端
    * 定向转发
    * 私有信标
    * 子网桥
    * 次要增强

* 多个客户端模型同时运行
    * 支持多个客户端模型同时向不同节点发送数据包
    * 客户端模型与服务器模型之间无阻塞

* NVS 存储
    * 存储 ESP-BLE-MESH 节点的入网和配置信息

Mesh 模型
"""""""""

* 基础模型
    * 配置服务器模型
    * 配置客户端模型
    * 健康服务器模型
    * 健康客户端模型
    * 远程供应服务器模型
    * 远程供应客户端模型
    * 定向转发配置服务器模型
    * 定向转发配置客户端模型
    * 桥接配置服务器模型
    * 桥接配置客户端模型
    * Mesh 专用信标服务器模型
    * Mesh 专用信标客户端模型
    * 按需提供的专用代理服务器模型
    * 按需提供的专用代理客户端模型
    * SAR 配置服务器模型
    * SAR 配置客户端模型
    * 请求 PDU RPL 配置服务器模型
    * 请求 PDU RPL 配置客户端模型
    * 操作码聚合器服务器模型
    * 操作码聚合器客户端模型
    * 大型合成数据服务器模型
    * 大型合成数据客户端模型

* 通用客户端模型
    * 通用开关客户端
    * 通用电平客户端
    * 通用默认过渡时间客户端
    * 通用电源开关客户端
    * 通用功率电平客户端
    * 通用电池客户端
    * 通用位置客户端
    * 通用属性客户端

* 传感器客户端模型
    * 传感器客户端

* 时间与场景客户端模型
    * 时间客户端
    * 场景客户端
    * 调度器客户端

* 照明客户端模型
    * 灯光亮度客户端
    * 灯光 CTL 客户端
    * 灯光 HSL 客户端
    * 灯光 xyL 客户端
    * 灯光 LC 客户端

* 通用服务器模型
    * 通用开关服务器
    * 通用电平服务器
    * 通用默认过渡时间服务器
    * 通用电源开关服务器
    * 通用电源开关设置服务器
    * 通用功率电平服务器
    * 通用功率电平设置服务器
    * 通用电池服务器
    * 通用位置服务器
    * 通用位置设置服务器
    * 通用用户属性服务器
    * 通用管理员属性服务器
    * 通用制造商属性服务器
    * 通用客户端属性服务器

* 传感器服务器模型
    * 传感器服务器
    * 传感器设置服务器

* 时间和场景服务器模型
    * 时间服务器
    * 时间设置服务器
    * 场景服务器
    * 场景设置服务器
    * 调度器服务器
    * 调度器设置服务器

* 照明服务器模型
    * 灯光亮度服务器
    * 灯光亮度设置服务器
    * 灯光 CTL 服务器
    * 灯光 CTL 温度服务器
    * 灯光 CTL 设置服务器
    * 灯光 HSL 服务器
    * 灯光 HSL 色调服务器
    * 灯光 HSL 饱和度服务器
    * 灯光 HSL 设置服务器
    * 灯光 xyL 服务器
    * 灯光 xyL 设置服务器
    * 灯光 LC 服务器
    * 灯光 LC 设置服务器

Mesh 示例
"""""""""""

* ESP-BLE-MESH 节点
    * :example_file:`教程 <bluetooth/esp_ble_mesh/onoff_models/onoff_client/tutorial/BLE_Mesh_Node_OnOff_Client_Example_Walkthrough.md>`
    * :example_file:`教程 <bluetooth/esp_ble_mesh/onoff_models/onoff_server/tutorial/BLE_Mesh_Node_OnOff_Server_Example_Walkthrough.md>`
    * :example:`示例 <bluetooth/esp_ble_mesh/onoff_models>`
* ESP-BLE-MESH 供应者
    * :example_file:`教程 <bluetooth/esp_ble_mesh/provisioner/tutorial/BLE_Mesh_Provisioner_Example_Walkthrough.md>`
    * :example:`示例 <bluetooth/esp_ble_mesh/provisioner>`
* ESP-BLE-MESH 快速入网
    * :example_file:`客户端模型快速入网教程 <bluetooth/esp_ble_mesh/fast_provisioning/fast_prov_client/tutorial/BLE_Mesh_Fast_Prov_Client_Example_Walkthrough.md>`
    * :example_file:`服务器模型快速入网教程 <bluetooth/esp_ble_mesh/fast_provisioning/fast_prov_server/tutorial/BLE_Mesh_Fast_Prov_Server_Example_Walkthrough.md>`
    * :example:`示例 <bluetooth/esp_ble_mesh/fast_provisioning>`
    * `演示视频 <https://dl.espressif.com/BLE/public/ESP32_BLE_Mesh_Fast_Provision.mp4>`__
* ESP-BLE-MESH 及 Wi-Fi 共存
    * :example_file:`教程 <bluetooth/esp_ble_mesh/wifi_coexist/tutorial/BLE_Mesh_WiFi_Coexist_Example_Walkthrough.md>`
    * :example:`示例 <bluetooth/esp_ble_mesh/wifi_coexist>`
    * `演示视频 <https://dl.espressif.com/BLE/public/ESP_BLE_MESH_WIFI_Coexistence.mp4>`__
