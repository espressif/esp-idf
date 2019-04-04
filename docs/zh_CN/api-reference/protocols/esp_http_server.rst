HTTP 服务器
===========
:link_to_translation:`en:[English]`

概述
----

HTTP Server 组件提供了在 ESP32 上运行轻量级 Web 服务器的功能，下面介绍使用 HTTP Server 组件 API 的详细步骤：

    * :cpp:func:`httpd_start`： 创建 HTTP 服务器的实例，根据具体的配置为其分配内存和资源，并返回该服务器实例的句柄。服务器使用了两个套接字，一个用来监听 HTTP 流量（TCP 类型），另一个用来处理控制信号（UDP 类型），它们在服务器的任务循环中轮流使用。通过向 httpd_start() 传递 httpd_config_t 结构体，可以在创建服务器实例时配置任务的优先级和堆栈的大小。TCP 流量被解析为 HTTP 请求，根据请求的 URI 来调用用户注册的处理程序，在处理程序中需要发送回 HTTP 响应数据包。
    * :cpp:func:`httpd_stop`： 根据传入的句柄停止服务器，并释放相关联的内存和资源。这是一个阻塞函数，首先给服务器任务发送停止信号，然后等待其终止。期间服务器任务会关闭所有已打开的连接，删除已注册的 URI 处理程序，并将所有会话的上下文数据重置为空。
    * :cpp:func:`httpd_register_uri_handler`： 通过传入 ``httpd_uri_t`` 结构体类型的对象来注册 URI 处理程序。该结构体包含如下成员：``uri`` 名字，``method`` 类型（比如 ``HTTPD_GET/HTTPD_POST/HTTPD_PUT`` 等等）， ``esp_err_t *handler (httpd_req_t *req)`` 类型的函数指针，指向用户上下文数据的 ``user_ctx`` 指针。

应用示例
--------

    .. highlight:: c

    ::

        /* URI 处理函数，在客户端发起 GET /uri 请求时被调用 */
        esp_err_t get_handler(httpd_req_t *req)
        {
            /* 发送回简单的响应数据包 */
            const char[] resp = "URI GET Response";
            httpd_resp_send(req, resp, strlen(resp));
            return ESP_OK;
        }

        /* URI 处理函数，在客户端发起 POST /uri 请求时被调用 */
        esp_err_t post_handler(httpd_req_t *req)
        {
            /* 定义 HTTP POST 请求数据的目标缓存区
             * httpd_req_recv() 只接收 char* 数据，但也可以是任意二进制数据（需要类型转换）
             * 对于字符串数据，null 终止符会被省略，content_len 会给出字符串的长度 */
            char[100] content;

            /* 如果内容长度大于缓冲区则截断 */
            size_t recv_size = MIN(req->content_len, sizeof(content));

            int ret = httpd_req_recv(req, content, recv_size);
            if (ret <= 0) {  /* 返回 0 表示连接已关闭 */
                /* 检查是否超时 */
                if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                    /* 如果是超时，可以调用 httpd_req_recv() 重试
                     * 简单起见，这里我们直接响应 HTTP 408（请求超时）错误给客户端 */
                    httpd_resp_send_408(req);
                }
                /* 如果发生了错误，返回 ESP_FAIL 可以确保底层套接字被关闭 */
                return ESP_FAIL;
            }

            /* 发送简单的响应数据包 */
            const char[] resp = "URI POST Response";
            httpd_resp_send(req, resp, strlen(resp));
            return ESP_OK;
        }

        /* GET /uri 的 URI 处理结构 */
        httpd_uri_t uri_get = {
            .uri      = "/uri",
            .method   = HTTP_GET,
            .handler  = get_handler,
            .user_ctx = NULL
        };

        /* POST /uri 的 URI 处理结构 */
        httpd_uri_t uri_post = {
            .uri      = "/uri",
            .method   = HTTP_POST,
            .handler  = post_handler,
            .user_ctx = NULL
        };

        /* 启动 Web 服务器的函数 */
        httpd_handle_t start_webserver(void)
        {
            /* 生成默认的配置参数 */
            httpd_config_t config = HTTPD_DEFAULT_CONFIG();

            /* 置空 esp_http_server 的实例句柄 */
            httpd_handle_t server = NULL;

            /* 启动 httpd server */
            if (httpd_start(&server, &config) == ESP_OK) {
                /* 注册 URI 处理程序 */
                httpd_register_uri_handler(server, &uri_get);
                httpd_register_uri_handler(server, &uri_post);
            }
            /* 如果服务器启动失败，返回的句柄是 NULL */
            return server;
        }

        /* 停止 Web 服务器的函数 */
        void stop_webserver(httpd_handle_t server)
        {
            if (server) {
                /* 停止 httpd server */
                httpd_stop(server);
            }
        }

简单 HTTP 服务器示例
^^^^^^^^^^^^^^^^^^^^

请查看位于 :example:`protocols/http_server/simple` 的 HTTP 服务器示例，该示例演示了如何处理任意内容长度的数据，读取请求头和 URL 查询参数，设置响应头。


HTTP 长连接
-----------

HTTP 服务器具有长连接的功能，允许重复使用同一个连接（会话）进行多次传输，同时保持会话的上下文数据。上下文数据可由处理程序动态分配，在这种情况下需要提前指定好自定义的回调函数，以便在连接/会话被关闭时释放这部分内存资源。

长连接示例
^^^^^^^^^^

.. highlight:: c

::

    /* 自定义函数，用来释放上下文数据 */
    void free_ctx_func(void *ctx)
    {
        /* 也可以是 free 以外的代码逻辑 */
        free(ctx);
    }

    esp_err_t adder_post_handler(httpd_req_t *req)
    {
        /* 如果会话上下文还不存在则新建一个 */
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


详情请参考位于 :example:`protocols/http_server/persistent_sockets` 的示例代码。


API 参考
--------

.. include:: /_build/inc/esp_http_server.inc
