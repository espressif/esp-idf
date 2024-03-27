Wi-Fi Aware\ :sup:`TM` (NAN)
===================================

:link_to_translation:`zh_CN:[中文]`

Wi-Fi Aware\ :sup:`TM` or NAN (Neighbor Awareness Networking) is a protocol that allows Wi-Fi devices to discover services in their proximity. Typically, location-based services are based on querying servers for information about the environment and the location knowledge is based on GPS or other location reckoning techniques. However, NAN does not require real-time connection to servers, GPS or other geo-location, but instead uses direct device-to-device Wi-Fi to discover and exchange information. NAN scales effectively in dense Wi-Fi environments and complements the connectivity of Wi-Fi by providing information about people and services in the proximity.

Multiple NAN devices which are in the vicinity form a NAN cluster which allows them to communicate with each other. Devices within a NAN cluster can advertise (Publish method) or look for (Subscribe method) services using NAN Service Discovery protocols. Matching of services is done by service name, once a match is found, a device can either send a message or establish an IPv6 Datapath with the peer.

{IDF_TARGET_NAME} supports Wi-Fi Aware in standalone mode with support for both Service Discovery and Datapath. Wi-Fi Aware is still an evolving protocol. Please refer to Wi-Fi Alliance's official page on `Wi-Fi Aware <https://www.wi-fi.org/discover-wi-fi/wi-fi-aware>`_ for more information. Many Android smartphones with Android 8 or higher support Wi-Fi Aware. Refer to Android's developer guide on Wi-Fi Aware `Wi-Fi Aware <https://www.wi-fi.org/discover-wi-fi/wi-fi-aware>`_ for more information.

Application Example
-------------------

A pair of examples for a Publisher-Subscriber use case: :example:`wifi/wifi_aware/nan_publisher` and :example:`wifi/wifi_aware/nan_subscriber`. A user interactive console example to explore full functionality of Wi-Fi Aware: :example:`wifi/wifi_aware/nan_console`. Please check the `README` for more details in respective example directories.

API Reference
-------------

.. include-build-file:: inc/esp_nan.inc
