ESP HTTP Client
===============

:link_to_translation:`zh_CN:[中文]`

Overview
--------

``esp_http_client`` component provides a set of APIs for making HTTP/S requests from ESP-IDF applications. The steps to use these APIs are as follows:

    * :cpp:func:`esp_http_client_init`: Creates an :cpp:type:`esp_http_client_handle_t` instance, i.e., an HTTP client handle based on the given :cpp:type:`esp_http_client_config_t` configuration. This function must be the first to be called; default values are assumed for the configuration values that are not explicitly defined by the user.
    * :cpp:func:`esp_http_client_perform`: Performs all operations of the ``esp_http_client`` - opening the connection, exchanging data, and closing the connection (as required), while blocking the current task before its completion. All related events are invoked through the event handler (as specified in :cpp:type:`esp_http_client_config_t`).
    * :cpp:func:`esp_http_client_cleanup`: Closes the connection (if any) and frees up all the memory allocated to the HTTP client instance. This must be the last function to be called after the completion of operations.


Application Examples
--------------------

- :example:`protocols/esp_http_client` demonstrates how to use the ESP HTTP Client to make HTTP/S requests.


Basic HTTP Request
------------------

Check out the example functions ``http_rest_with_url`` and ``http_rest_with_hostname_path`` in the application example for implementation details.


Persistent Connections
----------------------

Persistent connection means that the HTTP client can reuse the same connection for several exchanges. If the server does not request to close the connection with the ``Connection: close`` header, the connection is not dropped but is instead kept open and used for further requests.

To allow ESP HTTP client to take full advantage of persistent connections, one should make as many requests as possible using the same handle instance. Check out the example functions ``http_rest_with_url`` and ``http_rest_with_hostname_path`` in the application example. Here, once the connection is created, multiple requests (``GET``, ``POST``, ``PUT``, etc.) are made before the connection is closed.

Use Secure Element (ATECC608) for TLS
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A secure element (ATECC608) can be also used for the underlying TLS connection in the HTTP client connection. Please refer to the **ATECC608A (Secure Element) with ESP-TLS** section in the :doc:`ESP-TLS documentation </api-reference/protocols/esp_tls>` for more details. The secure element support has to be first enabled in menuconfig through :ref:`CONFIG_ESP_TLS_USE_SECURE_ELEMENT`. Then the HTTP client can be configured to use secure element as follows:

.. code-block:: c

    esp_http_client_config_t cfg = {
        /* other configurations options */
        .use_secure_element = true,
    };

.. only:: SOC_ECDSA_SUPPORTED

    Use ECDSA Peripheral for TLS
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The ECDSA peripheral can be used for the underlying TLS connection in the HTTP client connection. Please refer to the **ECDSA Peripheral with ESP-TLS** section in the :doc:`ESP-TLS documentation </api-reference/protocols/esp_tls>` for more details. The HTTP client can be configured to use ECDSA peripheral as follows:

    .. code-block:: c

        esp_http_client_config_t cfg = {
            /* other configurations options */
            .use_ecdsa_peripheral = true,
            .ecdsa_key_efuse_blk = 4,    // Low eFuse block for ECDSA key
            .ecdsa_key_efuse_blk_high = 5,   // High eFuse block for ECDSA key (SECP384R1 only)
            .ecdsa_curve = ESP_TLS_ECDSA_CURVE_SECP384R1, // set this to ESP_TLS_ECDSA_CURVE_SECP256R1 for SECP256R1 curve
        };


HTTPS Request
-------------

ESP HTTP client supports SSL connections using **mbedTLS**, with the ``url`` configuration starting with ``https`` scheme or ``transport_type`` set to ``HTTP_TRANSPORT_OVER_SSL``. HTTPS support can be configured via :ref:`CONFIG_ESP_HTTP_CLIENT_ENABLE_HTTPS` (enabled by default).

.. note::

    While making HTTPS requests, if server verification is needed, an additional root certificate (in PEM format) needs to be provided to the ``cert_pem`` member in the ``esp_http_client_config_t`` configuration. Users can also use the ``ESP x509 Certificate Bundle`` for server verification using the ``crt_bundle_attach`` member of the ``esp_http_client_config_t`` configuration.

Check out the example functions ``https_with_url`` and ``https_with_hostname_path`` in the application example for implementation details of the above note.


HTTP Stream
-----------

