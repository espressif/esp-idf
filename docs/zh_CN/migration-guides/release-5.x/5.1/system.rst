系统
======

:link_to_translation:`en:[English]`

FreeRTOS
--------

.. only:: SOC_SPIRAM_SUPPORTED

    动态内存分配
    ^^^^^^^^^^^^^^^^^^^^^^^^^

    过去，FreeRTOS 通常使用 ``malloc()`` 函数来分配动态内存。因此，如果应用程序允许 ``malloc()`` 从外部 RAM 分配内存（通过将 :ref:`CONFIG_SPIRAM_USE` 选项配置为 ``CONFIG_SPIRAM_USE_MALLOC``），FreeRTOS 就有可能从外部 RAM 分配动态内存，并且具体位置由堆分配器确定。

    .. note::

        任务的动态内存分配（通常占用大部分内存）与上述介绍的情况不同，是种例外情况。FreeRTOS 会使用单独的内存分配函数，确保为任务分配的动态内存始终位于内部 RAM 中。

    允许将 FreeRTOS 对象（如队列和信号量）放置在外部 RAM 中可能会出现问题，例如如果在访问这些对象时 cache 被禁用（如在 SPI flash 写入操作期间），则会导致 cache 访问错误（详细信息请参阅 :doc:`严重错误</api-guides/fatal-errors>`）。

    因此，FreeRTOS 已经更新为始终使用内部内存（即 DRAM）进行动态内存分配。调用 FreeRTOS 创建函数（例如 :cpp:func:`xTaskCreate` 或 :cpp:func:`xQueueCreate` ）将保证为这些任务/对象分配的内存来自内部内存（详细信息请参阅 :ref:`freertos-heap`）。

    .. warning::

        如果你之前使用 :ref:`CONFIG_SPIRAM_USE` 将 FreeRTOS 对象放置在外部内存中，这个更改则会导致内部内存的使用增加，因为现在 FreeRTOS 对象将被分配到内部内存中。

    现在将 FreeRTOS 任务/对象放置在外部内存中，需要显示地设置，可采用以下方法之一：

    - 使用 ``...CreateWithCaps()`` API 函数如 :cpp:func:`xTaskCreateWithCaps` 或  :cpp:func:`xQueueCreateWithCaps` 分配任务/对象到外部内存（详情请参阅 :ref:`freertos-idf-additional-api`）。
    - 使用 :cpp:func:`heap_caps_malloc` 为 FreeRTOS 对象手动分配外部内存，然后使用 ``...CreateStatic()`` FreeRTOS 函数从分配的内存中创建对象。

电源管理
----------

* ``esp_pm_config_esp32xx_t`` 已弃用，应使用 ``esp_pm_config_t`` 替代。
* ``esp32xx/pm.h`` 已弃用，应使用 ``esp_pm.h`` 替代。
