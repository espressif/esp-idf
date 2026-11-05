RF 校准
==============

:link_to_translation:`en:[English]`

{IDF_TARGET_NAME} 的 RF 校准行为由以下两个配置项共同决定：

1. :menuitem:`CONFIG_ESP_PHY_CALIBRATION_MODE`：选择部分校准、全面校准或不校准。

2. :menuitem:`CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE`：控制是否在 RF 初始化过程中自动加载和保存 NVS 中的 PHY 校准数据。

RF 校准配置
-------------------

:menuitem:`CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE` 控制是否在 RF 初始化过程中自动使用 NVS 中的 PHY 校准数据：

- 启用该选项时，系统会从 NVS 加载校准数据，普通启动时的校准方式由 :menuitem:`CONFIG_ESP_PHY_CALIBRATION_MODE` 决定。从 Deep-sleep 模式唤醒时，无论配置了哪种校准方式，系统都会优先选择不校准。如果 NVS 校准数据不存在或无效，系统会改为执行全面校准，并尝试将新数据存入 NVS。
- 禁用该选项时，:menuitem:`CONFIG_ESP_PHY_CALIBRATION_MODE` 不可配置，系统不自动加载或保存 NVS 校准数据，并始终执行全面校准。

启用 NVS 校准数据存储时，应在初始化 Wi-Fi 或 Bluetooth®/Bluetooth Low Energy 前调用 :cpp:func:`nvs_flash_init`。否则系统无法加载或保存校准数据，并会回退到全面校准。

部分校准
-------------------

当前版本中，部分校准和全面校准在功能上等效，默认使用全面校准。

全面校准
----------------

全面校准是 :menuitem:`CONFIG_ESP_PHY_CALIBRATION_MODE` 的默认选项。此外，在以下情况下，系统也会强制执行全面校准：

1. :menuitem:`CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE` 被禁用。

2. NVS 未初始化，或者存储校准数据的 NVS 分区或 ``phy`` 命名空间不存在。

3. 存储校准数据的 NVS 分区已擦除。

4. 硬件 MAC 地址变更。

5. PHY 库使用的校准数据格式版本变更。

6. 从 NVS 加载的 RF 校准数据长度不正确、校验失败或已经过期。

启用 :menuitem:`CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE` 时，如果 NVS 数据加载失败，系统会执行全面校准并尝试保存新数据；如果 NVS 数据加载成功，则只有在 PHY 库报告校准数据校验失败或已经过期时，才会更新 NVS 中的数据。

如需强制触发全面校准，可以使用以下方法：

1. 如果不介意擦除 NVS 分区中的所有数据，那么擦除 NVS 分区是最简单的方法。

2. 在初始化 Wi-Fi 和 Bluetooth®/Bluetooth Low Energy 前，根据某些条件（例如，在某些诊断模式下提供的选项）调用 API :cpp:func:`esp_phy_erase_cal_data_in_nvs`，此时仅会擦除 NVS 分区的 PHY 命名空间。

不校准
---------------

不校准仅适用于设备从 Deep-sleep 模式唤醒的场景。启用 :menuitem:`CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE` 后，从 Deep-sleep 模式唤醒时，系统会自动将校准方式设置为不校准，无需手动修改 :menuitem:`CONFIG_ESP_PHY_CALIBRATION_MODE`。如果此时无法从 NVS 加载有效的校准数据，系统仍会回退到全面校准。

不建议将 :menuitem:`CONFIG_ESP_PHY_CALIBRATION_MODE` 配置为不校准。这样配置后，只要 NVS 校准数据有效，普通冷启动也会跳过 RF 校准，可能导致 RF 性能出现问题。

配置组合总结
-------------------

当前版本中，部分校准和全面校准在功能上等效。以下总结中的 PARTIAL、FULL 和 NONE 表示传递给 PHY 库的校准模式：

- 启用 :menuitem:`CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE` 并选择 FULL：

  - 普通启动且 NVS 校准数据有效：使用 FULL。
  - 从 Deep-sleep 模式唤醒且 NVS 校准数据有效：使用 NONE。
  - NVS 校准数据无效或加载失败：使用 FULL，并尝试保存新校准数据。

- 启用 :menuitem:`CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE` 并选择 PARTIAL：当前版本中的行为与选择 FULL 相同。

- 启用 :menuitem:`CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE` 并选择 NONE：

  - 普通启动且 NVS 校准数据有效：使用 NONE。
  - 从 Deep-sleep 模式唤醒且 NVS 校准数据有效：使用 NONE。
  - NVS 校准数据无效或加载失败：使用 FULL，并尝试保存新校准数据。

- 禁用 :menuitem:`CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE`：

  - :menuitem:`CONFIG_ESP_PHY_CALIBRATION_MODE` 不可配置，其值会被忽略。
  - 所有场景均使用 FULL。
  - 不自动加载或保存 NVS 校准数据。
  - 从 Deep-sleep 模式唤醒时也使用 FULL。

PHY 初始化数据
-----------------------

PHY 初始化数据用于 RF 校准，通过以下两种方法，可以获取 PHY 初始化数据。

一是使用默认初始化数据，该数据位于头文件 :idf_file:`components/esp_phy/{IDF_TARGET_PATH_NAME}/include/phy_init_data.h` 中。默认初始化数据会在编译后嵌入到应用程序的二进制文件，然后存储在只读存储器 (DROM) 中。要使用默认的初始化数据，请前往 ``menuconfig`` 并禁用 :menuitem:`CONFIG_ESP_PHY_INIT_DATA_IN_PARTITION`。

二是将初始化数据存储在 PHY 数据分区中。默认的分区表中已经包含了 PHY 数据分区，但是使用自定义分区表时，请确保在自定义分区表中包含一个 PHY 数据分区（类型：``data``，子类型：``phy``）。无论使用自定义分区表还是默认分区表，只要初始化数据存储在分区中，就需要将其烧录到相应的分区中，否则运行时可能会出现错误。如果想要使用存储在分区中的初始化数据，请前往 ``menuconfig`` 并启用选项 :menuitem:`CONFIG_ESP_PHY_INIT_DATA_IN_PARTITION`。

API 参考
-------------

.. include-build-file:: inc/esp_phy_init.inc
.. include-build-file:: inc/esp_phy_cert_test.inc
