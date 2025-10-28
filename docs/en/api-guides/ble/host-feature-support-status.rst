:orphan:

ESP Host Major Feature Support Status
=======================================

:link_to_translation:`zh_CN:[中文]`

The table below shows the support status of major features on ESP-Bluedroid and ESP-NimBLE Host.
If you plan to run the BLE Controller and Host on {IDF_TARGET_NAME} together, the functionality of the Host may be limited by the support status of the Controller,
please check the :doc:`{IDF_TARGET_NAME} Major Feature Support Status Table <ble-feature-support-status>` .

|supported_def| **This feature has completed development and internal testing.** [1]_

|experimental_def| **This feature has been developed and is currently undergoing internal testing.**
You can explore these features for evaluation and feedback purposes but should be cautious of potential issues.

|developing_def| **The feature is currently being actively developed, and expected to be supported by the end of YYYY/MM.**
You should anticipate future updates regarding the progress and availability of these features.
If you do have an urgent need, please contact our `customer support team <https://www.espressif.com/en/contact-us/sales-questions>`__ for a possible feature trial.

|unsupported_def| **This feature is not supported on this Host.** If you have related requirements, please prioritize selecting other Espressif Bluetooth Host that support this feature.
If none of our chip series meet your needs, please contact `customer support team <https://www.espressif.com/en/contact-us/sales-questions>`__, and our R&D team will conduct an internal feasibility assessment for you.

|NA_def| The feature with this label could be the following type:
    - **Controller-only Feature**: The feature exists only below HCI, and cannot be configured/enabled via Host API, such as Advertising Channel Index.
      It does not require the support from the Host.

.. list-table::
    :width: 100%
    :widths: auto
    :header-rows: 1

    * - .. centered:: Core Spec
      - .. centered:: Major Features
      - .. centered:: ESP-Bluedroid Host
      - .. centered:: ESP-NimBLE Host
    * - .. centered:: |4.2|
      - LE Data Packet Length Extension
      - |supported|
      - |supported|
    * -
      - LE Secure Connections
      - |supported|
      - |supported|
    * -
      - Link Layer Privacy
      - |supported|
      - |supported|
    * -
      - Link Layer Extended Filter Policies
      - |supported|
      - |supported|
    * - .. centered:: |5.0|
      - 2 Msym/s PHY for LE
      - |supported|
      - |supported|
    * -
      - LE Long Range (Coded PHY S=2/S=8)
      - |supported|
      - |supported|
    * -
      - High Duty Cycle Non-Connectable Advertising
      - |supported|
      - |supported|
    * -
      - LE Advertising Extensions
      - |supported|
      - |supported|
    * -
      - LE Channel Selection Algorithm #2
      - |supported|
      - |supported|
    * - .. centered:: |5.1|
      - Angle of Arrival (AoA)/Angle of Departure (AoD)
      - |experimental|
      - |experimental|
    * -
      - GATT Caching
      - |experimental|
      - |experimental|
    * -
      - Randomized Advertising Channel Indexing
      - |NA|
      - |NA|
    * -
      - Periodic Advertising Sync Transfer
      - |supported|
      - |supported|
    * - .. centered:: |5.2|
      - LE Isochronous Channels (BIS/CIS)
      - |experimental|
      - |developing202512|
    * -
      - Enhanced Attribute Protocol
      - |unsupported|
      - |experimental|
    * -
      - LE Power Control
      - |experimental|
      - |experimental|
    * - .. centered:: |5.3|
      - AdvDataInfo in Periodic Advertising
      - |supported|
      - |supported|
    * -
      - LE Enhanced Connection Update (Connection Subrating)
      - |experimental|
      - |experimental|
    * -
      - LE Channel Classification
      - |supported|
      - |supported|
    * - .. centered:: |5.4|
      - Advertising Coding Selection
      - |experimental|
      - |supported|
    * -
      - Encrypted Advertising Data
      - |developing202512|
      - |experimental|
    * -
      - LE GATT Security Levels Characteristic
      - |experimental|
      - |experimental|
    * -
      - Periodic Advertising with Responses
      - |experimental|
      - |experimental|
    * - .. centered:: |6.0|
      - Channel Sounding
      - |experimental|
      - |unsupported|
    * -
      - LL Extended Feature Set
      - |developing202606|
      - |developing202606|
    * -
      - Decision-Based Advertising Filtering
      - |unsupported|
      - |unsupported|
    * -
      - Enhancements for ISOAL
      - |unsupported|
      - |unsupported|
    * -
      - Monitoring Advertisers
      - |developing202606|
      - |developing202606|
    * -
      - Frame Space Update
      - |unsupported|
      - |unsupported|

.. [1]
   If you would like to know the Bluetooth SIG certification information for supported features,
   please consult `SIG Bluetooth Product Database <https://qualification.bluetooth.com/Listings/Search>`__.

It is important to clarify that this document is not a binding commitment to our customers.
The above feature support status information is for general informational purposes only and is subject to change without notice.
You are encouraged to consult with our `customer support team <https://www.espressif.com/en/contact-us/sales-questions>`__ for the most up-to-date information and to verify the suitability of features for your specific needs.


.. |supported| image:: ../../../_static/ble/feature_status/supported.svg
   :class: align-center
.. |developing202512| image:: ../../../_static/ble/feature_status/developing202512.svg
   :class: align-center
   :width: 120px
.. |developing202603| image:: ../../../_static/ble/feature_status/developing202603.svg
   :class: align-center
   :width: 125px
.. |developing202606| image:: ../../../_static/ble/feature_status/developing202606.svg
   :class: align-center
   :width: 125px
.. |unsupported| image:: ../../../_static/ble/feature_status/unsupported.svg
   :class: align-center
.. |experimental| image:: ../../../_static/ble/feature_status/experimental.svg
   :class: align-center
.. |NA| image:: ../../../_static/ble/feature_status/NA.svg
   :class: align-center
.. |supported_def| image:: ../../../_static/ble/feature_status/supported.svg
.. |developing_def| image:: ../../../_static/ble/feature_status/developingYYYYMM.svg
.. |unsupported_def| image:: ../../../_static/ble/feature_status/unsupported.svg
.. |experimental_def| image:: ../../../_static/ble/feature_status/experimental.svg
.. |NA_def| image:: ../../../_static/ble/feature_status/NA.svg
.. |4.2| replace:: `4.2 <https://www.bluetooth.com/specifications/specs/core-specification-4-2/>`__
.. |5.0| replace:: `5.0 <https://www.bluetooth.com/specifications/specs/core-specification-5-0/>`__
.. |5.1| replace:: `5.1 <https://www.bluetooth.com/specifications/specs/core-specification-5-1/>`__
.. |5.2| replace:: `5.2 <https://www.bluetooth.com/specifications/specs/core-specification-5-2/>`__
.. |5.3| replace:: `5.3 <https://www.bluetooth.com/specifications/specs/core-specification-5-3/>`__
.. |5.4| replace:: `5.4 <https://www.bluetooth.com/specifications/specs/core-specification-5-4/>`__
.. |6.0| replace:: `6.0 <https://www.bluetooth.com/specifications/specs/core-specification-6-0/>`__
