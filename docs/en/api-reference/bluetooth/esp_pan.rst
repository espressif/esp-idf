Bluetooth® PAN API
==================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

PAN (Personal Area Networking) enables IP networking over Classic Bluetooth using BNEP (Bluetooth Network Encapsulation Protocol). Devices can act as a PAN User (PANU), Group Network (GN) hub, or Network Access Point (NAP) so peers can exchange Ethernet frames and obtain network access.

Application Examples
--------------------

- :example:`bluetooth/bluedroid/classic_bt/bt_pan_nap` demonstrates how to use the PAN Network Access Point (NAP) role so that PANU peers can reach the Internet through the ESP device's Wi-Fi STA uplink.

- :example:`bluetooth/bluedroid/classic_bt/bt_pan_panu` demonstrates how to use the PAN User (PANU) role to connect to a NAP (for example ``bt_pan_nap`` or a phone with Bluetooth tethering) and verify IP connectivity.

- :example:`bluetooth/bluedroid/classic_bt/bt_pan_gn` demonstrates how to use the PAN Group Network (GN) role as a hub that bridges multiple PANU clients so they can communicate on the same subnet.

API Reference
-------------

.. include-build-file:: inc/esp_pan_api.inc
