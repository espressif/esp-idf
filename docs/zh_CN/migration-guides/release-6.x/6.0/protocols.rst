协议
====

:link_to_translation:`en:[English]`

JSON
----

移除内置 JSON 组件
~~~~~~~~~~~~~~~~~~

ESP-IDF 已移除内置的 ``json`` 组件。用户应迁移至使用 `IDF 组件管理器 <https://components.espressif.com/>`_ 中的 ``espressif/cjson`` 组件。

迁移步骤
^^^^^^^^

1. **从 CMakeLists.txt 中移除 json**

   在组件的 ``CMakeLists.txt`` 文件中，从 ``REQUIRES`` 或 ``PRIV_REQUIRES`` 列表中移除 ``json``：

   .. code-block:: cmake

       # 迁移前
       idf_component_register(SRCS "main.c"
                              PRIV_REQUIRES json esp_http_server)

       # 迁移后
       idf_component_register(SRCS "main.c"
                              PRIV_REQUIRES esp_http_server)

2. **将 espressif/cjson 添加到 idf_component.yml**

   将 ``espressif/cjson`` 依赖项添加到组件的 ``idf_component.yml`` 文件中。如果该文件不存在，请在组件目录中创建它（例如 ``main/idf_component.yml``）：

   .. code-block:: yaml

       dependencies:
         espressif/cjson: "^1.7.19"

3. **无需更改代码**

   API 保持不变。现有的使用 cJSON 函数的代码无需修改即可继续工作：

   .. code-block:: c

       #include "cJSON.h"

       // 现有代码无需更改
       cJSON *root = cJSON_Parse(json_string);
       cJSON *item = cJSON_GetObjectItem(root, "key");
       cJSON_Delete(root);

更多信息：

- `espressif/cjson 组件 <https://components.espressif.com/components/espressif/cjson>`_
- `cJSON GitHub <https://github.com/espressif/idf-extra-components/tree/master/cjson>`_

ESP-TLS
-------

移除 wolfSSL 支持
~~~~~~~~~~~~~~~~~

ESP-TLS 已移除内置的 wolfSSL TLS 协议栈支持。使用 wolfSSL 的用户应迁移至以下两个方案之一：

1. **mbedTLS（推荐）**：默认 TLS 协议栈，完全集成并在 ESP-IDF 中维护。
2. **自定义 TLS 协议栈**：使用自定义协议栈 API（见下文方案 B）注册自己的 TLS 实现。

**已移除的 Kconfig 选项**

以下 Kconfig 选项已被移除：

- ``CONFIG_ESP_TLS_USING_WOLFSSL`` - 请改用 ``CONFIG_ESP_TLS_USING_MBEDTLS`` 或 ``CONFIG_ESP_TLS_CUSTOM_STACK``
- ``CONFIG_ESP_DEBUG_WOLFSSL`` - 进行 mbedTLS 调试，请使用 ``CONFIG_MBEDTLS_DEBUG``
- ``CONFIG_ESP_TLS_OCSP_CHECKALL`` - OCSP 功能应由所选 TLS 协议栈处理

**wolfSSL 用户迁移步骤**

如果你的项目通过 ESP-TLS 使用 wolfSSL：

1. **方案 A - 切换到 mbedTLS**

   - 从 sdkconfig 中移除 ``CONFIG_ESP_TLS_USING_WOLFSSL=y``
   - 将自动使用默认的 ``CONFIG_ESP_TLS_USING_MBEDTLS``
   - 使用标准 TLS 操作，无需更改代码

2. **方案 B - 使用自定义协议栈 API**

   如需要继续使用 wolfSSL 或其他 TLS 库，可以将其注册为自定义协议栈：

   - 在 menuconfig 中启用 ``CONFIG_ESP_TLS_CUSTOM_STACK``
   - 为你的 TLS 库实现 :cpp:type:`esp_tls_stack_ops_t` 接口
   - 在创建任何 TLS 连接之前调用 :cpp:func:`esp_tls_register_stack`

   有关实现自定义 TLS 协议栈的详细文档，请参阅 :ref:`esp_tls_custom_stack`。

