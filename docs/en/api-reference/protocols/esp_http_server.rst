HTTP Server
===========
:link_to_translation:`zh_CN:[中文]`

Overview
--------

The HTTP Server component provides an ability for running a lightweight web server on ESP32. Following are detailed steps to use the API exposed by HTTP Server:

    * :cpp:func:`httpd_start`: Creates an instance of HTTP server, allocate memory/resources for it depending upon the specified configuration and outputs a handle to the server instance. The server has both, a listening socket (TCP) for HTTP traffic, and a control socket (UDP) for control signals, which are selected in a round robin fashion in the server task loop. The task priority and stack size are configurable during server instance creation by passing httpd_config_t structure to httpd_start(). TCP traffic is parsed as HTTP requests and, depending on the requested URI, user registered handlers are invoked which are supposed to send back HTTP response packets.
    * :cpp:func:`httpd_stop`: This stops the server with the provided handle and frees up any associated memory/resources. This is a blocking function that first signals a halt to the server task and then waits for the task to terminate. While stopping, the task will close all open connections, remove registered URI handlers and reset all session context data to empty.
    * :cpp:func:`httpd_register_uri_handler`: A URI handler is registered by passing object of type ``httpd_uri_t`` structure which has members including ``uri`` name, ``method`` type (eg. ``HTTPD_GET/HTTPD_POST/HTTPD_PUT`` etc.), function pointer of type ``esp_err_t *handler (httpd_req_t *req)`` and ``user_ctx`` pointer to user context data.

Application Example
-------------------

    .. highlight:: c

    ::

        /* Our URI handler function to be called during GET /uri request */
        esp_err_t get_handler(httpd_req_t *req)
        {
            /* Send a simple response */
            const char resp[] = "URI GET Response";
            httpd_resp_send(req, resp, strlen(resp));
            return ESP_OK;
        }

        /* Our URI handler function to be called during POST /uri request */
        esp_err_t post_handler(httpd_req_t *req)
        {
            /* Destination buffer for content of HTTP POST request.
             * httpd_req_recv() accepts char* only, but content could
             * as well be any binary data (needs type casting).
             * In case of string data, null termination will be absent, and
             * content length would give length of string */
            char[100] content;

            /* Truncate if content length larger than the buffer */
            size_t recv_size = MIN(req->content_len, sizeof(content));

            int ret = httpd_req_recv(req, content, recv_size);
            if (ret <= 0) {  /* 0 return value indicates connection closed */
                /* Check if timeout occurred */
                if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                    /* In case of timeout one can choose to retry calling
                     * httpd_req_recv(), but to keep it simple, here we
                     * respond with an HTTP 408 (Request Timeout) error */
                    httpd_resp_send_408(req);
                }
                /* In case of error, returning ESP_FAIL will
                 * ensure that the underlying socket is closed */
                return ESP_FAIL;
            }

            /* Send a simple response */
            const char resp[] = "URI POST Response";
            httpd_resp_send(req, resp, strlen(resp));
            return ESP_OK;
        }

        /* URI handler structure for GET /uri */
        httpd_uri_t uri_get = {
            .uri      = "/uri",
            .method   = HTTP_GET,
            .handler  = get_handler,
            .user_ctx = NULL
        };

        /* URI handler structure for POST /uri */
        httpd_uri_t uri_post = {
            .uri      = "/uri",
            .method   = HTTP_POST,
            .handler  = post_handler,
            .user_ctx = NULL
        };

        /* Function for starting the webserver */
        httpd_handle_t start_webserver(void)
        {
            /* Generate default configuration */
            httpd_config_t config = HTTPD_DEFAULT_CONFIG();

            /* Empty handle to esp_http_server */
            httpd_handle_t server = NULL;

            /* Start the httpd server */
            if (httpd_start(&server, &config) == ESP_OK) {
                /* Register URI handlers */
                httpd_register_uri_handler(server, &uri_get);
                httpd_register_uri_handler(server, &uri_post);
            }
            /* If server failed to start, handle will be NULL */
            return server;
        }

        /* Function for stopping the webserver */
        void stop_webserver(httpd_handle_t server)
        {
            if (server) {
                /* Stop the httpd server */
                httpd_stop(server);
            }
        }

Simple HTTP server example
^^^^^^^^^^^^^^^^^^^^^^^^^^

Check HTTP server example under :example:`protocols/http_server/simple` where handling of arbitrary content lengths, reading request headers and URL query parameters, and setting response headers is demonstrated.


Persistent Connections
----------------------

HTTP server features persistent connections, allowing for the re-use of the same connection (session) for several transfers, all the while maintaining context specific data for the session. Context data may be allocated dynamically by the handler in which case a custom function may need to be specified for freeing this data when the connection/session is closed.

Persistent Connections Example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. highlight:: c

::

    /* Custom function to free context */
    void free_ctx_func(void *ctx)
    {
        /* Could be something other than free */
        free(ctx);
    }

    esp_err_t adder_post_handler(httpd_req_t *req)
    {
        /* Create session's context if not already available */
        if (! req->sess_ctx) {
            req->sess_ctx = malloc(sizeof(ANY_DATA_TYPE));  /*!< Pointer to context data */
            req->free_ctx = free_ctx_func;                  /*!< Function to free context data */
        }

        /* Access context data */
        ANY_DATA_TYPE *ctx_data = (ANY_DATA_TYPE *)req->sess_ctx;

        /* Respond */
        ...............
        ...............
        ...............

        return ESP_OK;
    }


Check the example under :example:`protocols/http_server/persistent_sockets`.


API Reference
-------------

.. include:: /_build/inc/esp_http_server.inc
