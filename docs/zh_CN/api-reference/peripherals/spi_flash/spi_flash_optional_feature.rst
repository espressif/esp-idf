flash 的可选功能
================

:link_to_translation:`en:[English]`

有些功能并非所有的 ESP 芯片和 flash 芯片都支持，具体功能可参阅以下列表：

- :ref:`auto-suspend-intro`

- :ref:`flash-unique-id`

- :ref:`high-performance-mode`

- :ref:`32-bit-flash-doc`

- :ref:`oct-flash-doc`

.. note::

    使用上述 flash 可选功能时，除了 ESP 芯片自身的功能以及所选的 ESP-IDF 版本，还需确保所用的 flash 芯片支持下列功能：

    - 如果使用乐鑫官方的模组或 SiP，请查阅 `技术规格书 <https://www.espressif.com/zh-hans/support/download/documents/modules>`__，确保其具备上述功能。也可以联系 `乐鑫商务部 <https://www.espressif.com/zh-hans/contact-us/sales-questions>`_，咨询具备此类功能的相关产品。

    - 如果使用其他厂商的 flash 芯片来自行组装模组，并且需要应用上述功能，请联系供应商确认所用芯片支持上述功能，并确保厂商会持续量产所用的芯片。

.. attention::

    本文档仅表明 ESP-IDF 代码支持 flash 芯片的部分功能，乐鑫官方并未对这些芯片进行全面的稳定性认证。如果使用其他厂商生产的 flash 芯片来组建硬件，即便这些芯片具备上述功能，也需要自行验证其可靠性。

.. _auto-suspend-intro:

自动挂起和恢复
--------------

目前仅有 ESP32-S3、ESP32-C2、ESP32-C3、ESP32-C6 及 ESP32-H2 支持此功能。

未来 ESP32-P4 可能会添加此功能。

.. only:: SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND

    支持此功能的 flash 芯片列表：

    1. XM25QxxC 系列
    2. GD25QxxE 系列
    3. FM25Q32

    .. attention::

        自动挂起功能有多重限制，在启用此功能前请务必先行阅读 :ref:`auto-suspend`。

.. _flash-unique-id:

flash 的唯一 ID
---------------

所有乐鑫芯片均支持此功能。

flash 的唯一 ID 并不等同于 flash ID，每个 flash 设备都有一个 64 位的唯一 ID。读取这个唯一 ID (4Bh) 的指令将访问出厂设置的 64 位只读数字，从而识别出每台设备。并非所有 flash 供应商都支持此功能，如果在没有此功能的芯片上读取该唯一 ID，则可能导致不确定的后果。

支持此功能的 flash 芯片列表：

1. ISSI
2. GD
3. TH
4. FM
5. Winbond
6. XMC
7. BOYA

.. _high-performance-mode:

QSPI flash 芯片的高性能模式
---------------------------

目前仅有 ESP32-S3 支持此功能。

未来 ESP32-S2, ESP32-C3, ESP32-C6, ESP32-H2 及 ESP32-P4 可能会添加此功能。

.. note::

    本节适用于 QSPI flash 芯片。ESP 芯片上使用的八线 flash 默认支持高性能模式。有关支持高性能模式的所有八线 flash 芯片，请参考 :ref:`oct-flash-doc`。

