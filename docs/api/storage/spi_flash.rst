.. include:: ../../../components/spi_flash/README.rst

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

