Driver for SPI flash read/write/erase operations
================================================

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

While flash operation is running, interrupts can still run on CPU B.
We assume that all interrupt code is placed into RAM.

Once flash operation is complete, function on CPU A sets another flag,
s_flash_op_complete, to let the task on CPU B know that it can re-enable
cache and release the CPU. Then the function on CPU A re-enables the cache on
CPU A as well and returns control to the calling code.

Additionally, all API functions are protected with a mutex (s_flash_op_mutex).

In a single core environment (CONFIG_FREERTOS_UNICORE enabled), we simply
disable both caches, no inter-CPU communication takes place.
