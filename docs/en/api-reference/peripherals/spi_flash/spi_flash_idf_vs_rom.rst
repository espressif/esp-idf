SPI Flash API ESP-IDF Version vs Chip-ROM Version
===================================================

.. toctree::
   :maxdepth: 1

There is a set of SPI Flash drivers in Chip-ROM which you can use by enabling  :ref:`CONFIG_SPI_FLASH_ROM_IMPL`. Most of the ESP-IDF SPI Flash driver code are in internal RAM, therefore enabling this option frees some internal RAM usage. Note if you enable this option, this means some SPI Flash driver features and bugfixes that are done in ESP-IDF might not be included in the Chip-ROM version.


Feature Supported by ESP-IDF but Not in Chip-ROM
------------------------------------------------

.. list::

    - Octal Flash chip support. See :ref:`oct-flash-doc` for details.
    - 32-bit-address support for GD25Q256. Note this feature is an optional feature, please do read :ref:`32-bit-flash-doc` for details.
    - TH Flash chip support.
    - Kconfig option :ref:`CONFIG_SPI_FLASH_CHECK_ERASE_TIMEOUT_DISABLED`.
    - :ref:`CONFIG_SPI_FLASH_VERIFY_WRITE`, enabling this option helps you detect bad writing.
    - :ref:`CONFIG_SPI_FLASH_LOG_FAILED_WRITE`, enabling this option prints the bad writing.
    - :ref:`CONFIG_SPI_FLASH_WARN_SETTING_ZERO_TO_ONE`, enabling this option checks if you are writing zero to one.
    - :ref:`CONFIG_SPI_FLASH_DANGEROUS_WRITE`, enabling this option checks for flash programming to certain protected regions like bootloader, partition table or application itself.
    - :ref:`CONFIG_SPI_FLASH_ENABLE_COUNTERS`, enabling this option to collect performance data for ESP-IDF SPI Flash driver APIs.
    - :ref:`CONFIG_SPI_FLASH_AUTO_SUSPEND`, enabling this option to automatically suspend / resume a long Flash operation when short Flash operation happens. Note this feature is an optional feature, please do read :ref:`auto-suspend-intro` for more limitations.

Bugfixes Introduced in ESP-IDF but Not in Chip-ROM
--------------------------------------------------

.. list::

    - Detected Flash physical size correctly, for larger than 256MBit Flash chips. (Commit ID: b4964279d44f73cce7cfd5cf684567fbdfd6fd9e)
    :esp32c3: - Improved SPI1 cs setup timing, otherwise issue may happen on ZB32Q128. (Commit ID: 08f1bbe0c75382f1702e40c941e93314285105d4)
    :esp32s3: - Fixed issue that 4-line Flash encryption cannot work normally when 8-line PSRAM enabled. (Commit ID: 683d92bc884e0f2a7eebea40a551cf05f0c28256)
    :esp32s2: - Fixed issue that only 4MB virtual address ranges can be mapped to read-only data on Flash.
    :esp32s3: - Fixed issue that only 128KB virtual address ranges can be mapped to instructions on Flash.
    :esp32s3: - Fixed issue that only 16MB virtual address ranges can be mapped to read-only data on Flash.
    :esp32c3: - Fixed issue that only 128KB virtual address ranges can be mapped to instructions on Flash.
    :esp32c2: - Fixed issue that only at most 128KB virtual address ranges can be mapped to instructions on Flash.
