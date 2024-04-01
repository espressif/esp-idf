Security
========

{IDF_TARGET_CIPHER_SCHEME:default="RSA", esp32h2="RSA or ECDSA", esp32p4="RSA or ECDSA"}

{IDF_TARGET_SIG_PERI:default="DS", esp32h2="DS or ECDSA", esp32p4="DS or ECDSA"}

:link_to_translation:`zh_CN:[中文]`

This guide provides an overview of the overall security features available in various Espressif solutions. It is highly recommended to consider this guide while designing the products with the Espressif platform and the ESP-IDF software stack from the **security** perspective.

Goals
-----

High level security goals are as follows:

#. Preventing untrustworthy code from being executed
#. Protecting the identity and integrity of the code stored in the off-chip flash memory
#. Securing device identity
#. Secure storage for confidential data
#. Authenticated and encrypted communication from the device

Platform Security
-----------------

.. _secure_boot-guide:

Secure Boot
~~~~~~~~~~~

The Secure Boot feature ensures that only authenticated software can execute on the device. The Secure Boot process forms a chain of trust by verifying all **mutable** software entities involved in the :doc:`../api-guides/startup`. Signature verification happens during both boot-up as well as in OTA updates.

Please refer to :doc:`secure-boot-v2` for detailed documentation about this feature.

.. only:: esp32

    For ESP32 before ECO3, please refer to :doc:`secure-boot-v1`.

.. important::

    It is highly recommended that Secure Boot be enabled on all production devices.

Secure Boot Best Practices
^^^^^^^^^^^^^^^^^^^^^^^^^^

* Generate the signing key on a system with a quality source of entropy.
* Always keep the signing key private. A leak of this key will compromise the Secure Boot system.
* Do not allow any third party to observe any aspects of the key generation or signing process using ``espsecure.py``. Both processes are vulnerable to timing or other side-channel attacks.
* Ensure that all security eFuses have been correctly programmed, including disabling of the debug interfaces, non-required boot mediums (e.g., UART DL mode), etc.


.. _flash_enc-guide:

Flash Encryption
~~~~~~~~~~~~~~~~

The Flash Encryption feature helps to encrypt the contents on the off-chip flash memory and thus provides the **confidentiality** aspect to the software or data stored in the flash memory.

Please refer to :doc:`flash-encryption` for detailed information about this feature.