.. only:: esp32s3

    高性能模式 (HPM) 是指 SPI1 和 flash 芯片在高频率（通常是大于 80 MHz）下工作。

    一般来说，SPI flash 芯片有超过三种方式可以实现高性能模式。第一，由寄存器中的 dummy 周期 (DC) 位控制；第二，由其他位（如 HPM 位）控制；第三，由某些特殊命令进行控制。由于实现高性能模式的策略不同，驱动程序需要针对每款芯片显式添加支持。

    .. attention::

        简单通过几种策略来涵盖所有情况并不现实，因此驱动程序需要为所有使用高性能模式的芯片显式添加支持。如果尝试使用未列在 :ref:`hpm_dc_support_list` 中的 flash 芯片，可能会导致一些错误。因此，尝试使用支持列表之外的 flash 芯片前，请先进行测试。

    此外，若 flash 芯片采用 `DC 调整` 策略，经软件复位后，flash 芯片将保持在 DC 与默认值不同的状态。调整 DC 以在应用中以更高频率运行的高性能模式的子模式称为 `HPM-DC` 。`HPM-DC` 功能需要在引导加载程序中启用 `DC Aware` 功能，否则，不同的 DC 值将导致二级引导加载程序在复位后无法启动。

    启用高性能模式的方法：

    1. 取消选择 :ref:`CONFIG_ESPTOOLPY_OCT_FLASH` 和 :ref:`CONFIG_ESPTOOLPY_FLASH_MODE_AUTO_DETECT`。高性能模式不用于八线 flash，启用相关选项可能会导致无法使用高性能模式。

    2. 启用 ``CONFIG_SPI_FLASH_HPM_ENA`` 选项。

    3. 将 flash 频率切换到高性能模式下的频率。例如，``CONFIG_ESPTOOLPY_FLASHFREQ_120M``。

    4. 检查引导加载程序是否支持 `DC Aware` 功能，选择正确的 `HPM-DC` 配置选项（在 ``CONFIG_SPI_FLASH_HPM_DC`` 选项下）。

        - 如果引导加载程序支持 `DC Aware`，请选择 ``CONFIG_SPI_FLASH_HPM_DC_AUTO``，从而使用采用 `DC 调整` 策略的 flash 芯片。

        - 如果引导加载程序不支持 `DC Aware`，请选择 ``CONFIG_SPI_FLASH_HPM_DC_DISABLE``，避免不支持 `DC Aware` 的引导加载程序运行 `HPM-DC` 模式造成不良后果。但如果选择了 ``CONFIG_SPI_FLASH_HPM_DC_DISABLE``，请避免使用采用 `DC 调整` 策略的 flash 芯片。请参阅以下列表，了解采用了 DC 策略的 flash 模组。

    通过以下方式检查引导加载程序是否支持 `DC Aware`：

    - 如果启动了新项目，建议通过在引导加载程序菜单中选择 :ref:`CONFIG_BOOTLOADER_FLASH_DC_AWARE` 选项来启用 `DC Aware`。请注意，此选项无法通过 OTA 修改，因为支持代码在引导加载程序中。

    - 如果想在现有项目中通过 OTA 来更新 `HPM-DC` 配置选项，请检查用于构建引导加载程序的 sdkconfig 文件（升级 ESP-IDF 版本可能会使此文件与用于构建引导加载程序的文件不同）：

        - 对于最新版本的 ESP-IDF（v5.2 及以上），如果选择了 :ref:`CONFIG_BOOTLOADER_FLASH_DC_AWARE`，则引导加载程序支持 `DC Aware`。

        - 对于某些范围内的 ESP-IDF 版本（v4.4.4+、v5.0+ 和 v5.1+），如果选择了 ``CONFIG_ESPTOOLPY_FLASHFREQ_120M``，则引导加载程序支持 `DC Aware`。此时，可启用 :ref:`CONFIG_BOOTLOADER_FLASH_DC_AWARE` 进行确认（不会影响实际应用中的引导加载程序）。

        - 对于低于 v4.4.4 的 ESP-IDF 版本，引导加载程序不支持 `DC Aware`。

    .. _hpm_dc_support_list:

    支持高性能模式的四线 flash 列表
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    不需要 HPM-DC 的 flash 芯片：

    1. GD25Q64C (ID: 0xC84017)
    2. GD25Q32C (ID: 0xC84016)
    3. ZB25VQ32B (ID: 0x5E4016)
    4. GD25LQ255E (ID: 0xC86019)

    以下 flash 芯片也具备高性能模式，但需要引导加载程序支持 `DC Aware`：

    1. GD25Q64E (ID: 0xC84017)
    2. GD25Q128E (ID: 0xC84018)
    3. XM25QH64C (ID: 0x204017)
    4. XM25QH128C (ID: 0x204018)


.. _32-bit-flash-doc:

QSPI flash 芯片的 32 位地址支持
-------------------------------

所有乐鑫芯片均支持此功能（见下文的应用限制章节）。

.. note::

    本节适用于 QSPI flash 芯片。八线 flash 芯片的 32 位地址支持属于八线 flash 支持的一部分。有关支持此功能的八线 flash 芯片列表，请参考 :ref:`oct-flash-doc`。

乐鑫芯片所用的大多数 NOR flash 芯片使用 24 位地址，可以覆盖 16 MB 的内存。但对于更大的内存（通常大于或等于 32 MB），flash 使用 32 位地址来访问超过 16 MB 内存的区域。但是，32 位地址的芯片具有厂商特定的命令，因此需要逐一支持这些芯片。

支持此功能的 flash 芯片列表：

1. W25Q256
2. GD25Q256

限制
^^^^

.. only:: not SOC_SPI_MEM_SUPPORT_CACHE_32BIT_ADDR_MAP

    .. important::

        上述超过 16 MB 内存的 flash 区域只能用于 ``数据保存``，如文件系统。

        将数据或指令映射到 32 位物理地址空间（以便由 CPU 访问）需要 MMU 的支持。但 {IDF_TARGET_NAME} 并不支持此功能。目前只有 ESP32-S3 和 ESP32-P4 支持此功能。

.. only:: SOC_SPI_MEM_SUPPORT_CACHE_32BIT_ADDR_MAP

    默认情况下，上述超过 16 MB 内存的 flash 区域可用于 ``数据保存``，如文件系统。

    此外，要将数据或指令映射到 32 位物理地址空间（以便由 CPU 访问），请启用配置 ``IDF_EXPERIMENTAL_FEATURES`` 和 ``BOOTLOADER_CACHE_32BIT_ADDR_QUAD_FLASH``。

    请注意，此选项为实验性选项，无法在所有 flash 芯片上稳定使用，详情请咨询 `乐鑫商务部 <https://www.espressif.com/zh-hans/contact-us/sales-questions>`_。

.. _oct-flash-doc:

OPI flash 支持
--------------

目前仅有 ESP32-S3 支持此功能。

OPI flash 是指 flash 芯片支持八线外设接口，具有八个 I/O 管脚。不同的八线 flash 有不同的配置和命令，因此必须仔细检查支持列表。

.. only:: esp32s3

    .. note::

        若想为具有不同 flash 和 PSRAM 的开发板配置 menuconfig，请参考 :ref:`flash-psram-configuration`。

    支持此功能的 flash 芯片列表：

    1. MX25UM25645G
    2. MX25UM12345G
