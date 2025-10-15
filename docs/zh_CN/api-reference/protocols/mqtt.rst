ESP-MQTT
========

:link_to_translation:`en:[English]`

概述
--------

ESP‑MQTT 是 `MQTT <https://mqtt.org/>`__ 协议客户端的实现，属于轻量级发布/订阅消息协议。ESP‑MQTT 现已支持 `MQTT v5.0 <https://docs.oasis-open.org/mqtt/mqtt/v5.0/os/mqtt-v5.0-os.html>`__。

自 ESP‑IDF v6.0 起，``ESP‑MQTT`` 组件已从 ESP‑IDF 中移至独立仓库：

* `esp-mqtt 组件仓库 <https://github.com/espressif/esp-mqtt>`__

如需在项目中添加 ESP‑MQTT 组件，请运行 ``idf.py add-dependency espressif/mqtt``。

托管文档
---------------------

文档链接如下：

* `esp-mqtt 文档 <https://docs.espressif.com/projects/esp-mqtt/zh_CN/latest/>`__

应用示例
--------------------

ESP‑IDF 中提供了两个参考示例：

- 基于 TLS（证书包）的 MQTT 示例：``examples/protocols/mqtt``
  - 默认使用 ESP‑IDF 证书包进行服务器验证。
  - 默认 Broker URI 为 ``mqtts://test.mosquitto.org:8886`` （Let's Encrypt 链）。
  - 也可固定 Mosquitto CA，并使用 ``mqtts://test.mosquitto.org:8883``。

- 基于 TLS（证书包）的 MQTT 5.0 示例：``examples/protocols/mqtt5``
  - 使用 MQTT v5.0 与 TLS，并通过证书包进行验证。
  - 默认 Broker URI 为 ``mqtts://test.mosquitto.org:8886``。

如需更多传输模式（TCP、WS/WSS、双向认证等）的示例，请参考 `espressif/mqtt 组件仓库 <https://components.espressif.com/components/espressif/mqtt>`__。

