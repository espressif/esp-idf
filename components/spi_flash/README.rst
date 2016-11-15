SPI flash related APIs
======================

Overview
--------
Spi_flash component contains APIs related to reading, writing, erasing,
memory mapping data in the external SPI flash. It also has higher-level
APIs which work with partition table and partitions.

Note that all the functionality is limited to the "main" flash chip,
i.e. the flash chip from which program runs. For ``spi_flash_*`` functions, 
this is software limitation. Underlying ROM functions which work with SPI flash 
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

There are some data alignment limitations which need to be considered when using
spi_flash_read/spi_flash_write functions:

- buffer in RAM must be 4-byte aligned
- size must be 4-byte aligned
- address in flash must be 4-byte aligned

These alignment limitations are purely software, and should be removed in future
versions.

It is assumed that correct SPI flash chip size is set at compile time using
menuconfig. While run-time detection of SPI flash chip size is possible, it is
not implemented yet. Applications which need this (e.g. to provide one firmware
binary for different flash sizes) can do flash chip size detection and set
the correct flash chip size in ``chip_size`` member of ``g_rom_flashchip``
structure. This size is used by ``spi_flash_*`` functions for bounds checking.

SPI flash APIs disable instruction and data caches while reading/writing/erasing.
See implementation notes below on details how this happens. For application
this means that at some periods of time, code can not be run from flash,
and constant data can not be fetched from flash by the CPU. This is not an
issue for normal code which runs in a task, because SPI flash APIs prevent
other tasks from running while caches are disabled. This is an issue for
interrupt handlers, which can still be called while flash operation is in
progress. If the interrupt handler is not placed into IRAM, there is a
possibility that interrupt will happen at the time when caches are disabled,
which will cause an illegal instruction exception.

To prevent this, make sure that all ISR code, and all functions called from ISR
code are placed into IRAM, or are located in ROM. Most useful C library
functions are located in ROM, so they can be called from ISR.

To place a function into IRAM, use ``IRAM_ATTR`` attribute, e.g.::

	#include "esp_attr.h"
	
	void IRAM_ATTR gpio_isr_handler(void* arg)
	{
		// ...		
	}
	
When flash encryption is enabled, ``spi_flash_read`` will read data as it is
stored in flash (without decryption), and ``spi_flash_write`` will write data
in plain text. In other words, ``spi_flash_read/write`` APIs don't have
provisions to deal with encrypted data.


Partition table APIs
--------------------

ESP-IDF uses partition table to maintain information about various regions of
SPI flash memory (bootloader, various application binaries, data, filesystems).
More information about partition tables can be found in docs/partition_tables.rst.

This component provides APIs to enumerate partitions found in the partition table
and perform operations on them. These functions are declared in ``esp_partition.h``:

- ``esp_partition_find`` used to search partition table for entries with specific type, returns an opaque iterator
- ``esp_partition_get`` returns a structure describing the partition, for the given iterator
- ``esp_partition_next`` advances iterator to the next partition found
- ``esp_partition_iterator_release`` releases iterator returned by ``esp_partition_find``
- ``esp_partition_find_first`` is a convenience function which returns structure describing the first partition found by esp_partition_find
- ``esp_partition_read``, ``esp_partition_write``, ``esp_partition_erase_range`` are equivalent to ``spi_flash_read``, ``spi_flash_write``, ``spi_flash_erase_range``, but operate within partition boundaries

Most application code should use ``esp_partition_*`` APIs instead of lower level
``spi_flash_*`` APIs. Partition APIs do bounds checking and calculate correct
offsets in flash based on data stored in partition table.

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
contents of flash when flash encryption is enabled. Decryption is performed at
hardware level.

Memory mapping APIs are declared in ``esp_spi_flash.h`` and ``esp_partition.h``:

- ``spi_flash_mmap`` maps a region of physical flash addresses into instruction space or data space of the CPU
- ``spi_flash_munmap`` unmaps previously mapped region
- ``esp_partition_mmap`` maps part of a partition into the instruction space or data space of the CPU

Differences between ``spi_flash_mmap`` and ``esp_partition_mmap`` are as follows:

- ``spi_flash_mmap`` must be given a 64KB aligned physical address
- ``esp_partition_mmap`` may be given an arbitrary offset within the partition, it will adjust returned pointer to mapped memory as necessary

Note that because memory mapping happens in 64KB blocks, it may be possible to
read data outside of the partition provided to ``esp_partition_mmap``. 

Implementation notes
--------------------

In order to perform some flash operations, we need to make sure both CPUs
are not running any code from flash for the duration of the flash operation.
In a single-core setup this is easy: we disable interrupts/scheduler and do
the flash operation. In the dual-core setup this is slightly more complicated.
We need to make sure that the other CPU doesn't run any code from flash.


When SPI flash API is called on CPU A (can be PRO or APP), we start
spi_flash_op_block_func function on CPU B using esp_ipc_call API. This API
wakes up high priority task on CPU B and tells it to execute given function,
in this case spi_flash_op_block_func. This function disables cache on CPU B and
signals that cache is disabled by setting s_flash_op_can_start flag.
Then the task on CPU A disables cache as well, and proceeds to execute flash
operation.

While flash operation is running, interrupts can still run on CPUs A and B.
We assume that all interrupt code is placed into RAM. Once interrupt allocation
API is added, we should add a flag to request interrupt to be disabled for
the duration of flash operations.

Once flash operation is complete, function on CPU A sets another flag,
s_flash_op_complete, to let the task on CPU B know that it can re-enable
cache and release the CPU. Then the function on CPU A re-enables the cache on
CPU A as well and returns control to the calling code.

Additionally, all API functions are protected with a mutex (s_flash_op_mutex).

In a single core environment (CONFIG_FREERTOS_UNICORE enabled), we simply
disable both caches, no inter-CPU communication takes place.
