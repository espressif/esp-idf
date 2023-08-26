| Supported Targets | ESP32 | ESP32-C3 | ESP32-C6 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- |

ESP-BLE-MESH 和 Wi-Fi Coexistence 示例
==========================================

此示例是用来测试当 ESP-BLE-MESH 进行配网和收发消息的时，Wi-Fi 所能达到的最大 throughput 值。用户可以根据此示例中的 throughput 值来实现自己需要的应用场景。如果用户对 ESP-BLE-MESH 的使用方法还不熟悉，请参考 [ESP-BLE-MESH Node Example](../onoff_models/onoff_server/README.md)。

此示例主要分为两个部分：

- Wi-Fi Iperf 协议
- ESP-BLE-MESH Fast Provisioning Server

Wi-Fi 的 Iperf 协议使用方法请参考 [Wi-Fi Iperf README](../../../wifi/iperf/README.md)。

当 Wi-Fi 的 Iperf 使能之后，ESP-BLE-MESH 就可以使用 fast provision 进行快速配网。此示例中的节点（Node）包含了一个 element，该 element 支持以下 model：

- Configuration Server model：该 model 主要用于负责设备加入 mesh 网络之后的相关配置，如是否支持中继、朋友特性、添加 NetKey 等。
- Configuration Client model：该 model 主要用于获取或设置其他节点的配置，如获取或设置中继、朋友特性、添加 NetKey 等。
- Generic On/Off Server model：该 model 主要用于表示节点 On/Off state 的值，不同状态值决定着设备的不同状态（如灯的开和关）。
- Generic On/Off Client model：该 model 通过设置其他 node 的 On/Off state 来控制设备的开或关。
- Fast Provision Server model：该 model 是为了进行快速配网而实现的自定义 model，当设备配置入网后，会临时作为 Provisioner 对其他设备进行配网，当配网工作完成后，会恢复 node 角色， 并将分配给所有节点的地址通过此 model 传给 Fast Provision Client model。
- Fast Provision Client model：该 model 需要与 Fast Provision Server model 配合使用，用来获取所有 Fast Provisioning Server 分配给节点的地址。
