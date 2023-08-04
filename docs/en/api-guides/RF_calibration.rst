RF Calibration
==============

:link_to_translation:`zh_CN:[中文]`

{IDF_TARGET_NAME} supports three RF calibration methods during RF initialization:

1. Partial calibration

2. Full calibration

3. No calibration

Partial Calibration
-------------------

During RF initialization, the partial calibration method is used by default for RF calibration. It is done based on the full calibration data which is stored in the NVS. To use this method, please go to ``menuconfig`` and enable :ref:`CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE`.

Full Calibration
----------------

Full calibration is triggered in the following conditions:

1. NVS does not exist.

2. The NVS partition to store calibration data has been erased.

3. Hardware MAC address has changed.

4. PHY library version has changed.

5. The RF calibration data loaded from the NVS partition is broken.

Full calibration takes 100 ms longer than the partial calibration method. If boot duration is not of critical importance to the application, the full calibration method is recommended. To switch to the full calibration method, go to ``menuconfig`` and disable :ref:`CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE`. If you use the default method of RF calibration, there are two ways to add the function of triggering full calibration as a last-resort remedy.

1. Erase the NVS partition if you do not mind all of the data stored in the NVS partition is erased. That is indeed the easiest way.

2. Call API :cpp:func:`esp_phy_erase_cal_data_in_nvs` before initializing Wi-Fi and Bluetooth®/Bluetooth Low Energy based on some conditions (e.g., an option provided in some diagnostic mode). In this case, only the PHY namespace of the NVS partition is erased.

No Calibration
---------------

The no calibration method is only used when the device wakes up from Deep-sleep mode.

PHY Initialization Data
-----------------------

The PHY initialization data is used for RF calibration. There are two ways to get the PHY initialization data.

One is to use the default initialization data which is located in the header file :idf_file:`components/esp_phy/{IDF_TARGET_PATH_NAME}/include/phy_init_data.h`. It is embedded into the application binary after compiling and then stored into read-only memory (DROM). To use the default initialization data, please go to ``menuconfig`` and disable :ref:`CONFIG_ESP_PHY_INIT_DATA_IN_PARTITION`.

An alternative is to store the initialization data in a PHY data partition. A PHY data partition is included in the default partition table. However, when using a custom partition table, please ensure that a PHY data partition (type: ``data``, subtype: ``phy``) is included in the custom partition table. Whether you are using a custom partition table or the default partition table, if initialization data is stored in a partition, it has to be flashed there, otherwise a runtime error occurs. If you want to use initialization data stored in a partition, go to ``menuconfig`` and enable the option :ref:`CONFIG_ESP_PHY_INIT_DATA_IN_PARTITION`.

API Reference
-------------

.. include-build-file:: inc/esp_phy_init.inc
.. include-build-file:: inc/esp_phy_cert_test.inc
