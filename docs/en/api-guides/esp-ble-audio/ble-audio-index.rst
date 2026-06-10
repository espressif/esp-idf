ESP-IDF Bluetooth LE Audio
==========================

:link_to_translation:`zh_CN:[中文]`

Bluetooth LE Audio is the audio architecture introduced in Bluetooth Core Specification 5.2. When reading this documentation, it helps to keep two ideas separate:

- The **standard** is the set of profiles, services, and roles defined by the Bluetooth SIG — the Generic Audio Framework, layered on top of the LE isochronous transport. It is platform-independent and is described in :doc:`Bluetooth LE Audio Standard <ble-audio-introduction>`.
- The **implementation** is the software that realizes the standard on a particular platform. The ESP-IDF implementation is described in :doc:`ESP-IDF Bluetooth LE Audio Architecture <ble-audio-architecture-overview>`.

In ESP-IDF, the implementation is delivered as two components:

- **ESP-BLE-ISO** provides the isochronous transport — the Connected and Broadcast Isochronous Streams (CIS/BIS) that carry the audio. See its :doc:`API reference <../../api-reference/bluetooth/esp-ble-iso>`.
- **ESP-BLE-AUDIO** provides the Generic Audio Framework profiles and services, built on top of ESP-BLE-ISO. See its :doc:`API reference <../../api-reference/bluetooth/esp-ble-audio>`.

Both components run on either of the two ESP-IDF Bluetooth host stacks, **Bluedroid** or **NimBLE**. Their API and behavior are kept identical across the two hosts wherever possible; where a behavioral difference is unavoidable, it is called out in the architecture document together with the reason.

For the support status of individual features, see :doc:`Feature Support Status <ble-audio-feature-support-status>`.

.. toctree::
    :maxdepth: 1

    ble-audio-introduction
    ble-audio-architecture-overview
    ble-audio-feature-support-status
