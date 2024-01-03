ESP HTTP 客户端
===============

:link_to_translation:`en:[English]`

概述
----

``esp_http_client`` 提供了一组 API，用于从 ESP-IDF 应用程序中发起 HTTP/S 请求，具体的使用步骤如下：

    * 首先调用 :cpp:func:`esp_http_client_init`，创建一个 :cpp:type:`esp_http_client_handle_t` 实例，即基于给定的 :cpp:type:`esp_http_client_config_t` 配置创建 HTTP 客户端句柄。此函数必须第一个被调用。若用户未明确定义参数的配置值，则使用默认值。
    * 其次调用 :cpp:func:`esp_http_client_perform`，执行 ``esp_http_client`` 的所有操作，包括打开连接、交换数据、关闭连接（如需要），同时在当前任务完成前阻塞该任务。所有相关的事件（在 :cpp:type:`esp_http_client_config_t` 中指定）将通过事件处理程序被调用。
    * 最后调用 :cpp:func:`esp_http_client_cleanup` 来关闭连接（如有），并释放所有分配给 HTTP 客户端实例的内存。此函数必须在操作完成后最后一个被调用。


应用示例
--------

使用 ESP HTTP 客户端发起 HTTP/S 请求的简单示例，可参考 :example:`protocols/esp_http_client`。


HTTP 基本请求
-------------

如需了解实现细节，请参考应用示例中的 ``http_rest_with_url`` 和 ``http_rest_with_hostname_path`` 函数。


持久连接
--------

持久连接是 HTTP 客户端在多次交换中重复使用同一连接的方法。如果服务器没有使用 ``Connection: close`` 头来请求关闭连接，连接就会一直保持开放，用于其他新请求。

为了使 ESP HTTP 客户端充分利用持久连接的优势，建议尽可能多地使用同一个句柄实例来发起请求，可参考应用示例中的函数 ``http_rest_with_url`` 和 ``http_rest_with_hostname_path``。示例中，一旦创建连接，即会在连接关闭前发出多个请求（如 ``GET``、 ``POST``、 ``PUT`` 等）。

.. only:: esp32

    为 TLS 使用安全元件 (ATECC608)
    __________________________________

    安全元件 (ATECC608) 也可用于 HTTP 客户端连接中的底层 TLS 连接。请参考 :doc:`ESP-TLS 文档 </api-reference/protocols/esp_tls>` 中的 *ESP-TLS 中的 ATECC608A（安全元件）支持* 小节，了解更多细节。如需支持安全元素，必须首先在 menuconfig 中通过 :ref:`CONFIG_ESP_TLS_USE_SECURE_ELEMENT` 对其进行启用，此后，可配置 HTTP 客户端使用安全元素，如下所示：

    .. code-block:: c

        esp_http_client_config_t cfg = {
            /* other configurations options */
            .use_secure_element = true,
        };


HTTPS 请求
-----------

ESP HTTP 客户端支持使用 **mbedTLS** 的 SSL 连接，需将 ``url`` 配置为以 ``https`` 开头，或将 ``transport_type`` 设置为 ``HTTP_TRANSPORT_OVER_SSL``。可以通过 :ref:`CONFIG_ESP_HTTP_CLIENT_ENABLE_HTTPS` 来配置 HTTPS 支持（默认启用）。

.. note::

    在发起 HTTPS 请求时，如需服务器验证，首先需要向 ``esp_http_client_config_t`` 配置中的 ``cert_pem`` 成员提供额外的根证书（PEM 格式）。用户还可以通过 ``esp_http_client_config_t`` 配置中的 ``crt_bundle_attach`` 成员，使用 ``ESP x509 Certificate Bundle`` 进行服务器验证。

如需了解上文备注中的实现细节，请参考应用示例中的函数 ``https_with_url`` 和 ``https_with_hostname_path``。


HTTP 流
--------

有些应用程序需要主动打开连接并控制数据交换（数据流）。在这种情况下，应用流程与常规请求不同。请参考以下示例：

    * :cpp:func:`esp_http_client_init`：创建一个 HTTP 客户端句柄。
    * ``esp_http_client_set_*`` 或 ``esp_http_client_delete_*``：修改 HTTP 连接参数（可选）。
    * :cpp:func:`esp_http_client_open`：用 ``write_len`` （该参数为需要写入服务器的内容长度）打开 HTTP 连接，设置 ``write_len=0`` 为只读连接。
    * :cpp:func:`esp_http_client_write`：向服务器写入数据，最大长度为 :cpp:func:`esp_http_client_open` 函数中的 ``write_len`` 值；配置 ``write_len=0`` 无需调用此函数。
    * :cpp:func:`esp_http_client_fetch_headers`：在发送完请求头和服务器数据（如有）后，读取 HTTP 服务器的响应头。从服务器返回 ``content-length``，并可以由 :cpp:func:`esp_http_client_get_status_code` 继承，以获取连接的 HTTP 状态。
    * :cpp:func:`esp_http_client_read`：读取 HTTP 流。
    * :cpp:func:`esp_http_client_close`：关闭连接。
    * :cpp:func:`esp_http_client_cleanup`：释放分配的资源。

