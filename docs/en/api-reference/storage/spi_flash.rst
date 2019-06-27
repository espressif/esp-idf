.. include:: ../../../../components/spi_flash/README.rst

See also
--------

- :doc:`Partition Table documentation <../../api-guides/partition-tables>`
- :doc:`Over The Air Update (OTA) API <../system/ota>` provides high-level API for updating app firmware stored in flash.
- :doc:`Non-Volatile Storage (NVS) API <nvs_flash>` provides a structured API for storing small pieces of data in SPI flash.


.. _spi-flash-implementation-details:

Implementation details
----------------------

In order to perform some flash operations, it is necessary to make sure that both CPUs are not running any code from flash for the duration of the flash operation:
- In a single-core setup, the SDK does it by disabling interrupts/scheduler before performing the flash operation.
- In a dual-core setup, this is slightly more complicated as the SDK needs to make sure that the other CPU is not running any code from flash.


When SPI flash API is called on CPU A (can be PRO or APP), start the spi_flash_op_block_func function on CPU B using the esp_ipc_call API. This API wakes up a high priority task on CPU B and tells it to execute a given function, in this case, spi_flash_op_block_func. This function disables cache on CPU B and signals that the cache is disabled by setting the s_flash_op_can_start flag. Then the task on CPU A disables cache as well and proceeds to execute flash operation.

While a flash operation is running, interrupts can still run on CPUs A and B. It is assumed that all interrupt code is placed into RAM. Once the interrupt allocation API is added, a flag should be added to request the interrupt to be disabled for the duration of a flash operations.

Once the flash operation is complete, the function on CPU A sets another flag, s_flash_op_complete, to let the task on CPU B know that it can re-enable cache and release the CPU. Then the function on CPU A re-enables the cache on CPU A as well and returns control to the calling code.

Additionally, all API functions are protected with a mutex (s_flash_op_mutex).

In a single core environment (:ref:`CONFIG_FREERTOS_UNICORE` enabled), you need to disable both caches, so that no inter-CPU communication can take place.

API Reference - SPI Flash
-------------------------

.. include:: /_build/inc/esp_flash_spi_init.inc
.. include:: /_build/inc/esp_flash.inc
.. include:: /_build/inc/spi_flash_types.inc

API Reference - Partition Table
-------------------------------

.. include:: /_build/inc/esp_partition.inc

API Reference - Flash Encrypt
-----------------------------

.. include:: /_build/inc/esp_flash_encrypt.inc


