FreeRTOS
========

Overview
--------

This section contains documentation of FreeRTOS types, functions, and macros. It is automatically generated from FreeRTOS header files.

.. note::
    ESP-IDF FreeRTOS is based on Vanilla FreeRTOS v10.4.3

- For more information about the SMP changes of ESP-IDF FreeRTOS, see :doc:`/api-guides/freertos-smp`
- For more information about the features added to ESP-IDF FreeRTOS, see :doc:`/api-reference/system/freertos_additions`.

Configuration
-------------

Vanilla FreeRTOS allows ports and applications to configure the kernel by adding various ``#define config...`` macros to ``FreeRTOSConfig.h``. Through these macros, the kernel's scheduling behavior and various kernel features can be enabled or disabled. **However, in ESP-IDF FreeRTOS, the ``FreeRTOSConfig.h`` file is considered a private and must not be modified by users**. Any FreeRTOS configuration that is exposed to the user will be done so via menuconfig.

ESP-IDF FreeRTOS can be configured in the project configuration menu (``idf.py menuconfig``) under ``Component Config/FreeRTOS``. The following section highlights some of the ESP-IDF FreeRTOS configuration options. For a full list of ESP-IDF FreeRTOS configurations, see :doc:`/api-reference/kconfig`

- :ref:`CONFIG_FREERTOS_UNICORE` will run ESP-IDF FreeRTOS only on CPU0. Note that this is **not equivalent to running Vanilla FreeRTOS**. Futhermore, this option may affect behavior of components other than :component:`freertos`. For more details regarding the effects of running ESP-IDF FreeRTOS on a single core, refer to :ref:`freertos-smp-single-core`. Alternatively, users can also search for occurrences of ``CONFIG_FREERTOS_UNICORE`` in the ESP-IDF components.

- :ref:`CONFIG_FREERTOS_ASSERT_ON_UNTESTED_FUNCTION` will trigger a halt in functions in ESP-IDF FreeRTOS that have not been fully tested in an SMP context.

- :ref:`CONFIG_FREERTOS_TASK_FUNCTION_WRAPPER` will enclose all task functions within a wrapper function. In the case that a task function mistakenly returns (i.e. does not call :cpp:func:`vTaskDelete`), the call flow will return to the wrapper function. The wrapper function will then log an error and abort the application, as illustrated below::

    E (25) FreeRTOS: FreeRTOS task should not return. Aborting now!
    abort() was called at PC 0x40085c53 on core 0

.. only:: CONFIG_FREERTOS_UNICORE

    .. note::
        As {IDF_TARGET_NAME} is a single core SoC, the :ref:`CONFIG_FREERTOS_UNICORE` configuration is always set.


.. _freertos-applications:

ESP-IDF FreeRTOS Applications
-----------------------------

Unlike Vanilla FreeRTOS, users must not call :cpp:func:`vTaskStartScheduler`. Instead, ESP-IDF FreeRTOS is started automatically. The entry point is a user defined ``void app_main(void)`` function.

- Typically, users would spawn the rest of their applications task from ``app_main``.
- The ``app_main`` function is allowed to return at any point (i.e., before the application terminates).
- The ``app_main`` function is called from the ``main`` task. 

The ``main`` task is one of multiple tasks that are automatically spawned by ESP-IDF during startup. These tasks are:

.. only:: not CONFIG_FREERTOS_UNICORE

    .. list-table:: List of Tasks Created During Startup
        :widths: 25 25 5 50
        :header-rows: 1

        * - Task Name
          - Affinity
          - Priority
          - Description
        * - Main Task (``main``)
          - CPU0
          - 1
          - Task that simply calls ``app_main``. This task will self delete when ``app_main`` returns
        * - Idle Tasks (``IDLEx``)
          - CPU0 and CPU1
          - 0
          - Idle tasks created for (and pinned to) each CPU
        * - IPC Tasks (``ipcx``)
          - CPU0 and CPU1
          - 24
          - IPC tasks created for (and pinned to ) each CPU. IPC tasks are used to implement the IPC feature. See :doc:`/api-reference/system/ipc` for more details.

.. only:: CONFIG_FREERTOS_UNICORE

    .. list-table:: List of Tasks Created During Startup
        :widths: 25 25 5 50
        :header-rows: 1

        * - Task Name
          - Affinity
          - Priority
          - Description
        * - Main Task (``main``)
          - CPU0
          - 1
          - Task that simply calls ``app_main``. This task will self delete when ``app_main`` returns
        * - Idle Tasks (``IDLEx``)
          - CPU0 and CPU1
          - 0
          - Idle task created for (and pinned to) each CPU

Task API
--------

.. include-build-file:: inc/task.inc

Queue API
---------

.. include-build-file:: inc/queue.inc

Semaphore API
-------------

.. include-build-file:: inc/semphr.inc

Timer API
---------

.. include-build-file:: inc/timers.inc


Event Group API
---------------

.. include-build-file:: inc/event_groups.inc

Stream Buffer API
-----------------

.. include-build-file:: inc/stream_buffer.inc


Message Buffer API
------------------

.. include-build-file:: inc/message_buffer.inc
