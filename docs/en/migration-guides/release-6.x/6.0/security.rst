Security
========

:link_to_translation:`zh_CN:[中文]`

Mbed TLS
--------

Starting from **ESP-IDF v6.0**, some already deprecated mbedtls header files like ``esp32/aes.h``, ``esp32/sha.h``, ``esp32s2/aes.h``, ``esp32s2/sha.h`` and ``esp32s2/gcm.h`` have been removed, instead, you should include ``aes/esp_aes.h``, ``sha/sha_core.h`` and ``aes/esp_aes_gcm.h`` respectively.

.. only:: SOC_SHA_SUPPORTED

    The SHA module headers ``sha/sha_dma.h`` and ``sha/sha_block.h`` are also deprecated and removed. You should include ``sha/sha_core.h`` instead.

PSA Crypto migration
~~~~~~~~~~~~~~~~~~~~

In ESP-IDF v6.0, multiple ESP-IDF components have been migrated from using legacy Mbed TLS cryptography APIs (for example, ``mbedtls_sha*_*()``, ``mbedtls_md*_*()``, etc.) to using the `PSA Crypto API <https://arm-software.github.io/psa-api/>`__.

This migration aligns ESP-IDF with Mbed TLS v4.x, where PSA Crypto is the primary cryptography interface, and it enables the use of ESP-IDF hardware acceleration through PSA drivers where available.

Mbed TLS v4.0 migration
^^^^^^^^^^^^^^^^^^^^^^^^

ESP-IDF v6.0 updates to Mbed TLS v4.0, where **PSA Crypto is the primary cryptography interface** (TF-PSA-Crypto provides cryptography; Mbed TLS focuses on TLS and X.509). This can impact applications directly using Mbed TLS cryptography primitives, TLS configuration, or Mbed TLS internal/private declarations.

- **Breaking change**: In Mbed TLS v4.0, **most legacy cryptography APIs have been removed** and PSA Crypto is the primary interface. If your application directly uses legacy ``mbedtls_*`` cryptography primitives, you may need to migrate to PSA Crypto APIs.
- **Breaking change**: ``psa_crypto_init()`` must be called before any cryptographic operation, including indirect operations such as parsing keys/certificates or starting a TLS handshake. ESP-IDF initializes PSA during normal startup; however, code that runs earlier than the normal startup sequence must call ``psa_crypto_init()`` explicitly.
- **Breaking change**: ``esp_ecdsa_load_pubkey()``, ``esp_ecdsa_privkey_load_mpi()``, ``esp_ecdsa_privkey_load_pk_context()``, ``esp_ecdsa_set_pk_context()``, ``esp_ecdsa_tee_load_pubkey()`` and ``esp_ecdsa_tee_set_pk_context()`` have been deprecated and removed. The legacy ``esp_ecdsa_pk_conf_t`` configuration is replaced by ``esp_ecdsa_opaque_key_t``, which represents an hardware-backed ECDSA opaque key. Applications can populate this structure and import it via ``psa_import_key()``, enabling all ECDSA operations through standard PSA Crypto APIs.
- **Breaking change**: APIs that previously required an application-provided RNG callback (``f_rng``, ``p_rng``) have changed in Mbed TLS v4.0 to use the PSA RNG instead. Update application code to the new prototypes (for example X.509 write APIs, SSL cookie setup, and SSL ticket setup).
- **Breaking change**: TLS 1.2 / DTLS 1.2 interoperability may be affected because Mbed TLS v4.0 removes support for key exchanges based on finite-field DHE and RSA key exchange without forward secrecy (and static ECDH). If a peer requires removed suites, TLS connections may fail; update server/client cipher suite configuration accordingly.
- **Breaking change**: certificates/peers using elliptic curves of less than 250 bits (for example secp192r1/secp224r1) are no longer supported in certificates and in TLS.
- **Note**:

  - void relying on Mbed TLS private declarations (for example headers under ``mbedtls/private/`` or declarations enabled via ``MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS`` / ``MBEDTLS_ALLOW_PRIVATE_ACCESS``). Such private interfaces may change without notice.
  - The PSA Crypto migration (TF-PSA-Crypto) can increase flash footprint, depending on the features enabled. As reference points:

    .. list-table::
       :header-rows: 1
       :widths: 30 15 15 15 10

       * - Example
         - non PSA build (bytes)
         - PSA migration (bytes)
         - Diff (bytes)
         - Diff (%)
       * - :example:`protocols/esp_http_client`
         - 609041
         - 646293
         - 37252
         - 5.76
       * - :example:`protocols/https_server`
         - 871021
         - 898717
         - 27696
         - 3.08
       * - :example:`protocols/http_server/simple`
         - 785825
         - 826909
         - 41084
         - 4.97



