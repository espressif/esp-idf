Mbed TLS
========

:link_to_translation:`zh_CN:[中文]`

`Mbed TLS <https://github.com/Mbed-TLS/mbedtls>`_ is a C library that implements cryptographic primitives, X.509 certificate manipulation and the SSL/TLS and DTLS protocols. Its small code footprint makes it suitable for embedded systems.

.. note::

    ESP-IDF uses a `fork <https://github.com/espressif/mbedtls>`_ of Mbed TLS which includes a few patches (related to hardware routines of certain modules like ``bignum (MPI)`` and ``ECC``) over vanilla Mbed TLS.

Mbed TLS supports TLS 1.2, TLS 1.3 and DTLS 1.2 communication by providing the following:

- TCP/IP communication functions: listen, connect, accept, read/write.
- SSL/TLS communication functions: init, handshake, read/write.
- X.509 functions: CRT, CRL and key handling
- Random number generation
- Hashing
- Encryption/decryption

.. note::

    Mbed TLS v3.x.x series supports only TLS 1.2 and TLS 1.3 protocols. Support for SSL 3.0, TLS 1.0/1.1 and DTLS 1.0 has been removed (deprecated). TLS 1.3 is fully supported starting Mbed TLS v3.6.0 release, before this release some features were still in experimental state. Please refer to :component_file:`Mbed TLS ChangeLog <mbedtls/mbedtls/ChangeLog>` for more details.

Mbed TLS Documentation
----------------------

For Mbed TLS documentation please refer to the following (upstream) pointers:

- `API Reference`_
- `Knowledge Base`_

Mbed TLS Support in ESP-IDF
---------------------------

Please find the information about the Mbed TLS versions presented in different branches of ESP-IDF `here <https://github.com/espressif/mbedtls/wiki#mbed-tls-support-in-esp-idf>`__.

.. note::

    Please refer the :ref:`migration_guide_mbedtls` to migrate from Mbed TLS version 2.x to version 3.0 or greater.

Configuration Presets
^^^^^^^^^^^^^^^^^^^^^^

ESP-IDF provides a preset-based configuration system for Mbed TLS to simplify setup and provide optimized starting points for different use cases. This system works alongside the existing manual configuration system and provides baseline configurations that can be further customized through menuconfig or additional configuration files.

.. list-table::
    :header-rows: 1
    :widths: 15 25 35
    :align: center

    * - Preset
      - Use Case
      - Key Features
    * - **Default**
      - General purpose applications
      - • TLS 1.2 & 1.3 support
        • Certificate bundle enabled
        • Hardware acceleration
        • Full cipher suite support
    * - **Minimal**
      - Resource-constrained applications
      - • TLS 1.2 client only
        • RSA & PSK key exchange
        • AES-128 CBC/CTR modes
        • Basic X.509 parsing
    * - **Bluetooth (BT)**
      - Bluetooth applications
      - • Optimized for Bluetooth LE security
        • ECC P-256 curve support
        • Minimal TLS overhead
        • Bluetooth-specific algorithms

Using Configuration Presets
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Presets serve as **starting points** for your mbedTLS configuration. You can use them as-is or customize them further using standard ESP-IDF configuration methods.

To use a preset configuration, add the following line to your project's ``CMakeLists.txt`` file **before** the ``project()`` call:

.. code-block:: cmake

    # Include the default preset (recommended for most applications)
    list(APPEND sdkconfig_defaults $ENV{IDF_PATH}/components/mbedtls/config/mbedtls_preset_default.conf)

    # Or for resource-constrained applications
    list(APPEND sdkconfig_defaults $ENV{IDF_PATH}/components/mbedtls/config/mbedtls_preset_minimal.conf)

    # Or for Bluetooth applications
    list(APPEND sdkconfig_defaults $ENV{IDF_PATH}/components/mbedtls/config/mbedtls_preset_bt.conf)

    # Standard ESP-IDF project setup
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    project(my_project)

.. note::

    The preset configurations are located in ``components/mbedtls/config/`` and can be customized or used as a starting point for your own configurations.

Customizing Preset Configurations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

After applying a preset, you can further customize the configuration using any of these methods:

**Method 1: Using menuconfig (Recommended)**

.. code-block:: bash

    # After applying a preset in CMakeLists.txt
    idf.py menuconfig

Navigate to ``Component Config`` > ``mbedTLS`` to modify any settings. Your changes will override the preset defaults.

**Method 2: Additional Configuration Files**

You can combine a preset with your own custom configuration by creating an additional configuration file:

.. code-block:: cmake

    # Use the minimal preset as a base, then add custom settings
    list(APPEND SDKCONFIG_DEFAULTS
        $ENV{IDF_PATH}/components/mbedtls/config/mbedtls_preset_minimal.conf
        ${CMAKE_CURRENT_SOURCE_DIR}/my_custom_mbedtls.conf
    )


