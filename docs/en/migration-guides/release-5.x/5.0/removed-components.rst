Removed or Deprecated Components
================================

:link_to_translation:`zh_CN:[中文]`

Components Moved to ESP-IDF Component Registry
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Following components are removed from ESP-IDF and moved to `ESP-IDF Component Registry <https://components.espressif.com/>`_:

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

    Please note that http parser functionality which was previously part of ``nghttp`` component is now part of :component:`http_parser <http_parser>` component.

These components can be installed using ``idf.py add-dependency`` command.

For example, to install libsodium component with the exact version X.Y, run ``idf.py add-dependency libsodium==X.Y``.

To install libsodium component with the latest version compatible to X.Y according to `semver <https://semver.org/>`_ rules, run ``idf.py add-dependency libsodium~X.Y``.

To find out which versions of each component are available, open https://components.espressif.com, search for the component by its name and check the versions listed on the component page.

Deprecated Components
^^^^^^^^^^^^^^^^^^^^^

The following components are removed since they were deprecated in ESP-IDF v4.x:

* ``tcpip_adapter``. Please use the :doc:`ESP-NETIF </api-reference/network/esp_netif>` component instead; you can follow the :ref:`tcpip-adapter`.

.. note::

    OpenSSL-API component is no longer supported. It is not available in the IDF Component Registry, either. Please use :doc:`ESP-TLS </api-reference/protocols/esp_tls>` or :component:`mbedtls` API directly.

.. note::

    ``esp_adc_cal`` component is no longer supported. New adc calibration driver is in ``esp_adc`` component. Legacy adc calibration driver has been moved into ``esp_adc`` component. To use legacy ``esp_adc_cal`` driver APIs, you should add ``esp_adc`` component to the list of component requirements in CMakeLists.txt. Also check :doc:`Peripherals Migration Guide </migration-guides/release-5.x/5.0/peripherals>` for more details.

The targets components are no longer necessary after refactoring and have been removed:

 * ``esp32``
 * ``esp32s2``
 * ``esp32s3``
 * ``esp32c2``
 * ``esp32c3``
 * ``esp32h2``
