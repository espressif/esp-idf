Wi-Fi Aware\ :sup:`TM` (NAN)
===================================

:link_to_translation:`zh_CN:[中文]`

Wi-Fi Aware\ :sup:`TM` or NAN (Neighbor Awareness Networking) is a protocol that allows Wi-Fi devices to discover services in their proximity. Typically, location-based services are based on querying servers for information about the environment and the location knowledge is based on GPS or other location reckoning techniques. However, NAN does not require real-time connection to servers, GPS or other geo-location, but instead uses direct device-to-device Wi-Fi to discover and exchange information. NAN scales effectively in dense Wi-Fi environments and complements the connectivity of Wi-Fi by providing information about people and services in the proximity.

Multiple NAN devices which are in the vicinity form a NAN cluster which allows them to communicate with each other. Devices within a NAN cluster can advertise (Publish method) or look for (Subscribe method) services using NAN Service Discovery protocols. Matching of services is done by service name, once a match is found, a device can either send a message or establish an IPv6 Datapath with the peer.

{IDF_TARGET_NAME} supports Wi-Fi Aware in standalone mode with support for both Service Discovery and Datapath. Wi-Fi Aware is still an evolving protocol. Please refer to Wi-Fi Alliance's official page on `Wi-Fi Aware <https://www.wi-fi.org/discover-wi-fi/wi-fi-aware>`_ for more information. Many Android smartphones with Android 8 or higher support Wi-Fi Aware. Refer to Android's developer guide on Wi-Fi Aware `Wi-Fi Aware <https://www.wi-fi.org/discover-wi-fi/wi-fi-aware>`_ for more information.

Application Examples
--------------------

- :example:`wifi/wifi_aware/nan_console` demonstrates how to use the NAN protocol to discover services in proximity, establish a datapath, and communicate between devices without requiring an Internet or AP connection. It provides console commands for configuring NAN services, publishing or subscribing to a service, sending messages, and initiating or terminating a datapath.

- :example:`wifi/wifi_aware/nan_publisher` demonstrates how to use the NAN protocol to publish a service for advertising a service to other devices in the vicinity, and how to respond to the devices that subscribes to the service.

- :example:`wifi/wifi_aware/nan_subscriber` demonstrates how to use the NAN protocol to discover other devices that publishes the required service in the proximity, and communicate with them either by sending a message or initiating a datapath.

API Reference
-------------

.. include-build-file:: inc/esp_nan.inc
