Wi-Fi
======

:link_to_translation:`zh_CN:[中文]`


Breaking Changes
~~~~~~~~~~~~~~~~

The parameters for the ESP-NOW sending data callback function has changed from ``uint8_t *mac_addr`` to ``esp_now_send_info_t *tx_info``.
