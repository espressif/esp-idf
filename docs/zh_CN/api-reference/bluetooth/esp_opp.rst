蓝牙\ :sup:`®` OPP API
======================

:link_to_translation:`en:[English]`

概述
----

OPP（Object Push Profile，对象推送配置文件）通过 OBEX 在蓝牙设备之间推送 vCard 等对象及其他文件。常用于联系人交换、文件共享及类似的单向对象传输场景。OPP API 同时提供服务器和客户端两种角色的功能。

应用示例
--------

- :example:`bluetooth/bluedroid/classic_bt/bt_opp_server` 演示如何实现接收对象的 OPP 服务器。
- :example:`bluetooth/bluedroid/classic_bt/bt_opp_client` 演示如何实现 OPP 客户端，按设备名称发现服务器并发送示例 vCard。

API 参考
--------

.. include-build-file:: inc/esp_opp_api.inc
