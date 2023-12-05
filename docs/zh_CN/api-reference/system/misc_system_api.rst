杂项系统 API
=========================

:link_to_translation:`en:[English]`

{IDF_TARGET_BASE_MAC_BLOCK: default="BLK1", esp32="BLK0"}
{IDF_TARGET_CPU_RESET_DES: default="CPU 复位", esp32="两个 CPU 均复位", esp32s3="两个 CPU 均复位", esp32p4="两个 CPU 均复位"}

软件复位
------------

函数 :cpp:func:`esp_restart` 用于执行芯片的软件复位。调用此函数时，程序停止执行，{IDF_TARGET_CPU_RESET_DES}，应用程序由 bootloader 加载并重启。

函数 :cpp:func:`esp_register_shutdown_handler` 用于注册复位前会自动调用的例程（复位过程由 :cpp:func:`esp_restart` 函数触发），这与 ``atexit`` POSIX 函数的功能类似。

复位原因
------------

ESP-IDF 应用程序启动或复位的原因有多种。调用 :cpp:func:`esp_reset_reason` 函数可获取最近一次复位的原因。复位的所有可能原因，请查看 :cpp:type:`esp_reset_reason_t` 中的描述。

堆内存
---------

ESP-IDF 中有两个与堆内存相关的函数：

* 函数 :cpp:func:`esp_get_free_heap_size` 用于查询当前可用的堆内存大小。
* 函数 :cpp:func:`esp_get_minimum_free_heap_size` 用于查询整个过程中可用的最小堆内存大小（例如应用程序生命周期内可用的最小堆内存大小）。

请注意，ESP-IDF 支持功能不同的的多个堆。上文中函数返回的堆内存大小可使用 ``malloc`` 函数族来进行分配。有关堆内存的更多信息，请参阅 :doc:`堆内存分配 <mem_alloc>`。

.. _MAC-Address-Allocation:

MAC 地址
-----------

以下 API 用于查询和自定义支持的网络接口（如 Wi-Fi、蓝牙、以太网）的 MAC 地址。

要获取特定接口（如 Wi-Fi、蓝牙、以太网）的 MAC 地址，请调用函数 :cpp:func:`esp_read_mac`。

在 ESP-IDF 中，各个网络接口的 MAC 地址是根据单个 **基准 MAC 地址 (Base MAC address)** 计算出来的。默认情况下使用乐鑫指定的基准 MAC 地址，该基准地址在产品生产过程中已预烧录至 {IDF_TARGET_NAME} eFuse。

.. only:: not esp32s2

    .. list-table::
        :widths: 20 40 40
        :header-rows: 1

        * - 接口
          - MAC 地址（默认 4 个全局地址）
          - MAC 地址（2 个全局地址）
        * - Wi-Fi Station
          - base_mac
          - base_mac
        * - Wi-Fi SoftAP
          - base_mac 最后一组字节后加 1
          - :ref:`本地 MAC <local-mac-addresses>` （由 Wi-Fi Station MAC 生成）
        * - 蓝牙
          - base_mac 最后一组字节后加 2
          - base_mac 最后一组字节后加 1
        * - 以太网
          - base_mac 最后一组字节后加 3
          - :ref:`本地 MAC <local-mac-addresses>` （由蓝牙 MAC 生成）

    .. note::

        :ref:`配置选项 <CONFIG_{IDF_TARGET_CFG_PREFIX}_UNIVERSAL_MAC_ADDRESSES>` 配置了乐鑫提供的全局 MAC 地址的数量。

.. only:: esp32s2

    .. list-table::
        :widths: 20 40 40
        :header-rows: 1

        * - 接口
          - MAC 地址（默认 2 个全局地址）
          - MAC 地址（1 个全局地址）
        * - Wi-Fi Station
          - base_mac
          - base_mac
        * - Wi-Fi SoftAP
          - base_mac 最后一组字节后加 1
          - :ref:`本地 MAC <local-mac-addresses>` （由 Wi-Fi Station MAC 生成）
        * - 以太网
          - :ref:`本地 MAC <local-mac-addresses>` （由 Wi-Fi SoftAP MAC 生成）
          - :ref:`本地 MAC <local-mac-addresses>` （在 base_mac 最后一组字节后加 1 生成，不推荐）

    .. note::

        :ref:`配置选项 <CONFIG_{IDF_TARGET_CFG_PREFIX}_UNIVERSAL_MAC_ADDRESSES>` 配置了乐鑫提供的全局 MAC 地址的数量。

