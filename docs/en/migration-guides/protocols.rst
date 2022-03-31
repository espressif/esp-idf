Migration of Protocol Components to ESP-IDF 5.0
===============================================

.. _migration_guide_mbedtls:

Mbed TLS
--------

For ESP-IDF v5.0, `Mbed TLS <https://github.com/ARMmbed/mbedtls>`_ has been updated from v2.x to v3.1.0.

The official guide for Mbed TLS to migrate from version 2.x to version 3.0 or greater can be found `here <https://github.com/espressif/mbedtls/blob/9bb5effc3298265f829878825d9bd38478e67514/docs/3.0-migration-guide.md>`__.

Breaking Changes (Summary)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Most structure fields are now private
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Direct access to fields of structures (``struct`` types) declared in public headers is no longer supported.
- Appropriate accessor functions (getter/setter) must be used for the same. A temporary workaround would be to use ``MBEDTLS_PRIVATE`` macro (**not recommended**).
- For more details, refer to the official guide `here <https://github.com/espressif/mbedtls/blob/9bb5effc3298265f829878825d9bd38478e67514/docs/3.0-migration-guide.md#most-structure-fields-are-now-private>`__.

SSL
^^^
- Removed support for TLS 1.0, 1.1 and DTLS 1.0
- Removed support for SSL 3.0

Deprecated functions were removed from cryptography modules
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- The functions ``mbedtls_*_ret()`` (related to MD, SHA, RIPEMD, RNG, HMAC modules) was renamed to replace the corresponding functions without ``_ret`` appended and updated return value.
- For more details, refer to the official guide `here <https://github.com/espressif/mbedtls/blob/9bb5effc3298265f829878825d9bd38478e67514/docs/3.0-migration-guide.md#deprecated-functions-were-removed-from-hashing-modules>`__.


Deprecated Config Options
^^^^^^^^^^^^^^^^^^^^^^^^^
Following are some of the important config options deprecated by this update. The configs related to and/or dependent on these have also been deprecated.

- ``MBEDTLS_SSL_PROTO_SSL3``  : Support for SSL 3.0
- ``MBEDTLS_SSL_PROTO_TLS1``  : Support for TLS 1.0
- ``MBEDTLS_SSL_PROTO_TLS1_1``: Support for TLS 1.1
- ``MBEDTLS_SSL_PROTO_DTLS``  : Support for DTLS 1.1 (Only DTLS 1.2 is supported now)
- ``MBEDTLS_DES_C``           : Support for 3DES ciphersuites
- ``MBEDTLS_RC4_MODE``        : Support for RC4-based ciphersuites

.. note:: This list includes only major options configurable through ``idf.py menuconfig``. For more details on deprecated options, refer to the official migration guide.


Miscellaneous
-------------

Disabled Diffie-Hellman Key Exchange modes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Diffie-Hellman Key Exchange modes have now been disabled by default due to security risks (see warning text `here <https://github.com/espressif/mbedtls/blob/9bb5effc3298265f829878825d9bd38478e67514/include/mbedtls/dhm.h#L20>`__). Related configs are given below:

- ``MBEDTLS_DHM_C``                 : Support for the Diffie-Hellman-Merkle module
- ``MBEDTLS_KEY_EXCHANGE_DHE_PSK``  : Support for Diffie-Hellman PSK (pre-shared-key) TLS authentication modes
- ``MBEDTLS_KEY_EXCHANGE_DHE_RSA``  : Support for cipher suites with the prefix ``TLS-DHE-RSA-WITH-``

.. note:: During the initial step of the handshake (i.e. ``client_hello``), the server selects a cipher from the list that the client publishes. As the DHE_PSK/DHE_RSA ciphers have now been disabled by the above change, the server would fall back to an alternative cipher; if in a rare case, it does not support any other cipher, the handshake would fail. To retrieve the list of ciphers supported by the server, one must attempt to connect with the server with a specific cipher from the client-side. Few utilities can help do this, e.g. ``sslscan``.

Remove certs module from X509 library
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
`mbedtls/certs.h` header is no longer available in mbedtls 3.1, most applications can safely remove it from the list of includes.

Breaking change for "esp_crt_bundle_set" API
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
:cpp:func:`esp_crt_bundle_set()` API now requires one additional argument named ``bundle_size``. The return type of the API has also been changed to :cpp:type:`esp_err_t` from ``void``.

ESP HTTPS SERVER
-----------------

Breaking Changes (Summary)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Names of variables holding different certs in :cpp:type:`httpd_ssl_config_t` structure have been updated.

.. list::
    * :cpp:member:`httpd_ssl_config::servercert` variable inherits role of `cacert_pem` variable.
    * :cpp:member:`httpd_ssl_config::servercert_len` variable inherits role of `cacert_len` variable
    * :cpp:member:`httpd_ssl_config::cacert_pem` variable inherits role of `client_verify_cert_pem` variable
    * :cpp:member:`httpd_ssl_config::cacert_len` variable inherits role of `client_verify_cert_len` variable


ESP HTTPS OTA
--------------

Breaking Changes (Summary)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- The function :cpp:func:`esp_https_ota()` now requires pointer to :cpp:type:`esp_https_ota_config_t` as argument instead of pointer to :cpp:type:`esp_http_client_config_t`.
