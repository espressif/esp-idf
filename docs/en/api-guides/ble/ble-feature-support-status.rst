Major Feature Support Status
================================

:link_to_translation:`zh_CN:[中文]`

The table below shows the support status of Bluetooth Low Energy major features on {IDF_TARGET_NAME}.

|supported_def| **This feature has completed development and internal testing.** [1]_

|experimental_def| **This feature has been developed and is currently undergoing internal testing.**
You can explore these features for evaluation and feedback purposes but should be cautious of potential issues.

|developing_def| **The feature is currently being actively developed, and expected to be supported by the end of YYYY/MM.**
You should anticipate future updates regarding the progress and availability of these features.
If you do have an urgent need, please contact our `customer support team <https://www.espressif.com/en/contact-us/sales-questions>`__ for a possible feature trial.

|unsupported_def| **This feature is not supported on this chip series.** If you have related requirements, please prioritize selecting other Espressif chip series that support this feature.
If none of our chip series meet your needs, please contact `customer support team <https://www.espressif.com/en/contact-us/sales-questions>`__, and our R&D team will conduct an internal feasibility assessment for you.

|NA_def| The feature with this label could be the following two types:
    - **Host-only Feature**: The feature exists only above HCI, such as GATT Caching. It does not require the support from the Controller.
    - **Controller-only Feature**: The feature exists only below HCI, and cannot be configured/enabled via Host API, such as Advertising Channel Index. It does not require the support from the Host.

