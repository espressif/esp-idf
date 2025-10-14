HTTPS 服务器
============

:link_to_translation:`en:[English]`

概述
------

HTTPS 服务器组件建立在 :doc:`esp_http_server` 组件的基础上。该服务器借助常规 HTTP 服务器中的钩子注册函数，注册 SSL 会话回调处理函数。

:doc:`esp_http_server` 组件的所有文档同样适用于用户按照本文档搭建的服务器。

API 说明
---------

下列 :doc:`esp_http_server` 的 API 已不适用于 :doc:`esp_https_server`。这些 API 仅限内部使用，用于处理安全会话和维护内部状态。

* "send"、"receive" 和 "pending" 回调注册函数——处理安全套接字

  * :cpp:func:`httpd_sess_set_send_override`
  * :cpp:func:`httpd_sess_set_recv_override`
  * :cpp:func:`httpd_sess_set_pending_override`

* "transport context"——传输层上下文

  * :cpp:func:`httpd_sess_get_transport_ctx`：返回会话使用的 SSL
  * :cpp:func:`httpd_sess_set_transport_ctx`
  * :cpp:func:`httpd_get_global_transport_ctx`：返回共享的 SSL 上下文
  * :cpp:member:`httpd_config::global_transport_ctx`
  * :cpp:member:`httpd_config::global_transport_ctx_free_fn`
  * :cpp:member:`httpd_config::open_fn`：用于设置安全套接字

其他 API 均可使用，没有其他限制。

.. note:: HTTPS 服务器的 API 并不是线程安全的。如果需要线程安全，应用层需自行确保在多个任务之间进行适当的同步。

如何使用
---------

请参考示例 :example:`protocols/https_server` 来学习如何搭建安全的服务器。

总体而言，只需生成证书，将其嵌入到固件中，并且在初始化结构体中配置好正确的证书地址和长度后，将其传入服务器启动函数。

通过改变初始化配置结构体中的标志 :cpp:member:`httpd_ssl_config::transport_mode`，可以选择是否需要 SSL 连接来启动服务器。在测试时或在速度比安全性更重要的可信环境中，可以使用此功能。

性能
-----

建立起始会话大约需要两秒，在时钟速度较慢或日志记录冗余信息较多的情况下，可能需要花费更多时间。后续通过已打开的安全套接字建立请求的速度会更快，最快只需不到 100 ms。

事件处理
--------------

ESP HTTPS 服务器在特定事件发生时，可以通过 :doc:`../system/esp_event` 触发事件处理程序。处理程序必须使用 :cpp:func:`esp_event_handler_register` 进行注册，以帮助 ESP HTTPS 服务器处理事件。

:cpp:enum:`esp_https_server_event_id_t` 包含了 ESP HTTPS 服务器可能发生的所有事件。

事件循环中不同 ESP HTTPS 服务器事件的预期数据类型如下所示：

    - HTTPS_SERVER_EVENT_ERROR          :   ``esp_https_server_last_error_t``
    - HTTPS_SERVER_EVENT_START          :   ``NULL``
    - HTTPS_SERVER_EVENT_ON_CONNECTED   :   ``NULL``
    - HTTPS_SERVER_EVENT_ON_DATA        :   ``int``
    - HTTPS_SERVER_EVENT_SENT_DATA      :   ``NULL``
    - HTTPS_SERVER_EVENT_DISCONNECTED   :   ``NULL``
    - HTTPS_SERVER_EVENT_STOP           :   ``NULL``

应用示例
--------------------

- :example:`protocols/https_server/simple` 演示了如何创建一个 HTTPS 服务器，当访问时返回一个简单的 HTML 页面。

- :example:`protocols/https_server/wss_server` 演示了如何创建一个支持多客户端的 SSL 服务器，具备简单的 WebSocket 请求处理器、PING-PONG 机制，并能够向所有客户端发送异步消息。

HTTPS 服务器证书选择钩子
------------------------

ESP HTTPS 服务器组件提供了设置服务器证书选择钩子的选项。启用此功能后，在服务器握手过程中，可以配置并使用证书选择回调函数。该回调函数会根据客户端 hello 消息中提供的 TLS 扩展（如 ALPN 和 SNI），动态选择合适的证书发送给客户端。

要启用此功能，请先在 ESP HTTPS 服务器的 menuconfig 中启用 :ref:`CONFIG_ESP_HTTPS_SERVER_CERT_SELECT_HOOK`。请注意，只有当 Mbedtls 被用作 ESP-TLS 的 TLS 协议栈（默认行为）时，ESP-TLS 选项才可使用。

启用此功能后，请使用 :cpp:type:`httpd_ssl_config_t` 结构体中的 :cpp:member:`httpd_ssl_config::cert_select_cb` 成员，设置证书选择回调函数。

.. code-block:: c

  int cert_selection_callback(mbedtls_ssl_context *ssl)
    {
        /* 回调函数应执行的代码 */
        return 0;
    }

    httpd_ssl_config_t cfg = {
        cert_select_cb = cert_section_callback,
    };


API 参考
---------

.. include-build-file:: inc/esp_https_server.inc
