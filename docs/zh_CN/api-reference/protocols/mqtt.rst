ESP-MQTT
========

:link_to_translation:`en:[English]`

概述
--------

ESP-MQTT 是 `MQTT <https://mqtt.org/>`__ 协议客户端的实现，MQTT 是一种基于发布/订阅模式的轻量级消息传输协议。ESP-MQTT 当前支持 `MQTT v5.0 <https://docs.oasis-open.org/mqtt/mqtt/v5.0/os/mqtt-v5.0-os.html>`__。


特性
--------

   * 支持基于 TCP 的 MQTT、基于 Mbed TLS 的 SSL、基于 WebSocket 的 MQTT 以及基于 WebSocket Secure 的 MQTT
   * 通过 URI 简化配置流程
   * 多个实例（一个应用程序中有多个客户端）
   * 支持订阅、发布、认证、遗嘱消息、保持连接心跳机制以及 3 个服务质量 (QoS) 级别（组成全功能客户端）


应用示例
-------------------

   * :example:`protocols/mqtt/tcp`：基于 TCP 的 MQTT，默认端口 1883
   * :example:`protocols/mqtt/ssl`：基于 TLS 的 MQTT，默认端口 8883
   * :example:`protocols/mqtt/ssl_ds`：基于 TLS 的 MQTT，使用数字签名外设进行身份验证，默认端口 8883
   * :example:`protocols/mqtt/ssl_mutual_auth`：基于 TLS 的 MQTT，使用证书进行身份验证，默认端口 8883
   * :example:`protocols/mqtt/ssl_psk`：基于 TLS 的 MQTT，使用预共享密钥进行身份验证，默认端口 8883
   * :example:`protocols/mqtt/ws`：基于 WebSocket 的 MQTT，默认端口 80
   * :example:`protocols/mqtt/wss`：基于 WebSocket Secure 的 MQTT，默认端口 443
   * :example:`protocols/mqtt5`: 使用 ESP-MQTT 库连接 MQTT v5.0 的服务器

MQTT 消息重传
--------------------------

调用 :cpp:func:`esp_mqtt_client_publish <esp_mqtt_client_publish()>` 或其非阻塞形式 :cpp:func:`esp_mqtt_client_enqueue <esp_mqtt_client_enqueue()>`，可以创建新的 MQTT 消息。

QoS 0 的消息将只发送一次，QoS 1 和 2 具有不同行为，因为协议需要执行额外步骤来完成该过程。

ESP-MQTT 库将始终重新传输未确认的 QoS 1 和 2 发布消息，以避免连接错误导致信息丢失，虽然 MQTT 规范要求仅在重新连接且 Clean Session 标志设置为 0 时重新传输（针对此行为，将 :cpp:member:`disable_clean_session <esp_mqtt_client_config_t::session_t::disable_clean_session>` 设置为 true）。

可能需要重传的 QoS 1 和 2 消息总是处于排队状态，但若使用 :cpp:func:`esp_mqtt_client_publish <esp_mqtt_client_publish>` 则会立即进行第一次传输尝试。未确认消息的重传将在 :cpp:member:`message_retransmit_timeout <esp_mqtt_client_config_t::session_t::message_retransmit_timeout>` 之后进行。在 :ref:`CONFIG_MQTT_OUTBOX_EXPIRED_TIMEOUT_MS` 之后，消息会过期并被删除。如已设置 :ref:`CONFIG_MQTT_REPORT_DELETED_MESSAGES`，则会发送事件来通知用户。

配置
-------------

通过设置 :cpp:class:`esp_mqtt_client_config_t` 结构体中的字段来进行配置。配置结构体包含以下子结构体，用于配置客户端的多种操作。

   * :cpp:class:`esp_mqtt_client_config_t::broker_t` - 允许设置地址和安全验证。
   * :cpp:class:`esp_mqtt_client_config_t::credentials_t` - 用于身份验证的客户端凭据。
   * :cpp:class:`esp_mqtt_client_config_t::session_t` - MQTT 会话相关配置。
   * :cpp:class:`esp_mqtt_client_config_t::network_t` - 网络相关配置。
   * :cpp:class:`esp_mqtt_client_config_t::task_t` - 允许配置 FreeRTOS 任务。
   * :cpp:class:`esp_mqtt_client_config_t::buffer_t` - 输入输出的缓冲区大小。

下文将详细介绍不同配置。

服务器
^^^^^^^^^^^^

===========
地址
===========

