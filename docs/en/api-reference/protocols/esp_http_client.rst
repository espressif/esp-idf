ESP HTTP Client
===============

Overview
--------

``esp_http_client`` provides an API for making HTTP/S requests from ESP-IDF applications. The steps to use this API are as follows:

    * :cpp:func:`esp_http_client_init`: Creates an :cpp:type:`esp_http_client_config_t` instance i.e. a HTTP client handle based on the given :cpp:type:`esp_http_client_config_t` configuration. This function must be the first to be called; default values will be assumed for the configuration values that are not explicitly defined by the user.
    * :cpp:func:`esp_http_client_perform`: Performs all operations of the esp_http_client - opening the connection, exchanging data and closing the connection (as required), while blocking the current task until its completion. All related events will be invoked through the event handler (as specified in :cpp:type:`esp_http_client_config_t`).
    * :cpp:func:`esp_http_client_cleanup`: Closes the connection (if any) and frees up all the memory allocated to the HTTP client instance. This must be the last function to be called after the completion of operations.


Application Example
-------------------

Simple example that uses ESP HTTP Client to make HTTP/S requests at :example:`protocols/esp_http_client`.


Basic HTTP request
------------------

Check out the example functions ``http_rest_with_url`` and ``http_rest_with_hostname_path`` in the application example for implementation details.

Persistent Connections
----------------------

Persistent connection means that the HTTP client can re-use the same connection for several exchanges. If the server does not request to close the connection with the ``Connection: close`` header, the connection is not dropped but is instead kept open and used for further requests.

To allow ESP HTTP client to take full advantage of persistent connections, one should make as many requests as possible using the same handle instance.

Check out the example functions ``http_rest_with_url`` and ``http_rest_with_hostname_path`` in the application example. Here, once the connection is created, multiple requests (``GET``, ``POST``, ``PUT``, etc.) are made before the connection is closed.

HTTPS Request
-------------

ESP HTTP client supports SSL connections using **mbedTLS**, with the ``url`` configuration starting with ``https`` scheme or ``transport_type`` set to ``HTTP_TRANSPORT_OVER_SSL``. HTTPS support can be configured via :ref:`CONFIG_ESP_HTTP_CLIENT_ENABLE_HTTPS` (enabled by default).

.. note:: While making HTTPS requests, if server verification is needed, additional root certificate (in PEM format) needs to be provided to the ``cert_pem`` member in ``esp_http_client_config_t`` configuration. Users can also use the ``ESP x509 Certificate Bundle`` for server verification using the ``crt_bundle_attach`` member of the ``esp_http_client_config_t`` configuration.

Check out the example functions ``https_with_url`` and ``https_with_hostname_path`` in the application example. (Implementation details of the above note are found here)


HTTP Stream
-----------

Some applications need to open the connection and control the exchange of data actively (data streaming). In such cases, the application flow is different from regular requests. Example flow is given below:

    * :cpp:func:`esp_http_client_init`: Create a HTTP client handle
    * ``esp_http_client_set_*`` or ``esp_http_client_delete_*``: Modify the HTTP connection parameters (optional)
    * :cpp:func:`esp_http_client_open`: Open the HTTP connection with ``write_len`` parameter (content length that needs to be written to server), set ``write_len=0`` for read-only connection
    * :cpp:func:`esp_http_client_write`: Write data to server with a maximum length equal to ``write_len`` of :cpp:func:`esp_http_client_open` function; no need to call this function for ``write_len=0``
    * :cpp:func:`esp_http_client_fetch_headers`: Read the HTTP Server response headers, after sending the request headers and server data (if any). Returns the ``content-length`` from the server and can be succeeded by :cpp:func:`esp_http_client_get_status_code` for getting the HTTP status of the connection.
    * :cpp:func:`esp_http_client_read`: Read the HTTP stream
    * :cpp:func:`esp_http_client_close`: Close the connection
    * :cpp:func:`esp_http_client_cleanup`: Release allocated resources

Check out the example function ``http_perform_as_stream_reader`` in the application example for implementation details.


HTTP Authentication
-------------------

ESP HTTP client supports both **Basic** and **Digest** Authentication.
    * Users can provide the username and password in the ``url`` or the ``username`` and ``password`` members of the ``esp_http_client_config_t`` configuration. For ``auth_type = HTTP_AUTH_TYPE_BASIC``, the HTTP client takes only 1 perform operation to pass the authentication process.
    * If ``auth_type = HTTP_AUTH_TYPE_NONE``, but the ``username`` and ``password`` fields are present in the configuration, the HTTP client takes 2 perform operations. The client will receive the ``401 Unauthorized`` header in its first attempt to connect to the server. Based on this information, it decides which authentication method to choose and performs it in the second operation.
    * Check out the example functions ``http_auth_basic``, ``http_auth_basic_redirect`` (for Basic authentication) and ``http_auth_digest`` (for Digest authentication) in the application example for implementation details.

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



API Reference
-------------

.. include-build-file:: inc/esp_http_client.inc