Default configuration changes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Note: The default Mbed TLS configuration in ESP-IDF v6.0 has been tightened for security and footprint:

  - ``MBEDTLS_ARIA_C`` is disabled by default. Applications that rely on ARIA must explicitly enable it in ``menuconfig`` (Component config -> mbedTLS) or by customizing ``components/mbedtls/config/mbedtls_preset_default.conf``.
  - Support for ``secp192r1`` is disabled by default, consistent with the removal of support for elliptic curves smaller than 250 bits in certificates and TLS. If an application still requires legacy curve support outside TLS/certificates, it must be enabled explicitly (for example by defining ``PSA_WANT_ECC_SECP_R1_192=1``) and validated for compatibility. Note: this legacy support may be disabled in the next minor ESP-IDF release.

References
^^^^^^^^^^

- :idf_file:`Mbed TLS 4.0 migration guide <components/mbedtls/mbedtls/docs/4.0-migration-guide.md>`
- :idf_file:`TF-PSA-Crypto 1.0 migration guide <components/mbedtls/mbedtls/tf-psa-crypto/docs/1.0-migration-guide.md>`
- :idf_file:`TF-PSA-Crypto PSA transition notes <components/mbedtls/mbedtls/tf-psa-crypto/docs/psa-transition.md>`

Upstream Mbed TLS PSA notes
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Some data structures and internals that applications may have accessed previously are no longer available when using PSA-backed Mbed TLS versions. If your application relied on direct access to Mbed TLS internal state (for example entropy/DRBG contexts as struct fields), migrate to supported public APIs instead.

PSA persistent storage (ITS) on ESP-IDF
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ESP-IDF provides an implementation of PSA Internal Trusted Storage (ITS) backed by NVS, so PSA persistent storage can be used without a filesystem. If your application uses PSA persistent keys/storage, ensure that NVS is available and initialized before first use.

Removed deprecated APIs (Mbed TLS / crypto)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The following deprecated functions have been removed:

- :cpp:func:`esp_aes_encrypt` – Use :cpp:func:`esp_internal_aes_encrypt` instead.
- :cpp:func:`esp_aes_decrypt` – Use :cpp:func:`esp_internal_aes_decrypt` instead.
- :cpp:func:`esp_crypto_shared_gdma_start` – Use :cpp:func:`esp_crypto_shared_gdma_start_axi_ahb` instead.

Note that the new AES functions return error codes for better error handling, unlike the old void functions.

BluFi
-----

BluFi (Wi-Fi provisioning over BLE) is affected by the Mbed TLS v4.x / PSA Crypto migration in ESP-IDF v6.0.

- **Breaking change**: The BluFi protocol version has been updated (``BTC_BluFi_SUB_VER`` bumped from ``0x03`` to ``0x04``). The BluFi security negotiation implementation in ESP-IDF has been migrated to use PSA Crypto (see the updated ``examples/bluetooth/blufi`` example).

- **Impact**: Existing BluFi client applications (for example, mobile apps) built against the previous BluFi crypto/protocol implementation may no longer interoperate with devices built with ESP-IDF v6.0. This typically shows up as BluFi negotiation or connection failures during provisioning.

- **Required action**: Update both sides together:

  - Update the device firmware to ESP-IDF v6.0.
  - Update the BluFi client application to a version compatible with the updated BluFi protocol and security negotiation used by ESP-IDF v6.0.

Bootloader Support
------------------

**Removed Deprecated APIs**

The following deprecated functions have been removed:

- :cpp:func:`esp_secure_boot_verify_signature_block` – Use :cpp:func:`esp_secure_boot_verify_ecdsa_signature_block` instead.

.. only:: SOC_HMAC_SUPPORTED

    NVS Security Provider
    ---------------------

    - When NVS encryption is enabled on SoCs with the HMAC peripheral that have flash encryption enabled, the HMAC-based NVS encryption scheme is now selected as default instead of the flash encryption-based scheme. If your application previously used the flash encryption-based scheme, you need to manually configure the NVS encryption scheme to flash encryption from HMAC through ``menuconfig`` or your project's ``sdkconfig`` (i.e., setting ``CONFIG_NVS_SEC_KEY_PROTECT_USING_FLASH_ENC=y``).