Some applications need to open the connection and control the exchange of data actively (data streaming). In such cases, the application flow is different from regular requests. Example flow is given below:

    * :cpp:func:`esp_http_client_init`: Create a HTTP client handle.
    * ``esp_http_client_set_*`` or ``esp_http_client_delete_*``: Modify the HTTP connection parameters (optional).
    * :cpp:func:`esp_http_client_open`: Open the HTTP connection with ``write_len`` parameter (content length that needs to be written to server), set ``write_len=0`` for read-only connection.
    * :cpp:func:`esp_http_client_write`: Write data to server with a maximum length equal to ``write_len`` of :cpp:func:`esp_http_client_open` function; no need to call this function for ``write_len=0``.
    * :cpp:func:`esp_http_client_fetch_headers`: Read the HTTP Server response headers, after sending the request headers and server data (if any). Returns the ``content-length`` from the server and can be succeeded by :cpp:func:`esp_http_client_get_status_code` for getting the HTTP status of the connection.
    * :cpp:func:`esp_http_client_read`: Read the HTTP stream.
    * :cpp:func:`esp_http_client_close`: Close the connection.
    * :cpp:func:`esp_http_client_cleanup`: Release allocated resources.

Check out the example function ``http_perform_as_stream_reader`` in the application example for implementation details.


HTTP Authentication
-------------------

ESP HTTP client supports both **Basic** and **Digest** Authentication.
    * Users can provide the username and password in the ``url`` or the ``username`` and ``password`` members of the ``esp_http_client_config_t`` configuration. For ``auth_type = HTTP_AUTH_TYPE_BASIC``, the HTTP client takes only one perform operation to pass the authentication process.
    * If ``auth_type = HTTP_AUTH_TYPE_NONE``, but the ``username`` and ``password`` fields are present in the configuration, the HTTP client takes two perform operations. The client will receive the ``401 Unauthorized`` header in its first attempt to connect to the server. Based on this information, it decides which authentication method to choose and performs it in the second operation.
    * Check out the example functions ``http_auth_basic``, ``http_auth_basic_redirect`` (for Basic authentication) and ``http_auth_digest`` (for Digest authentication) in the application example for implementation details.
    * Currently, Digest authentication supports only MD5 and SHA-256 algorithms.


Examples of Authentication Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    * Authentication with URI

        .. highlight:: c

        ::

            esp_http_client_config_t config = {
                .url = "http://user:passwd@httpbin.org/basic-auth/user/passwd",
                .auth_type = HTTP_AUTH_TYPE_BASIC,
            };


    * Authentication with username and password entry

        .. highlight:: c

        ::

            esp_http_client_config_t config = {
                .url = "http://httpbin.org/basic-auth/user/passwd",
                .username = "user",
                .password = "passwd",
                .auth_type = HTTP_AUTH_TYPE_BASIC,
            };

Response Header Access
----------------------

ESP HTTP Client provides the ability to save and retrieve HTTP response headers from the server. This feature is useful when applications need to access metadata such as content type, cache control directives, custom server headers, or other response information.

Configuration
^^^^^^^^^^^^^

To enable response header saving, the following Kconfig options must be configured:

    * :ref:`CONFIG_ESP_HTTP_CLIENT_SAVE_RESPONSE_HEADERS`: Enable saving of response headers (disabled by default to conserve memory).
    * :ref:`CONFIG_ESP_HTTP_CLIENT_MAX_SAVED_RESPONSE_HEADERS`: Maximum number of response headers to save (default: 10).
    * :ref:`CONFIG_ESP_HTTP_CLIENT_MAX_RESPONSE_HEADER_SIZE`: Maximum size in bytes for both header key and value (default: 128 bytes each).

Usage
^^^^^

Once enabled, response headers can be retrieved using the :cpp:func:`esp_http_client_get_response_header` function after performing an HTTP request. The function returns the header value for a given key.

Example:

.. code-block:: c

    #if CONFIG_ESP_HTTP_CLIENT_SAVE_RESPONSE_HEADERS
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        char *content_type = NULL;
        err = esp_http_client_get_response_header(client, "Content-Type", &content_type);
        if (err == ESP_OK && content_type != NULL) {
            ESP_LOGI(TAG, "Content-Type: %s", content_type);
        } else if (err == ESP_ERR_NOT_FOUND) {
            ESP_LOGW(TAG, "Content-Type header not found");
        }

        char *date = NULL;
        err = esp_http_client_get_response_header(client, "Date", &date);
        if (err == ESP_OK && date != NULL) {
            ESP_LOGI(TAG, "Date: %s", date);
        }
    }

    esp_http_client_cleanup(client);
    #endif