Migration from Manual Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The preset system complements manual configuration. If you have an existing manually configured mbedTLS setup:

**Option 1: Keep Your Existing Configuration**

Your current manual configuration will continue to work without any changes.

**Option 2: Migrate to Preset + Customization**

1. **Choose a base preset** that's closest to your current configuration.
2. **Apply the preset** in your ``CMakeLists.txt``.
3. **Use menuconfig** to adjust settings to match your requirements.
4. **Test thoroughly** to ensure functionality is maintained.

Configuration Categories
^^^^^^^^^^^^^^^^^^^^^^^^

The new mbedTLS configuration system is organized into logical categories for easier navigation:

**Core Configuration**
    Basic mbedTLS settings including memory allocation, threading, and debug options.

**TLS Protocol Configuration**
    TLS/DTLS protocol versions, modes (client/server), and protocol-specific features.

**Symmetric Ciphers**
    Block ciphers (AES, ARIA, etc.), cipher modes (CBC, GCM, etc.), and symmetric cryptography.

**Asymmetric Ciphers**
    RSA, ECC, and other public key cryptography algorithms.

**Hash Functions**
    Message digest algorithms (SHA-256, SHA-512, etc.) and HMAC.

**Hardware Acceleration**
    ESP32-specific hardware acceleration for cryptographic operations.

**Certificate Support**
    X.509 certificate parsing, validation, and certificate bundle management.


Application Examples
--------------------

Examples in ESP-IDF use :doc:`/api-reference/protocols/esp_tls` which provides a simplified API interface for accessing the commonly used TLS functionality.

Refer to the examples :example:`protocols/https_server/simple` (simple HTTPS server) and :example:`protocols/https_request` (make HTTPS requests) for more information.

If you plan to use the Mbed TLS API directly, refer to the example :example:`protocols/https_mbedtls`. This example demonstrates how to establish an HTTPS connection using Mbed TLS by setting up a secure socket with a certificate bundle for verification.


Important Config Options
------------------------

The Mbed TLS configuration system supports preset configurations. Following is a brief list of important config options accessible at ``Component Config`` > ``mbedTLS``. The full list of config options can be found :ref:`here <CONFIG_MBEDTLS_MEM_ALLOC_MODE>`.

**Core Configuration:**

.. list::

    :SOC_SHA_SUPPORTED: - :ref:`CONFIG_MBEDTLS_HARDWARE_SHA`: Support for hardware SHA acceleration
    :SOC_AES_SUPPORTED: - :ref:`CONFIG_MBEDTLS_HARDWARE_AES`: Support for hardware AES acceleration
    :SOC_MPI_SUPPORTED: - :ref:`CONFIG_MBEDTLS_HARDWARE_MPI`: Support for hardware MPI (bignum) acceleration
    :SOC_ECC_SUPPORTED: - :ref:`CONFIG_MBEDTLS_HARDWARE_ECC`: Support for hardware ECC acceleration
    - :ref:`CONFIG_MBEDTLS_MEM_ALLOC_MODE`: Memory allocation strategy (Internal/External/Custom)
    - :ref:`CONFIG_MBEDTLS_ASYMMETRIC_CONTENT_LEN`: Asymmetric in/out fragment length for memory optimization
    - :ref:`CONFIG_MBEDTLS_DYNAMIC_BUFFER`: Enable dynamic TX/RX buffer allocation
    - :ref:`CONFIG_MBEDTLS_DEBUG`: Enable mbedTLS debugging (useful for debugging)

**TLS Protocol Configuration:**

.. list::

    - :ref:`CONFIG_MBEDTLS_TLS_ENABLED`: Enable TLS protocol support
    - :ref:`CONFIG_MBEDTLS_SSL_PROTO_TLS1_2`: Support for TLS 1.2 (recommended)
    - :ref:`CONFIG_MBEDTLS_SSL_PROTO_TLS1_3`: Support for TLS 1.3 (latest standard)
    - :ref:`CONFIG_MBEDTLS_SSL_PROTO_DTLS`: Support for DTLS (UDP-based TLS)
    - :ref:`CONFIG_MBEDTLS_CLIENT_SSL_SESSION_TICKETS`: Support for TLS Session Resumption (client session tickets)
    - :ref:`CONFIG_MBEDTLS_SERVER_SSL_SESSION_TICKETS`: Support for TLS Session Resumption Server session tickets
    - :ref:`CONFIG_MBEDTLS_SSL_ALPN`: Support for Application Layer Protocol Negotiation
    - :ref:`CONFIG_MBEDTLS_SSL_SERVER_NAME_INDICATION`: Support for Server Name Indication (SNI)

