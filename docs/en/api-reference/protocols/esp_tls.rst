ESP-TLS
=======

:link_to_translation:`zh_CN:[中文]`

Overview
--------

The ESP-TLS component provides a simplified API interface for accessing the commonly used TLS functions. It supports common scenarios like CA certification validation, SNI, ALPN negotiation, and non-blocking connection among others. All the configurations can be specified in the ``esp_tls_cfg_t`` data structure. Once done, TLS communication can be conducted using the following APIs:

    * :cpp:func:`esp_tls_init`: for initializing the TLS connection handle.
    * :cpp:func:`esp_tls_conn_new_sync`: for opening a new blocking TLS connection.
    * :cpp:func:`esp_tls_conn_new_async`: for opening a new non-blocking TLS connection.
    * :cpp:func:`esp_tls_conn_read`: for reading from the connection.
    * :cpp:func:`esp_tls_conn_write`: for writing into the connection.
    * :cpp:func:`esp_tls_conn_destroy`: for freeing up the connection.

Any application layer protocol like HTTP1, HTTP2, etc can be executed on top of this layer.

Application Example
-------------------

Simple HTTPS example that uses ESP-TLS to establish a secure socket connection: :example:`protocols/https_request`.

Tree Structure for ESP-TLS Component
-------------------------------------

.. code-block:: none

    ├── esp_tls.c
    ├── esp_tls.h
    ├── esp_tls_mbedtls.c
    ├── esp_tls_wolfssl.c
    └── private_include
        ├── esp_tls_mbedtls.h
        └── esp_tls_wolfssl.h

The ESP-TLS component has a file :component_file:`esp-tls/esp_tls.h` which contains the public API headers for the component. Internally, the ESP-TLS component operates using either MbedTLS or WolfSSL, which are SSL/TLS libraries. APIs specific to MbedTLS are present in :component_file:`esp-tls/private_include/esp_tls_mbedtls.h` and APIs specific to WolfSSL are present in :component_file:`esp-tls/private_include/esp_tls_wolfssl.h`.

.. _esp_tls_server_verification:

TLS Server Verification
-----------------------

ESP-TLS provides multiple options for TLS server verification on the client side. The ESP-TLS client can verify the server by validating the peer's server certificate or with the help of pre-shared keys. The user should select only one of the following options in the :cpp:type:`esp_tls_cfg_t` structure for TLS server verification. If no option is selected, the client will return a fatal error by default during the TLS connection setup.

    *  **cacert_buf** and **cacert_bytes**: The CA certificate can be provided in a buffer to the :cpp:type:`esp_tls_cfg_t` structure. The ESP-TLS uses the CA certificate present in the buffer to verify the server. The following variables in the :cpp:type:`esp_tls_cfg_t` structure must be set.

        * ``cacert_buf`` - pointer to the buffer which contains the CA certification.
        * ``cacert_bytes`` - the size of the CA certificate in bytes.
    * **use_global_ca_store**: The ``global_ca_store`` can be initialized and set at once. Then it can be used to verify the server for all the ESP-TLS connections which have set ``use_global_ca_store = true`` in their respective :cpp:type:`esp_tls_cfg_t` structure. See the API Reference section below for information regarding different APIs used for initializing and setting up the ``global_ca_store``.
    * **crt_bundle_attach**: The ESP x509 Certificate Bundle API provides an easy way to include a bundle of custom x509 root certificates for TLS server verification. More details can be found at :doc:`ESP x509 Certificate Bundle </api-reference/protocols/esp_crt_bundle>`.
    * **psk_hint_key**: To use pre-shared keys for server verification, :ref:`CONFIG_ESP_TLS_PSK_VERIFICATION` should be enabled in the ESP-TLS menuconfig. Then the pointer to the PSK hint and key should be provided to the :cpp:type:`esp_tls_cfg_t` structure. The ESP-TLS will use the PSK for server verification only when no other option regarding server verification is selected.
    * **skip server verification**: This is an insecure option provided in the ESP-TLS for testing purposes. The option can be set by enabling :ref:`CONFIG_ESP_TLS_INSECURE` and :ref:`CONFIG_ESP_TLS_SKIP_SERVER_CERT_VERIFY` in the ESP-TLS menuconfig. When this option is enabled the ESP-TLS will skip server verification by default when no other options for server verification are selected in the :cpp:type:`esp_tls_cfg_t` structure.

      .. warning::

          If this option is enabled, there is a risk of establishing a TLS connection with a server that has a fake identity, unless the server certificate is provided through the API or other mechanisms like ``ca_store``.

