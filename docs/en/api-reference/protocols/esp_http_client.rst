ESP HTTP Client
===============

Overview
--------

``esp_http_client`` provides an API for making HTTP/S requests from ESP-IDF programs. The steps to use this API for an HTTP request are:

    * :cpp:func:`esp_http_client_init`: To use the HTTP client, the first thing we must do is create an :cpp:class:`esp_http_client` by pass into this function with the :cpp:class:`esp_http_client_config_t` configurations. Which configuration values we do not define, the library will use default.
    * :cpp:func:`esp_http_client_perform`: The :cpp:class:`esp_http_client` argument created from the init function is needed. This function performs all operations of the esp_http_client, from opening the connection, sending data, downloading data and closing the connection if necessary. All related events will be invoked in the event_handle (defined by :cpp:class:`esp_http_client_config_t`). This function performs its job and blocks the current task until it's done
    * :cpp:func:`esp_http_client_cleanup`: After completing our **esp_http_client's** task, this is the last function to be called. It will close the connection (if any) and free up all the memory allocated to the HTTP client


Application Example
-------------------

    .. highlight:: c

    ::

        esp_err_t _http_event_handle(esp_http_client_event_t *evt)
        {
            switch(evt->event_id) {
                case HTTP_EVENT_ERROR:
                    ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
                    break;
                case HTTP_EVENT_ON_CONNECTED:
                    ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
                    break;
                case HTTP_EVENT_HEADER_SENT:
                    ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
                    break;
                case HTTP_EVENT_ON_HEADER:
                    ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
                    printf("%.*s", evt->data_len, (char*)evt->data);
                    break;
                case HTTP_EVENT_ON_DATA:
                    ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
                    if (!esp_http_client_is_chunked_response(evt->client)) {
                        printf("%.*s", evt->data_len, (char*)evt->data);
                    }

                    break;
                case HTTP_EVENT_ON_FINISH:
                    ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
                    break;
                case HTTP_EVENT_DISCONNECTED:
                    ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
                    break;
            }
            return ESP_OK;
        }

        esp_http_client_config_t config = {
           .url = "http://httpbin.org/redirect/2",
           .event_handler = _http_event_handle,
        };
        esp_http_client_handle_t client = esp_http_client_init(&config);
        esp_err_t err = esp_http_client_perform(client);

        if (err == ESP_OK) {
           ESP_LOGI(TAG, "Status = %d, content_length = %d",
                   esp_http_client_get_status_code(client),
                   esp_http_client_get_content_length(client));
        }
        esp_http_client_cleanup(client);


Persistent Connections
----------------------

Persistent connections means that the HTTP client can re-use the same connection for several transfers. If the server does not request to close the connection with the ``Connection: close`` header, the new transfer with sample ip address, port, and protocol.

To allow the HTTP client to take full advantage of persistent connections, you should do as many of your file transfers as possible using the same handle.

Persistent Connections example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. highlight:: c

::

    esp_err_t err;
    esp_http_client_config_t config = {
        .url = "http://httpbin.org/get",
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    // first request
    err = esp_http_client_perform(client);

    // second request
    esp_http_client_set_url(client, "http://httpbin.org/anything")
    esp_http_client_set_method(client, HTTP_METHOD_DELETE);
    esp_http_client_set_header(client, "HeaderKey", "HeaderValue");
    err = esp_http_client_perform(client);

    esp_http_client_cleanup(client);


HTTPS
-----

The HTTP client supports SSL connections using **mbedtls**, with the **url** configuration starting with ``https`` scheme (or ``transport_type = HTTP_TRANSPORT_OVER_SSL``). HTTPS support can be configured via :ref:CONFIG_ENABLE_HTTPS (enabled by default)..

.. note:: By providing information using HTTPS, the library will use the SSL transport type to connect to the server. If you want to verify server, then need to provide additional certificate in PEM format, and provide to ``cert_pem`` in ``esp_http_client_config_t``

HTTPS example
^^^^^^^^^^^^^

.. highlight:: c

::

    static void https()
    {
        esp_http_client_config_t config = {
            .url = "https://www.howsmyssl.com",
            .cert_pem = howsmyssl_com_root_cert_pem_start,
        };
        esp_http_client_handle_t client = esp_http_client_init(&config);
        esp_err_t err = esp_http_client_perform(client);

        if (err == ESP_OK) {
            ESP_LOGI(TAG, "Status = %d, content_length = %d",
                    esp_http_client_get_status_code(client),
                    esp_http_client_get_content_length(client));
        }
        esp_http_client_cleanup(client);
    }

HTTP Stream
-----------

Some applications need to open the connection and control the reading of the data in an active manner. the HTTP client supports some functions to make this easier, of course, once you use these functions you should not use the :cpp:func:`esp_http_client_perform` function with that handle, and :cpp:func:`esp_http_client_init` alway to called first to get the handle. Perform that functions in the order below:

    * :cpp:func:`esp_http_client_init`: to create and handle
    * ``esp_http_client_set_*`` or ``esp_http_client_delete_*``: to modify the http connection information (optional)
    * :cpp:func:`esp_http_client_open`: Open the http connection with ``write_len`` parameter, ``write_len=0`` if we only need read
    * :cpp:func:`esp_http_client_write`: Upload data, max length equal to ``write_len`` of :cpp:func:`esp_http_client_open` function. We may not need to call it if ``write_len=0``
    * :cpp:func:`esp_http_client_fetch_headers`: After sending the headers and write data (if any) to the server, this function will read the HTTP Server response headers. Calling this function will return the ``content-length`` from the Server, and we can call :cpp:func:`esp_http_client_get_status_code` for the HTTP status of the connection.
    * :cpp:func:`esp_http_client_read`: Now, we can read the HTTP stream by this function. 
    * :cpp:func:`esp_http_client_close`: We should the connection after finish
    * :cpp:func:`esp_http_client_cleanup`: And release the resources

Perform HTTP request as Stream reader
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Check the example function ``http_perform_as_stream_reader`` at :example:`protocols/esp_http_client`.


HTTP Authentication
-------------------

The HTTP client supports both **Basic** and **Digest** Authentication. By providing usernames and passwords in ``url`` or in the ``username``, ``password`` of config entry. And with ``auth_type = HTTP_AUTH_TYPE_BASIC``, the HTTP client takes only 1 perform to pass the authentication process. If ``auth_type = HTTP_AUTH_TYPE_NONE``, but there are ``username`` and ``password`` in the configuration, the HTTP client takes 2 performs. The first time it connects to the server and receives the UNAUTHORIZED header. Based on this information, it will know which authentication method to choose, and perform it on the second.


Config authentication example with URI
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. highlight:: c

::

    esp_http_client_config_t config = {
        .url = "http://user:passwd@httpbin.org/basic-auth/user/passwd",
        .auth_type = HTTP_AUTH_TYPE_BASIC,
    };


Config authentication example with username, password entry
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. highlight:: c

::

    esp_http_client_config_t config = {
        .url = "http://httpbin.org/basic-auth/user/passwd",
        .username = "user",
        .password = "passwd",
        .auth_type = HTTP_AUTH_TYPE_BASIC,
    };
    

HTTP Client example: :example:`protocols/esp_http_client`.


API Reference
-------------

.. include:: /_build/inc/esp_http_client.inc