**Certificate Support:**

.. list::

    - :ref:`CONFIG_MBEDTLS_CERTIFICATE_BUNDLE`: Support for trusted root certificate bundle (more about this: :doc:`/api-reference/protocols/esp_crt_bundle`)
    - :ref:`CONFIG_MBEDTLS_X509_USE_C`: Enable X.509 certificate support
    - :ref:`CONFIG_MBEDTLS_PEM_PARSE_C`: Read & Parse PEM formatted certificates
    - :ref:`CONFIG_MBEDTLS_PEM_WRITE_C`: Write PEM formatted certificates
    - :ref:`CONFIG_MBEDTLS_X509_CRT_PARSE_C`: Parse X.509 certificates
    - :ref:`CONFIG_MBEDTLS_X509_CRL_PARSE_C`: Parse X.509 certificate revocation lists

**Cryptographic Algorithms:**

.. list::

    - :ref:`CONFIG_MBEDTLS_AES_C`: AES block cipher support
    - :ref:`CONFIG_MBEDTLS_RSA_C`: RSA public key cryptosystem
    - :ref:`CONFIG_MBEDTLS_ECP_C`: Elliptic Curve Cryptography support
    - :ref:`CONFIG_MBEDTLS_ECDSA_C`: Elliptic Curve Digital Signature Algorithm
    - :ref:`CONFIG_MBEDTLS_ECDH_C`: Elliptic Curve Diffie-Hellman key exchange
    - :ref:`CONFIG_MBEDTLS_SHA256_C`: SHA-256 hash function
    - :ref:`CONFIG_MBEDTLS_SHA512_C`: SHA-512 hash function
    - :ref:`CONFIG_MBEDTLS_GCM_C`: Galois/Counter Mode for authenticated encryption

.. note::

    The new configuration structure provides better organization with categories like "Core Configuration", "TLS Protocol Configuration", "Symmetric Ciphers", "Asymmetric Ciphers", "Hash Functions", and "Hardware Acceleration" for easier navigation and configuration management.

Debugging mbedTLS
^^^^^^^^^^^^^^^^^

To enable debugging, add these configurations:

.. code-block:: kconfig

    CONFIG_MBEDTLS_DEBUG=y
    CONFIG_MBEDTLS_DEBUG_LEVEL=3
    CONFIG_LOG_DEFAULT_LEVEL_DEBUG=y

Performance Optimization
^^^^^^^^^^^^^^^^^^^^^^^^

For optimal performance, **enable hardware acceleration** when available:

.. code-block:: kconfig

    CONFIG_MBEDTLS_HARDWARE_AES=y
    CONFIG_MBEDTLS_HARDWARE_SHA=y
    CONFIG_MBEDTLS_HARDWARE_MPI=y
    CONFIG_MBEDTLS_HARDWARE_ECC=y

Performance and Memory Tweaks
-----------------------------

.. _reducing_ram_usage_mbedtls:

Reducing Heap Usage
^^^^^^^^^^^^^^^^^^^

The following table shows typical memory usage with different configs when the :example:`protocols/https_request` example (with Server Validation enabled) is run with Mbed TLS as the SSL/TLS library.

.. list-table::
    :header-rows: 1
    :widths: 25 60 30
    :align: center

    * - Mbed TLS Test
      - Related Configs
      - Heap Usage (approx.)
    * - Default
      - NA
      - 42196 B
    * - Enable SSL Dynamic Buffer Length
      - :ref:`CONFIG_MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH`
      -  42120 B
    * - Disable Keep Peer Certificate
      - :ref:`CONFIG_MBEDTLS_SSL_KEEP_PEER_CERTIFICATE`
      - 38533 B
    * - Enable Dynamic TX/RX Buffer
      - :ref:`CONFIG_MBEDTLS_DYNAMIC_BUFFER`
        :ref:`CONFIG_MBEDTLS_DYNAMIC_FREE_CONFIG_DATA`
        :ref:`CONFIG_MBEDTLS_DYNAMIC_FREE_CA_CERT`
      - 22013 B

.. note::

    These values are subject to change with changes in configuration options and versions of Mbed TLS.


Reducing Binary Size
^^^^^^^^^^^^^^^^^^^^

Under ``Component Config`` > ``mbedTLS``, several Mbed TLS features are enabled by default. These can be disabled if not needed to save code size. More information is available in the :ref:`Minimizing Binary Size <minimizing_binary_mbedtls>` documentation.


.. _`API Reference`: https://mbed-tls.readthedocs.io/projects/api/en/v3.6.5/
.. _`Knowledge Base`: https://mbed-tls.readthedocs.io/en/latest/kb/
