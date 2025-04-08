HTTP 服务器
===========

:link_to_translation:`en:[English]`

概述
----

HTTP Server 组件提供了在 ESP32 上运行轻量级 Web 服务器的功能，下面介绍使用 HTTP Server 组件 API 的详细步骤：

    * :cpp:func:`httpd_start`： 创建 HTTP 服务器的实例，根据具体的配置为其分配内存和资源，并返回该服务器实例的句柄。服务器使用了两个套接字，一个用来监听 HTTP 流量（TCP 类型），另一个用来处理控制信号（UDP 类型），它们在服务器的任务循环中轮流使用。通过向 ``httpd_start()`` 传递 ``httpd_config_t`` 结构体，可以在创建服务器实例时配置任务的优先级和堆栈的大小。TCP 流量被解析为 HTTP 请求，根据请求的 URI 来调用用户注册的处理程序，在处理程序中需要发送回 HTTP 响应数据包。
    * :cpp:func:`httpd_stop`： 根据传入的句柄停止服务器，并释放相关联的内存和资源。这是一个阻塞函数，首先给服务器任务发送停止信号，然后等待其终止。期间服务器任务会关闭所有已打开的连接，删除已注册的 URI 处理程序，并将所有会话的上下文数据重置为空。
    * :cpp:func:`httpd_register_uri_handler`： 通过传入 ``httpd_uri_t`` 结构体类型的对象来注册 URI 处理程序。该结构体包含如下成员：``uri`` 名字，``method`` 类型（比如 ``HTTP_GET/HTTP_POST/HTTP_PUT`` 等等）， ``esp_err_t *handler (httpd_req_t *req)`` 类型的函数指针，指向用户上下文数据的 ``user_ctx`` 指针。

应用示例
--------

- :example:`protocols/http_server/simple` 演示了如何处理任意内容长度的数据，读取请求头和 URL 查询参数，并设置响应头。

- :example:`protocols/http_server/advanced_tests` 演示了如何使用 HTTP 服务器进行高级测试。

HTTP 长连接
-----------

HTTP 服务器具有长连接的功能，允许重复使用同一个连接（会话）进行多次传输，同时保持会话的上下文数据。上下文数据可由处理程序动态分配，在这种情况下需要提前指定好自定义的回调函数，以便在连接/会话被关闭时释放这部分内存资源。

长连接示例
^^^^^^^^^^

.. code-block:: c

    /* 自定义函数，用来释放上下文数据 */
    void free_ctx_func(void *ctx)
    {
        /* 也可以是 free 以外的代码逻辑 */
        free(ctx);
    }

    esp_err_t adder_post_handler(httpd_req_t *req)
    {
        /* 若上下文中不存在会话，则新建一个 */
        if (! req->sess_ctx) {
            req->sess_ctx = malloc(sizeof(ANY_DATA_TYPE));  /*!< 指向上下文数据 */
            req->free_ctx = free_ctx_func;                  /*!< 释放上下文数据的函数 */
        }

        /* 访问上下文数据 */
        ANY_DATA_TYPE *ctx_data = (ANY_DATA_TYPE *)req->sess_ctx;

        /* 响应 */
        ...............
        ...............
        ...............

        return ESP_OK;
    }


详情请参考位于 :example:`protocols/http_server/persistent_sockets` 的示例代码。该示例演示了如何设置和使用带有持久套接字的 HTTP 服务器，允许每个客户端拥有独立的会话或上下文。


WebSocket 服务器
----------------

HTTP 服务器组件提供 websocket 支持。可以在 menuconfig 中使用 :ref:`CONFIG_HTTPD_WS_SUPPORT` 选项启用 websocket 功能。

:example:`protocols/http_server/ws_echo_server` 演示了如何使用 HTTP 服务器创建一个 WebSocket 回显服务器，该服务器在本地网络上启动，与 WebSocket 客户端进行交互，回显接收到的 WebSocket 帧。


事件处理
--------------

ESP HTTP 服务器有各种事件，当特定事件发生时，:doc:`事件循环库 <../system/esp_event>` 可以触发处理程序。 必须使用 :cpp:func:`esp_event_handler_register` 注册处理程序以便 ESP HTTP 服务器进行事件处理。

:cpp:enum:`esp_http_server_event_id_t` 包含 ESP HTTP 服务器可能发生的所有事件。

以下为事件循环中不同 ESP HTTP 服务器事件的预期数据类型：

    - HTTP_SERVER_EVENT_ERROR           :   ``httpd_err_code_t``
    - HTTP_SERVER_EVENT_START           :   ``NULL``
    - HTTP_SERVER_EVENT_ON_CONNECTED    :   ``int``
    - HTTP_SERVER_EVENT_ON_HEADER       :   ``int``
    - HTTP_SERVER_EVENT_HEADERS_SENT    :   ``int``
    - HTTP_SERVER_EVENT_ON_DATA         :   ``esp_http_server_event_data``
    - HTTP_SERVER_EVENT_SENT_DATA       :   ``esp_http_server_event_data``
    - HTTP_SERVER_EVENT_DISCONNECTED    :   ``int``
    - HTTP_SERVER_EVENT_STOP            :   ``NULL``

文件服务
------------

:example:`protocols/http_server/file_serving` 演示了如何创建一个简单的 HTTP 文件服务器，支持文件上传和下载功能。

强制网络门户
-----------------

:example:`protocols/http_server/captive_portal` 演示了创建强制网络门户的两种方法，用户在浏览前会被引导到一个认证页面。这两种方法分别使用 DNS 查询和 HTTP 请求重定向，或通过 DHCP offer 中的字段来实现。

异步处理程序
---------------------

:example:`protocols/http_server/async_handlers` 演示了如何在 HTTP 服务器中处理多个长时间运行的并发请求，使用不同的 URI 来处理异步请求、快速请求以及主页请求。

RESTful API
-----------

:example:`protocols/http_server/restful_server` 演示了如何实现 RESTful API 服务器和 HTTP 服务器，并结合前端浏览器 UI，设计了多个 API 来获取资源，使用 mDNS 解析域名，并通过半主机技术将网页部署到主机 PC、SPI flash 或 SD 卡上。

URI 处理程序
------------

HTTP 服务器可以注册 URI 处理程序以处理不同的 HTTP 请求。每个 URI 处理程序都与特定的 URI 和 HTTP 方法（如 GET、POST 等）相关联。当接收到与 URI 和 HTTP 方法相匹配的请求时，会调用相应的处理程序函数。

处理程序函数应返回 :cpp:type:`esp_err_t` 值。

.. code-block:: c

    esp_err_t my_uri_handler(httpd_req_t *req)
    {
        // 处理请求
        // ……

        // 如果请求处理成功，则返回 ESP_OK
        return ESP_OK;

        // 返回错误代码以关闭连接
        // 返回 ESP_FAIL
    }

    void register_uri_handlers(httpd_handle_t server)
    {
        httpd_uri_t my_uri = {
            .uri       = "/my_uri",
            .method    = HTTP_GET,
            .handler   = my_uri_handler,
            .user_ctx  = NULL
        };

        httpd_register_uri_handler(server, &my_uri);
    }

在此示例中，`my_uri_handler` 函数用于处理对 `/my_uri` URI 的请求。如果处理程序返回 :c:macro:`ESP_OK`，则连接保持打开状态。如果返回其他值，则连接关闭。因此，应用程序可以根据特定事件或条件来管理连接的状态。

API 参考
--------

.. include-build-file:: inc/esp_http_server.inc
