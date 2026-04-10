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

    - ``examples/protocols/mqtt``: MQTT over TLS.
    - ``examples/protocols/mqtt5``: MQTT v5.0 over TLS.