Important Limitations
^^^^^^^^^^^^^^^^^^^^^

When using response header access, be aware of the following limitations:

    * **Header Count Limit**: Only the first ``CONFIG_ESP_HTTP_CLIENT_MAX_SAVED_RESPONSE_HEADERS`` headers are saved. Additional headers beyond this limit are discarded with a warning log.
    * **Size Constraints**: Headers where either the key or value exceeds ``CONFIG_ESP_HTTP_CLIENT_MAX_RESPONSE_HEADER_SIZE`` bytes are discarded with a warning log showing the actual sizes.
    * **Multi-Value Headers**: For headers that appear multiple times in the response (e.g., ``Set-Cookie``), only the last value is retained.
    * **Case Sensitivity**: Header lookups are case-insensitive, but the original case is preserved in storage.
    * **Memory Overhead**: Enabling this feature increases memory consumption. Calculate approximate memory usage as: ``(CONFIG_ESP_HTTP_CLIENT_MAX_SAVED_RESPONSE_HEADERS * CONFIG_ESP_HTTP_CLIENT_MAX_RESPONSE_HEADER_SIZE * 2)`` bytes per client instance.
    * **Header Lifecycle**: Response headers are cleared when starting a new request with the same client handle via :cpp:func:`esp_http_client_perform` or :cpp:func:`esp_http_client_prepare`.

.. note::

    The returned header value pointer is managed internally by the HTTP client and must not be freed by the application. The pointer remains valid until the client handle is cleaned up or a new request is initiated.

Event Handling
--------------

ESP HTTP Client supports event handling by triggering an event handler corresponding to the event which takes place. :cpp:enum:`esp_http_client_event_id_t` contains all the events which could occur while performing an HTTP request using the ESP HTTP Client.

To enable event handling, you just need to set a callback function using the :cpp:member:`esp_http_client_config_t::event_handler` member.

ESP HTTP Client Diagnostic Information
--------------------------------------

Diagnostic information could be helpful to gain insights into a problem. In the case of ESP HTTP Client, the diagnostic information can be collected by registering an event handler with :doc:`the Event Loop library <../system/esp_event>`. This feature has been added by keeping in mind the `ESP Insights <https://github.com/espressif/esp-insights>`_ framework which collects the diagnostic information. However, this feature can also be used without any dependency on the ESP Insights framework for the diagnostic purpose. Event handler can be registered to the event loop using the :cpp:func:`esp_event_handler_register` function.

Expected data types for different HTTP Client events in the event loop are as follows:

    - HTTP_EVENT_ERROR              : ``esp_http_client_handle_t``
    - HTTP_EVENT_ON_CONNECTED       : ``esp_http_client_handle_t``
    - HTTP_EVENT_HEADERS_SENT       : ``esp_http_client_handle_t``
    - HTTP_EVENT_ON_HEADER          : ``esp_http_client_handle_t``
    - HTTP_EVENT_ON_HEADERS_COMPLETE: ``esp_http_client_handle_t``
    - HTTP_EVENT_ON_STATUS_CODE     : ``esp_http_client_handle_t``
    - HTTP_EVENT_ON_DATA            : ``esp_http_client_on_data_t``
    - HTTP_EVENT_ON_FINISH          : ``esp_http_client_handle_t``
    - HTTP_EVENT_DISCONNECTED       : ``esp_http_client_handle_t``
    - HTTP_EVENT_REDIRECT           : ``esp_http_client_redirect_event_data_t``

The :cpp:type:`esp_http_client_handle_t` received along with the event data will be valid until :cpp:enumerator:`HTTP_EVENT_DISCONNECTED <esp_http_client_event_id_t::HTTP_EVENT_DISCONNECTED>` is not received. This handle has been sent primarily to differentiate between different client connections and must not be used for any other purpose, as it may change based on client connection state.

TLS Protocol Version
--------------------

TLS protocol version to be used for the underlying TLS connection can be set in :cpp:type:`esp_http_client_config_t`. Please refer to the **TLS Protocol Version** section in the :doc:`/api-reference/protocols/esp_tls` for more details.

The TLS protocol version for the HTTP client can be configured as follows:

    .. code-block:: c

        #include "esp_http_client.h"
        esp_http_client_config_t config = {
            .tls_version = ESP_HTTP_CLIENT_TLS_VER_TLS_1_2,
        };

API Reference
-------------

.. include-build-file:: inc/esp_http_client.inc