.. only:: not SOC_EMAC_SUPPORTED

    .. note::

      {IDF_TARGET_NAME} 内部未集成以太网 MAC 地址，但仍可以计算得出该地址。不过，以太网 MAC 地址只能与外部以太网接口（如 SPI 以太网设备）一起使用，具体请参阅 :doc:`/api-reference/network/esp_eth`。

自定义接口 MAC
^^^^^^^^^^^^^^^^

有时用户可能需要自定义 MAC 地址，这些地址并不由基准 MAC 地址生成。如需设置自定义接口 MAC 地址，请使用 :cpp:func:`esp_iface_mac_addr_set` 函数。该函数用于覆盖由基准 MAC 地址设置（或尚未设置）的接口 MAC 地址。一旦设置某个接口 MAC 地址，即使更改基准 MAC 地址，也不会对其产生影响。

自定义基准 MAC
^^^^^^^^^^^^^^^

乐鑫已将默认的基准 MAC 地址预烧录至 eFuse {IDF_TARGET_BASE_MAC_BLOCK} 中。如需设置自定义基准 MAC 地址，请在初始化任一网络接口或调用 :cpp:func:`esp_read_mac` 函数前调用 :cpp:func:`esp_base_mac_addr_set` 函数。自定义基准 MAC 地址可以存储在任何支持的存储设备中（例如 flash、NVS）。

分配自定义基准 MAC 地址时，应避免 MAC 地址重叠。请根据上面的表格配置选项 :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_UNIVERSAL_MAC_ADDRESSES`，设置可从自定义基准 MAC 地址生成的有效全局 MAC 地址。

.. note::

    也可以调用函数 :cpp:func:`esp_netif_set_mac`，在网络初始化后设置网络接口使用的特定 MAC。但建议使用此处介绍的自定义基准 MAC 地址的方法，以避免原始 MAC 地址在更改前短暂出现在网络上。


eFuse 中的自定义 MAC 地址
@@@@@@@@@@@@@@@@@@@@@@@@@@@

ESP-IDF 提供了 :cpp:func:`esp_efuse_mac_get_custom` 函数，从 eFuse 读取自定义 MAC 地址时，调用该函数将从 eFuse BLK3 加载 MAC 地址。用户也可以调用 :cpp:func:`esp_read_mac` 函数，此时需使用 ``ESP_MAC_EFUSE_CUSTOM`` 参数。:cpp:func:`esp_efuse_mac_get_custom` 函数假定自定义基准 MAC 地址的存储格式如下：

.. only:: esp32

    .. list-table::
        :widths: 20 15 20 45
        :header-rows: 1

        * - 字段
          - 比特数
          - 比特范围
          - 说明
        * - Version
          - 8
          - 191:184
          - 0：无效；其他：有效
        * - Reserved
          - 128
          - 183:56
          -
        * - MAC address
          - 48
          - 55:8
          -
        * - MAC address CRC
          - 8
          - 7:0
          - CRC-8-CCITT，多项式 0x07

    .. note::

        如果启用了 3/4 编码方案，则必须同时烧写该块中的所有 eFuse 字段。

.. only:: not esp32

    .. list-table::
        :widths: 30 30 30
        :header-rows: 1

        * - 字段
          - 比特数
          - 比特范围
        * - MAC address
          - 48
          - 200:248

    .. note::

        eFuse BLK3 在烧写时使用 RS 编码，这意味着必须同时烧写该块中的所有 eFuse 字段。

调用 :cpp:func:`esp_efuse_mac_get_custom` 或 :cpp:func:`esp_read_mac` 函数获得自定义 eFuse MAC 地址后，请将此 MAC 地址设置为基准 MAC 地址。有以下两种方法：

1. 使用原有 API：调用 :cpp:func:`esp_base_mac_addr_set`。
2. 使用新 API：调用 :cpp:func:`esp_iface_mac_addr_set`，此时需使用 ``ESP_MAC_BASE`` 参数。


.. _local-mac-addresses:

本地 MAC 地址和全局 MAC 地址
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

在 {IDF_TARGET_NAME} 中，乐鑫已预烧录足够数量的有效乐鑫全局 MAC 地址，供所有内部接口使用。上文中的表格已经介绍了如何根据基准 MAC 地址计算出具体接口的 MAC 地址。

当使用自定义 MAC 地址时，可能并非所有接口都能被分配到一个全局 MAC 地址。此时，接口会被分配一个本地 MAC 地址。请注意，这些地址仅用于单个本地网络。

本地 MAC 地址和全局 MAC 地址的定义，请参见 `此处 <https://en.wikipedia.org/wiki/MAC_address#Universal_vs._local_(U/L_bit)>`_。

