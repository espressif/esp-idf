.. include:: ../../../components/spi_flash/README.rst

See also
--------

- :doc:`Partition Table documentation <../../api-guides/partition-tables>`
- :doc:`Over The Air Update (OTA) API <../system/ota>` provides high-level API for updating app firmware stored in flash.
- :doc:`Non-Volatile Storage (NVS) API <nvs_flash>` provides a structured API for storing small items of data in SPI flash.

API Reference
-------------

Header Files
^^^^^^^^^^^^

  * :component_file:`spi_flash/include/esp_spi_flash.h`
  * :component_file:`spi_flash/include/esp_partition.h`
  * :component_file:`bootloader_support/include/esp_flash_encrypt.h`

Macros
^^^^^^

.. doxygendefine:: ESP_ERR_FLASH_BASE
.. doxygendefine:: ESP_ERR_FLASH_OP_FAIL
.. doxygendefine:: ESP_ERR_FLASH_OP_TIMEOUT
.. doxygendefine:: SPI_FLASH_SEC_SIZE
.. doxygendefine:: SPI_FLASH_MMU_PAGE_SIZE
.. doxygendefine:: ESP_PARTITION_SUBTYPE_OTA
.. doxygendefine:: SPI_FLASH_CACHE2PHYS_FAIL

Type Definitions
^^^^^^^^^^^^^^^^

.. doxygentypedef:: spi_flash_mmap_handle_t
.. doxygentypedef:: esp_partition_iterator_t

Enumerations
^^^^^^^^^^^^

.. doxygenenum:: spi_flash_mmap_memory_t
.. doxygenenum:: esp_partition_type_t
.. doxygenenum:: esp_partition_subtype_t

Structures
^^^^^^^^^^

.. doxygenstruct:: esp_partition_t

Functions
^^^^^^^^^

.. doxygenfunction:: spi_flash_init
.. doxygenfunction:: spi_flash_get_chip_size
.. doxygenfunction:: spi_flash_erase_sector
.. doxygenfunction:: spi_flash_erase_range
.. doxygenfunction:: spi_flash_write
.. doxygenfunction:: spi_flash_write_encrypted
.. doxygenfunction:: spi_flash_read
.. doxygenfunction:: spi_flash_read_encrypted
.. doxygenfunction:: spi_flash_mmap
.. doxygenfunction:: spi_flash_munmap
.. doxygenfunction:: spi_flash_mmap_dump
.. doxygenfunction:: spi_flash_cache2phys
.. doxygenfunction:: spi_flash_phys2cache
.. doxygenfunction:: spi_flash_cache_enabled
.. doxygenfunction:: esp_partition_find
.. doxygenfunction:: esp_partition_find_first
.. doxygenfunction:: esp_partition_get
.. doxygenfunction:: esp_partition_next
.. doxygenfunction:: esp_partition_iterator_release
.. doxygenfunction:: esp_partition_read
.. doxygenfunction:: esp_partition_write
.. doxygenfunction:: esp_partition_erase_range
.. doxygenfunction:: esp_partition_mmap
.. doxygenfunction:: esp_flash_encryption_enabled

.. _spi-flash-implementation-details:

Implementation details
----------------------

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
