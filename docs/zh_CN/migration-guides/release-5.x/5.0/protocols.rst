协议
=========

:link_to_translation:`en:[English]`

.. _migration_guide_mbedtls:

Mbed TLS
--------

在 ESP-IDF v5.0 版本中，`Mbed TLS <https://github.com/Mbed-TLS/mbedtls>`_ 已从 v2.x 版本更新到 v3.1.0 版本。

更多有关 Mbed TLS 从 v2.x 版本迁移到 v3.0 或更高版本的详细信息，请参考 `官方指南 <https://github.com/espressif/mbedtls/blob/9bb5effc3298265f829878825d9bd38478e67514/docs/3.0-migration-guide.md>`__。

重大更新（概述）
~~~~~~~~~~~~~~~~~~~~

增加私有结构体字段数量
^^^^^^^^^^^^^^^^^^^^^^^^^^

- 不再支持直接访问公共头文件中声明的结构体（ ``struct`` 类型）字段。
- 当前版本下，访问公共头文件中声明的结构体字段需要使用特定的访问函数 (getter/setter)。另外，也可以用 ``MBEDTLS_PRIVATE`` 宏暂时代替，但不建议使用此种方法。
- 更多详细信息，请参考 `官方指南 <https://github.com/espressif/mbedtls/blob/9bb5effc3298265f829878825d9bd38478e67514/docs/3.0-migration-guide.md#most-structure-fields-are-now-private>`__。


SSL
^^^
- 不再支持 TLS 1.0、TLS 1.1 和 DTLS 1.0
- 不再支持 SSL 3.0

移除密码模块中的废弃函数
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- 更新了与 MD、SHA、RIPEMD、RNG、HMAC 模块相关的函数 ``mbedtls_*_ret()`` 的返回值，并将其重新命名，以取代未附加 ``_ret`` 的相应函数。
- 更多详细信息，请参考 `官方指南 <https://github.com/espressif/mbedtls/blob/9bb5effc3298265f829878825d9bd38478e67514/docs/3.0-migration-guide.md#deprecated-functions-were-removed-from-hashing-modules>`__。


废弃配置选项
^^^^^^^^^^^^^^^^^^^^^^^^^

下列为在此次更新中废弃的重要配置选项。与以下配置有关或是依赖于下列配置的相关配置也已相应废弃。

- ``MBEDTLS_SSL_PROTO_SSL3``：原用于支持 SSL 3.0
- ``MBEDTLS_SSL_PROTO_TLS1``：原用于支持 TLS 1.0
- ``MBEDTLS_SSL_PROTO_TLS1_1``：原用于支持 TLS 1.1
- ``MBEDTLS_SSL_PROTO_DTLS``：原用于支持 DTLS 1.1（当前版本仅支持 DTLS 1.2）
- ``MBEDTLS_DES_C``：原用于支持 3DES 密码套件
- ``MBEDTLS_RC4_MODE``：原用于支持基于 RC4 的密码套件

.. note::

  上述仅列出了可通过 ``idf.py menuconfig`` 配置的主要选项。更多有关废弃选项的信息，请参考 `官方指南 <https://github.com/espressif/mbedtls/blob/9bb5effc3298265f829878825d9bd38478e67514/docs/3.0-migration-guide.md>`__。


其他更新
-------------

禁用 Diffie-Hellman 密码交换模式
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

为避免 `安全风险 <https://github.com/espressif/mbedtls/blob/9bb5effc3298265f829878825d9bd38478e67514/include/mbedtls/dhm.h#L20>`__，当前版本已默认禁用 Diffie-Hellman 密码交换模式。以下为相应的禁用配置项：

- ``MBEDTLS_DHM_C``：原用于支持 Diffie-Hellman-Merkle 模块
- ``MBEDTLS_KEY_EXCHANGE_DHE_PSK``：原用于支持 Diffie-Hellman 预共享密钥 (PSK) TLS 认证模式
- ``MBEDTLS_KEY_EXCHANGE_DHE_RSA``：原用于支持带有前缀的密码套件 ``TLS-DHE-RSA-WITH-``

.. note::

  在信号交换的初始步骤（即 ``client_hello``）中，服务器会在客户端提供的列表中选择一个密码。由于 DHE_PSK/DHE_RSA 密码已在本次更新中禁用，服务器将退回到一个替代密码。在极个别情况中，服务器不支持任何其他的代码，此时，初始步骤将失败。若要检索服务器所支持的密码列表，需要首先在客户端使用特定的密码连接服务器，可以使用 ``sslscan`` 等工具完成连接。

从 X509 库中移除 ``certs`` 模块
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- mbedtls 3.1 不再支持 ``mbedtls/certs.h`` 头文件。大多数应用程序支持从包含列表中安全删除该头文件。

对 ``esp_crt_bundle_set`` API 的重大更新
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- 更新后，调用 :cpp:func:`esp_crt_bundle_set()` API 需要一个额外的参数 ``bundle_size``。该 API 的返回类型也从 ``void`` 变为了 :cpp:type:`esp_err_t`。

对 ``esp_ds_rsa_sign`` API 的重大更新
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- 更新后，调用 :cpp:func:`esp_ds_rsa_sign()` API 无需再使用参数 ``mode``。

HTTPS 服务器
------------

重大更新（概述）
~~~~~~~~~~~~~~~~~~~~~~~~~~

更新 :cpp:type:`httpd_ssl_config_t` 结构体中持有不同证书的变量名。

.. list::
    * :cpp:member:`httpd_ssl_config::servercert`：原 ``cacert_pem``
    * :cpp:member:`httpd_ssl_config::servercert_len`：原 ``cacert_len``
    * :cpp:member:`httpd_ssl_config::cacert_pem`：原 ``client_verify_cert_pem``
    * :cpp:member:`httpd_ssl_config::cacert_len`：原 ``client_verify_cert_len``

