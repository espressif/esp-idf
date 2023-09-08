ESP32-WROOM-32SE（安全元件）
=================================

:link_to_translation:`en:[English]`

概述
--------

ESP32-WROOM-32SE 模组集成了 Microchip 的 ATECC608A 加密认证芯片。ATECC608A 安全元件可以在硬件中生成并存储 ECC 私钥。通过使用基于 X.509 的双向认证，ECC 私钥可增强设备与物联网云服务连接的安全性。

有关如何使用存储在 ATECC608A 中的 ECC 私钥进行 ECDSA 签名和验证，请参阅以下应用示例。

应用示例
-------------------

请参阅安全元件 ECDSA 签名/验证示例 :example:`peripherals/secure_element/atecc608_ecdsa`。

对 ESP32-WROOM-32SE 配置并配网以进行 TLS 连接
-------------------------------------------------------

要在 ESP32-WROOM-32SE 上配置 ATECC608A 芯片，并对其进行配网，请参阅 `esp_cryptoauth_utility <https://github.com/espressif/esp-cryptoauthlib/blob/master/esp_cryptoauth_utility/README.md#esp_cryptoauth_utility>`_。

使用 ESP32-WROOM-32SE 的 ATECC608A 进行 TLS 连接
------------------------------------------------------

通过使用 ESP-TLS，ATECC608A 可实现 TLS 连接。

要配置 ESP-TLS 以使用安全元件，请参阅 :doc:`../protocols/esp_tls` 中的 **ESP-TLS 中的 ATECC608A（安全元件）** 一节。
