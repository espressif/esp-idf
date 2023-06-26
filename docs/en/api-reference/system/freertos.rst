FreeRTOS (Overview)
===================

Overview
--------

FreeRTOS is an open source real-time operating system kernel that acts as the operating system for ESP-IDF applications and is integrated into ESP-IDF as a component. The FreeRTOS component in ESP-IDF contains ports of the FreeRTOS kernel for all the CPU architectures used by ESP targets (i.e., Xtensa and RISC-V). Furthermore, ESP-IDF provides different implementations of FreeRTOS in order to support SMP (Symmetric Multiprocessing) on multi-core ESP targets. This document provides an overview of the FreeRTOS component, the FreeRTOS implementations offered by ESP-IDF, and the common aspects across all implementations.

Implementations
---------------

The `official FreeRTOS <https://www.freertos.org/index.html>`_ (henceforth referred to as Vanilla FreeRTOS) is a single-core RTOS. In order to support the various multi-core ESP targets, ESP-IDF supports different FreeRTOS implementations, namely **ESP-IDF FreeRTOS** and **Amazon SMP FreeRTOS**.

ESP-IDF FreeRTOS
^^^^^^^^^^^^^^^^

ESP-IDF FreeRTOS is a FreeRTOS implementation based on Vanilla FreeRTOS v10.4.3, but contains significant modifications to support SMP. ESP-IDF FreeRTOS only supports two cores at most (i.e., dual core SMP), but is more optimized for this scenario by design. For more details regarding ESP-IDF FreeRTOS and its modifications, please refer to the :doc:`freertos_idf` document.

.. note::
    ESP-IDF FreeRTOS is currently the default FreeRTOS implementation for ESP-IDF.

.. _amazon_smp_freertos:

Amazon SMP FreeRTOS
^^^^^^^^^^^^^^^^^^^

Amazon SMP FreeRTOS is an SMP implementation of FreeRTOS that is officially supported by Amazon. Amazon SMP FreeRTOS is able to support N-cores (i.e., more than two cores). Amazon SMP FreeRTOS can be enabled via the :ref:`CONFIG_FREERTOS_SMP` option. For more details regarding Amazon SMP FreeRTOS, please refer to the `official Amazon SMP FreeRTOS documentation <https://freertos.org/symmetric-multiprocessing-introduction.html>`_.

.. warning::
    The Amazon SMP FreeRTOS implementation (and its port in ESP-IDF) are currently in experimental/beta state. Therefore, significant behavioral changes and breaking API changes can occur.

Configuration
-------------

Kernel Configuration
^^^^^^^^^^^^^^^^^^^^

Vanilla FreeRTOS requires that ports and applications configure the kernel by adding various ``#define config...`` macros to ``FreeRTOSConfig.h``. Vanilla FreeRTOS supports a list of kernel configuration options which allow various kernel behaviors and features to be enabled or disabled.

**However, for all FreeRTOS ports in ESP-IDF, the ``FreeRTOSConfig.h`` file is considered private and must not be modified by users**. A large number of kernel configuration options in ``FreeRTOSConfig.h`` are hard coded as they are either required or not supported in ESP-IDF. All kernel configuration options that are configurable by the user will be exposed via menuconfig under ``Component Config/FreeRTOS/Kernel``.

For the full list of user configurable kernel options, see :doc:`/api-reference/kconfig`. The list below highlights some commonly used kernel configuration options:

- :ref:`CONFIG_FREERTOS_UNICORE` will run FreeRTOS only on CPU0. Note that this is **not equivalent to running Vanilla FreeRTOS**. Furthermore, this option may affect behavior of components other than :component:`freertos`. For more details regarding the effects of running FreeRTOS on a single core, refer to :ref:`freertos-smp-single-core` (if using ESP-IDF FreeRTOS) or the official Amazon SMP FreeRTOS documentation. Alternatively, users can also search for occurrences of ``CONFIG_FREERTOS_UNICORE`` in the ESP-IDF components.

.. only:: CONFIG_FREERTOS_UNICORE

    .. note::
        As {IDF_TARGET_NAME} is a single core SoC, the :ref:`CONFIG_FREERTOS_UNICORE` configuration is always set.

- :ref:`CONFIG_FREERTOS_ENABLE_BACKWARD_COMPATIBILITY` enables backward compatibility with some FreeRTOS macros/types/functions that were deprecated from v8.0 onwards.

Port Configuration
^^^^^^^^^^^^^^^^^^

All other FreeRTOS related configuration options that are not part of the kernel configuration are exposed via menuconfig under ``Component Config/FreeRTOS/Port``. These options configure aspects such as:

