Wi-Fi Easy Connect\ :sup:`TM` (DPP)
===================================

:link_to_translation:`zh_CN:[中文]`

Wi-Fi Easy Connect\ :sup:`TM`, also known as Device Provisioning Protocol (DPP) or Easy Connect, is a provisioning protocol certified by Wi-Fi Alliance. It is a secure and standardized provisioning protocol for configuration of Wi-Fi Devices. With Easy Connect, adding a new device to a network is as simple as scanning a QR Code. This reduces complexity and enhances user experience while onboarding devices without UI like Smart Home and IoT products. Unlike old protocols like Wi-Fi Protected Setup (WPS), Wi-Fi Easy Connect in corporates strong encryption through public key cryptography to ensure networks remain secure as new devices are added.

Easy Connect brings many benefits in the user experience:

  - Simple and intuitive to use; no lengthy instructions to follow for new device setup
  - No need to remember and enter passwords into the device being provisioned
  - Works with electronic or printed QR codes, or human-readable strings
  - Supports both WPA2 and WPA3 networks

Please refer to Wi-Fi Alliance's official page on `Easy Connect <https://www.wi-fi.org/discover-wi-fi/wi-fi-easy-connect>`_ for more information.

{IDF_TARGET_NAME} supports Enrollee mode of Easy Connect with QR Code as the provisioning method. A display is required to display this QR Code. Users can scan this QR Code using their capable device and provision the {IDF_TARGET_NAME} to their Wi-Fi network. The provisioning device needs to be connected to the AP which need not support Wi-Fi Easy Connect\ :sup:`TM`.

Easy Connect is still an evolving protocol. Of known platforms that support the QR Code method are some Android smartphones with Android 10 or higher. To use Easy Connect, no additional App needs to be installed on the supported smartphone.

Application Example
-------------------

Example on how to provision {IDF_TARGET_NAME} using a supported smartphone: :example:`wifi/wifi_easy_connect/dpp-enrollee`.

API Reference
-------------

.. include-build-file:: inc/esp_dpp.inc