.. list-table::
    :width: 100%
    :widths: auto
    :header-rows: 1

    * - .. centered:: Core Spec
      - .. centered:: Major Features
      - .. centered:: ESP Controller
      - .. centered:: ESP-Bluedroid Host
      - .. centered:: ESP-NimBLE Host
    * - .. centered:: |4.2|
      - LE Data Packet Length Extension
      - |supported|
      - |supported|
      - |supported|
    * -
      - LE Secure Connections
      - |supported|
      - |supported|
      - |supported|
    * -
      - Link Layer Privacy
      - |supported|
      - |supported|
      - |supported|
    * -
      - Link Layer Extended Filter Policies
      - |supported|
      - |supported|
      - |supported|
    * - .. centered:: |5.0|
      - 2 Msym/s PHY for LE
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
    * -
      - LE Long Range (Coded PHY S=2/S=8)
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
    * -
      - High Duty Cycle Non-Connectable Advertising
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
    * -
      - LE Advertising Extensions
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
    * -
      - LE Channel Selection Algorithm #2
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
    * - .. centered:: |5.1|
      - .. only:: esp32h2

            Angle of Arrival (AoA)/Angle of Departure (AoD) [2]_
        .. only:: not esp32h2

            Angle of Arrival (AoA)/Angle of Departure (AoD)
      - .. only:: SOC_BLE_CTE_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_CTE_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_CTE_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_CTE_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_CTE_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_CTE_SUPPORTED

            |unsupported|
    * -
      - GATT Caching
      - |NA|
      - |experimental|
      - |experimental|
    * -
      - Randomized Advertising Channel Indexing
      - .. only:: esp32c2 or not SOC_ESP_NIMBLE_CONTROLLER

            |unsupported|
        .. only:: SOC_ESP_NIMBLE_CONTROLLER and not esp32c2

            |experimental|
      - |NA|
      - |NA|
    * -
      - Periodic Advertising Sync Transfer
      - .. only:: SOC_ESP_NIMBLE_CONTROLLER

            |supported|
        .. only:: not SOC_ESP_NIMBLE_CONTROLLER

            |unsupported|
      - .. only:: SOC_ESP_NIMBLE_CONTROLLER

            |supported|
        .. only:: not SOC_ESP_NIMBLE_CONTROLLER

            |unsupported|
      - .. only:: SOC_ESP_NIMBLE_CONTROLLER

            |supported|
        .. only:: not SOC_ESP_NIMBLE_CONTROLLER

            |unsupported|
    * - .. centered:: |5.2|
      - LE Isochronous Channels (BIS/CIS)
      - .. only:: SOC_BLE_ISO_SUPPORTED

            |experimental|
        .. only:: not SOC_BLE_ISO_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_ISO_SUPPORTED

            |experimental|
        .. only:: not SOC_BLE_ISO_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_ISO_SUPPORTED

            |experimental|
        .. only:: not SOC_BLE_ISO_SUPPORTED

            |unsupported|
    * -
      - Enhanced Attribute Protocol
      - |NA|
      - |unsupported|
      - |experimental|
    * -
      - LE Power Control
      - .. only:: SOC_BLE_POWER_CONTROL_SUPPORTED

            |experimental|
        .. only:: not SOC_BLE_POWER_CONTROL_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_POWER_CONTROL_SUPPORTED

            |experimental|
        .. only:: not SOC_BLE_POWER_CONTROL_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_POWER_CONTROL_SUPPORTED

            |experimental|
        .. only:: not SOC_BLE_POWER_CONTROL_SUPPORTED

            |unsupported|
    * - .. centered:: |5.3|
      - AdvDataInfo in Periodic Advertising
      - .. only:: SOC_BLE_PERIODIC_ADV_ENH_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_PERIODIC_ADV_ENH_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_PERIODIC_ADV_ENH_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_PERIODIC_ADV_ENH_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_PERIODIC_ADV_ENH_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_PERIODIC_ADV_ENH_SUPPORTED

            |unsupported|
    * -
      - LE Enhanced Connection Update (Connection Subrating)
      - .. only:: SOC_BLE_SUBRATE_SUPPORTED

            |experimental|
        .. only:: not SOC_BLE_SUBRATE_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_SUBRATE_SUPPORTED

            |experimental|
        .. only:: not SOC_BLE_SUBRATE_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_SUBRATE_SUPPORTED

            |experimental|
        .. only:: not SOC_BLE_SUBRATE_SUPPORTED

            |unsupported|
    * -
      - LE Channel Classification
      - .. only:: SOC_ESP_NIMBLE_CONTROLLER and not esp32c2

            |experimental|
        .. only:: not SOC_ESP_NIMBLE_CONTROLLER or esp32c2

            |unsupported|
      - .. only:: SOC_ESP_NIMBLE_CONTROLLER and not esp32c2

            |experimental|
        .. only:: not SOC_ESP_NIMBLE_CONTROLLER or esp32c2

            |unsupported|
      - .. only:: SOC_ESP_NIMBLE_CONTROLLER and not esp32c2

            |experimental|
        .. only:: not SOC_ESP_NIMBLE_CONTROLLER or esp32c2

            |unsupported|
    * - .. centered:: |5.4|
      - Advertising Coding Selection
      - .. only:: (SOC_ESP_NIMBLE_CONTROLLER or esp32c3 or esp32s3) and not esp32c2

            |experimental|
        .. only:: not ((SOC_ESP_NIMBLE_CONTROLLER or esp32c3 or esp32s3) and not esp32c2)

            |unsupported|
      - .. only:: (SOC_ESP_NIMBLE_CONTROLLER or esp32c3 or esp32s3) and not esp32c2

            |experimental|
        .. only:: not ((SOC_ESP_NIMBLE_CONTROLLER or esp32c3 or esp32s3) and not esp32c2)

            |unsupported|
      - .. only:: (SOC_ESP_NIMBLE_CONTROLLER or esp32c3 or esp32s3) and not esp32c2

            |experimental|
        .. only:: not ((SOC_ESP_NIMBLE_CONTROLLER or esp32c3 or esp32s3) and not esp32c2)

            |unsupported|
    * -
      - Encrypted Advertising Data
      - |NA|
      - |experimental|
      - |experimental|
    * -
      - LE GATT Security Levels Characteristic
      - |NA|
      - |unsupported|
      - |experimental|
    * -
      - Periodic Advertising with Responses
      - .. only:: SOC_BLE_PERIODIC_ADV_WITH_RESPONSE

            |experimental|
        .. only:: not SOC_BLE_PERIODIC_ADV_WITH_RESPONSE

            |unsupported|
      - .. only:: SOC_BLE_PERIODIC_ADV_WITH_RESPONSE

            |experimental|
        .. only:: not SOC_BLE_PERIODIC_ADV_WITH_RESPONSE

            |unsupported|
      - .. only:: SOC_BLE_PERIODIC_ADV_WITH_RESPONSE

            |experimental|
        .. only:: not SOC_BLE_PERIODIC_ADV_WITH_RESPONSE

            |unsupported|
    * - .. centered:: |6.0|
      - Channel Sounding
      - |unsupported|
      - |unsupported|
      - |unsupported|
    * -
      - LL Extended Feature Set
      - .. only:: SOC_ESP_NIMBLE_CONTROLLER and not esp32c2

            |developing202612|
        .. only:: not SOC_ESP_NIMBLE_CONTROLLER or esp32c2

            |unsupported|
      - .. only:: SOC_ESP_NIMBLE_CONTROLLER and not esp32c2

            |developing202612|
        .. only:: not SOC_ESP_NIMBLE_CONTROLLER or esp32c2

            |unsupported|
      - .. only:: SOC_ESP_NIMBLE_CONTROLLER and not esp32c2

            |developing202612|
        .. only:: not SOC_ESP_NIMBLE_CONTROLLER or esp32c2

            |unsupported|
    * -
      - Decision-Based Advertising Filtering
      - |unsupported|
      - |unsupported|
      - |unsupported|
    * -
      - Enhancements for ISOAL
      - .. only:: SOC_BLE_ISO_SUPPORTED

            |experimental|
        .. only:: not SOC_BLE_ISO_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_ISO_SUPPORTED

            |experimental|
        .. only:: not SOC_BLE_ISO_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_ISO_SUPPORTED

            |experimental|
        .. only:: not SOC_BLE_ISO_SUPPORTED

            |unsupported|
    * -
      - Monitoring Advertisers
      - .. only:: SOC_ESP_NIMBLE_CONTROLLER and not esp32c2

            |developing202612|
        .. only:: not SOC_ESP_NIMBLE_CONTROLLER or esp32c2

            |unsupported|
      - .. only:: SOC_ESP_NIMBLE_CONTROLLER and not esp32c2

            |developing202612|
        .. only:: not SOC_ESP_NIMBLE_CONTROLLER or esp32c2

            |unsupported|
      - .. only:: SOC_ESP_NIMBLE_CONTROLLER and not esp32c2

            |developing202612|
        .. only:: not SOC_ESP_NIMBLE_CONTROLLER or esp32c2

            |unsupported|
    * -
      - Frame Space Update
      - |unsupported|
      - |unsupported|
      - |unsupported|
    * - .. centered:: |6.1|
      - Randomized Resolvable Private Address (RPA) Updates
      - |unsupported|
      - |unsupported|
      - |unsupported|
    * - .. centered:: |6.2|
      - HCI USB LE Isochronous Support
      - |unsupported|
      - |unsupported|
      - |unsupported|
    * -
      - LE Test Mode Enhancements
      - |unsupported|
      - |unsupported|
      - |unsupported|
    * -
      - Shorter Connection Intervals
      - |unsupported|
      - |unsupported|
      - |unsupported|
    * -
      - Channel Sounding Amplitude-based Attack Resilience
      - |unsupported|
      - |unsupported|
      - |unsupported|
    * - .. centered:: |6.3|
      - Channel Sounding Inline Phase Correction Term Transfer
      - |unsupported|
      - |unsupported|
      - |unsupported|
    * -
      - Channel Sounding PHY-specific RTT Accuracy
      - |unsupported|
      - |unsupported|
      - |unsupported|
    * -
      - Running Out of Bits
      - |unsupported|
      - |unsupported|
      - |unsupported|