SNI (Server Name Indication)
----------------------------

SNI is an extension to the TLS protocol that allows the client to specify the hostname it is connecting to during the TLS handshake. This is required when connecting to servers that host multiple domains on the same IP address.

**How to ensure SNI works properly:**

* SNI is enabled by default in ESP-TLS when using HTTPS connections.
* To explicitly set the SNI hostname, use the ``common_name`` field in :cpp:type:`esp_tls_cfg_t`. This ensures that the correct hostname is sent to the server during the handshake.
* The value of ``common_name`` must match the server certificate's CN (Common Name).
* The ``skip_common_name`` field should be set to ``false`` to ensure the server certificate is properly validated against the hostname. This is required for SNI to function correctly.

Example:

.. code-block:: c

    esp_tls_cfg_t cfg = {
        .cacert_buf = ...,
        .cacert_bytes = ...,
        .common_name = "example.com", // SNI hostname
        .skip_common_name = false,    // Ensure certificate is validated
    };

ESP-TLS Server Cert Selection Hook
----------------------------------

The ESP-TLS component provides an option to set the server certification selection hook when using the MbedTLS stack. This provides an ability to configure and use a certificate selection callback during server handshake. The callback helps to select a certificate to present to the client based on the TLS extensions supplied in the client hello message, such as ALPN and SNI. To enable this feature, please enable  :ref:`CONFIG_ESP_TLS_SERVER_CERT_SELECT_HOOK` in the ESP-TLS menuconfig.

The certificate selection callback can be configured in the :cpp:type:`esp_tls_cfg_t` structure as follows:

.. code-block:: c

    int cert_selection_callback(mbedtls_ssl_context *ssl)
    {
        /* Code that the callback should execute */
        return 0;
    }

    esp_tls_cfg_t cfg = {
        cert_select_cb = cert_section_callback,
    };

.. _esp_tls_wolfssl:

Underlying SSL/TLS Library Options
----------------------------------

The ESP-TLS component offers the option to use MbedTLS or WolfSSL as its underlying SSL/TLS library. By default, only MbedTLS is available and used, WolfSSL SSL/TLS library is also available publicly at https://github.com/espressif/esp-wolfssl. The repository provides the WolfSSL component in binary format, and it also provides a few examples that are useful for understanding the API. Please refer to the repository ``README.md`` for information on licensing and other options. Please see the below section for instructions on how to use WolfSSL in your project.

.. note::

    As the library options are internal to ESP-TLS, switching the libraries will not change ESP-TLS specific code for a project.

How to Use WolfSSL with ESP-IDF
-------------------------------

There are two ways to use WolfSSL in your project:

- Add WolfSSL as a component directly to your project. For this, go to your project directory and run:

  .. code-block:: none

      mkdir components
      cd components
      git clone --recursive https://github.com/espressif/esp-wolfssl.git

- Add WolfSSL as an extra component in your project.

    1. Download WolfSSL with:

       .. code-block:: none

           git clone --recursive https://github.com/espressif/esp-wolfssl.git

    2. Include ESP-WolfSSL in ESP-IDF with setting ``EXTRA_COMPONENT_DIRS`` in ``CMakeLists.txt`` of your project as done in `wolfssl/examples <https://github.com/espressif/esp-wolfssl/tree/master/examples>`_. For reference see :ref:`optional_project_variable` in :doc:`build-system </api-guides/build-system>`.

After the above steps, you will have the option to choose WolfSSL as the underlying SSL/TLS library in the configuration menu of your project as follow:

.. code-block:: none

    idf.py menuconfig > ESP-TLS > SSL/TLS Library > Mbedtls/Wolfssl

Comparison Between MbedTLS and WolfSSL
--------------------------------------

The following table shows a typical comparison between WolfSSL and MbedTLS when the :example:`protocols/https_request` example (which includes server authentication) is running with both SSL/TLS libraries and with all respective configurations set to default. For MbedTLS, the IN_CONTENT length and OUT_CONTENT length are set to 16384 bytes and 4096 bytes respectively.