内部调用函数 :cpp:func:`esp_derive_local_mac`，可从全局 MAC 地址生成本地 MAC 地址。具体流程如下：

1. 在全局 MAC 地址的第一个字节组中设置 U/L 位（位值为 0x2），创建本地 MAC 地址。
2. 如果该位已存在于全局 MAC 地址中（即现有的“全局”MAC 地址实际上已经是本地 MAC 地址），则本地 MAC 地址的第一个字节组与 0x4 异或。

芯片版本
------------

:cpp:func:`esp_chip_info` 函数用于填充 :cpp:class:`esp_chip_info_t` 结构体中的芯片信息，包括芯片版本、CPU 数量和芯片中已启用功能的位掩码。

.. _idf-version-h:

SDK 版本
----------

调用函数 :cpp:func:`esp_get_idf_version` 可返回一个字符串，该字符串包含了用于编译应用程序的 ESP-IDF 版本，与构建系统中通过 ``IDF_VER`` 变量所获得的值相同。该版本字符串的格式即 ``git describe`` 命令的运行结果。

也有其它的版本宏可用于在构建过程中获取 ESP-IDF 版本，它们可根据 ESP-IDF 版本启用或禁用部分程序。

* :c:macro:`ESP_IDF_VERSION_MAJOR`、 :c:macro:`ESP_IDF_VERSION_MINOR` 和 :c:macro:`ESP_IDF_VERSION_PATCH` 分别被定义为代表主要版本、次要版本和补丁版本的整数。

* :c:macro:`ESP_IDF_VERSION_VAL` 和 :c:macro:`ESP_IDF_VERSION` 可在确认版本时使用：

  .. code-block:: c

      #include "esp_idf_version.h"

      #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)
          // 启用 ESP-IDF v4.0 中的功能
      #endif


.. _app-version:

应用程序版本
-------------

应用程序版本存储在 :cpp:class:`esp_app_desc_t` 结构体中。该结构体位于 DROM 扇区，有一个从二进制文件头部计算的固定偏移值。该结构体位于 :cpp:class:`esp_image_header_t` 和 :cpp:class:`esp_image_segment_header_t` 结构体之后。字段 Version 类型为字符串，最大长度为 32 字节。

若需手动设置版本，需要在项目的 ``CMakeLists.txt`` 文件中设置 ``PROJECT_VER`` 变量，即在 ``CMakeLists.txt`` 文件中，在包含 ``project.cmake`` 之前添加 ``set(PROJECT_VER "0.1.0.1")``。

如果设置了 :ref:`CONFIG_APP_PROJECT_VER_FROM_CONFIG` 选项，则将使用 :ref:`CONFIG_APP_PROJECT_VER` 的值。否则，如果在项目中未设置 ``PROJECT_VER`` 变量，则该变量将从 ``$(PROJECT_PATH)/version.txt`` 文件（若有）中检索，或使用 git 命令 ``git describe`` 检索。如果两者都不可用，则 ``PROJECT_VER`` 将被设置为 “1”。应用程序可通过调用 :cpp:func:`esp_app_get_description` 或 :cpp:func:`esp_ota_get_partition_description` 函数来获取应用程序的版本信息。

API 参考
-------------

.. include-build-file:: inc/esp_system.inc
.. include-build-file:: inc/esp_idf_version.inc
.. include-build-file:: inc/esp_mac.inc
.. include-build-file:: inc/esp_chip_info.inc
.. include-build-file:: inc/esp_cpu.inc
.. include-build-file:: inc/esp_app_desc.inc
