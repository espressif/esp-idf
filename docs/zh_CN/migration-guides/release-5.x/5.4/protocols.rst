协议
=========

:link_to_translation:`en:[English]`

HTTPS 服务器
---------------

证书选择钩子
~~~~~~~~~~~~~~~~~~~~~~~~~~

在 ESP HTTPS 服务器中，如需启用证书选择钩子功能，现在需要启用 :ref:`CONFIG_ESP_HTTPS_SERVER_CERT_SELECT_HOOK`，而不是 :ref:`CONFIG_ESP_TLS_SERVER_CERT_SELECT_HOOK`。

新的 :ref:`CONFIG_ESP_HTTPS_SERVER_CERT_SELECT_HOOK` 选项会自动选择 :ref:`CONFIG_ESP_TLS_SERVER_CERT_SELECT_HOOK`。
