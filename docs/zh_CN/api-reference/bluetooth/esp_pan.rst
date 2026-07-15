蓝牙\ :sup:`®` PAN API
======================

:link_to_translation:`en:[English]`

概述
----

PAN（Personal Area Networking，个人区域网络）通过 BNEP（Bluetooth Network Encapsulation Protocol，蓝牙网络封装协议）在经典蓝牙上实现 IP 组网。设备可作为 PAN 用户（PANU）、组网中心（GN）或网络接入点（NAP），使对端设备能够交换以太网帧并获得网络访问。

应用示例
--------

- :example:`bluetooth/bluedroid/classic_bt/bt_pan_nap` 演示如何使用 PAN 网络接入点（NAP）角色，使 PANU 对端可通过 ESP 设备的 Wi-Fi STA 上行链路访问互联网。

- :example:`bluetooth/bluedroid/classic_bt/bt_pan_panu` 演示如何使用 PAN 用户（PANU）角色连接 NAP（例如 ``bt_pan_nap`` 或开启蓝牙网络共享的手机），并验证 IP 连通性。

- :example:`bluetooth/bluedroid/classic_bt/bt_pan_gn` 演示如何使用 PAN 组网（GN）角色作为中枢，桥接多个 PANU 客户端，使其能在同一子网内通信。

API 参考
--------

.. include-build-file:: inc/esp_pan_api.inc