.. list-table::
    :header-rows: 1
    :widths: 40 30 30
    :align: center

    * - Property
      - WolfSSL
      - MbedTLS
    * - Total Heap Consumed
      - ~ 19 KB
      - ~ 37 KB
    * - Task Stack Used
      - ~ 2.2 KB
      - ~ 3.6 KB
    * - Bin size
      - ~ 858 KB
      - ~ 736 KB

.. note::

    These values can vary based on configuration options and version of respective libraries.

ATECC608A (Secure Element) with ESP-TLS
--------------------------------------------------

ESP-TLS provides support for using ATECC608A cryptoauth chip with ESP32 series of SoCs. The use of ATECC608A is supported only when ESP-TLS is used with MbedTLS as its underlying SSL/TLS stack. ESP-TLS uses MbedTLS as its underlying TLS/SSL stack by default unless changed manually.

.. note::

    ATECC608A chip interfaced to ESP32 series must be already configured. For details, please refer to `esp_cryptoauth_utility <https://github.com/espressif/esp-cryptoauthlib/blob/master/esp_cryptoauth_utility/README.md#esp_cryptoauth_utility>`_.

To enable the secure element support, and use it in your project for TLS connection, you have to follow the below steps:

1) Add `esp-cryptoauthlib <https://github.com/espressif/esp-cryptoauthlib>`_ in your project, for details please refer `how to use esp-cryptoauthlib with ESP-IDF <https://github.com/espressif/esp-cryptoauthlib#how-to-use-esp-cryptoauthlib-with-esp-idf>`_.

2) Enable the menuconfig option :ref:`CONFIG_ESP_TLS_USE_SECURE_ELEMENT`:

   .. code-block:: none

       menuconfig > Component config > ESP-TLS > Use Secure Element (ATECC608A) with ESP-TLS

3) Select type of ATECC608A chip with following option:

   .. code-block:: none

       menuconfig > Component config > esp-cryptoauthlib > Choose Type of ATECC608A chip

   To know more about different types of ATECC608A chips and how to obtain the type of ATECC608A connected to your ESP module, please visit `ATECC608A chip type <https://github.com/espressif/esp-cryptoauthlib/blob/master/esp_cryptoauth_utility/README.md#find-type-of-atecc608a-chip-connected-to-esp32-wroom32-se>`_.

4) Enable the use of ATECC608A in ESP-TLS by providing the following config option in :cpp:type:`esp_tls_cfg_t`:

   .. code-block:: c

       esp_tls_cfg_t cfg = {
           /* other configurations options */
           .use_secure_element = true,
       };

.. only:: SOC_DIG_SIGN_SUPPORTED

    .. _digital-signature-with-esp-tls:

    Digital Signature with ESP-TLS
    ------------------------------

    ESP-TLS provides support for using the Digital Signature (DS) with {IDF_TARGET_NAME}. Use of the DS for TLS is supported only when ESP-TLS is used with MbedTLS (default stack) as its underlying SSL/TLS stack. For more details on Digital Signature, please refer to the :doc:`Digital Signature (DS) </api-reference/peripherals/ds>`. The technical details of Digital Signature such as how to calculate private key parameters can be found in **{IDF_TARGET_NAME} Technical Reference Manual** > **Digital Signature (DS)** [`PDF <{IDF_TARGET_TRM_EN_URL}#digsig>`__]. The DS peripheral must be configured before it can be used to perform Digital Signature, see :ref:`configure-the-ds-peripheral`.

    The DS peripheral must be initialized with the required encrypted private key parameters, which are obtained when the DS peripheral is configured. ESP-TLS internally initializes the DS peripheral when provided with the required DS context, i.e., DS parameters. Please see the below code snippet for passing the DS context to the ESP-TLS context. The DS context passed to the ESP-TLS context should not be freed till the TLS connection is deleted.

    .. code-block:: c

            #include "esp_tls.h"
            esp_ds_data_ctx_t *ds_ctx;
            /* initialize ds_ctx with encrypted private key parameters, which can be read from the nvs or provided through the application code */
            esp_tls_cfg_t cfg = {
                .clientcert_buf = /* the client certification */,
                .clientcert_bytes = /* length of the client certification */,
                /* other configurations options */
                .ds_data = (void *)ds_ctx,
            };

    .. note::

        When using Digital Signature for the TLS connection, along with the other required params, only the client certification (`clientcert_buf`) and the DS params (`ds_data`) are required and the client key (`clientkey_buf`) can be set to NULL.

    * An example of mutual authentication with the DS peripheral can be found at :example:`ssl mutual auth<protocols/mqtt/ssl_mutual_auth>` which internally uses (ESP-TLS) for the TLS connection.

