Protocols
=========

:link_to_translation:`zh_CN:[中文]`

.. _migration_guide_mbedtls:

Mbed TLS
--------

For ESP-IDF v5.0, `Mbed TLS <https://github.com/Mbed-TLS/mbedtls>`_ has been updated from v2.x to v3.1.0.

For more details about Mbed TLS's migration from version 2.x to version 3.0 or greater, please refer to the `official guide <https://github.com/espressif/mbedtls/blob/9bb5effc3298265f829878825d9bd38478e67514/docs/3.0-migration-guide.md>`__.

Breaking Changes (Summary)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Most Structure Fields Are Now Private
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Direct access to fields of structures (``struct`` types) declared in public headers is no longer supported.
- Appropriate accessor functions (getter/setter) must be used for the same. A temporary workaround would be to use ``MBEDTLS_PRIVATE`` macro (**not recommended**).
- For more details, refer to the `official guide <https://github.com/espressif/mbedtls/blob/9bb5effc3298265f829878825d9bd38478e67514/docs/3.0-migration-guide.md#most-structure-fields-are-now-private>`__.


SSL
^^^
- Removed support for TLS 1.0, 1.1, and DTLS 1.0
- Removed support for SSL 3.0

Deprecated Functions Were Removed from Cryptography Modules
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- The functions ``mbedtls_*_ret()`` (related to MD, SHA, RIPEMD, RNG, HMAC modules) was renamed to replace the corresponding functions without ``_ret`` appended and updated return value.
- For more details, refer to the `official guide <https://github.com/espressif/mbedtls/blob/9bb5effc3298265f829878825d9bd38478e67514/docs/3.0-migration-guide.md#deprecated-functions-were-removed-from-hashing-modules>`__.


Deprecated Config Options
^^^^^^^^^^^^^^^^^^^^^^^^^

Following are some of the important config options deprecated by this update. The configs related to and/or dependent on these have also been deprecated.

- ``MBEDTLS_SSL_PROTO_SSL3``  : Support for SSL 3.0
- ``MBEDTLS_SSL_PROTO_TLS1``  : Support for TLS 1.0
- ``MBEDTLS_SSL_PROTO_TLS1_1``: Support for TLS 1.1
- ``MBEDTLS_SSL_PROTO_DTLS``  : Support for DTLS 1.1 (Only DTLS 1.2 is supported now)
- ``MBEDTLS_DES_C``           : Support for 3DES ciphersuites
- ``MBEDTLS_RC4_MODE``        : Support for RC4-based ciphersuites

.. note::

  This list includes only major options configurable through ``idf.py menuconfig``. For more details on deprecated options, refer to the `official guide <https://github.com/espressif/mbedtls/blob/9bb5effc3298265f829878825d9bd38478e67514/docs/3.0-migration-guide.md#most-structure-fields-are-now-private>`__.


Miscellaneous
-------------

Disabled Diffie-Hellman Key Exchange Modes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Diffie-Hellman Key Exchange modes have now been disabled by default due to security risks (see warning text `here <https://github.com/espressif/mbedtls/blob/9bb5effc3298265f829878825d9bd38478e67514/include/mbedtls/dhm.h#L20>`__). Related configs are given below:

- ``MBEDTLS_DHM_C``                 : Support for the Diffie-Hellman-Merkle module
- ``MBEDTLS_KEY_EXCHANGE_DHE_PSK``  : Support for Diffie-Hellman PSK (pre-shared-key) TLS authentication modes
- ``MBEDTLS_KEY_EXCHANGE_DHE_RSA``  : Support for cipher suites with the prefix ``TLS-DHE-RSA-WITH-``

.. note::

  During the initial step of the handshake (i.e., ``client_hello``), the server selects a cipher from the list that the client publishes. As the DHE_PSK/DHE_RSA ciphers have now been disabled by the above change, the server would fall back to an alternative cipher; if in a rare case, it does not support any other cipher, the handshake would fail. To retrieve the list of ciphers supported by the server, one must attempt to connect with the server with a specific cipher from the client-side. Few utilities can help do this, e.g., ``sslscan``.

Remove ``certs`` Module from X509 Library
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- The ``mbedtls/certs.h`` header is no longer available in mbedtls 3.1. Most applications can safely remove it from the list of includes.

Breaking Change for ``esp_crt_bundle_set`` API
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- The :cpp:func:`esp_crt_bundle_set()` API now requires one additional argument named ``bundle_size``. The return type of the API has also been changed to :cpp:type:`esp_err_t` from ``void``.

Breaking Change for ``esp_ds_rsa_sign`` API
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- The :cpp:func:`esp_ds_rsa_sign()` API now requires one less argument. The argument ``mode`` is no longer required.

HTTPS Server
------------

Breaking Changes (Summary)
~~~~~~~~~~~~~~~~~~~~~~~~~~

Names of variables holding different certs in :cpp:type:`httpd_ssl_config_t` structure have been updated.

.. list::
    * :cpp:member:`httpd_ssl_config::servercert` variable inherits role of ``cacert_pem`` variable.
    * :cpp:member:`httpd_ssl_config::servercert_len` variable inherits role of ``cacert_len`` variable
    * :cpp:member:`httpd_ssl_config::cacert_pem` variable inherits role of ``client_verify_cert_pem`` variable
    * :cpp:member:`httpd_ssl_config::cacert_len` variable inherits role of ``client_verify_cert_len`` variable

