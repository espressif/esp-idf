Encrypted Advertising Data (EAD)
================================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

Encrypted Advertising Data (EAD) was introduced in Bluetooth Core Specification 5.4. It allows a device to encrypt one or more advertising structures with AES-CCM, so that only peers that hold the corresponding session key and IV can recover the plaintext.

The Bluedroid host exposes this as a pair of synchronous APIs in ``esp_ble_ead.h``. Encryption and decryption are performed in the host and do not require a controller feature bit.

These APIs are compiled when ``CONFIG_BT_BLE_FEAT_ENC_ADV_DATA`` is enabled.

The GAP Key Material characteristic (UUID 0x2B88, ``CONFIG_BT_GATTS_KEY_MATERIAL_CHAR``) is the standard way for a peripheral to publish the session key and IV. Enabling that option also selects the EAD APIs. Call :cpp:func:`esp_ble_gap_set_key_material` to set the value so a peer can read it over an encrypted GATT connection, then decrypt with :cpp:func:`esp_ble_ead_decrypt`.

A central that already has a pre-shared key only needs ``CONFIG_BT_BLE_FEAT_ENC_ADV_DATA``.

Application Examples
--------------------

- :example:`bluetooth/bluedroid/ble/ble_enc_adv_data/enc_adv_data_prph` demonstrates encrypting advertising data and exposing Key Material through the GAP service.

- :example:`bluetooth/bluedroid/ble/ble_enc_adv_data/enc_adv_data_cent` demonstrates scanning for encrypted advertising data and decrypting it after reading, or using, the Key Material.

In menuconfig, see ``Bluedroid Options`` > ``Encrypted Advertising Data (EAD)``.

API Reference
-------------

.. include-build-file:: inc/esp_ble_ead.inc
