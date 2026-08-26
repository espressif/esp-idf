Protocols
=========

:link_to_translation:`zh_CN:[中文]`

HTTPS Server
------------

Certificate Selection Hook
~~~~~~~~~~~~~~~~~~~~~~~~~~

In order to enable the Certificate Selection hook feature in ESP HTTPS Server, now you need to enable :menuitem:`CONFIG_ESP_HTTPS_SERVER_CERT_SELECT_HOOK` instead of :menuitem:`CONFIG_ESP_TLS_SERVER_CERT_SELECT_HOOK`.

The new :menuitem:`CONFIG_ESP_HTTPS_SERVER_CERT_SELECT_HOOK` option automatically selects :menuitem:`CONFIG_ESP_TLS_SERVER_CERT_SELECT_HOOK`.
