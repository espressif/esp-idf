SPI Flash API (Legacy)
========================

Overview
--------

This is the readme for the APIs before IDF v4.0. Enable the kconfig option ``SPI_FLASH_USE_LEGACY_IMPL`` to use the
legacy implementation.

The spi_flash component contains API functions related to reading, writing, erasing, memory mapping for data in the external SPI flash. The spi_flash component also has higher-level API functions which work with partitions defined in the :doc:`partition table </api-guides/partition-tables>`.

Note that all the functionality is limited to the "main" SPI flash chip, the same SPI flash chip from which programs are runs. For ``spi_flash_*`` functions, this is a software limitation. The underlying ROM functions which work with SPI flash do not have provisions for working with flash chips attached to SPI peripherals other than SPI0.

SPI flash access API
--------------------

This is the set of API functions for working with data in flash:

- :cpp:func:`spi_flash_read` reads data from flash to RAM
- :cpp:func:`spi_flash_write` writes data from RAM to flash
- :cpp:func:`spi_flash_erase_sector` erases individual sectors of flash
- :cpp:func:`spi_flash_erase_range` erases ranges of addresses in flash
- :cpp:func:`spi_flash_get_chip_size` returns flash chip size, in bytes, as configured in menuconfig

Generally, try to avoid using the raw SPI flash functions in favor of :ref:`partition-specific functions <flash-partition-apis>`.

SPI Flash Size
--------------

The SPI flash size is configured by writing a field in the software bootloader image header, flashed at offset 0x1000.

By default, the SPI flash size is detected by esptool.py when this bootloader is written to flash, and the header is updated with the correct size. Alternatively, it is possible to generate a fixed flash size by setting :envvar:`CONFIG_ESPTOOLPY_FLASHSIZE` in project configuration.

If it is necessary to override the configured flash size at runtime, it is possible to set the ``chip_size`` member of the ``g_rom_flashchip`` structure. This size is used by ``spi_flash_*`` functions (in both software & ROM) to check the bounds.

Concurrency Constraints
-----------------------

Because the SPI flash is also used for firmware execution via the instruction & data caches, these caches must be disabled while reading/writing/erasing. This means that both CPUs must be running code from IRAM and must only be reading data from DRAM while flash write operations occur.

If you use the API functions documented here, then these constraints are applied automatically and transparently. However, note that it will have some performance impact on other tasks in the system.

For differences between IRAM, DRAM, and flash cache, please refer to the :ref:`application memory layout <memory-layout>` documentation.

To avoid reading flash cache accidentally, when one CPU initiates a flash write or erase operation, the other CPU is put into a blocked state, and all non-IRAM-safe interrupts are disabled on both CPUs until the flash operation completes.

If one CPU initiates a flash write or erase operation, the other CPU is put into a blocked state to avoid reading flash cache accidentally. All interrupts not safe for IRAM are disabled on both CPUs until the flash operation completes.

.. _iram-safe-interrupt-handlers:

IRAM-Safe Interrupt Handlers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you have an interrupt handler that you want to execute while a flash operation is in progress (for example, for low latency operations), set the ``ESP_INTR_FLAG_IRAM`` flag when the :doc:`interrupt handler is registered </api-reference/system/intr_alloc>`.

You must ensure that all data and functions accessed by these interrupt handlers, including the ones that handlers call, are located in IRAM or DRAM.

Use the ``IRAM_ATTR`` attribute for functions::

    #include "esp_attr.h"

    void IRAM_ATTR gpio_isr_handler(void* arg)
    {
        // ...
    }

Use the ``DRAM_ATTR`` and ``DRAM_STR`` attributes for constant data::

    void IRAM_ATTR gpio_isr_handler(void* arg)
    {
       const static DRAM_ATTR uint8_t INDEX_DATA[] = { 45, 33, 12, 0 };
       const static char *MSG = DRAM_STR("I am a string stored in RAM");
    }

Note that knowing which data should be marked with ``DRAM_ATTR`` can be hard, the compiler will sometimes recognize that a variable or expression is constant (even if it is not marked ``const``) and optimize it into flash, unless it is marked with ``DRAM_ATTR``.

If a function or symbol is not correctly put into IRAM/DRAM, and the interrupt handler reads from the flash cache during a flash operation, it will cause a crash due to Illegal Instruction exception (for code which should be in IRAM) or garbage data to be read (for constant data which should be in DRAM).

.. _flash-partition-apis:

Partition table API
-------------------

ESP-IDF projects use a partition table to maintain information about various regions of SPI flash memory (bootloader, various application binaries, data, filesystems). More information on partition tables can be found :doc:`here </api-guides/partition-tables>`.

This component provides API functions to enumerate partitions found in the partition table and perform operations on them. These functions are declared in ``esp_partition.h``:

- :cpp:func:`esp_partition_find` checks a partition table for entries with specific type, returns an opaque iterator.
- :cpp:func:`esp_partition_get` returns a structure describing the partition for a given iterator.
- :cpp:func:`esp_partition_next` shifts the iterator to the next found partition.
- :cpp:func:`esp_partition_iterator_release` releases iterator returned by ``esp_partition_find``.
- :cpp:func:`esp_partition_find_first` - a convenience function which returns the structure describing the first partition found by ``esp_partition_find``.
- :cpp:func:`esp_partition_read`, :cpp:func:`esp_partition_write`, :cpp:func:`esp_partition_erase_range` are equivalent to :cpp:func:`spi_flash_read`, :cpp:func:`spi_flash_write`, :cpp:func:`spi_flash_erase_range`, but operate within partition boundaries.

.. note::
    Application code should mostly use these ``esp_partition_*`` API functions instead of lower level ``spi_flash_*`` API functions. Partition table API functions do bounds checking and calculate correct offsets in flash, based on data stored in a partition table.

SPI Flash Encryption
--------------------

It is possible to encrypt the contents of SPI flash and have it transparently decrypted by hardware.

Refer to the :doc:`Flash Encryption documentation </security/flash-encryption>` for more details.

Memory mapping API
------------------

ESP32 features memory hardware which allows regions of flash memory to be mapped into instruction and data address spaces. This mapping works only for read operations. It is not possible to modify contents of flash memory by writing to a mapped memory region.

Mapping happens in 64KB pages. Memory mapping hardware can map up to four megabytes of flash into data address space and up to 16 megabytes of flash into instruction address space. See the technical reference manual for more details about memory mapping hardware.

Note that some 64KB pages are used to map the application itself into memory, so the actual number of available 64KB pages may be less.

Reading data from flash using a memory mapped region is the only way to decrypt contents of flash when :doc:`flash encryption </security/flash-encryption>` is enabled. Decryption is performed at the hardware level.

Memory mapping API are declared in ``esp_spi_flash.h`` and ``esp_partition.h``:

- :cpp:func:`spi_flash_mmap` maps a region of physical flash addresses into instruction space or data space of the CPU.
- :cpp:func:`spi_flash_munmap` unmaps previously mapped region.
- :cpp:func:`esp_partition_mmap` maps part of a partition into the instruction space or data space of the CPU.

Differences between :cpp:func:`spi_flash_mmap` and :cpp:func:`esp_partition_mmap` are as follows:

- :cpp:func:`spi_flash_mmap` must be given a 64KB aligned physical address.
- :cpp:func:`esp_partition_mmap` may be given any arbitrary offset within the partition, it will adjust the returned pointer to mapped memory as necessary

Note that since memory mapping happens in 64KB blocks, it may be possible to read data outside of the partition provided to ``esp_partition_mmap``.
