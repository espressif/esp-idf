SPI Flash APIs
==============

Overview
--------
The spi_flash component contains APIs related to reading, writing, erasing,
memory mapping data in the external SPI flash. It also has higher-level
APIs which work with partitions defined in the :doc:`partition table </api-guides/partition-tables>`.

Note that all the functionality is limited to the "main" SPI flash chip,
the same SPI flash chip from which program runs. For ``spi_flash_*`` functions,
this is a software limitation. The underlying ROM functions which work with SPI flash
do not have provisions for working with flash chips attached to SPI peripherals
other than SPI0.

SPI flash access APIs
---------------------

This is the set of APIs for working with data in flash:

- ``spi_flash_read`` used to read data from flash to RAM
- ``spi_flash_write`` used to write data from RAM to flash
- ``spi_flash_erase_sector`` used to erase individual sectors of flash
- ``spi_flash_erase_range`` used to erase range of addresses in flash
- ``spi_flash_get_chip_size`` returns flash chip size, in bytes, as configured in menuconfig

Generally, try to avoid using the raw SPI flash functions in favour of
partition-specific functions.

SPI Flash Size
--------------

The SPI flash size is configured by writing a field in the software bootloader
image header, flashed at offset 0x1000.

By default, the SPI flash size is detected by esptool.py when this bootloader is
written to flash, and the header is updated with the correct
size. Alternatively, it is possible to generate a fixed flash size by disabling
detection in ``make menuconfig`` (under Serial Flasher Config).

If it is necessary to override the configured flash size at runtime, is is
possible to set the ``chip_size`` member of ``g_rom_flashchip`` structure. This
size is used by ``spi_flash_*`` functions (in both software & ROM) for bounds
checking.

Concurrency Constraints
-----------------------

Because the SPI flash is also used for firmware execution (via the instruction &
data caches), these caches much be disabled while reading/writing/erasing. This
means that both CPUs must be running code from IRAM and only reading data from
DRAM while flash write operations occur.

Refer to the :ref:`application memory layout <memory-layout>` documentation for
an explanation of the differences between IRAM, DRAM and flash cache.

To avoid reading flash cache accidentally, when one CPU commences a flash write
or erase operation the other CPU is put into a blocked state and all
non-IRAM-safe interrupts are disabled on both CPUs, until the flash operation
completes.

.. _iram-safe-interrupt-handlers:

IRAM-Safe Interrupt Handlers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you have an interrupt handler that you want to execute even when a flash
operation is in progress (for example, for low latency operations), set the
``ESP_INTR_FLAG_IRAM`` flag when the :doc:`interrupt handler is registered
</api-reference/system/intr_alloc>`.

You must ensure all data and functions accessed by these interrupt handlers are
located in IRAM or DRAM. This includes any functions that the handler calls.

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

Note that knowing which data should be marked with ``DRAM_ATTR`` can be hard,
the compiler will sometimes recognise that a variable or expression is constant
(even if it is not marked ``const``) and optimise it into flash, unless it is
marked with ``DRAM_ATTR``.

If a function or symbol is not correctly put into IRAM/DRAM and the interrupt
handler reads from the flash cache during a flash operation, it will cause a
crash due to Illegal Instruction exception (for code which should be in IRAM) or
garbage data to be read (for constant data which should be in DRAM).

Partition table APIs
--------------------

ESP-IDF projects use a partition table to maintain information about various regions of
SPI flash memory (bootloader, various application binaries, data, filesystems).
More information about partition tables can be found :doc:`here </api-guides/partition-tables>`.

This component provides APIs to enumerate partitions found in the partition table
and perform operations on them. These functions are declared in ``esp_partition.h``:

- ``esp_partition_find`` used to search partition table for entries with
  specific type, returns an opaque iterator
- ``esp_partition_get`` returns a structure describing the partition, for the given iterator
- ``esp_partition_next`` advances iterator to the next partition found
- ``esp_partition_iterator_release`` releases iterator returned by ``esp_partition_find``
- ``esp_partition_find_first`` is a convenience function which returns structure
  describing the first partition found by esp_partition_find
- ``esp_partition_read``, ``esp_partition_write``, ``esp_partition_erase_range``
  are equivalent to ``spi_flash_read``, ``spi_flash_write``,
  ``spi_flash_erase_range``, but operate within partition boundaries

Most application code should use ``esp_partition_*`` APIs instead of lower level
``spi_flash_*`` APIs. Partition APIs do bounds checking and calculate correct
offsets in flash based on data stored in partition table.

SPI Flash Encryption
--------------------

It is possible to encrypt SPI flash contents, and have it transparenlty decrypted by hardware.

Refer to the :doc:`Flash Encryption documentation </security/flash-encryption>` for more details.

Memory mapping APIs
-------------------

ESP32 features memory hardware which allows regions of flash memory to be mapped
into instruction and data address spaces. This mapping works only for read operations,
it is not possible to modify contents of flash memory by writing to mapped memory
region. Mapping happens in 64KB pages. Memory mapping hardware can map up to
4 megabytes of flash into data address space, and up to 16 megabytes of flash into
instruction address space. See the technical reference manual for more details
about memory mapping hardware.

Note that some number of 64KB pages is used to map the application
itself into memory, so the actual number of available 64KB pages may be less.

Reading data from flash using a memory mapped region is the only way to decrypt
contents of flash when :doc:`flash encryption </security/flash-encryption>` is enabled.
Decryption is performed at hardware level.

Memory mapping APIs are declared in ``esp_spi_flash.h`` and ``esp_partition.h``:

- ``spi_flash_mmap`` maps a region of physical flash addresses into instruction space or data space of the CPU
- ``spi_flash_munmap`` unmaps previously mapped region
- ``esp_partition_mmap`` maps part of a partition into the instruction space or data space of the CPU

Differences between ``spi_flash_mmap`` and ``esp_partition_mmap`` are as follows:

- ``spi_flash_mmap`` must be given a 64KB aligned physical address
- ``esp_partition_mmap`` may be given an arbitrary offset within the partition,
  it will adjust returned pointer to mapped memory as necessary

Note that because memory mapping happens in 64KB blocks, it may be possible to
read data outside of the partition provided to ``esp_partition_mmap``.