通过 :cpp:class:`address <esp_mqtt_client_config_t::broker_t::address_t>` 结构体的 :cpp:member:`uri <esp_mqtt_client_config_t::broker_t::address_t::uri>` 字段或者 :cpp:member:`hostname <esp_mqtt_client_config_t::broker_t::address_t::hostname>`、:cpp:member:`transport <esp_mqtt_client_config_t::broker_t::address_t::transport>` 以及 :cpp:member:`port <esp_mqtt_client_config_t::broker_t::address_t::port>` 的组合，可以设置服务器地址。也可以选择设置 :cpp:member:`path <esp_mqtt_client_config_t::broker_t::address_t::path>`，该字段对 WebSocket 连接而言非常有用。

使用 :cpp:member:`uri <esp_mqtt_client_config_t::broker_t::address_t::uri>` 字段的格式为 ``scheme://hostname:port/path``。

- 当前支持 ``mqtt``、``mqtts``、``ws`` 和 ``wss`` 协议
- 基于 TCP 的 MQTT 示例：

   -  ``mqtt://mqtt.eclipseprojects.io``：基于 TCP 的 MQTT，默认端口 1883
   -  ``mqtt://mqtt.eclipseprojects.io:1884``：基于 TCP 的 MQTT，端口 1884
   -  ``mqtt://username:password@mqtt.eclipseprojects.io:1884``：基于 TCP 的 MQTT，
      端口 1884，带有用户名和密码

- 基于 SSL 的 MQTT 示例：

   -  ``mqtts://mqtt.eclipseprojects.io``：基于 SSL 的 MQTT，端口 8883
   -  ``mqtts://mqtt.eclipseprojects.io:8884``：基于 SSL 的 MQTT，端口 8884

- 基于 WebSocket 的 MQTT 示例：

   -  ``ws://mqtt.eclipseprojects.io:80/mqtt``

- 基于 WebSocket Secure 的 MQTT 示例：

   -  ``wss://mqtt.eclipseprojects.io:443/mqtt``

- 最简配置：

.. code-block:: c

    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://mqtt.eclipseprojects.io",
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);

.. note::

   默认情况下，MQTT 客户端使用事件循环库来发布相关 MQTT 事件（已连接、已订阅、已发布等）。

=============
验证
=============

为验证服务器身份，对于使用 TLS 的安全链接，必须设置 :cpp:class:`verification <esp_mqtt_client_config_t::broker_t::verification_t>` 结构体。
服务器证书可设置为 PEM 或 DER 格式。如要选择 DER 格式，必须设置等效 :cpp:member:`certificate_len <esp_mqtt_client_config_t::broker_t::verification_t::certificate_len>` 字段，否则应在 :cpp:member:`certificate <esp_mqtt_client_config_t::broker_t::verification_t::certificate>` 字段传入以空字符结尾的 PEM 格式字符串。

-  从服务器获取证书，例如：``mqtt.eclipseprojects.io``
    .. code::

       openssl s_client -showcerts -connect mqtt.eclipseprojects.io:8883 < /dev/null \
       2> /dev/null | openssl x509 -outform PEM > mqtt_eclipse_org.pem

-  检查示例应用程序：:example:`protocols/mqtt/ssl`
-  配置：

.. code:: c

    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker = {
          .address.uri = "mqtts://mqtt.eclipseprojects.io:8883",
          .verification.certificate = (const char *)mqtt_eclipse_org_pem_start,
        },
    };

了解其他字段的详细信息，请查看 `API 参考`_ 以及 :ref:`esp_tls_server_verification`。

客户端凭据
^^^^^^^^^^^^^^^^^^^^^^^^

:cpp:class:`credentials <esp_mqtt_client_config_t::credentials_t>` 字段下包含所有客户端相关凭据。

 * :cpp:member:`username <esp_mqtt_client_config_t::credentials_t::username>`：指向用于连接服务器用户名的指针，也可通过 URI 设置
 * :cpp:member:`client_id <esp_mqtt_client_config_t::credentials_t::client_id>`：指向客户端 ID 的指针，默认为 ``ESP32_%CHIPID%``，其中 ``%CHIPID%`` 是十六进制 MAC 地址的最后 3 个字节

===============
认证
===============

