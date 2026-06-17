ESP-BLE-ISO
===========

ESP-BLE-ISO provides APIs for Bluetooth LE Isochronous Channels, enabling time-synchronized, connection-oriented (CIS) and connectionless (BIS) streaming. The implementation is built on the Bluetooth controller ISO support and is intended for use with the :doc:`esp-ble-audio` component.

**Channel types**

* **Connected Isochronous Streams (CIS)** — Bidirectional isochronous links between two devices; requires a prior ACL connection to be established. Used for time-synchronized unicast audio (e.g., LE Audio).
* **Broadcast Isochronous Streams (BIS)** — Unidirectional isochronous streams from a Broadcaster to one or more Synchronized Receivers, without a prior connection. Used for broadcast audio and group listening.

With these APIs you can create or join Connected ISO Groups (CIG), create or synchronize to Broadcast ISO Groups (BIG), set up ISO data paths, and send or receive isochronous data (e.g., LC3 audio). For higher-level LE Audio profiles and codec APIs, see :doc:`esp-ble-audio`.

.. warning::

    This is a **preview release**. The ESP-BLE-ISO APIs, data structures, and configuration parameters are subject to change in future releases. Breaking changes — such as renamed or restructured types, modified function signatures, or removed fields — may be introduced without prior notice.

Application Examples
--------------------

* **BIG (Broadcast Isochronous Group)**
  - :example:`bluetooth/esp_ble_iso/big_broadcaster` demonstrates how to create a BIG and send isochronous data over BIS.
  - :example:`bluetooth/esp_ble_iso/big_receiver` demonstrates how to synchronize to a BIG and receive isochronous data from BIS.
* **CIS (Connected Isochronous Stream)**
  - :example:`bluetooth/esp_ble_iso/cis_central` demonstrates how to act as a CIS Central that creates a CIG/CIS and sends isochronous data.
  - :example:`bluetooth/esp_ble_iso/cis_peripheral` demonstrates how to act as a CIS Peripheral that accepts a CIS and receives isochronous data.


API Reference
-------------

ESP-BLE-ISO APIs are divided into the following parts:

* `ESP-BLE-ISO Common API`_


ESP-BLE-ISO Common API
----------------------

This section contains macros, type definitions, and functions from ``esp_ble_iso_common_api.h``: BIS index and data path macros, controller delay and interval limits, CIG/BIG parameters and structures, ISO server and channel operations, data path setup, BIG create/sync/terminate, and GAP event types and ISO common initialization.

.. include-build-file:: inc/esp_ble_iso_common_api.inc
