ESP32-WROOM-32SE (Secure Element)
=================================

Overview
--------

The ESP32-WROOM-32SE has integrated Microchip's ATECC608A cryptoauth chip in the module. ATECC608A is secure element
which would generate and store ECC private key in the hardware.The ECC private key can be used to enhance security
to connect to IoT cloud services with use of X.509 based mutual authentication.
The application example demonstrates ECDSA sign and verify functions using ECC private key stored in ATECC608A

Application Example
-------------------

Secure Element ECDSA Sign/Verify example: :example:`peripherals/secure_element/atecc608_ecdsa`.

How to configure and provision ESP32-WROOM-32SE for TLS
-------------------------------------------------------
To configure and provision ATECC608A chip on ESP32-WROOM-32SE please visit `esp_cryptoauth_utility <https://github.com/espressif/esp-cryptoauthlib/blob/master/esp_cryptoauth_utility/README.md#esp_cryptoauth_utility>`_

How to use ATECC608A of ESP32-WROOM-32SE for TLS
------------------------------------------------
ATECC608A can be used for TLS connections using ESP-TLS.
To configure ESP-TLS for using secure element please refer `ATECC608A with ESP-TLS` in :doc:`ESP-TLS documentation.<../protocols/esp_tls>`