可以通过 :cpp:class:`authentication <esp_mqtt_client_config_t::credentials_t::authentication_t>` 字段设置认证参数。客户端支持以下认证方式：

 * :cpp:member:`password <esp_mqtt_client_config_t::credentials_t::authentication_t::password>`：使用密码
 * * :cpp:member:`certificate <esp_mqtt_client_config_t::credentials_t::authentication_t::certificate>` 和 :cpp:member:`key <esp_mqtt_client_config_t::credentials_t::authentication_t::key>`：进行双向 TLS 身份验证，PEM 或 DER 格式均可
 * :cpp:member:`use_secure_element <esp_mqtt_client_config_t::credentials_t::authentication_t::use_secure_element>`：使用 ESP32 中的安全元素 (ATECC608A)
 * :cpp:member:`ds_data <esp_mqtt_client_config_t::credentials_t::authentication_t::ds_data>`：使用某些乐鑫设备的数字签名外设

会话
^^^^^^^^^^^^

使用 :cpp:class:`session <esp_mqtt_client_config_t::session_t>` 字段进行 MQTT 会话相关配置。

========================
遗嘱消息 (LWT)
========================

通过设置 :cpp:class:`last_will <esp_mqtt_client_config_t::session_t::last_will_t>` 结构体的以下字段，MQTT 会在一个客户端意外断开连接时通过遗嘱消息通知其他客户端。

 * :cpp:member:`topic <esp_mqtt_client_config_t::session_t::last_will_t::topic>`：指向 LWT 消息主题的指针
 * :cpp:member:`msg <esp_mqtt_client_config_t::session_t::last_will_t::msg>`：指向 LWT 消息的指针
 * :cpp:member:`msg_len <esp_mqtt_client_config_t::session_t::last_will_t::msg_len>`：LWT 消息的长度，:cpp:member:`msg <esp_mqtt_client_config_t::session_t::last_will_t::msg>` 不以空字符结尾时需要该字段
 * :cpp:member:`qos <esp_mqtt_client_config_t::session_t::last_will_t::qos>`：LWT 消息的服务质量
 * :cpp:member:`retain <esp_mqtt_client_config_t::session_t::last_will_t::retain>`：指定 LWT 消息的保留标志

在项目配置菜单中设置 MQTT
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

通过 :code:`idf.py menuconfig`，可以在 ``Component config`` > ``ESP-MQTT Configuration`` 中找到 MQTT 设置。

相关设置如下：

- :ref:`CONFIG_MQTT_PROTOCOL_311`：启用 MQTT 协议 3.1.1 版本

- :ref:`CONFIG_MQTT_TRANSPORT_SSL` 和 :ref:`CONFIG_MQTT_TRANSPORT_WEBSOCKET`：启用特定 MQTT 传输层，例如 SSL、WEBSOCKET 和 WEBSOCKET_SECURE

- :ref:`CONFIG_MQTT_CUSTOM_OUTBOX`：禁用 mqtt_outbox 默认实现，因此可以提供特定实现


事件
------------
MQTT 客户端可能会发布以下事件：

* ``MQTT_EVENT_BEFORE_CONNECT``：客户端已初始化并即将开始连接至服务器。
* ``MQTT_EVENT_CONNECTED``：客户端已成功连接至服务器。客户端已准备好收发数据。
* ``MQTT_EVENT_DISCONNECTED``：由于无法读取或写入数据，例如因为服务器无法使用，客户端已终止连接。
* ``MQTT_EVENT_SUBSCRIBED``：服务器已确认客户端的订阅请求。事件数据将包含订阅消息的消息 ID。
* ``MQTT_EVENT_UNSUBSCRIBED``：服务器已确认客户端的退订请求。事件数据将包含退订消息的消息 ID。
* ``MQTT_EVENT_PUBLISHED``：服务器已确认客户端的发布消息。消息将仅针对 QoS 级别 1 和 2 发布，因为级别 0 不会进行确认。事件数据将包含发布消息的消息 ID。
* ``MQTT_EVENT_DATA``：客户端已收到发布消息。事件数据包含：消息 ID、发布消息所属主题名称、收到的数据及其长度。对于超出内部缓冲区的数据，将发布多个 ``MQTT_EVENT_DATA``，并更新事件数据的 :cpp:member:`current_data_offset <esp_mqtt_event_t::current_data_offset>` 和 :cpp:member:`total_data_len<esp_mqtt_event_t::total_data_len>` 以跟踪碎片化消息。
* ``MQTT_EVENT_ERROR``：客户端遇到错误。使用事件数据 :cpp:type:`error_handle <esp_mqtt_error_codes_t>` 字段中的 :cpp:type:`error_type <esp_mqtt_error_type_t>`，可以发现错误。错误类型决定 :cpp:type:`error_handle <esp_mqtt_error_codes_t>` 结构体的哪些部分会被填充。

API 参考
-------------

.. include-build-file:: inc/mqtt_client.inc
