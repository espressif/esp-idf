从 ESP-IDF 中移出或弃用的组件
================================

:link_to_translation:`en:[English]`

移至 ESP-IDF Component Registry 的组件
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

以下组件已经从 ESP-IDF 中迁出至 `ESP-IDF Component Registry <https://components.espressif.com/>`_：

* `libsodium <https://components.espressif.com/component/espressif/libsodium>`_
* `cbor <https://components.espressif.com/component/espressif/cbor>`_
* `jsmn <https://components.espressif.com/component/espressif/jsmn>`_
* `esp_modem <https://components.espressif.com/component/espressif/esp_modem>`_
* `nghttp <https://components.espressif.com/component/espressif/nghttp>`_
* `mdns <https://components.espressif.com/component/espressif/mdns>`_
* `esp_websocket_client <https://components.espressif.com/component/espressif/esp_websocket_client>`_
* `asio <https://components.espressif.com/component/espressif/asio>`_
* `freemodbus <https://components.espressif.com/component/espressif/esp-modbus>`_
* `sh2lib <https://components.espressif.com/component/espressif/sh2lib>`_
* `expat <https://components.espressif.com/component/espressif/expat>`_
* `coap <https://components.espressif.com/component/espressif/coap>`_
* `esp-cryptoauthlib <https://components.espressif.com/component/espressif/esp-cryptoauthlib>`_
* `qrcode <https://components.espressif.com/component/espressif/qrcode>`_
* `tjpgd <https://components.espressif.com/component/espressif/esp_jpeg>`_
* `esp_serial_slave_link <https://components.espressif.com/components/espressif/esp_serial_slave_link>`_
* `tinyusb <https://components.espressif.com/components/espressif/esp_tinyusb>`_

.. note::

    请注意，http 解析功能以前属于 ``nghttp`` 组件一部分，但现在属于 :component:`http_parser <http_parser>` 组件。

可使用 ``idf.py add-dependency`` 命令安装以上组件。

例如，要安装 X.Y 版本的 libsodium 组件，请运行：``idf.py add-dependency libsodium==X.Y``。

根据 `semver <https://semver.org/>`_ 规则安装与 X.Y 兼容的最新版本 libsodium 组件，请运行 ``idf.py add-dependency libsodium~X.Y``。

可前往 https://components.espressif.com 查询每个组件有哪些版本，按名称搜索该组件，组件页面上会列出所有版本。

弃用的组件
^^^^^^^^^^^^^^^^^^^^^

ESP-IDF v4.x 版本中已不再使用以下组件，这些组件已弃用：

* ``tcpip_adapter``。可使用 :doc:`ESP-NETIF </api-reference/network/esp_netif>` 组件代替，具体可参考 :ref:`tcpip-adapter`。

.. note::

    不再支持 OpenSSL-API 组件。ESP-IDF Component Registry 中也没有该组件。请直接使用 :doc:`ESP-TLS </api-reference/protocols/esp_tls>` 或 :component:`mbedtls` API。

.. note::

    不再支持 ``esp_adc_cal`` 组件。 新的 adc 校准驱动在 ``esp_adc`` 组件中。旧版 adc 校准驱动已被迁移进 ``esp_adc`` 组件中。 要使用旧版 ``esp_adc_cal`` 驱动接口，你应该在 CMakeLists.txt 文件的组件依赖列表中增加 esp_adc。更多细节请查看 :doc:`Peripherals Migration Guide </migration-guides/release-5.x/5.0/peripherals>`。

版本更新后无需目标组件，因此以下目标组件也已经从 ESP-IDF 中删除：

 * ``esp32``
 * ``esp32s2``
 * ``esp32s3``
 * ``esp32c2``
 * ``esp32c3``
 * ``esp32h2``