- The FreeRTOS ports themselves (e.g., tick timer selection, ISR stack size)
- Additional features added to the FreeRTOS implementation or ports

Using FreeRTOS
--------------

Application Entry Point
^^^^^^^^^^^^^^^^^^^^^^^

Unlike Vanilla FreeRTOS, users of FreeRTOS in ESP-IDF **must never call** :cpp:func:`vTaskStartScheduler` and :cpp:func:`vTaskEndScheduler`. Instead, ESP-IDF will start FreeRTOS automatically. Users must define a ``void app_main(void)`` function which acts as the entry point for user's application and is automatically called on ESP-IDF startup.

- Typically, users would spawn the rest of their application's task from ``app_main``.
- The ``app_main`` function is allowed to return at any point (i.e., before the application terminates).
- The ``app_main`` function is called from the ``main`` task.

.. _freertos_system_tasks:

Background Tasks
^^^^^^^^^^^^^^^^

During startup, ESP-IDF and FreeRTOS will automatically create multiple tasks that run in the background (listed in the the table below).

.. list-table:: List of Tasks Created During Startup
    :widths: 10 75 5 5 5
    :header-rows: 1

    * - Task Name
      - Description
      - Stack Size
      - Affinity
      - Priority
    * - Idle Tasks (``IDLEx``)
      - An idle task (``IDLEx``) is created for (and pinned to) each CPU, where ``x`` is the CPU's number.
      - :ref:`CONFIG_FREERTOS_IDLE_TASK_STACKSIZE`
      - CPUx
      - ``0``
    * - FreeRTOS Timer Task (``Tmr Svc``)
      - FreeRTOS will create the Timer Service/Daemon Task if any FreeRTOS Timer APIs are called by the application.
      - :ref:`CONFIG_FREERTOS_TIMER_TASK_STACK_DEPTH`
      - CPU0
      - :ref:`CONFIG_FREERTOS_TIMER_TASK_PRIORITY`
    * - Main Task (``main``)
      - Task that simply calls ``app_main``. This task will self delete when ``app_main`` returns
      - :ref:`CONFIG_ESP_MAIN_TASK_STACK_SIZE`
      - :ref:`CONFIG_ESP_MAIN_TASK_AFFINITY`
      - ``1``
    * - IPC Tasks (``ipcx``)
      - When :ref:`CONFIG_FREERTOS_UNICORE` is false, an IPC task (``ipcx``) is created for (and pinned to) each CPU. IPC tasks are used to implement the Inter-processor Call (IPC) feature.
      - :ref:`CONFIG_ESP_IPC_TASK_STACK_SIZE`
      - CPUx
      - ``24``
    * - ESP Timer Task (``esp_timer``)
      - ESP-IDF will create the ESP Timer Task used to process ESP Timer callbacks.
      - :ref:`CONFIG_ESP_TIMER_TASK_STACK_SIZE`
      - CPU0
      - ``22``

.. note::
    Note that if an application uses other ESP-IDF features (e.g., WiFi or Bluetooth), those features may create their own background tasks in addition to the tasks listed in the table above.

FreeRTOS Additions
------------------

ESP-IDF provides some supplemental features to FreeRTOS such as Ring Buffers, ESP-IDF style Tick and Idle Hooks, and TLSP deletion callbacks. See :doc:`freertos_additions` for more details.

.. _freertos-heap:

FreeRTOS Heap
-------------

Vanilla FreeRTOS provides its own `selection of heap implementations <https://www.freertos.org/a00111.html>`_. However, ESP-IDF already implements its own heap (see :doc:`/api-reference/system/mem_alloc`), thus ESP-IDF does not make use of the heap implementations provided by Vanilla FreeRTOS. All FreeRTOS ports in ESP-IDF map FreeRTOS memory allocation/free calls (e.g., ``pvPortMalloc()`` and ``pvPortFree()``) to ESP-IDF heap API (i.e., :cpp:func:`heap_caps_malloc` and :cpp:func:`heap_caps_free`). However, the FreeRTOS ports ensure that all dynamic memory allocated by FreeRTOS is placed in internal memory.

.. note::
    If users wish to place FreeRTOS tasks/objects in external memory, users can use the following methods:

    - Allocate the task/object using one of the ``...CreateWithCaps()`` API such as :cpp:func:`xTaskCreateWithCaps` and :cpp:func:`xQueueCreateWithCaps` (see :ref:`freertos-idf-additional-api` for more details).
    - Manually allocate external memory for those objects using :cpp:func:`heap_caps_malloc`, then create the objects from the allocated memory using on of the ``...CreateStatic()`` FreeRTOS functions.
