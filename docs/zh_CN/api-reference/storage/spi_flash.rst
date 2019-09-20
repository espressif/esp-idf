.. include:: ../../../../components/spi_flash/README_CN.rst

另请参考
------------

- :doc:`分区表 <../../api-guides/partition-tables>`
- :doc:`OTA API <../system/ota>` 提供了高层 API 用于更新存储在 flash 中的 app 固件。
- :doc:`NVS API <nvs_flash>` 提供了结构化 API 用于存储 SPI flash 中的碎片数据。

.. _spi-flash-implementation-details:

实现细节
------------

必须确保操作期间，两个 CPU 均未从 flash 运行代码，实现细节如下：

- 单核模式下，SDK 在执行 flash 操作前将禁用中断或调度算法。
- 双核模式下，实现细节更为复杂，SDK 需确保两个 CPU 均未运行 flash 代码。

如果有 SPI flash API 在 CPU A（PRO 或 APP）上调用，它使用 ``esp_ipc_call`` API 在 CPU B 上运行 ``spi_flash_op_block_func`` 函数。``esp_ipc_call`` API 在 CPU B 上唤醒一个高优先级任务，即运行 ``spi_flash_op_block_func`` 函数。运行该函数将禁用 CPU B 上的 cache，并使用 ``s_flash_op_can_start`` 旗帜来标志 cache 已禁用。然后，CPU A 上的任务也会禁用 cache 并继续执行 flash 操作。

执行 flash 操作时，CPU A 和 CPU B 仍然可以执行中断操作。默认中断代码均存储于 RAM 中，如果新添加了中断分配 API，则应添加一个标志位以请求在 flash 操作期间禁用该新分配的中断。

Flash 操作完成后，CPU A 上的函数将设置另一标志位，即 ``s_flash_op_complete``，用以通知 CPU B 上的任务可以重新启用 cache 并释放 CPU。接着，CPU A 上的函数也重新启用 cache，并将控制权返还给调用者。

另外，所有 API 函数均受互斥量 ``s_flash_op_mutex`` 保护。

在单核环境中（启用 :ref:`CONFIG_FREERTOS_UNICORE`），您需要禁用上述两个 cache 以防发生 CPU 间通信。

SPI Flash API 参考
-------------------------

.. include:: /_build/inc/esp_flash_spi_init.inc
.. include:: /_build/inc/esp_flash.inc
.. include:: /_build/inc/spi_flash_types.inc

分区表 API 参考
-------------------------------

.. include:: /_build/inc/esp_partition.inc

Flash 加密 API 参考
-----------------------------

.. include:: /_build/inc/esp_flash_encrypt.inc