.. only:: SOC_ECDSA_SUPPORTED

    .. _ecdsa-peri-with-esp-tls:

    ECDSA Peripheral with ESP-TLS
    -----------------------------

    ESP-TLS provides support for using the ECDSA peripheral with {IDF_TARGET_NAME}. The use of ECDSA peripheral is supported only when ESP-TLS is used with MbedTLS as its underlying SSL/TLS stack. The ECDSA private key should be present in the eFuse for using the ECDSA peripheral. Please refer to :doc:`ECDSA Guide <../peripherals/ecdsa>` for programming the ECDSA key in the eFuse.

    This will enable the use of ECDSA peripheral for private key operations. As the client private key is already present in the eFuse, it need not be supplied to the :cpp:type:`esp_tls_cfg_t` structure. Please see the below code snippet for enabling the use of ECDSA peripheral for a given ESP-TLS connection.

    .. code-block:: c

        #include "esp_tls.h"
        esp_tls_cfg_t cfg = {
            .use_ecdsa_peripheral = true,
            .ecdsa_key_efuse_blk = 4,     // Low eFuse block for ECDSA key
            .ecdsa_key_efuse_blk_high = 5,   // High eFuse block for ECDSA key (SECP384R1 only)
            .ecdsa_curve = ESP_TLS_ECDSA_CURVE_SECP384R1, // set this to ESP_TLS_ECDSA_CURVE_SECP256R1 for SECP256R1 curve
        };

    .. note::

        When using ECDSA peripheral with TLS, only ``MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256`` ciphersuite is supported. If using TLS v1.3, ``MBEDTLS_TLS1_3_AES_128_GCM_SHA256`` ciphersuite is supported.


.. _esp_tls_client_session_tickets:

Client Session Tickets
----------------------

ESP-TLS supports client-side session resumption, which can significantly reduce the time and resources spent on full TLS handshakes for subsequent connections to the same server. This feature is available when ESP-TLS uses MbedTLS as its underlying SSL/TLS stack.

The mechanism for session resumption differs slightly between TLS versions:

*   **TLS 1.2**: Session resumption can be achieved using session IDs (managed internally by the TLS stack) or session tickets (as per `RFC 5077 <https://tools.ietf.org/html/rfc5077>`_). ESP-TLS focuses on the session ticket mechanism for explicit application control.
*   **TLS 1.3**: Session resumption is accomplished exclusively through session tickets, which are sent by the server via a "NewSessionTicket" message after the main handshake is complete. Unlike TLS 1.2, these tickets can be sent at any time during the session, not just immediately after the handshake.

To enable and use client session tickets:

1. Enable the Kconfig option :ref:`CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS`.
2. After a successful TLS connection (and handshake completion), retrieve the session ticket using :cpp:func:`esp_tls_get_client_session`.

    * **For TLS 1.3**: Since session tickets can arrive from the server at any point after the handshake, an application might need to call :cpp:func:`esp_tls_get_client_session` periodically or after specific application-level exchanges if it wants to ensure it has the most recent ticket. Each new ticket received and processed by the TLS stack supersedes the previous one for future resumption attempts.

3. Store this session ticket securely.
4. For subsequent connections to the same server, provide the stored session ticket in the :cpp:member:`esp_tls_cfg_t::client_session` field.
5. Remember to free the client session context using :cpp:func:`esp_tls_free_client_session` when it's no longer needed or before obtaining a new one.

