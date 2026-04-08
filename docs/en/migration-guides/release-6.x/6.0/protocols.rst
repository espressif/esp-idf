Protocols
=========

:link_to_translation:`zh_CN:[中文]`

JSON
----

Removed Built-in JSON Component
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The built-in ``json`` component has been removed from ESP-IDF. Users should migrate to using the ``espressif/cjson`` component from the `IDF Component Manager <https://components.espressif.com/>`_.

Migration Steps
^^^^^^^^^^^^^^^

1. **Remove json from CMakeLists.txt**

   In your component's ``CMakeLists.txt``, remove ``json`` from the ``REQUIRES`` or ``PRIV_REQUIRES`` list:

   .. code-block:: cmake

       # Before
       idf_component_register(SRCS "main.c"
                              PRIV_REQUIRES json esp_http_server)

       # After
       idf_component_register(SRCS "main.c"
                              PRIV_REQUIRES esp_http_server)

2. **Add espressif/cjson to idf_component.yml**

   Add the ``espressif/cjson`` dependency to your component's ``idf_component.yml`` file. If this file doesn't exist, create it in your component directory (e.g., ``main/idf_component.yml``):

   .. code-block:: yaml

       dependencies:
         espressif/cjson: "^1.7.19"

3. **No Code Changes Required**

   The API remains the same. Your existing code using cJSON functions will continue to work without modifications:

   .. code-block:: c

       #include "cJSON.h"

       // Existing code works unchanged
       cJSON *root = cJSON_Parse(json_string);
       cJSON *item = cJSON_GetObjectItem(root, "key");
       cJSON_Delete(root);

For more information:

- `espressif/cjson component <https://components.espressif.com/components/espressif/cjson>`_
- `cJSON on GitHub <https://github.com/espressif/idf-extra-components/tree/master/cjson>`_

ESP-TLS
-------

Removed wolfSSL Support
~~~~~~~~~~~~~~~~~~~~~~~

The built-in wolfSSL TLS stack support has been removed from ESP-TLS. Users who were using wolfSSL should migrate to either:

1. **mbedTLS (Recommended)**: The default TLS stack, fully integrated and maintained within ESP-IDF.
2. **Custom TLS Stack**: Register your own TLS implementation using the new custom stack API (see Option B below).

**Removed Kconfig Options**

The following Kconfig options have been removed:

- ``CONFIG_ESP_TLS_USING_WOLFSSL`` - Use ``CONFIG_ESP_TLS_USING_MBEDTLS`` or ``CONFIG_ESP_TLS_CUSTOM_STACK`` instead
- ``CONFIG_ESP_DEBUG_WOLFSSL`` - For mbedTLS debugging, use ``CONFIG_MBEDTLS_DEBUG``
- ``CONFIG_ESP_TLS_OCSP_CHECKALL`` - OCSP functionality should be handled by the chosen TLS stack

**Migration Steps for wolfSSL Users**

If your project was using wolfSSL via ESP-TLS:

1. **Option A - Switch to mbedTLS**

   - Remove ``CONFIG_ESP_TLS_USING_WOLFSSL=y`` from your sdkconfig
   - The default ``CONFIG_ESP_TLS_USING_MBEDTLS`` will be used automatically
   - No code changes required for standard TLS operations

2. **Option B - Use Custom Stack API**

   If you need to continue using wolfSSL or another TLS library, you can register it as a custom stack:

   - Enable ``CONFIG_ESP_TLS_CUSTOM_STACK`` in menuconfig
   - Implement the :cpp:type:`esp_tls_stack_ops_t` interface for your TLS library
   - Call :cpp:func:`esp_tls_register_stack` before creating any TLS connections

   For detailed documentation on implementing a custom TLS stack, see :ref:`esp_tls_custom_stack`.

Removed Deprecated API
~~~~~~~~~~~~~~~~~~~~~~

The deprecated :cpp:func:`esp_tls_conn_http_new` function has been removed. Use either:

- :cpp:func:`esp_tls_conn_http_new_sync` for blocking connections
- :cpp:func:`esp_tls_conn_http_new_async` for non-blocking connections

The new API requires you to create the :cpp:type:`esp_tls_t` structure using :cpp:func:`esp_tls_init` and provides better control over the connection process.

Unified Private Key Interface
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``use_secure_element`` field has been removed from :cpp:type:`esp_tls_cfg`, :cpp:type:`esp_tls_cfg_server`, and :cpp:type:`httpd_ssl_config`. The ATECC608A secure element and all other hardware-backed key sources (DS peripheral, ECDSA peripheral, Key Manager) are now accessed through a unified :cpp:type:`esp_key_config_t` interface via PSA Crypto key IDs.

**Migration Steps**

1. Replace ``use_secure_element = true`` with the new :cpp:type:`esp_key_config_t` using ``ESP_KEY_SOURCE_PSA`` and a PSA key ID obtained from ``psa_import_key()``.

2. The ``atcab_init()`` call is no longer performed internally by ESP-TLS. Applications using the ATECC608A must ensure the secure element is initialized at the application level before use. Refer to the `esp-cryptoauthlib documentation <https://github.com/espressif/esp-cryptoauthlib>`_ for details.

3. The ``esp_transport_ssl_use_secure_element()`` function has been removed from ``tcp_transport``. Use ``esp_transport_ssl_set_client_key_config()`` instead.

4. The Kconfig options for the secure element driver have been consolidated from ``CONFIG_MBEDTLS_ATCA_HW_ECDSA_SIGN`` / ``CONFIG_MBEDTLS_ATCA_HW_ECDSA_VERIFY`` into a single ``CONFIG_MBEDTLS_SECURE_ELEMENT_DRIVER_ENABLED``. The old names are automatically mapped via ``sdkconfig.rename``.

For detailed usage examples, see :ref:`atecc608a-with-esp-tls`.

ESP HTTP Server
---------------

WebSocket Handler No Longer Called During Handshake
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

From v6.0.1, the URI handler registered for a WebSocket endpoint is **no longer called** during the WebSocket handshake.

Prior to this change, the handler was invoked with ``req->method == HTTP_GET`` immediately after the handshake completed, which applications used for connection-time initialization:

.. code-block:: c

    /* Pre-v6.0.1 pattern — no longer works from v6.0.1 onwards */
    static esp_err_t ws_handler(httpd_req_t *req)
    {
        if (req->method == HTTP_GET) {
            ESP_LOGI(TAG, "New WebSocket connection established");
            return ESP_OK;
        }
        /* Handle WebSocket frames ... */
    }

From v6.0.1, the handler is invoked only for subsequent WebSocket data frames, so the ``HTTP_GET`` check is no longer needed in frame handlers.

Migration Options
^^^^^^^^^^^^^^^^^

**Option 1 (Recommended)** — Move connection-time logic into a dedicated post-handshake callback:

1. Enable :ref:`CONFIG_HTTPD_WS_POST_HANDSHAKE_CB_SUPPORT` in menuconfig.
2. Register a ``ws_post_handshake_cb`` on the ``httpd_uri_t`` struct. The frame handler remains clean with no ``HTTP_GET`` check.

.. code-block:: c

    static esp_err_t ws_on_connect(httpd_req_t *req)
    {
        ESP_LOGI(TAG, "New WebSocket connection established");
        return ESP_OK;
    }

    static esp_err_t ws_handler(httpd_req_t *req)
    {
        /* Handle WebSocket frames only */
    }

    static const httpd_uri_t ws_uri = {
        .uri                  = "/ws",
        .method               = HTTP_GET,
        .handler              = ws_handler,
        .is_websocket         = true,
        .ws_post_handshake_cb = ws_on_connect,
    };

**Option 2 (Minimal change)** — Set ``.ws_post_handshake_cb`` to the same function as ``.handler``:

