RF calibration
==============

{IDF_TARGET_NAME} supports three RF calibration methods during RF initialization:

1. Partial calibration

2. Full calibration

3. No calibration

Partial calibration
-------------------
During RF initialization, the partial calibration method is used by default for RF calibration.
It is done based on the full calibration data which is stored in the NVS.
To use this method, please go to ``menuconfig`` and enable :ref:`CONFIG_ESP32_PHY_CALIBRATION_AND_DATA_STORAGE`.

Full calibration
----------------
Full calibration is triggered in the follwing conditions:

1. NVS does not exist.

2. The NVS partition to store calibration data is erased.

3. Hardware MAC address is changed.

4. PHY library version is changed.

5. The RF calibration data loaded from the NVS partition is broken.

It takes about 100ms more than partial calibration.
If boot duration is not critical, it is suggested to use the full calibration method.
To switch to the full calibration method, go to ``menuconfig`` and disable :ref:`CONFIG_ESP32_PHY_CALIBRATION_AND_DATA_STORAGE`.
If you use the default method of RF calibration, there are two ways to add the function of triggering full calibration as a last-resort remedy.

1. Erase the NVS partition if you don't mind all of the data stored in the NVS partition is erased. That is indeed the easiest way.

2. Call API :cpp:func:`esp_phy_erase_cal_data_in_nvs` before initializing WiFi and BT/BLE based on some conditions (e.g. an option provided in some diagnostic mode).
   In this case, only phy namespace of the NVS partition is erased.

No calibration
---------------
No calibration method is only used when the device wakes up from deep sleep.

PHY initialization data
-----------------------
The PHY initialization data is used for RF calibration.
There are two ways to get the PHY initialization data.

One is the default initialization data which is located in the header file :idf_file:`components/esp_wifi/esp32/include/phy_init_data.h`.

It is embedded into the application binary after compiling and then stored into read-only memory (DROM).
To use the default initialization data, please go to ``menuconfig`` and disable :ref:`CONFIG_ESP32_PHY_INIT_DATA_IN_PARTITION`.

Another is the initialization data which is stored in a partition.
When using a custom partition table, make sure that PHY data partition is included (type: `data`, subtype: `phy`).
With default partition table, this is done automatically.
If initialization data is stored in a partition, it has to be flashed there, otherwise runtime error will occur.
To switch to the initialization data stored in a partition, go to ``menuconfig`` and enable :ref:`CONFIG_ESP32_PHY_INIT_DATA_IN_PARTITION`.
