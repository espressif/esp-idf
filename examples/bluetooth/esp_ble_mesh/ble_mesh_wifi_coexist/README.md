| Supported Targets | ESP32 |
| ----------------- | ----- |

ESP BLE Mesh and WiFi Coexist example
========================

此 example 是用来测试当 BLE Mesh 实现正常的配网和收发消息的正常功能时，Wi-Fi 所能达到的最大 throughput 值。用户可以根据此 example 中的 throughput 值来实现自己需要的应用场景。如果用户对 ESP32 BLE Mesh 的使用方法还不熟悉，请参考[BLE Mesh Node Example](../ble_mesh_node/onoff_server/README.md).

此demo主要分为两个部分：

- WiFi Iperf协议
- BLE Mesh fast provision Server

WiFi的Iperf协议使用方法请参考[WiFi Iperf README](../../../wifi/iperf/README.md)

当WiFi的Iperf开起来之后，BLE Mesh就可以使用fast provsion 进行配网了，此demo只实现了fast provsion 的server功能。此demo 的BLE Mesh模块实现了一个Element和以下几个Model：

- Config Server Model: 此Model是当对方需要配置APP Key, Dev Key,等信息时，需要使用此Model
- Config Client Model： 此Model是当需要配置APP Key, Dev Key,等信息时，需要使用此Model
- Generic OnOff Server Model：此Model通过暴露自己的OnOff State，从而实现LED 灯的开关功能
- Generic OnOff Client Model： 使用此Model可以实现开关功能，控制别的node 的LED灯的开关
- Fast Provision Server Model： 此Model是为了进行快速配网而实现的自定义Model，通过此Model当节点作为临时provisioner进行配网成功后，需要将生成的Element地址通过此Model进行传给provisioner
- Fast Provision Client Model：此Model和Fast Provision Server Model是配合使用的