.. code-block:: c

    #include "esp_tls.h"

    // Global or persistent storage for the client session
    esp_tls_client_session_t *saved_session = NULL;

    void connect_to_server(bool use_saved_session_arg) {
        esp_tls_cfg_t cfg = {0}; // Initialize other config parameters as needed
        // ... set other cfg members like cacert_buf, common_name etc. ...

        if (use_saved_session_arg && saved_session) {
            cfg.client_session = saved_session;
            // ESP_LOGI(TAG, "Attempting connection with saved session ticket.");
        } else {
            // ESP_LOGI(TAG, "Attempting connection without a saved session ticket (full handshake).");
        }

        esp_tls_t *tls = esp_tls_init();
        if (!tls) {
            // ESP_LOGE(TAG, "Failed to initialize ESP-TLS handle.");
            return;
        }

        if (esp_tls_conn_http_new_sync("https://your-server.com", &cfg, tls) == 1) {
            // ESP_LOGI(TAG, "Connection successful.");

            // Always try to get/update the session ticket to have the latest one.
            // This is beneficial whether the connection was a new handshake or a resumption,
            // especially for TLS 1.3 where new tickets can arrive post-handshake.
            if (saved_session) {
                esp_tls_free_client_session(saved_session); // Free previous session if any
                saved_session = NULL;
            }
            saved_session = esp_tls_get_client_session(tls);
            if (saved_session) {
                // ESP_LOGI(TAG, "Successfully retrieved/updated client session ticket.");
            } else {
                // ESP_LOGW(TAG, "Failed to get client session ticket even after a successful connection.");
            }

            // ... do TLS communication ...

        }
        esp_tls_conn_destroy(tls);
    }

.. note::

    - The session ticket obtained from a server is typically valid for a limited time. The server dictates this lifetime.
    - When attempting a connection using a stored session ticket, if the ticket is found to be invalid by the server (e.g., it has expired or is otherwise rejected), ESP-TLS will automatically attempt to perform a full TLS handshake to establish the connection. The application does not need to implement separate logic to retry the connection without the ticket in this scenario. A connection failure will only be reported if both the session resumption and the subsequent internal attempt at a full handshake are unsuccessful.
    - The :cpp:type:`esp_tls_client_session_t` context should be freed using :cpp:func:`esp_tls_free_client_session` when it is no longer needed, or before a new session is obtained and stored in the same pointer.
    - For TLS 1.3, be mindful that the server can send multiple NewSessionTicket messages during a connection. Each successful call to :cpp:func:`esp_tls_get_client_session` will provide the context of the latest ticket processed by the underlying TLS stack. It is the application's responsibility to manage and update its stored session if it wishes to use the newest tickets for resumption.

TLS Ciphersuites
----------------

ESP-TLS provides the ability to set a ciphersuites list in client mode. The TLS ciphersuites list informs the server about the supported ciphersuites for the specific TLS connection regardless of the TLS stack configuration. If the server supports any ciphersuite from this list, then the TLS connection will succeed; otherwise, it will fail.

You can set ``ciphersuites_list`` in the :cpp:type:`esp_tls_cfg_t` structure during client connection as follows:

.. code-block:: c

    /* ciphersuites_list must end with 0 and must be available in the memory scope active during the entire TLS connection */
    static const int ciphersuites_list[] = {MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384, MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384, 0};
    esp_tls_cfg_t cfg = {
        .ciphersuites_list = ciphersuites_list,
    };

ESP-TLS will not check the validity of ``ciphersuites_list`` that was set, you should call :cpp:func:`esp_tls_get_ciphersuites_list` to get ciphersuites list supported in the TLS stack and cross-check it against the supplied list.

.. note::

   This feature is supported only in the MbedTLS stack.

TLS Protocol Version
--------------------

ESP-TLS provides the ability to set the TLS protocol version for the respective TLS connection. Once the version is specified, it should be exclusively used to establish the TLS connection. This provides an ability to route different TLS connections to different protocol versions like TLS 1.2 and TLS 1.3 at runtime.

.. note::

   At the moment, the feature is supported only when ESP-TLS is used with MbedTLS as its underlying SSL/TLS stack.

To set TLS protocol version with ESP-TLS, set :cpp:member:`esp_tls_cfg_t::tls_version` to the required protocol version from :cpp:type:`esp_tls_proto_ver_t`. If the protocol version field is not set, then the default policy is to allow TLS connection based on the server requirement.

The ESP-TLS connection can be configured to use the specified protocol version as follows:

.. code-block:: c

    #include "esp_tls.h"
    esp_tls_cfg_t cfg = {
        .tls_version = ESP_TLS_VER_TLS_1_2,
    };

API Reference
-------------

.. include-build-file:: inc/esp_tls.inc
.. include-build-file:: inc/esp_tls_errors.inc