.. only:: SOC_SPIRAM_SUPPORTED and not esp32

    If {IDF_TARGET_NAME} is connected to an external SPI RAM, the contents written to or read from the SPI RAM will also be encrypted and decrypted respectively (via the MMU's flash cache, provided that FLash Encryption is enabled). This provides an additional safety layer for the data stored in SPI RAM, hence configurations like ``CONFIG_MBEDTLS_EXTERNAL_MEM_ALLOC`` can be safely enabled in this case.

Flash Encryption Best Practices
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* It is recommended to use flash Encryption release mode for the production use-cases.
* It is recommended to have a unique flash encryption key per device.
* Enable :ref:`secure_boot-guide` as an extra layer of protection, and to prevent an attacker from selectively corrupting any part of the flash before boot.


.. only:: SOC_DIG_SIGN_SUPPORTED

    Device Identity
    ~~~~~~~~~~~~~~~

    The Digital Signature peripheral in {IDF_TARGET_NAME} produces hardware-accelerated RSA digital signatures with the assistance of HMAC, without the RSA private key being accessible by software. This allows the private key to be kept secured on the device without anyone other than the device hardware being able to access it.

    .. only:: SOC_ECDSA_SUPPORTED

        {IDF_TARGET_NAME} also supports ECDSA peripheral for generating hardware-accelerated ECDSA digital signatures. ECDSA private key can be directly programmed in an eFuse block and marked as read protected from the software.

    {IDF_TARGET_SIG_PERI} peripheral can help to establish the **Secure Device Identity** to the remote endpoint, e.g., in the case of TLS mutual authentication based on the {IDF_TARGET_CIPHER_SCHEME} cipher scheme.

    .. only:: not SOC_ECDSA_SUPPORTED

        Please refer to the :doc:`../api-reference/peripherals/ds` for detailed documentation.

    .. only:: SOC_ECDSA_SUPPORTED

        Please refer to the :doc:`../api-reference/peripherals/ecdsa` and :doc:`../api-reference/peripherals/ds` guides for detailed documentation.

.. only:: SOC_MEMPROT_SUPPORTED or SOC_CPU_IDRAM_SPLIT_USING_PMP

    Memory Protection
    ~~~~~~~~~~~~~~~~~

    {IDF_TARGET_NAME} supports the **Memory Protection** scheme, either through architecture or special peripheral like PMS, which provides an ability to enforce and monitor permission attributes to memory and, in some cases, peripherals. ESP-IDF application startup code configures the permissions attributes like Read/Write access on data memories and Read/Execute access on instruction memories using the relevant peripheral. If there is any attempt made that breaks these permission attributes, e.g., a write operation to instruction memory region, then a violation interrupt is raised, and it results in system panic.

    This feature depends on the config option :ref:`CONFIG_ESP_SYSTEM_MEMPROT_FEATURE` and it is kept enabled by default. Please note that the API for this feature is **private** and used exclusively by ESP-IDF code only.

    .. note::

        This feature can help to prevent the possibility of remote code injection due to the existing vulnerabilities in the software.

.. only:: SOC_CRYPTO_DPA_PROTECTION_SUPPORTED

    DPA (Differential Power Analysis) Protection
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    {IDF_TARGET_NAME} has support for protection mechanisms against the Differential Power Analysis related security attacks. DPA protection dynamically adjusts the clock frequency of the crypto peripherals, thereby blurring the power consumption trajectory during its operation. Based on the configured DPA security level, the clock variation range changes. Please refer to the TRM for more details on this topic.

    :ref:`CONFIG_ESP_CRYPTO_DPA_PROTECTION_LEVEL` can help to select the DPA level. Higher level means better security, but it can also have an associated performance impact. By default, the lowest DPA level is kept enabled but it can be modified based on the security requirement.

    .. note::

        Please note that hardware :doc:`RNG <../api-reference/system/random>` must be enabled for DPA protection to work correctly.

Debug Interfaces
~~~~~~~~~~~~~~~~

JTAG
^^^^

.. list::

    - JTAG interface stays disabled if any of the security features are enabled. Please refer to :ref:`jtag-debugging-security-features` for more information.
    - JTAG interface can also be disabled in the absence of any other security features using :ref:`efuse_API`.
    :SOC_HMAC_SUPPORTED: - {IDF_TARGET_NAME} supports soft disabling the JTAG interface and it can be re-enabled by programming a secret key through HMAC. (:ref:`hmac_for_enabling_jtag`)

UART Download Mode
^^^^^^^^^^^^^^^^^^

.. only:: esp32

    For ESP32 ECO3 case, UART Download mode stays disabled if any of the security features are enabled in their release configuration. Alternatively, it can also be disabled by calling :cpp:func:`esp_efuse_disable_rom_download_mode` at runtime.

    .. important::

        If UART Download mode is disabled then ``esptool.py`` can not work on the device.

.. only:: SOC_SUPPORTS_SECURE_DL_MODE

    In {IDF_TARGET_NAME}, Secure UART Download mode gets activated if any of the security features are enabled.

    * Secure UART Download mode can also be enabled by calling :cpp:func:`esp_efuse_enable_rom_secure_download_mode`.
    * This mode does not allow any arbitrary code to execute if downloaded through the UART download mode.
    * It also limits the available commands in Download mode to update SPI config, e.g., changing baud rate, basic flash write, and the command to return a summary of currently enabled security features (``get_security_info``).
    * To disable Download Mode entirely, select the :ref:`CONFIG_SECURE_UART_ROM_DL_MODE` to the recommended option ``Permanently disable ROM Download Mode`` or call :cpp:func:`esp_efuse_disable_rom_download_mode` at runtime.

    .. important::

        In Secure UART Download mode, ``esptool.py`` can only work with the argument ``--no-stub``.

.. only:: SOC_WIFI_SUPPORTED

    Network Security
    ----------------

    Wi-Fi
    ~~~~~

    In addition to the traditional security methods (WEP/WPA-TKIP/WPA2-CCMP), Wi-Fi driver in ESP-IDF also supports additional state-of-the-art security protocols. Please refer to the :doc:`../api-guides/wifi-security` for detailed documentation.

    TLS (Transport Layer Security)
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    It is recommended to use TLS (Transport Layer Security) in all external communications (e.g., cloud communication, OTA updates) from the ESP device. ESP-IDF supports :doc:`../api-reference/protocols/mbedtls` as the official TLS stack.

    TLS is default integrated in :doc:`../api-reference/protocols/esp_http_client`, :doc:`../api-reference/protocols/esp_https_server` and several other components that ship with ESP-IDF.

    .. note::

        It is recommended to use the ESP-IDF protocol components in their default configuration, which has been ensured to be secure. Disabling of HTTPS and similar security-critical configurations should be avoided.

    ESP-TLS Abstraction
    ^^^^^^^^^^^^^^^^^^^

    ESP-IDF provides an abstraction layer for the most-used TLS functionalities. Hence, it is recommended that an application uses the API exposed by :doc:`../api-reference/protocols/esp_tls`.

    :ref:`esp_tls_server_verification` section highlights diverse ways in which the identity of server could be established on the device side.

    ESP Certificate Bundle
    ^^^^^^^^^^^^^^^^^^^^^^

    The :doc:`../api-reference/protocols/esp_crt_bundle` API provides an easy way to include a bundle of custom x509 root certificates for TLS server verification. The certificate bundle is the easiest way to verify the identity of almost all standard TLS servers.

    .. important::

        It is highly recommended to verify the identity of the server based on X.509 certificates to avoid establishing communication with the **fake** server.


    Managing Root Certificates
    ^^^^^^^^^^^^^^^^^^^^^^^^^^

    Root Certificates embedded inside the application must be managed carefully. Any update to the root certificate list or the :doc:`../api-reference/protocols/esp_crt_bundle` can have an impact on the TLS connection with the remote endpoint. This includes a connection to the OTA update server. In some cases, the problem shall be visible on the next OTA update and it may leave device unable to perform OTA updates forever.

    Root certificates list update could have following reasons:

    - New firmware has different set of remote endpoint(s).
    - The existing certificate has expired.
    - The certificate has been added or retracted from the upstream certificate bundle.
    - The certificate list changed due to market share statistics (``CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_DEFAULT_CMN`` case).

    Some guidelines to consider on this topic:

    - Please consider enabling :ref:`OTA rollback <ota_rollback>` and then keep the successful connection to the OTA update server as the checkpoint to cancel the rollback process. This ensures that the newly updated firmware can successfully reach till the OTA update server, otherwise rollback process will go back to the previous firmware on the device.
    - If you plan to enable the :ref:`CONFIG_MBEDTLS_HAVE_TIME_DATE` option, then please consider to have the time sync mechanism (SNTP) and sufficient number of trusted certificates in place.

Product Security
----------------

.. only:: SOC_WIFI_SUPPORTED

    Secure Provisioning
    ~~~~~~~~~~~~~~~~~~~

    Secure Provisioning refers to a process of secure on-boarding of the ESP device on to the Wi-Fi network. This mechanism also allows provision of additional custom configuration data during the initial provisioning phase from the provisioning entity, e.g., Smartphone.

    ESP-IDF provides various security schemes to establish a secure session between ESP and the provisioning entity, they are highlighted at :ref:`provisioning_security_schemes`.

    Please refer to the :doc:`../api-reference/provisioning/wifi_provisioning` documentation for details and the example code for this feature.

    .. note::

        Espressif provides Android and iOS Phone Apps along with their sources, so that it could be easy to further customize them as per the product requirement.

Secure OTA (Over-the-air) Updates
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- OTA Updates must happen over secure transport, e.g., HTTPS.
- ESP-IDF provides a simplified abstraction layer :doc:`../api-reference/system/esp_https_ota` for this.
- If :ref:`secure_boot-guide` is enabled, then the server should host the signed application image.
- If :ref:`flash_enc-guide` is enabled, then no additional steps are required on the server side, encryption shall be taken care on the device itself during flash write.
- OTA update :ref:`ota_rollback` can help to switch the application as ``active`` only after its functionality has been verified.


Anti-Rollback Protection
^^^^^^^^^^^^^^^^^^^^^^^^

Anti-rollback protection feature ensures that device only executes the application that meets the security version criteria as stored in its eFuse. So even though the application is trusted and signed by legitimate key, it may contain some revoked security feature or credential. Hence, device must reject any such application.

ESP-IDF allows this feature for the application only and it is managed through 2nd stage bootloader. The security version is stored in the device eFuse and it is compared against the application image header during both bootup and over-the-air updates.

Please see more information to enable this feature in the :ref:`anti-rollback` guide.

Encrypted Firmware Distribution
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Encrypted firmware distribution during over-the-air updates ensures that the application stays encrypted **in transit** from the server to the the device. This can act as an additional layer of protection on top of the TLS communication during OTA updates and protect the identity of the application.

Please see working example for this documented in :ref:`ota_updates_pre-encrypted-firmware` section.

Secure Storage
~~~~~~~~~~~~~~

Secure storage refers to the application-specific data that can be stored in a secure manner on the device, i.e., off-chip flash memory. This is typically a read-write flash partition and holds device specific configuration data, e.g., Wi-Fi credentials.

ESP-IDF provides the **NVS (Non-volatile Storage)** management component which allows encrypted data partitions. This feature is tied with the platform :ref:`flash_enc-guide` feature described earlier.

Please refer to the :ref:`nvs_encryption` for detailed documentation on the working and instructions to enable this feature.

.. important::

    By default, ESP-IDF components writes the device specific data into the default NVS partition, including Wi-Fi credentials too, and it is recommended to protect this data using **NVS Encryption** feature.

Secure Device Control
~~~~~~~~~~~~~~~~~~~~~

ESP-IDF provides capability to control an ESP device over ``Wi-Fi/Ethernet + HTTP`` or ``BLE`` in a secure manner using ESP Local Control component.

Please refer to the :doc:`../api-reference/protocols/esp_local_ctrl` for detailed documentation about this feature.

Security Policy
---------------

The ESP-IDF GitHub repository has attached `Security Policy Brief`_.

Advisories
~~~~~~~~~~

- Espressif publishes critical `Security Advisories`_, which includes security advisories regarding both hardware and software.
- The specific advisories of the ESP-IDF software components are published through the `GitHub repository`_.

Software Updates
~~~~~~~~~~~~~~~~

Critical security issues in the ESP-IDF components, and third-party libraries are fixed as and when we find them or when they are reported to us. Gradually, we make the fixes available in all applicable release branches in ESP-IDF.

Applicable security issues and CVEs for the ESP-IDF components, third-party libraries are mentioned in the ESP-IDF release notes.

.. important::

    We recommend periodically updating to the latest bugfix version of the ESP-IDF release to have all critical security fixes available.


.. _`Security Policy Brief`: https://github.com/espressif/esp-idf/blob/master/SECURITY.md
.. _`Security Advisories`: https://www.espressif.com/en/support/documents/advisories
.. _`GitHub repository`: https://github.com/espressif/esp-idf/security/advisories
