HTTP Server
===========

:link_to_translation:`zh_CN:[中文]`

Overview
--------

The HTTP Server component provides an ability for running a lightweight web server on {IDF_TARGET_NAME}. Following are detailed steps to use the API exposed by HTTP Server:

    * :cpp:func:`httpd_start`: Creates an instance of HTTP server, allocate memory/resources for it depending upon the specified configuration and outputs a handle to the server instance. The server has both, a listening socket (TCP) for HTTP traffic, and a control socket (UDP) for control signals, which are selected in a round robin fashion in the server task loop. The task priority and stack size are configurable during server instance creation by passing ``httpd_config_t`` structure to ``httpd_start()``. TCP traffic is parsed as HTTP requests and, depending on the requested URI, user registered handlers are invoked which are supposed to send back HTTP response packets.
    * :cpp:func:`httpd_stop`: This stops the server with the provided handle and frees up any associated memory/resources. This is a blocking function that first signals a halt to the server task and then waits for the task to terminate. While stopping, the task closes all open connections, removes registered URI handlers and resets all session context data to empty.
    * :cpp:func:`httpd_register_uri_handler`: A URI handler is registered by passing object of type ``httpd_uri_t`` structure which has members including ``uri`` name, ``method`` type (eg. ``HTTP_GET/HTTP_POST/HTTP_PUT`` etc.), function pointer of type ``esp_err_t *handler (httpd_req_t *req)`` and ``user_ctx`` pointer to user context data.

Application Examples
--------------------

- :example:`protocols/http_server/simple` demonstrates how to handle arbitrary content lengths, read request headers and URL query parameters, and set response headers.

- :example:`protocols/http_server/advanced_tests` demonstrates how to use the HTTP server for advanced testing.

Persistent Connections
----------------------

HTTP server features persistent connections, allowing for the reuse of the same connection (session) for several transfers, all the while maintaining context specific data for the session. Context data may be allocated dynamically by the handler in which case a custom function may need to be specified for freeing this data when the connection/session is closed.

Persistent Connections Example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

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


Check the example under :example:`protocols/http_server/persistent_sockets`. This example demonstrates how to set up and use an HTTP server with persistent sockets, allowing for independent sessions or contexts per client.


WebSocket Server
----------------

The HTTP server component provides WebSocket support. The WebSocket feature can be enabled in menuconfig using the :ref:`CONFIG_HTTPD_WS_SUPPORT` option.

:example:`protocols/http_server/ws_echo_server` demonstrates how to create a WebSocket echo server using the HTTP server, which starts on a local network and requires a WebSocket client for interaction, echoing back received WebSocket frames.


Event Handling
--------------

ESP HTTP server has various events for which a handler can be triggered by :doc:`the Event Loop library <../system/esp_event>` when the particular event occurs. The handler has to be registered using :cpp:func:`esp_event_handler_register`. This helps in event handling for ESP HTTP server.

:cpp:enum:`esp_http_server_event_id_t` has all the events which can happen for ESP HTTP server.

Expected data type for different ESP HTTP server events in event loop:

    - HTTP_SERVER_EVENT_ERROR           :   ``httpd_err_code_t``
    - HTTP_SERVER_EVENT_START           :   ``NULL``
    - HTTP_SERVER_EVENT_ON_CONNECTED    :   ``int``
    - HTTP_SERVER_EVENT_ON_HEADER       :   ``int``
    - HTTP_SERVER_EVENT_HEADERS_SENT    :   ``int``
    - HTTP_SERVER_EVENT_ON_DATA         :   ``esp_http_server_event_data``
    - HTTP_SERVER_EVENT_SENT_DATA       :   ``esp_http_server_event_data``
    - HTTP_SERVER_EVENT_DISCONNECTED    :   ``int``
    - HTTP_SERVER_EVENT_STOP            :   ``NULL``

File Serving
------------

:example:`protocols/http_server/file_serving` demonstrates how to create a simple HTTP file server, with both upload and download capabilities.

Captive Portal
--------------

:example:`protocols/http_server/captive_portal` demonstrates two methods of creating a captive portal, which directs users to an authentication page before browsing, using either DNS queries and HTTP requests redirection or a modern method involving a field in the DHCP offer.

Asynchronous Handlers
---------------------

:example:`protocols/http_server/async_handlers` demonstrates how to handle multiple long-running simultaneous requests within the HTTP server, using different URIs for asynchronous requests, quick requests, and the index page.

RESTful API
-----------

:example:`protocols/http_server/restful_server` demonstrates how to implement a RESTful API server and HTTP server, with a frontend browser UI, and designs several APIs to fetch resources, using mDNS to parse the domain name, and deploying the webpage to host PC via semihost technology or to SPI flash or SD Card.

API Reference
-------------

.. include-build-file:: inc/esp_http_server.inc