:cpp:func:`httpd_ssl_stop` API 的返回类型从 ``void`` 变为了 :cpp:type:`esp_err_t`。

ESP HTTPS OTA
--------------

重大更新（概述）
~~~~~~~~~~~~~~~~~~~~~~~~~~

- 函数 :cpp:func:`esp_https_ota` 现需以指向 :cpp:type:`esp_https_ota_config_t` 的指针作为参数，而非之前的指向 :cpp:type:`esp_http_client_config_t` 的指针。


ESP-TLS
--------------

重大更新（概述）
~~~~~~~~~~~~~~~~~~~~~~~~~~

私有化 ``esp_tls_t`` 结构体
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

更新后，:cpp:type:`esp_tls_t` 已完全私有化，用户无法直接访问其内部结构。之前需要通过 ESP-TLS 句柄获得的必要数据，现在可由对应的 getter/setter 函数获取。如需特定功能的 getter/setter 函数，请在 ESP-IDF 的 `Issue 板块 <https://github.com/espressif/esp-idf/issues>`__ 提出。


下列为新增的 getter/setter 函数：

.. list::

    * :cpp:func:`esp_tls_get_ssl_context`：从 ESP-TLS 句柄获取底层 ssl 栈的 ssl 上下文。

废弃函数及推荐的替代函数
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

下表总结了在 ESP-IDF v5.0 中废弃的函数以及相应的替代函数。

.. list-table::
   :widths: 50 50
   :header-rows: 1

   * - 废弃函数
     - 替代函数
   * - ``esp_tls_conn_new()``
     - :cpp:func:`esp_tls_conn_new_sync`
   * - ``esp_tls_conn_delete()``
     - :cpp:func:`esp_tls_conn_destroy`

- 函数 :cpp:func:`esp_tls_conn_http_new` 现已废弃。请使用替代函数 :cpp:func:`esp_tls_conn_http_new_sync` （或其异步函数 :cpp:func:`esp_tls_conn_http_new_async` ）。请注意，使用替代函数时，需要额外的参数 :cpp:type:`esp_tls_t`，此参数必须首先通过 :cpp:func:`esp_tls_init` 函数进行初始化。

HTTP 服务器
-----------

重大更新（概述）
~~~~~~~~~~~~~~~~~~~~~~~~~~

-  ``esp_http_server`` 现不再支持 ``http_server.h`` 头文件。请使用 ``esp_http_server.h``。

ESP HTTP 客户端
---------------

重大更新（概述）
~~~~~~~~~~~~~~~~~~~~~~~~~~

- 函数 :cpp:func:`esp_http_client_read` 和 :cpp:func:`esp_http_client_fetch_headers` 现在会返回额外的返回值 ``-ESP_ERR_HTTP_EAGAIN`` 用于处理超时错误，即数据准备好前就已调用超时的情况。


TCP 传输
-------------

重大更新（概述）
~~~~~~~~~~~~~~~~~~~~~~~~~~

- 更新后，出现连接超时的情况时，函数 :cpp:func:`esp_transport_read` 将返回 ``0``，对其他错误则返回 ``< 0``。请参考 :cpp:enum:`esp_tcp_transport_err_t`，查看所有可能的返回值。


MQTT 客户端
-----------

重大更新（概述）
~~~~~~~~~~~~~~~~~~~~~~~~~~

- :cpp:type:`esp_mqtt_client_config_t` 的所有字段都分组存放在子结构体中。

以下为较为常用的配置选项：

- 通过 :cpp:member:`esp_mqtt_client_config_t::broker::address::uri` 配置 MQTT Broker
- 通过 :cpp:member:`esp_mqtt_client_config_t::broker::verification` 配置 MQTT Broker 身份验证的相关安全问题
- 通过 :cpp:member:`esp_mqtt_client_config_t::credentials::username` 配置客户端用户名

- :cpp:type:`esp_mqtt_client_config_t` 不再支持 ``user_context`` 字段。之后注册事件处理程序，请使用 :cpp:func:`esp_mqtt_client_register_event`；最后一个参数 ``event_handler_arg`` 可用于将用户上下文传递给处理程序。


ESP-Modbus
----------

重大更新（概述）
~~~~~~~~~~~~~~~~~~~~~~~~~~

本次更新从 ESP-IDF 中移除了组件 ``freemodbus``，该组件已作为一个独立组件受到支持。可前往如下的独立仓库，查看更多有关 ``ESP-Modbus`` 的信息：

* `GitHub 中的 ESP-Modbus 组件 <https://www.github.com/espressif/esp-modbus>`__

在新版应用程序中， ``main`` 组件文件夹应包括组件管理器清单文件 ``idf_component.yml``，如下所示：

.. code-block:: text

  dependencies:
    espressif/esp-modbus:
      version: "^1.0"

可以前往 `组件管理器注册表 <https://components.espressif.com/component/espressif/esp-modbus>`__ 找到 ``ESP-Modbus`` 组件。更多有关如何设置组件管理器的信息，请参考 `组件管理器文档 <https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/tools/idf-component-manager.html>`__。

对于使用 ESP-IDF v4.x 及以后版本的应用程序，需要通过添加组件管理器清单文件 ``idf_component.yml`` 拉取新版 ``ESP-Modbus`` 组件。同时，在编译时，应去掉已过时的 ``freemodbus`` 组件。此项操作可通过项目 ``CMakeLists.txt`` 中的以下语句实现：

.. code-block:: cmake

  set(EXCLUDE_COMPONENTS freemodbus)
