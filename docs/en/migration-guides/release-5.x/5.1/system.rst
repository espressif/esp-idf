System
======

:link_to_translation:`zh_CN:[中文]`

FreeRTOS
--------

.. only:: SOC_SPIRAM_SUPPORTED

    Dynamic Memory Allocation
    ^^^^^^^^^^^^^^^^^^^^^^^^^

    Previously, most FreeRTOS dynamic allocation would eventually call to ``malloc()``. Thus, if an application was configured to allow ``malloc()`` to allocate from external RAM (i.e., :ref:`CONFIG_SPIRAM_USE` was set ``CONFIG_SPIRAM_USE_MALLOC``), then there was a possibility that FreeRTOS could allocate dynamic memory from external RAM, with the exact placement being decided by the heap allocator.

    .. note::
        Dynamic memory allocation for tasks (which are likely to consume the most memory) were an exception to the scenario above. FreeRTOS would use a separate memory allocation function to guarantee that dynamic memory allocate for a task was always placed in internal RAM.

    Allowing FreeRTOS objects (such as queues and semaphores) to be placed in external RAM becomes an issue if those objects are accessed while the cache is disabled (such as during SPI Flash write operations) and would lead to a cache access errors (see :doc:`Fatal Errors </api-guides/fatal-errors>` for more details).

    Therefore, FreeRTOS has been updated to always use internal memory (i.e., DRAM) for dynamic memory allocation. Calling FreeRTOS creation functions (e.g., :cpp:func:`xTaskCreate`, :cpp:func:`xQueueCreate`) will guarantee that the memory allocated for those tasks/objects is from internal memory (see :ref:`freertos-heap` for more details).

    .. warning::
        For users that previously relied on :ref:`CONFIG_SPIRAM_USE` to place FreeRTOS objects into external memory, this change will lead to increased usage of internal memory due the FreeRTOS objects now being allocated there.

    Users that want to place a FreeRTOS task/object into external memory will now need to do so explicitly. Users can use one of the following methods:

    - Allocate the task/object using one of the ``...CreateWithCaps()`` API such as :cpp:func:`xTaskCreateWithCaps` and :cpp:func:`xQueueCreateWithCaps` (see :ref:`freertos-idf-additional-api` for more details).
    - Manually allocate external memory for those objects using :cpp:func:`heap_caps_malloc`, then create the objects from the allocated memory using one of the ``...CreateStatic()`` FreeRTOS functions.

Power Management
-----------------------

* ``esp_pm_config_esp32xx_t`` is deprecated, use ``esp_pm_config_t`` instead.
* ``esp32xx/pm.h`` is deprecated, use ``esp_pm.h`` instead.
