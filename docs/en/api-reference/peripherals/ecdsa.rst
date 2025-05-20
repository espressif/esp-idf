Elliptic Curve Digital Signature Algorithm (ECDSA)
==================================================

:link_to_translation:`zh_CN:[中文]`

The Elliptic Curve Digital Signature Algorithm (ECDSA) offers a variant of the Digital Signature Algorithm (DSA) which uses elliptic-curve cryptography.

{IDF_TARGET_NAME}'s ECDSA peripheral provides a secure and efficient environment for computing ECDSA signatures. It offers fast computations while ensuring the confidentiality of the signing process to prevent information leakage. ECDSA private key used in the signing process is accessible only to the hardware peripheral, and it is not readable by software.

ECDSA peripheral can help to establish **Secure Device Identity** for TLS mutual authentication and similar use-cases.

Supported Features
------------------

- ECDSA digital signature generation and verification
- Two different elliptic curves, namely P-192 and P-256 (FIPS 186-3 specification)
- Two hash algorithms for message hash in the ECDSA operation, namely SHA-224 and SHA-256 (FIPS PUB 180-4 specification)


ECDSA on {IDF_TARGET_NAME}
--------------------------

On {IDF_TARGET_NAME}, the ECDSA module works with a secret key burnt into an eFuse block. This eFuse key is made completely inaccessible (default mode) for any resources outside the cryptographic modules, thus avoiding key leakage.

ECDSA key can be programmed externally through ``idf.py`` script. Here is an example of how to program the ECDSA key:

.. code:: bash

   idf.py efuse-burn-key <BLOCK_NUM> </path/to/ecdsa_private_key.pem> ECDSA_KEY

.. only:: SOC_EFUSE_BLOCK9_KEY_PURPOSE_QUIRK

    .. note::

        Five physical eFuse blocks can be used as keys for the ECDSA module: block 4 ~ block 8. E.g., for block 4 (which is the first key block) , the argument should be ``BLOCK_KEY0``.

.. only:: not SOC_EFUSE_BLOCK9_KEY_PURPOSE_QUIRK

    .. note::

        Six physical eFuse blocks can be used as keys for the ECDSA module: block 4 ~ block 9. E.g., for block 4 (which is the first key block) , the argument should be ``BLOCK_KEY0``.


Alternatively the ECDSA key can also be programmed through the application running on the target.

Following code snippet uses :cpp:func:`esp_efuse_write_key` to set physical key block 0 in the eFuse with key purpose as :cpp:enumerator:`esp_efuse_purpose_t::ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY`:

.. code-block:: c

    #include "esp_efuse.h"

    const uint8_t key_data[32] = { ... };

    esp_err_t status = esp_efuse_write_key(EFUSE_BLK_KEY0,
                        ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY,
                        key_data, sizeof(key_data));

    if (status == ESP_OK) {
        // written key
    } else {
        // writing key failed, maybe written already
    }


.. only:: SOC_ECDSA_P192_CURVE_DEFAULT_DISABLED

    ECDSA Curve Configuration
    -------------------------

    .. only:: esp32h2

        The ECDSA peripheral of the ESP32-H2 supports both ECDSA-P192 and ECDSA-P256 operations. However, starting with ESP32-H2 revision 1.2, only ECDSA-P256 operations are enabled by default. You can enable ECDSA-P192 operations using the following configuration options:

    .. only:: not esp32h2

        The ECDSA peripheral of {IDF_TARGET_NAME} supports both ECDSA-P192 and ECDSA-P256 operations, but only ECDSA-P256 operations are enabled by default. You can enable ECDSA-P192 operations through the following configuration options:

    - :ref:`CONFIG_ESP_ECDSA_ENABLE_P192_CURVE` enables support for ECDSA-P192 curve operations, allowing the device to perform ECDSA operations with both 192-bit and 256-bit curves. However, if ECDSA-P192 operations have already been permanently disabled during eFuse write protection, enabling this option can not re-enable ECDSA-P192 curve operations.

    - :cpp:func:`esp_efuse_enable_ecdsa_p192_curve_mode()` enables ECDSA-P192 curve operations programmatically by writing the appropriate value to the eFuse, allowing both P-192 and P-256 curve operations. Note that this API will fail if the eFuse is already write-protected.

.. only:: SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE

    Deterministic Signature Generation
    -----------------------------------

    The ECDSA peripheral of {IDF_TARGET_NAME} also supports generation of deterministic signatures using deterministic derivation of the parameter K as specified in the `RFC 6979 <https://tools.ietf.org/html/rfc6979>`_ section 3.2.

Non-Determinisitic Signature Generation
---------------------------------------

Dependency on TRNG
^^^^^^^^^^^^^^^^^^

ECDSA peripheral relies on the hardware True Random Number Generator (TRNG) for its internal entropy requirement for generating non-deterministic signatures. During ECDSA signature creation, the algorithm requires a random integer to be generated as specified in the `RFC 6090 <https://tools.ietf.org/html/rfc6090>`_ section 5.3.2.

Please ensure that hardware :doc:`RNG <../system/random>` is enabled before starting ECDSA computations (primarily signing) in the application.

Application Outline
-------------------

Please refer to the :ref:`ecdsa-peri-with-esp-tls` guide for details on how-to use ECDSA peripheral for establishing a mutually authenticated TLS connection.

The ECDSA peripheral in Mbed TLS stack is integrated by overriding the ECDSA signing and verifying APIs. Please note that, the ECDSA peripheral does not support all curves or hash algorithms, and hence for cases where the hardware requirements are not met, the implementation falls back to the software.

For a particular TLS context, additional APIs have been supplied to populate certain fields (e.g., private key ctx) to differentiate routing to hardware. ESP-TLS layer integrates these APIs internally and hence no additional work is required at the application layer. However, for custom use-cases please refer to API details below.

API Reference
-------------

.. include-build-file:: inc/ecdsa_alt.inc
