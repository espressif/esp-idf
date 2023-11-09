.. _auto-suspend:

Flash Auto Suspend Feature
--------------------------

.. important::

    1. The flash chip you are using should have a suspend/resume feature.

    2. The MSPI hardware should support the auto-suspend feature, i.e., hardware can send suspend command automatically.

    If you use suspend feature on an unsupported chip, it may cause a severe crash. Therefore, we strongly suggest you reading the flash chip datasheets first. Ensure the flash chip satisfies the following conditions at minimum.

    1. With the current software implementation, SUS bit in status registers should in SR2 bit7 or SR bit15.

    2. With the current software implementation, suspend command should be 75H, with resume command being 7AH.

    3. When the flash is successfully suspended, all address of the flash, except from the section/block being erased, can be read correctly. At this state，resume can be sent immediately as well.

    4. When the flash is successfully resumed, another suspend can be sent immediately at this state.

When :ref:`CONFIG_SPI_FLASH_AUTO_SUSPEND` is enabled, the caches will be kept enabled. They would be disabled if :ref:`CONFIG_SPI_FLASH_AUTO_SUSPEND` is disabled. The hardware handles the arbitration between SPI0 and SPI1. If the SPI1 operation is short, such as a reading operation, the CPU and the cache will wait until the SPI1 operation is completed. However, during processes like erasing, page programming, or status register writing (e.g., ``SE``, ``PP``, and ``WRSR``), an auto suspend will happen, interrupting the ongoing flash operation. This allows the CPU to access data from the cache and flash within limited time.

This approach allows certain code/variables to be stored in flash/PSRAM instead of IRAM/DRAM, while still being executable during flash erasing. This reduces the usage of IRAM/DRAM.

Please note that this feature comes with the overhead of flash suspend/resume. Frequent interruptions during flash erasing can significantly prolong the erasing process. To avoid this, you may use FreeRTOS task priorities to ensure that only real-time critical tasks are executed at a higher priority than flash erasing, allowing the flash erasing to complete in reasonable time.

There are three kinds of code:

1. Critical code: inside IRAM/DRAM. This kind of code usually has high performance requirements, related to cache/flash/PSRAM, or is called very often.

2. Cached code: inside flash/PSRAM. This kind of code has lower performance requirements or is called less often. They will execute during erasing, with some overhead.

3. Low-priority code: inside flash/PSRAM and disabled during erasing. This kind of code should be forbidden from being executed to avoid affecting the flash erasing, by setting a lower task priority than the erasing task.

Regarding the flash suspend feature usage and corresponding response time delay, please also see the :example:`system/flash_suspend` example.

.. note::

    The flash auto suspend feature relies heavily on strict timing. You can see it as a kind of optimization plan, which means that you cannot use it in every situation, like high requirement of real-time system or triggering interrupt very frequently (e.g., LCD flush, bluetooth, Wi-Fi, etc.). You should take following steps to evaluate what kind of ISR can be used together with flash suspend.

    .. wavedrom:: /../_static/diagrams/spi_flash/flash_auto_suspend_timing.json

    As you can see from the diagram, two key values should be noted:

    1. ISR time: The ISR time cannot be very long, at least not larger than the value you set in ``IWDT``. But it will significantly lengthen the erasing/writing completion time.

    2. ISR interval: ISR cannot be triggered very often. The most important time is the **ISR interval minus ISR time** (from point b to point c in the diagram). During this time, SPI1 will send resume command to restart the operation. However, it needs a time ``tsus`` for preparation, and the typical value of ``tsus`` is about **40 us**. If SPI1 cannot resume the operation but another suspend command comes, it will cause CPU starve and ``TWDT`` may be triggered.

    The ``tsus`` time mentioned in point 2 can be found by looking through the flash datasheets, usually in the AC CHARACTERISTICS section. Users needs to make sure that the ``tsus`` value obtained from the datasheets is not greater than the :ref:`CONFIG_SPI_FLASH_SUSPEND_TSUS_VAL_US` value in Kconfig.

    Furthermore, the flash suspend might be delayed. If both the CPU and the cache access the flash via SPI0 frequently and SPI1 sends the suspend command frequently as well, the efficiency of MSPI data transfer will be influenced. So, we have a **lock** inside to prevent this. When SPI1 sends the suspend command, SPI0 will take over memory SPI bus and take the lock. After SPI0 finishes sending data, it will retain control of the memory SPI bus until the lock delay period time finishes. During this lock delay period, if there is any other SPI0 transaction, then the SPI0 transaction will be proceeded and a new lock delay period will start. Otherwise, SPI0 will release the memory bus and start SPI0/1 arbitration.
