SPI Flash API
=============

:link_to_translation:`zh_CN:[中文]`

Overview
--------
The spi_flash component contains API functions related to reading, writing,
erasing, memory mapping for data in the external flash. The spi_flash
component also has higher-level API functions which work with partitions
defined in the :doc:`partition table </api-guides/partition-tables>`.

Different from the API before IDF v4.0, the functionality is not limited to
the "main" SPI flash chip (the same SPI flash chip from which program runs).
With different chip pointers, you can access to external flashes chips on not
only SPI0/1 but also HSPI/VSPI buses.

.. note::

    Flash APIs after IDF v4.0 are no longer *atomic*. A writing operation
    during another on-going read operation, on the overlapped flash address,
    may cause the return data from the read operation to be partly same as
    before, and partly updated as new written.


Kconfig option :ref:`CONFIG_SPI_FLASH_USE_LEGACY_IMPL` can be used to switch
``spi_flash_*`` functions back to the implementation before IDF v4.0.
However, the code size may get bigger if you use the new API and the old API
the same time.

Encrypted reads and writes use the old implementation, even if
:ref:`CONFIG_SPI_FLASH_USE_LEGACY_IMPL` is not enabled. As such, encrypted
flash operations are only supported with the main flash chip (and not with
other flash chips on SPI1 with different CS).

Initializing a flash device
---------------------------

To use ``esp_flash_*`` APIs, you need to have a chip initialized on a certain
SPI bus.

1. Call :cpp:func:`spi_bus_initialize` to properly initialize an SPI bus.
   This functions initialize the resources (I/O, DMA, interrupts) shared
   among devices attached to this bus.

2. Call :cpp:func:`spi_bus_add_flash_device` to attach the flash device onto
   the bus. This allocates memory, and fill the members for the
   ``esp_flash_t`` structure. The CS I/O is also initialized here.

3. Call :cpp:func:`esp_flash_init` to actually communicate with the chip.
   This will also detect the chip type, and influence the following
   operations.

.. note:: Multiple flash chips can be attached to the same bus now. However,
          using ``esp_flash_*`` devices and ``spi_device_*`` devices on the
          same SPI bus is not supported yet.

SPI flash access API
--------------------

This is the set of API functions for working with data in flash:

- :cpp:func:`esp_flash_read` reads data from flash to RAM
- :cpp:func:`esp_flash_write` writes data from RAM to flash
- :cpp:func:`esp_flash_erase_region` erases specific region of flash
- :cpp:func:`esp_flash_erase_chip` erases the whole flash
- :cpp:func:`esp_flash_get_chip_size` returns flash chip size, in bytes, as configured in menuconfig

Generally, try to avoid using the raw SPI flash functions to the "main" SPI
flash chip in favour of :ref:`partition-specific functions
<flash-partition-apis>`.

SPI Flash Size
--------------

The SPI flash size is configured by writing a field in the software bootloader image header, flashed at offset 0x1000.

By default, the SPI flash size is detected by esptool.py when this bootloader is written to flash, and the header is updated with the correct size. Alternatively, it is possible to generate a fixed flash size by setting :envvar:`CONFIG_ESPTOOLPY_FLASHSIZE` in project configuration.

If it is necessary to override the configured flash size at runtime, it is possible to set the ``chip_size`` member of the ``g_rom_flashchip`` structure. This size is used by ``esp_flash_*`` functions (in both software & ROM) to check the bounds.

Concurrency Constraints for flash on SPI1
-----------------------------------------

Because the SPI1 flash is also used for firmware execution via the instruction & data caches, these caches must be disabled while reading/writing/erasing. This means that both CPUs must be running code from IRAM and must only be reading data from DRAM while flash write operations occur.

If you use the API functions documented here, then these constraints are applied automatically and transparently. However, note that it will have some performance impact on other tasks in the system.

There are no such constraints and impacts for flash chips on other SPI buses than SPI0/1.

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

Implementation
--------------

The ``esp_flash_t`` structure holds chip data as well as three important parts of this API:

1. The host driver, which provides the hardware support to access the chip;
2. The chip driver, which provides compatibility service to different chips;
3. The OS functions, provides support of some OS functions (e.g. lock, delay)
   in different stages (1st/2st boot, or the app).

Host driver
^^^^^^^^^^^

The host driver relies on an interface (``spi_flash_host_driver_t``) defined
in the ``spi_flash_host_drv.h`` (in the ``soc/include/hal`` folder). This
interface provides some common functions to communicate with the chip.

In other files of the SPI HAL, some of these functions are implemented with
existing ESP32 memory-spi functionalities. However due to the speed
limitations of ESP32, the HAL layer can't provide high-speed implementations
to some reading commands (So we didn't do it at all). The files
(``memspi_host_driver.h`` and ``.c``) implement the high-speed version of
these commands with the ``common_command`` function provided in the HAL, and
wrap these functions as ``spi_flash_host_driver_t`` for upper layer to use.

You can also implement your own host driver, even with the GPIO. As long as
all the functions in the ``spi_flash_host_driver_t`` are implemented, the
esp_flash API can access to the flash regardless of the low-level hardware.

Chip driver
^^^^^^^^^^^

The chip driver, defined in ``spi_flash_chip_driver.h``, wraps basic
functions provided by the host driver for the API layer to use.

Some operations need some commands to be sent first, or read some status
after. Some chips need different command or value, or need special
communication ways.

There is a type of chip called ``generic chip`` which stands for common
chips. Other special chip drivers can be developed on the base of the generic
chip.

The chip driver relies on the host driver.

OS functions
^^^^^^^^^^^^

Currently the OS function layer provides a lock and a delay entries.

The lock is used to resolve the conflicts between the SPI chip access and
other functions. E.g. the cache (used for the code and PSRAM data fetch)
should be disabled when the flash chip on the SPI0/1 is being accessed. Also,
some devices which don't have CS wire, or the wire is controlled by the
software (e.g. SD card via SPI interface), requires the bus to be monopolized
during a period.

The delay is used by some long operations which requires the master to wait
or polling periodically.


The top API wraps these the chip driver and OS functions into an entire
component, and also provides some argument checking.
