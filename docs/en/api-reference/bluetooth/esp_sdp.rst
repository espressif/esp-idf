Bluetooth® SDP API
==================

Overview
--------

SDP (Service Discovery Protocol) API enables devices to discover services and service attributes offered by remote Bluetooth devices. It supports service search, service attribute retrieval, and the establishment of service connections.

Application Examples
--------------------

- :example:`bluetooth/bluedroid/classic_bt/bt_l2cap_client` demonstrates how to use SDP APIs to search for services on remote Bluetooth devices. It shows how to register SDP callbacks, initialize SDP, perform service discovery using ``esp_sdp_search_record()``, and retrieve the L2CAP PSM (Protocol/Service Multiplexer) value from the search results to establish an L2CAP connection.

- :example:`bluetooth/bluedroid/classic_bt/bt_l2cap_server` demonstrates how to use SDP APIs to create and publish service records. It shows how to register SDP callbacks, initialize SDP, create an SDP record using ``esp_sdp_create_record()`` with L2CAP PSM information, making the service discoverable by remote clients.

API Reference
-------------

.. include-build-file:: inc/esp_sdp_api.inc


