ESP32-WROOM-32SE (Secure Element)
=================================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

ESP32-WROOM-32SE integrates Microchip's ATECC608A cryptoauth chip in the module. ATECC608A is a secure element, which would generate and store ECC private key in the hardware. The ECC private key can be used to enhance security to connect to IoT cloud services with the use of X.509-based mutual authentication.

The application example demonstrates ECDSA sign and verify functions using ECC private key stored in ATECC608A.

Application Example
-------------------

Secure Element ECDSA Sign/Verify example: :example:`peripherals/secure_element/atecc608_ecdsa`.

How to Configure and Provision ESP32-WROOM-32SE for TLS
-------------------------------------------------------

To configure and provision ATECC608A chip on ESP32-WROOM-32SE please visit `esp_cryptoauth_utility <https://github.com/espressif/esp-cryptoauthlib/blob/master/esp_cryptoauth_utility/README.md#esp_cryptoauth_utility>`_.

How to Use ATECC608A of ESP32-WROOM-32SE for TLS
------------------------------------------------

ATECC608A can be used for TLS connections using ESP-TLS.

To configure ESP-TLS for using a secure element, please refer to **ATECC608A (Secure Element) with ESP-TLS** in :doc:`../protocols/esp_tls`.