如需了解实现细节，请参考应用示例中的函数 ``http_perform_as_stream_reader``。


HTTP 认证
---------

ESP HTTP 客户端同时支持 **基本** 和 **摘要** 认证。
    * 用户可以在 ``url`` 或 ``esp_http_client_config_t`` 配置中的 ``username`` 和 ``password`` 处输入用户名和密码。对于 ``auth_type = HTTP_AUTH_TYPE_BASIC``，HTTP 客户端只需执行一项操作就可通过认证过程。
    * 如果 ``auth_type = HTTP_AUTH_TYPE_NONE``，但配置中有 ``username`` 和 ``password`` 字段，HTTP 客户端需要执行两项操作。客户端在第一次尝试连接服务器时，会收到 ``401 Unauthorized`` 头，而后再根据这些信息来选择认证方法，并在第二项操作中执行。
    * 如需了解实现细节，请参考应用示例中的函数 ``http_auth_basic``、 ``http_auth_basic_redirect`` （用于基本认证）和 ``http_auth_digest`` （用于摘要认证）。
    * 目前，摘要认证仅支持 MD5 和 SHA-256 算法。


认证配置示例
^^^^^^^^^^^^

    * 基于 URI 的认证

        .. highlight:: c

        ::

            esp_http_client_config_t config = {
                .url = "http://user:passwd@httpbin.org/basic-auth/user/passwd",
                .auth_type = HTTP_AUTH_TYPE_BASIC,
            };


    * 基于用户名和密码的认证

        .. highlight:: c

        ::

            esp_http_client_config_t config = {
                .url = "http://httpbin.org/basic-auth/user/passwd",
                .username = "user",
                .password = "passwd",
                .auth_type = HTTP_AUTH_TYPE_BASIC,
            };

事件处理
---------

ESP HTTP 客户端支持事件处理，发生相关事件时会触发相应的事件处理程序。:cpp:enum:`esp_http_client_event_id_t` 中包含了所有使用 ESP HTTP 客户端执行 HTTP 请求时可能发生的事件。

通过 :cpp:member:`esp_http_client_config_t::event_handler` 设置回调函数即可启用事件处理功能。

ESP HTTP 客户端诊断信息
--------------------------

诊断信息可以帮助用户深入了解出现的问题。在 ESP HTTP 客户端中，可以通过在 :doc:`事件循环库 <../system/esp_event>` 中注册事件处理程序来获取诊断信息。此功能的增加基于 `ESP Insights <https://github.com/espressif/esp-insights>`_ 框架，该框架可帮助收集诊断信息。然而，即使不依赖 ESP Insights 框架，也可以获取诊断信息。事件处理程序可通过 :cpp:func:`esp_event_handler_register` 函数注册到事件循环中。

事件循环中不同 HTTP 客户端事件的预期数据类型如下所示：

    - HTTP_EVENT_ERROR            :   ``esp_http_client_handle_t``
    - HTTP_EVENT_ON_CONNECTED     :   ``esp_http_client_handle_t``
    - HTTP_EVENT_HEADERS_SENT     :   ``esp_http_client_handle_t``
    - HTTP_EVENT_ON_HEADER        :   ``esp_http_client_handle_t``
    - HTTP_EVENT_ON_DATA          :   ``esp_http_client_on_data_t``
    - HTTP_EVENT_ON_FINISH        :   ``esp_http_client_handle_t``
    - HTTP_EVENT_DISCONNECTED     :   ``esp_http_client_handle_t``
    - HTTP_EVENT_REDIRECT         :   ``esp_http_client_redirect_event_data_t``

在无法接收到 :cpp:enumerator:`HTTP_EVENT_DISCONNECTED <esp_http_client_event_id_t::HTTP_EVENT_DISCONNECTED>` 之前，与事件数据一起接收到的 :cpp:type:`esp_http_client_handle_t` 将始终有效。这个句柄主要是为了区分不同的客户端连接，无法用于其他目的，因为它可能会随着客户端连接状态的变化而改变。

TLS 协议版本
--------------------

可在 :cpp:type:`esp_http_client_config_t` 中设置用于底层 TLS 连接的 TLS 协议版本。了解更多信息，请参考 :doc:`/api-reference/protocols/esp_tls` 中的 **TLS 协议版本** 章节。

HTTP 客户端的 TLS 协议版本可按如下方式配置：

    .. code-block:: c

        #include "esp_http_client.h"
        esp_http_client_config_t config = {
            .tls_version = ESP_HTTP_CLIENT_TLS_VER_TLS_1_2,
        };

API 参考
---------

.. include-build-file:: inc/esp_http_client.inc
