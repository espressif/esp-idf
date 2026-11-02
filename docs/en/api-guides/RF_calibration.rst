RF Calibration
==============

:link_to_translation:`zh_CN:[中文]`

The RF calibration behavior of {IDF_TARGET_NAME} is jointly determined by the following two configuration options:

1. :menuitem:`CONFIG_ESP_PHY_CALIBRATION_MODE`: Selects partial calibration, full calibration, or no calibration.

2. :menuitem:`CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE`: Controls whether PHY calibration data in NVS is automatically loaded and stored during RF initialization.

RF Calibration Configuration
----------------------------

:menuitem:`CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE` controls whether PHY calibration data in NVS is automatically used during RF initialization:

- If this option is enabled, the system loads calibration data from NVS, and :menuitem:`CONFIG_ESP_PHY_CALIBRATION_MODE` determines the calibration method used during a normal boot. When the device wakes up from Deep-sleep mode, no calibration is selected regardless of the configured calibration method. If the NVS calibration data is missing or invalid, the system switches to full calibration and attempts to store the new data in NVS.
- If this option is disabled, :menuitem:`CONFIG_ESP_PHY_CALIBRATION_MODE` is not configurable, the system does not automatically load or store NVS calibration data, and full calibration is always performed.

When NVS calibration data storage is enabled, call :cpp:func:`nvs_flash_init` before initializing Wi-Fi or Bluetooth®/Bluetooth Low Energy. Otherwise, calibration data cannot be loaded or stored, and the system falls back to full calibration.

Partial Calibration
-------------------

In the current version, partial calibration and full calibration are functionally equivalent. Full calibration is selected by default.

Full Calibration
----------------

Full calibration is the default option for :menuitem:`CONFIG_ESP_PHY_CALIBRATION_MODE`. It is also forced under the following conditions:

1. :menuitem:`CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE` is disabled.

2. NVS is not initialized, or the NVS partition or ``phy`` namespace containing the calibration data does not exist.

3. The NVS partition containing calibration data has been erased.

4. The hardware MAC address has changed.

5. The calibration data format version used by the PHY library has changed.

6. The RF calibration data loaded from NVS has an invalid length, fails validation, or is outdated.

When :menuitem:`CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE` is enabled and loading the NVS data fails, the system performs full calibration and attempts to store the new data. If the NVS data is loaded successfully, it is updated only when the PHY library reports that the calibration data failed validation or is outdated.

To force full calibration, use one of the following methods:

1. Erase the NVS partition if erasing all data stored in the partition is acceptable.

2. Call API :cpp:func:`esp_phy_erase_cal_data_in_nvs` before initializing Wi-Fi and Bluetooth®/Bluetooth Low Energy based on some conditions (e.g., an option provided in some diagnostic mode). In this case, only the PHY namespace of the NVS partition is erased.

No Calibration
---------------

No calibration is intended only for devices waking up from Deep-sleep mode. When :menuitem:`CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE` is enabled, the system automatically changes the calibration method to no calibration after a Deep-sleep wakeup. There is no need to change :menuitem:`CONFIG_ESP_PHY_CALIBRATION_MODE` manually. If valid calibration data cannot be loaded from NVS, the system still falls back to full calibration.

Do not set :menuitem:`CONFIG_ESP_PHY_CALIBRATION_MODE` to no calibration. If it is selected and the NVS calibration data is valid, RF calibration is also skipped during a normal cold boot, which may cause RF performance issues.

Configuration Summary
---------------------

In the current version, partial calibration and full calibration are functionally equivalent. PARTIAL, FULL, and NONE in the following summary indicate the calibration mode passed to the PHY library:

- :menuitem:`CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE` enabled with FULL selected:

  - Normal boot with valid NVS calibration data: FULL.
  - Deep-sleep wakeup with valid NVS calibration data: NONE.
  - Invalid NVS calibration data or a loading failure: FULL, followed by an attempt to store the new calibration data.

- :menuitem:`CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE` enabled with PARTIAL selected: In the current version, the behavior is the same as when FULL is selected.

- :menuitem:`CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE` enabled with NONE selected:

  - Normal boot with valid NVS calibration data: NONE.
  - Deep-sleep wakeup with valid NVS calibration data: NONE.
  - Invalid NVS calibration data or a loading failure: FULL, followed by an attempt to store the new calibration data.

- :menuitem:`CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE` disabled:

  - :menuitem:`CONFIG_ESP_PHY_CALIBRATION_MODE` is not configurable and its value is ignored.
  - FULL is used in all cases.
  - NVS calibration data is not automatically loaded or stored.
  - FULL is also used after a Deep-sleep wakeup.

PHY Initialization Data
-----------------------

The PHY initialization data is used for RF calibration. There are two ways to get the PHY initialization data.

One is to use the default initialization data which is located in the header file :idf_file:`components/esp_phy/{IDF_TARGET_PATH_NAME}/include/phy_init_data.h`. It is embedded into the application binary after compiling and then stored into read-only memory (DROM). To use the default initialization data, please go to ``menuconfig`` and disable :menuitem:`CONFIG_ESP_PHY_INIT_DATA_IN_PARTITION`.

An alternative is to store the initialization data in a PHY data partition. A PHY data partition is included in the default partition table. However, when using a custom partition table, please ensure that a PHY data partition (type: ``data``, subtype: ``phy``) is included in the custom partition table. Whether you are using a custom partition table or the default partition table, if initialization data is stored in a partition, it has to be flashed there, otherwise a runtime error occurs. If you want to use initialization data stored in a partition, go to ``menuconfig`` and enable the option :menuitem:`CONFIG_ESP_PHY_INIT_DATA_IN_PARTITION`.

API Reference
-------------

.. include-build-file:: inc/esp_phy_init.inc
.. include-build-file:: inc/esp_phy_cert_test.inc
