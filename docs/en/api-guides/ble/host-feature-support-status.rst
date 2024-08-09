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

|unsupported_def| **This feature is not supported on this chip series.** If you have related requirements, please prioritize selecting other Espressif chip series that support this feature.
If none of our chip series meet your needs, please contact `customer support team <https://www.espressif.com/en/contact-us/sales-questions>`__, and our R&D team will conduct an internal feasibility assessment for you.

|NA_def|  The feature with this label could be the following type:
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
      - |unsupported|
      - |developing202412|
    * -
      - GATT Caching
      - |experimental|
      - |experimental|
    * -
      - Advertising Channel Index
      - |NA|
      - |NA|
    * -
      - Periodic Advertising Sync Transfer
      - |supported|
      - |supported|
    * - .. centered:: |5.2|
      - LE Isochronous Channels (BIS/CIS)
      - |unsupported|
      - |developing202412|
    * -
      - Enhanced Attribute Protocol
      - |unsupported|
      - |developing202412|
    * -
      - LE Power Control
      - |unsupported|
      - |developing202412|
    * - .. centered:: |5.3|
      - AdvDataInfo in Periodic Advertising
      - |supported|
      - |supported|
    * -
      - LE Enhanced Connection Update (Connection Subrating)
      - |unsupported|
      - |supported|
    * -
      - LE Channel Classification
      - |supported|
      - |supported|
    * - .. centered:: |5.4|
      - Advertising Coding Selection
      - |unsupported|
      - |supported|
    * -
      - Encrypted Advertising Data
      - |unsupported|
      - |experimental|
    * -
      - LE GATT Security Levels Characteristic
      - |unsupported|
      - |developing202412|
    * -
      - Periodic Advertising with Responses
      - |unsupported|
      - |developing202412|

.. [1]
   If you would like to know the Bluetooth SIG certification information for supported features,
   please consult `SIG Bluetooth Product Database <https://qualification.bluetooth.com/Listings/Search>`__.

It is important to clarify that this document is not a binding commitment to our customers.
The above feature support status information is for general informational purposes only and is subject to change without notice.
You are encouraged to consult with our `customer support team <https://www.espressif.com/en/contact-us/sales-questions>`__ for the most up-to-date information and to verify the suitability of features for your specific needs.


.. |supported| image:: https://img.shields.io/badge/supported-green
   :class: align-center
.. |developing| image:: https://img.shields.io/badge/in_progress-YYYY/MM-blue
   :class: align-center
.. |developing202412| image:: https://img.shields.io/badge/In_Progress-2024/12-blue
   :class: align-center
.. |unsupported| image:: https://img.shields.io/badge/unsupported-D8D8D8
   :class: align-center
.. |experimental| image:: https://img.shields.io/badge/experimental-orange
   :class: align-center
.. |NA| image:: https://img.shields.io/badge/N%2FA-D8D8D8
   :class: align-center
.. |supported_def| image:: https://img.shields.io/badge/supported-green
.. |developing_def| image:: https://img.shields.io/badge/In_Progress-YYYY/MM-blue
.. |unsupported_def| image:: https://img.shields.io/badge/unsupported-D8D8D8
.. |experimental_def| image:: https://img.shields.io/badge/experimental-orange
.. |NA_def| image:: https://img.shields.io/badge/N%2FA-D8D8D8
.. |4.2| replace:: `4.2 <https://www.bluetooth.com/specifications/specs/core-specification-4-2/>`__
.. |5.0| replace:: `5.0 <https://www.bluetooth.com/specifications/specs/core-specification-5-0/>`__
.. |5.1| replace:: `5.1 <https://www.bluetooth.com/specifications/specs/core-specification-5-1/>`__
.. |5.2| replace:: `5.2 <https://www.bluetooth.com/specifications/specs/core-specification-5-2/>`__
.. |5.3| replace:: `5.3 <https://www.bluetooth.com/specifications/specs/core-specification-5-3/>`__
.. |5.4| replace:: `5.4 <https://www.bluetooth.com/specifications/specs/core-specification-5-4/>`__
