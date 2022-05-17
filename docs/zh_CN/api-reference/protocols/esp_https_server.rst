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

如何使用
---------

请参考示例 :example:`protocols/https_server` 来学习如何搭建安全的服务器。

总体而言，您只需要生成证书，将其嵌入到固件中，并且在初始化结构体中配置好正确的证书地址和长度后，将其传入服务器启动函数。

通过改变初始化配置结构体中的标志 :cpp:member:`httpd_ssl_config::transport_mode`，可以选择是否需要 SSL 连接来启动服务器。在测试时或在速度比安全性更重要的可信环境中，您可以使用此功能。

性能
-----

建立起始会话大约需要两秒，在时钟速度较慢或日志记录冗余信息较多的情况下，可能需要花费更多时间。后续通过已打开的安全套接字建立请求的速度会更快，最快只需不到 100 ms。

API 参考
---------

.. include-build-file:: inc/esp_https_server.inc
