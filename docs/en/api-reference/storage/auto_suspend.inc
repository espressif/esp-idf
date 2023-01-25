.. _auto-suspend:

When auto suspend is enabled
----------------------------

.. important::

    The flash chip you are using must have suspend/resume function, even then, and not all flash chips with suspend/resume support auto-suspend on {IDF_TARGET_NAME}. If you use suspend feature on a unsupported chip, it may cause a severe crash. Therefore, we strongly suggest you reading the flash chip datasheets first. Ensure the flash chip satisfies the following conditions at minimum. Even then, thorough testing is recommended.

    1. SUS bit in status registers should in SR2 bit7 (or SR bit15)(This is caused by the restriction of out software implementation).

    2. Suspend command is 75H, resume command is 7AH(This is caused by the restriction of out software implementation).

    3. When the flash is successfully suspended, all address of the flash, except from the section/block being erased, can be read correctly. And resume can be sent immediately at this state.

    4. When the flash is successfully resumed, another suspend can be sent immediately at this state.

When auto suspend is enabled, the cache will be kept enabled while accessing the SPI1 bus (e.g. erasing/writing/reading main flash). The hardware handles the arbitration between them.

If SPI1 operation is short (like reading operation), the CPU and the cache will wait until the SPI1 operation is done. However if it's an erasing, auto suspend will happen, interrupting the erasing, making the CPU able to read from cache in limited time.

This way some code/variables can be put into the flash/psram instead of IRAM/DRAM, while still able to be executed during flash erasing. This reduces the some usage of IRAM/DRAM.

Please note this feature has the overhead of the flash suspend/resume. The flash erasing can be extremely long if the erasing is interrupted too often. Use FreeRTOS task priorities to ensure that only real-time critical tasks are executed at higher priority than flash erase, to allow the flash erase to complete in reasonable time.


In other words, there are three kinds of code:

1. Critical code: inside IRAM/DRAM. This kind of code usually has high performance requirements, related to cache/flash/psram, or called very often.

2. Cached code: inside flash/psram. This kind of code has lower performance requirements or called less often. They will execute during erasing, with some overhead.

3. Low priority code: inside flash/psram and disabled during erasing. This kind of code should be forbidden from executed to avoid affecting the flash erasing, by setting a lower task priority than the erasing task.
