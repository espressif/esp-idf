SPI Flash API ESP-IDF 版本与芯片 ROM 版本的对比
===============================================

:link_to_translation:`en:[English]`

.. toctree::
   :maxdepth: 1

芯片 ROM 中有一组 SPI flash 驱动程序，可以通过启用 :ref:`CONFIG_SPI_FLASH_ROM_IMPL` 来使用。大多数 ESP-IDF SPI flash 驱动程序的代码都在内部 RAM 中，因此启用此选项可以释放一些内部 RAM 的占用量。请注意，如果启用此选项，一些 ESP-IDF 中的 SPI flash 驱动程序功能和错误修复可能不会体现在芯片 ROM 版本中。


ESP-IDF 支持但不包含在芯片 ROM 中的功能
---------------------------------------

.. list::

    - 八线 flash 芯片。详情请参阅 :ref:`oct-flash-doc`。
    - Flash 的 32 位地址。请注意，此功能为可选功能，详情请参阅 :ref:`32-bit-flash-doc`。
    - TH flash 芯片。
    - Kconfig 选项 :ref:`CONFIG_SPI_FLASH_CHECK_ERASE_TIMEOUT_DISABLED`。
    - :ref:`CONFIG_SPI_FLASH_VERIFY_WRITE`，启用此选项可检测错误写入。
    - :ref:`CONFIG_SPI_FLASH_LOG_FAILED_WRITE`，启用此选项会打印错误写入。
    - :ref:`CONFIG_SPI_FLASH_WARN_SETTING_ZERO_TO_ONE`，启用此选项会检查是否将 0 写入为 1。
    - :ref:`CONFIG_SPI_FLASH_DANGEROUS_WRITE`，启用此选项会检查是否对某些受保护的区域（如引导加载程序、分区表或应用程序本身）进行了 flash 编程。
    - :ref:`CONFIG_SPI_FLASH_ENABLE_COUNTERS`，启用此选项以收集 ESP-IDF SPI flash 驱动程序 API 的性能数据。
    - :ref:`CONFIG_SPI_FLASH_AUTO_SUSPEND`，启用此选项可在 flash 短时操作时自动挂起或恢复 flash 长时操作。请注意，此功能为可选功能，详情请参阅 :ref:`auto-suspend-intro`。
    :ESP_ROM_HAS_SPI_FLASH_MMAP and SOC_SPIRAM_XIP_SUPPORTED and not esp32s3: - :ref:`CONFIG_SPIRAM_XIP_FROM_PSRAM`，启用该选项后，可将外部 PSRAM 用作指令 cache 和只读数据 cache。但请注意，ROM 中的某些函数不支持此用法，而 ESP-IDF 提供了这些 ROM 函数的替代版本。
    :esp32s3: - 启用 :ref:`CONFIG_SPIRAM_FETCH_INSTRUCTIONS` 和 :ref:`CONFIG_SPIRAM_RODATA` 后，可将外部 PSRAM 用作指令 cache 和只读数据 cache。但请注意，ROM 中的某些函数不支持此用法，而 ESP-IDF 提供了这些 ROM 函数的替代版本。

在 ESP-IDF 中引入，但不包含在芯片 ROM 中的错误修复
--------------------------------------------------

.. list::

    - 正确检测到大于 256 MBit 的 flash 芯片的物理大小。（提交 ID: b4964279d44f73cce7cfd5cf684567fbdfd6fd9e）
    :esp32c3: - 改进了 SPI1 的 CS 建立时间，否则可能会在 ZB32Q128 上报错。（提交 ID: 08f1bbe0c75382f1702e40c941e93314285105d4）
    :esp32s3: - 修复了当启用 8 线 PSRAM 时 4 线 flash 加密无法正常运行的问题。（提交 ID: 683d92bc884e0f2a7eebea40a551cf05f0c28256）
    :esp32s2: - 修复了只能将 4 MB 虚拟地址范围映射到 flash 只读数据中的问题。
    :esp32s3: - 修复了只能将 128 KB 虚拟地址范围映射到 flash 指令中的问题。
    :esp32s3: - 修复了只能将 16 MB 虚拟地址范围映射到 flash 只读数据中的问题。
    :esp32c3: - 修复了只能将 128 KB 虚拟地址范围映射到 flash 指令中的问题。
    :esp32c2: - 修复了只能将最多 128 KB 虚拟地址范围映射到 flash 指令中的问题。
