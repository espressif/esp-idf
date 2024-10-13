SmartConfig
===========

:link_to_translation:`zh_CN:[中文]`

The SmartConfig\ :sup:`TM` is a provisioning technology developed by TI to connect a new Wi-Fi device to a Wi-Fi network. It uses a mobile application to broadcast the network credentials from a smartphone, or a tablet, to an un-provisioned Wi-Fi device.

The advantage of this technology is that the device does not need to directly know SSID or password of an Access Point (AP). This information is provided using the smartphone. This is particularly important to headless device and systems, due to their lack of a user interface.

If you are looking for other options to provision your {IDF_TARGET_NAME} devices, check :doc:`../provisioning/index`.


Application Example
-------------------

Connect {IDF_TARGET_NAME} to the target AP using SmartConfig: :example:`wifi/smart_config`.


API Reference
-------------

.. include-build-file:: inc/esp_smartconfig.inc