已移除的废弃 API
~~~~~~~~~~~~~~~~

已移除废弃函数 :cpp:func:`esp_tls_conn_http_new`。请使用以下替代函数：

- :cpp:func:`esp_tls_conn_http_new_sync` 用于阻塞连接
- :cpp:func:`esp_tls_conn_http_new_async` 用于非阻塞连接

新 API 需要您使用 :cpp:func:`esp_tls_init` 创建 :cpp:type:`esp_tls_t` 结构，并提供对连接过程的更好控制。

ESP-Modbus
----------

乐鑫ESP-Modbus 库 (esp-modbus) 支持基于 RS485、Wi-Fi 和以太网接口网络的 Modbus 通信。

``esp-modbus v2 (v2.x.x)`` 组件是当前支持的组件版本：

* `GitHub 上的 ESP-Modbus 组件 <https://github.com/espressif/esp-modbus/tree/main>`__

文档
~~~~

* `ESP-MODBUS 稳定版文档 v2.x.x <https://docs.espressif.com/projects/esp-modbus/en/stable>`__
* `旧版本 v1.x.x 文档 <https://docs.espressif.com/projects/esp-modbus/en/v1>`__

应用示例
~~~~~~~~

自 ESP-IDF v6.0 版本起，已从 ESP-IDF 中移除已废弃的 ``esp-modbus v1`` 组件示例。

- `旧版 esp-modbus v1.x.x 示例 (esp-idf v5.5) <https://github.com/espressif/esp-idf/tree/release/v5.5/examples/protocols/modbus>`__

以下示例分别演示了串行和 TCP 端口的 ESP-Modbus 库在从站和主站实现中的应用。

- `mb_serial_slave <https://github.com/espressif/esp-modbus/tree/main/examples/serial/mb_serial_slave>`__ - 演示如何将 {IDF_TARGET_NAME} 用作 Modbus 串行从站设备，通过 esp-modbus 协议栈，使外部 Modbus 主机能够使用 Modbus 协议读写设备参数。

- `mb_serial_master <https://github.com/espressif/esp-modbus/tree/main/examples/serial/mb_serial_master>`__ - 演示如何在 {IDF_TARGET_NAME} 上使用 esp-modbus 协议栈端口作为 Modbus 串行主站设备，能够读写 Modbus 段中从站设备的值。

- `mb_tcp_slave <https://github.com/espressif/esp-modbus/tree/main/examples/tcp/mb_tcp_slave>`__ - 演示 esp-modbus TCP 从站协议栈端口，允许外部 Modbus 主机通过 Modbus 协议读写设备参数。

- `mb_tcp_master <https://github.com/espressif/esp-modbus/tree/main/examples/tcp/mb_tcp_master>`__ - 演示如何在 {IDF_TARGET_NAME} 上使用 esp-modbus 协议栈端口作为 Modbus TCP 主站设备，能够读写 Modbus 网络中从站设备的值。

有关详细信息，请参阅每个具体示例的 ``README.md`` 文档。

讨论
~~~~

* `v2 版本讨论 <https://github.com/espressif/esp-modbus/discussions>`__

ESP-MQTT
--------

重大变更：ESP-MQTT 迁移为托管组件，并更新了示例集。

- ESP-MQTT 组件已从 ESP-IDF 中移除，现作为托管组件 ``espressif/mqtt`` 提供。

  - 要将该组件添加至应用，运行 ``idf.py add-dependency espressif/mqtt``。
  - 头文件与 API 保持兼容（使用 ``mqtt_client.h``），但需通过组件管理器获取该组件。

- ESP-IDF 示例变更：

  - 移除了 ``examples/protocols/mqtt/ssl*`` 路径下的旧版 MQTT TLS 示例。
  - 新增参考示例：

    - ``examples/protocols/mqtt``：基于 TLS 的 MQTT。
    - ``examples/protocols/mqtt5``：基于 TLS 的 MQTT v5.0。
