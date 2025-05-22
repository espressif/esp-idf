Introduction to Low Power Mode in Bluetooth\ :sup:`®` Low Energy Scenarios
================================================================================

:link_to_translation:`zh_CN:[中文]`

This section introduces clock source selection in low power modes for Bluetooth Low Energy (BLE), along with common related issues.

Clock Source Selection in Low Power Mode
--------------------------------------------

According to the Bluetooth specification, the sleep clock accuracy must be within 500 PPM, so make sure the clock source selected for BLE low power mode should meet that requirement. Otherwise BLE may not perform normally and cause a series of problem such as ACL connection establishment failure or ACL connection timeout, etc.


Selecting Main XTAL
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To select the main XTAL as the BLE internal clock source, configure the following option:

.. only:: esp32 or esp32c3 or esp32s3

    **Configuration Path:**

    ``Component config → Bluetooth → Controller Options → MODEM SLEEP Options → Bluetooth modem sleep → Bluetooth Modem sleep Mode 1 → Bluetooth low power clock``

    **Configuration Option:**

    - \ (X) Main crystal

.. only:: esp32c2 or esp32c6 or esp32h2 or esp32c5 or esp32c61

    **Configuration Path:**

    ``Component config → Bluetooth → Controller Options → BLE low power clock source``

    **Configuration Option:**

    - \ (X) Use main XTAL as RTC clock source

When this is selected, the main XTAL remains powered on during light-sleep, resulting in higher current consumption. The current reference for light-sleep using XTAL versus a 32kHz external crystal is shown below:

.. list-table:: Current Reference Values for Different Clock Sources in Light-sleep Mode
    :widths: auto
    :header-rows: 1

    * - Clock Source
      - Typical Light-sleep Current
    * - Main XTAL
      - 3.3 mA
    * - 32kHz External Crystal / 136 kHz RC
      - 34 uA


Selecting 32kHz External Crystal
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To use a 32kHz external crystal as the BLE internal clock source, configure the following options:

.. only:: esp32 or esp32c3 or esp32s3

    **Configuration Path:**

    ``Component config → Bluetooth → Controller Options → MODEM SLEEP Options → Bluetooth modem sleep → Bluetooth Modem sleep Mode 1 → Bluetooth low power clock``

    **Configuration Option:**

    - \ (X) External 32kHz crystal/oscillator

.. only:: esp32c2 or esp32c6 or esp32h2 or esp32c5 or esp32c61

    **Configuration Path 1:**

    ``Component config → Bluetooth → Controller Options → BLE low power clock source``

    **Configuration Option:**

    - \ (X) Use system RTC slow clock source

    **Configuration Path 2:**

    ``Component config → Hardware Settings → RTC Clock Config → RTC clock source``

    **Configuration Option:**

    - \ (X) External 32 kHz crystal

**Note:** Even if 32kHz is selected in menuconfig, the system will fall back to the main XTAL if the external crystal is not detected during BLE initialization. This may lead to unexpected current consumption in light-sleep mode.


Selecting 136 kHz RC Oscillator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: esp32c3 or esp32s3

    To use a 136 kHz internal RC oscillator as the BLE internal clock source, configure the following option:

    **Configuration Path:** `

    `Component config → Bluetooth → Controller Options → MODEM SLEEP Options → Bluetooth modem sleep → Bluetooth Modem sleep Mode 1 → Bluetooth low power clock``

    **Configuration Option:**

    - \ (X) Internal 136kHz RC oscillator

    Generally, the 136 kHz RC oscillator cannot meet the accuracy requirement of BLE. It is only suitable for scenarios with low clock accuracy requirements, such as legacy advertising (ADV) or scanning.

    However, for testing purposes, if the peer device also uses 136 kHz RC as the clock source, BLE can function under a 136 kHz RC clock through the following configuration:

    **Configuration Path:**

    ``Component config → Bluetooth → Controller Options``

    **Configuration Options:**

    - \ [*] Enable to set constant peer SCA
    - \ (3000) Constant peer sleep clock accuracy value

    **Note:** Using the 136 kHz RC oscillator may cause rare issues like connection drops or failures to establish a connection.

.. only:: esp32

    **Note:** ESP32 does not support using 136 kHz RC oscillator as the BLE clock source.

.. only:: esp32c2 or esp32c6 or esp32h2 or esp32c5 or esp32c61

    To use a 136 kHz internal RC oscillator as the BLE internal clock source, configure the following options:

    **Configuration Path 1:**

    ``Component config → Bluetooth → Controller Options → BLE low power clock source``

    **Configuration Option:**

    - \ (X) Use system RTC slow clock source

    **Configuration Path 2:**

    ``Component config → Hardware Settings → RTC Clock Config → RTC clock source``

    **Configuration Option:**

    - \ (X) Internal 136 kHz RC oscillator

    Generally, the 136 kHz RC oscillator cannot meet the accuracy requirement of BLE. It is only suitable for scenarios with low clock accuracy requirements, such as legacy advertising (ADV) or scanning.

    If low current consumption is required but have no access to the External 32kHz Crystal, then this clock source is recommended. However, selecting this clock source will have the sleep clock accuracy larger than 500 PPM, which is supported if the peer device is also an ESP chip. If the peer device is not an ESP chip, here's some BLE event not supported:

    1. Central role of Connection
    2. Advertiser of Periodic Advertising

    If the peer device also uses 136 kHz RC as the clock source, the following configuration should be set:

    **Configuration Path:**

    ``Component config → Bluetooth → Controller Options``

    **Configuration Options:**

    - \ [*] Enable to set constant peer SCA
    - \ (3000) Constant peer sleep clock accuracy value

    **Note:** Using the 136 kHz RC oscillator may cause rare issues like connection establishment failure or connection timeout.


**How to Check the Current Clock Source Used by BLE**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You can determine the BLE clock source from the logs during BLE initialization:

.. list-table:: BLE Initialization Log Messages and Clock Source Correspondence
    :widths: 50 50
    :header-rows: 1

    * - Log Message
      - Clock Source
    * - Using main XTAL as clock source
      - Main XTAL
    * - Using 136 kHz RC as clock source
      - Internal 136 kHz RC oscillator
    * - Using external 32.768 kHz crystal as clock source
      - External 32 kHz crystal
    * - Using external 32.768 kHz oscillator at 32K_XP pin as clock source
      - External 32 kHz oscillator at 32K_XP pin


FAQ
--------------------------------------

**1. BLE ACL Connection Fails or Disconnects in Low Power Mode**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

As explained in the clock source selection section above, when ACL connections fail to establish or unexpectedly disconnect in low power mode, first verify whether the current clock source meets BLE accuracy requirements.


**2. Measured light-sleep Current Is Higher Than Expected**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

As introduced in the clock source selection section above, if the main XTAL is used as the clock source, it will remain powered on during light-sleep, resulting in higher current consumption than other clock sources.

Also, BLE operates in Auto Light-sleep mode, meaning the system decides whether to enter sleep after entering IDLE. During advertising or scanning, current will be significantly higher than during light-sleep.

Thus, the average current in low power mode is somewhere between light-sleep current and peak current (usually TX current).

The average current can be application specific, and depends on the BLE configuration and the period of time in light-sleep mode. Some application may have larger average current because it has BLE taking a larger ratio of time transmitting and receiving.

**3. Unable to Enter light-sleep Mode**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When Auto light-sleep is enabled but the device fails to enter light-sleep, it's often due to insufficient IDLE duration, which prevents meeting the automatic light-sleep entry conditions.

This could be caused by excessive logging or BLE configuration that prevents sufficient IDLE time, such as continuous scan mode.