.. [1]
   If you would like to know the Bluetooth SIG certification information for supported features,
   please consult `SIG Bluetooth Product Database <https://qualification.bluetooth.com/Listings/Search>`__.

.. only:: esp32h2

    .. [2]
       Note: On ESP32-H2, connection-oriented Bluetooth LE Direction Finding (AoA/AoD) is not supported when the Bluetooth LE connection is encrypted.

For certain features, if the majority of the development is completed on the Controller, the Host's support status will be limited by the Controller's support status.
If you want Bluetooth LE Controller and Host to run on different Espressif chips, the functionality of the Host will not be limited by the Controller's support status on the chip running the Host,
please check the :doc:`ESP Host Feature Support Status Table <host-feature-support-status>` .

It is important to clarify that this document is not a binding commitment to our customers.
The above feature support status information is for general informational purposes only and is subject to change without notice.
You are encouraged to consult with our `customer support team <https://www.espressif.com/en/contact-us/sales-questions>`__ for the most up-to-date information and to verify the suitability of features for your specific needs.


.. |supported| image:: ../../../_static/ble/feature_status/supported.svg
   :class: align-center
   :width: 65px
.. |developing202512| image:: ../../../_static/ble/feature_status/developing202512.svg
   :class: align-center
   :width: 120px
.. |developing202603| image:: ../../../_static/ble/feature_status/developing202603.svg
   :class: align-center
   :width: 125px
.. |developing202612| image:: ../../../_static/ble/feature_status/developing202612.svg
   :class: align-center
   :width: 125px
.. |unsupported| image:: ../../../_static/ble/feature_status/unsupported.svg
   :class: align-center
   :width: 75px
.. |experimental| image:: ../../../_static/ble/feature_status/experimental.svg
   :class: align-center
   :width: 75px
.. |NA| image:: ../../../_static/ble/feature_status/NA.svg
   :class: align-center
   :width: 25px
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
.. |6.1| replace:: `6.1 <https://www.bluetooth.com/specifications/specs/core-specification-6-1/>`__
.. |6.2| replace:: `6.2 <https://www.bluetooth.com/specifications/specs/core-specification-6-2/>`__
.. |6.3| replace:: `6.3 <https://www.bluetooth.com/specifications/specs/core-specification-6-3/>`__