1. Enable :ref:`CONFIG_HTTPD_WS_POST_HANDSHAKE_CB_SUPPORT` in menuconfig.
2. Set ``.ws_post_handshake_cb = ws_handler`` in the URI registration. The existing ``if (req->method == HTTP_GET)`` check inside the handler continues to work without any further code changes.

.. code-block:: c

    static const httpd_uri_t ws_uri = {
        .uri                  = "/ws",
        .method               = HTTP_GET,
        .handler              = ws_handler,
        .is_websocket         = true,
        .ws_post_handshake_cb = ws_handler,   /* same function restores old behavior */
    };

ESP-Modbus
----------

The Espressif ESP-Modbus Library (esp-modbus) supports Modbus communication in the networks based on RS485, Wi-Fi, and Ethernet interfaces.

The component ``esp-modbus v2 (v2.x.x)`` is the current supported component version:

* `ESP-Modbus component on GitHub <https://github.com/espressif/esp-modbus/tree/main>`__

Documentation
~~~~~~~~~~~~~

* `ESP-MODBUS stable documentation v2.x.x <https://docs.espressif.com/projects/esp-modbus/en/stable>`__
* `Documentation for legacy version v1.x.x <https://docs.espressif.com/projects/esp-modbus/en/v1>`__

Application Examples
~~~~~~~~~~~~~~~~~~~~

Since ESP-IDF version v6.0, the examples for component ``esp-modbus v1`` which is obsolete have been removed from ESP-IDF.

- `legacy esp-modbus v1.x.x examples (esp-idf v5.5) <https://github.com/espressif/esp-idf/tree/release/v5.5/examples/protocols/modbus>`__

The examples below demonstrate the ESP-Modbus library of serial and TCP ports for both slave and master implementations respectively.

- `mb_serial_slave <https://github.com/espressif/esp-modbus/tree/main/examples/serial/mb_serial_slave>`__ - demonstrates how to use {IDF_TARGET_NAME} as a Modbus serial slave device with the esp-modbus stack, enabling an external Modbus host to read and write device parameters using the Modbus protocol.

- `mb_serial_master <https://github.com/espressif/esp-modbus/tree/main/examples/serial/mb_serial_master>`__ - demonstrates how to use the esp-modbus stack port on {IDF_TARGET_NAME} as a Modbus serial master device, capable of reading and writing values from slave devices in a Modbus segment.

- `mb_tcp_slave <https://github.com/espressif/esp-modbus/tree/main/examples/tcp/mb_tcp_slave>`__ - demonstrates the esp-modbus TCP slave stack port, allowing an external Modbus host to read and write device parameters via the Modbus protocol.

- `mb_tcp_master <https://github.com/espressif/esp-modbus/tree/main/examples/tcp/mb_tcp_master>`__ - demonstrates how to use the esp-modbus stack port on {IDF_TARGET_NAME} as a Modbus TCP master device, capable of reading and writing values from slave devices in a Modbus network.

Please refer to the ``README.md`` documents of each specific example for details.

Discussions
~~~~~~~~~~~

* `Discussions for version v2 <https://github.com/espressif/esp-modbus/discussions>`__

ESP-MQTT
--------

Breaking change: ESP-MQTT moved to a managed component and example set updated.

- The ESP-MQTT component has been removed from ESP-IDF and is now a managed component: ``espressif/mqtt``.

  - To add the component to an application, run ``idf.py add-dependency espressif/mqtt``.
  - Include headers and APIs remain the same (``mqtt_client.h``), but the component is fetched via the Component Manager.

- Example changes in ESP-IDF:

  - Legacy MQTT TLS examples under ``examples/protocols/mqtt/ssl*`` were removed.
  - New reference examples are available:

    - :example:`protocols/mqtt`: MQTT over TLS.
    - :example:`protocols/mqtt5`: MQTT v5.0 over TLS.