The return type of the :cpp:func:`httpd_ssl_stop` API has been changed to :cpp:type:`esp_err_t` from ``void``.

ESP HTTPS OTA
--------------

Breaking Changes (Summary)
~~~~~~~~~~~~~~~~~~~~~~~~~~

- The function :cpp:func:`esp_https_ota` now requires pointer to :cpp:type:`esp_https_ota_config_t` as argument instead of pointer to :cpp:type:`esp_http_client_config_t`.


ESP-TLS
--------------

Breaking Changes (Summary)
~~~~~~~~~~~~~~~~~~~~~~~~~~

``esp_tls_t`` Structure Is Now Private
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The :cpp:type:`esp_tls_t` has now been made completely private. You cannot access its internal structures directly. Any necessary data that needs to be obtained from the ESP-TLS handle can be done through respective getter/setter functions. If there is a requirement of a specific getter/setter function, please raise an `issue <https://github.com/espressif/esp-idf/issues>`__ on ESP-IDF.


The list of newly added getter/setter function is as as follows:

.. list::

    * :cpp:func:`esp_tls_get_ssl_context` - Obtain the ssl context of the underlying ssl stack from the ESP-TLS handle.

Function Deprecations And Recommended Alternatives
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Following table summarizes the deprecated functions removed and their alternatives to be used from ESP-IDF v5.0 onwards.

.. list-table::
   :widths: 50 50
   :header-rows: 1

   * - Deprecated Function
     - Alternative
   * - ``esp_tls_conn_new()``
     - :cpp:func:`esp_tls_conn_new_sync`
   * - ``esp_tls_conn_delete()``
     - :cpp:func:`esp_tls_conn_destroy`

- The function :cpp:func:`esp_tls_conn_http_new` has now been termed as deprecated. Please use the alternative function :cpp:func:`esp_tls_conn_http_new_sync` (or its asynchronous :cpp:func:`esp_tls_conn_http_new_async`). Note that the alternatives need an additional parameter :cpp:type:`esp_tls_t`, which has to be initialized using the :cpp:func:`esp_tls_init` function.

HTTP Server
-----------

Breaking Changes (Summary)
~~~~~~~~~~~~~~~~~~~~~~~~~~

- ``http_server.h`` header is no longer available in ``esp_http_server``. Please use ``esp_http_server.h`` instead.

ESP HTTP Client
---------------

Breaking Changes (Summary)
~~~~~~~~~~~~~~~~~~~~~~~~~~

- The functions :cpp:func:`esp_http_client_read` and :cpp:func:`esp_http_client_fetch_headers` now return an additional return value ``-ESP_ERR_HTTP_EAGAIN`` for timeout errors - call timed-out before any data was ready.


TCP Transport
-------------

Breaking Changes (Summary)
~~~~~~~~~~~~~~~~~~~~~~~~~~

- The function :cpp:func:`esp_transport_read` now returns ``0`` for a connection timeout and ``< 0`` for other errors. Please refer :cpp:enum:`esp_tcp_transport_err_t` for all possible return values.


MQTT Client
-----------

Breaking Changes (Summary)
~~~~~~~~~~~~~~~~~~~~~~~~~~

- :cpp:type:`esp_mqtt_client_config_t` have all fields grouped in sub structs.

Most common configurations are listed below:

- Broker address now is set in :cpp:member:`esp_mqtt_client_config_t::broker::address::uri`
- Security related to broker verification in :cpp:member:`esp_mqtt_client_config_t::broker::verification`
- Client username is set in :cpp:member:`esp_mqtt_client_config_t::credentials::username`

- :cpp:type:`esp_mqtt_client_config_t` no longer supports the ``user_context`` field. Please use :cpp:func:`esp_mqtt_client_register_event` instead for registering an event handler; the last argument ``event_handler_arg`` can be used to pass user context to the handler.


ESP-Modbus
-----------

Breaking Changes (Summary)
~~~~~~~~~~~~~~~~~~~~~~~~~~

The ESP-IDF component ``freemodbus`` has been removed from ESP-IDF and is supported as a separate component. Additional information for the ``ESP-Modbus`` component can be found in the separate repository:

* `ESP-Modbus component on GitHub <https://www.github.com/espressif/esp-modbus>`__

The ``main`` component folder of the new application shall include the component manager manifest file ``idf_component.yml`` as in the example below:

.. code-block:: text

  dependencies:
    espressif/esp-modbus:
      version: "^1.0"

The ``esp-modbus`` component can be found in `component manager registry <https://components.espressif.com/component/espressif/esp-modbus>`__. Refer to `component manager documentation <https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/tools/idf-component-manager.html>`__ for more information on how to set up the component manager.

For applications targeting v4.x releases of ESP-IDF that need to use new ``esp-modbus`` component, adding the component manager manifest file ``idf_component.yml`` will be sufficient to pull in the new component. However, users should also exclude the legacy ``freemodbus`` component from the build. This can be achieved using the statement below in the project's ``CMakeLists.txt``:

.. code-block:: cmake

  set(EXCLUDE_COMPONENTS freemodbus)
