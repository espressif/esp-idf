Maximizing Execution Speed
==========================

{IDF_TARGET_CONTROLLER_CORE_CONFIG:default="CONFIG_BT_CTRL_PINNED_TO_CORE", esp32="CONFIG_BTDM_CTRL_PINNED_TO_CORE_CHOICE", esp32s3="CONFIG_BT_CTRL_PINNED_TO_CORE_CHOICE"}
{IDF_TARGET_RF_TYPE:default="Wi-Fi/BT", esp32s2="Wi-Fi"}

Overview
--------

Optimizing execution speed is a key element of software performance. Code that executes faster can also have other positive effects, like reducing overall power consumption. However, improving execution speed may have trade-offs with other aspects of performance such as :doc:`size`.

Choose What To Optimize
-----------------------

If a function in the application firmware is executed once per week in the background, it may not matter if that function takes 10 ms or 100 ms to execute. If a function is executed constantly at 10 Hz, it matters greatly if it takes 10 ms or 100 ms to execute.

Most application firmwares will only have a small set of functions which require optimal performance. Perhaps those functions are executed very often, or have to meet some application requirements for latency or throughput. Optimization efforts should be targeted at these particular functions.

Measuring Performance
---------------------

The first step to improving something is to measure it.

Basic Performance Measurements
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If measuring performance relative to an external interaction with the world, you may be able to measure this directly (for example see the examples :example:`wifi/iperf` and :example:`ethernet/iperf` for measuring general network performance, or you can use an oscilloscope or logic analyzer to measure timing of an interaction with a device peripheral.)

Otherwise, one way to measure performance is to augment the code to take timing measurements:

.. code-block:: c

    #include "esp_timer.h"

    void measure_important_function(void) {
        const unsigned MEASUREMENTS = 5000;
        uint64_t start = esp_timer_get_time();

        for (int retries = 0; retries < MEASUREMENTS; retries++) {
            important_function(); // This is the thing you need to measure
        }

        uint64_t end = esp_timer_get_time();

        printf("%u iterations took %ull milliseconds (%ull microseconds per invocation)\n",
               MEASUREMENTS, (end - start)/1000, (end - start)/MEASUREMENTS);
    }

Executing the target multiple times can help average out factors like RTOS context switches, overhead of measurements, etc.

- Using :cpp:func:`esp_timer_get_time` generates "wall clock" timestamps with microsecond precision, but has moderate overhead each time the timing functions are called.
- It's also possible to use the standard Unix ``gettimeofday()`` and ``utime()`` functions, although the overhead is slightly higher.
- Otherwise, including ``hal/cpu_hal.h`` and calling the HAL function ``cpu_hal_get_cycle_count()`` will return the number of CPU cycles executed. This function has lower overhead than the others. It is good for measuring very short execution times with high precision.

  .. only:: not CONFIG_FREERTOS_UNICORE

      The CPU cycles are counted per-core, so only use this method from an interrupt handler, or a task that is pinned to a single core.

- If making "microbenchmarks" (i.e. benchmarking only a very small routine of code that runs in less than 1-2 milliseconds) then flash cache performance can sometimes cause big variations in timing measurements depending on the binary. This happens because binary layout can cause different patterns of cache misses in a particular sequence of execution. If the test code is larger then this effect usually averages out. Executing a small function multiple times when benchmarking can help reduce the impact of flash cache misses. Alternatively, move this code to IRAM (see :ref:`speed-targeted-optimizations`).

External Tracing
^^^^^^^^^^^^^^^^

The :doc:`/api-guides/app_trace` allows measuring code execution with minimal impact on the code itself.

Tasks
^^^^^

If the option :ref:`CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS` is enabled then the FreeRTOS API :cpp:func:`vTaskGetRunTimeStats` can be used to retrieve runtime information about the processor time used by each FreeRTOS task.

:ref:`SEGGER SystemView <app_trace-system-behaviour-analysis-with-segger-systemview>` is an excellent tool for visualizing task execution and looking for performance issues or improvements in the system as a whole.

Improving Overall Speed
-----------------------

The following optimizations will improve the execution of nearly all code - including boot times, throughput, latency, etc:

.. list::

    :esp32: - Set :ref:`CONFIG_ESPTOOLPY_FLASHFREQ` to 80 MHz. This is double the 40 MHz default value and will double the speed at which code is loaded or executed from flash. You should verify that the board or module that connects the {IDF_TARGET_NAME} to the flash chip is rated for 80 MHz operation at the relevant temperature ranges, before changing this setting. The hardware datasheet(s) will have this information.
    - Set :ref:`CONFIG_ESPTOOLPY_FLASHMODE` to QIO or QOUT mode (Quad I/O). Both will almost double the speed at which code is loaded or executed from flash compared to the default DIO mode. QIO is slightly faster than QOUT if both are supported. Note that both the flash chip model and the electrical connections between the {IDF_TARGET_NAME} and the flash chip must support quad I/O modes or the SoC will not work correctly.
    - Set :ref:`CONFIG_COMPILER_OPTIMIZATION` to "Optimize for performance (-O2)". This may slightly increase binary size compared to the default setting, but will almost certainly increase performance of some code. Note that if your code contains C or C++ Undefined Behaviour then increasing the compiler optimization level may expose bugs that otherwise are not seen.
    :SOC_CPU_HAS_FPU: - Avoid using floating point arithmetic (``float``). Even though {IDF_TARGET_NAME} has a single precision hardware floating point unit, floating point calculations are always slower than integer calculations. If possible then use fixed point representations, a different method of integer representation, or convert part of the calculation to be integer only before switching to floating point.
    :not SOC_CPU_HAS_FPU: - Avoid using floating point arithmetic (``float``). On {IDF_TARGET_NAME} these calculations are emulated in software and are very slow. If possible then use fixed point representations, a different method of integer representation, or convert part of the calculation to be integer only before switching to floating point.
    - Avoid using double precision floating point arithmetic (``double``). These calculations are emulated in software and are very slow. If possible then use an integer-based representation, or single-precision floating point.

Reduce Logging Overhead
^^^^^^^^^^^^^^^^^^^^^^^
Although standard output is buffered, it's possible for an application to be limited by the rate at which it can print data to log output once buffers are full. This is particularly relevant for startup time if a lot of output is logged, but can happen at other times as well. There are multiple ways to solve this problem:

.. list::

    - Reduce the volume of log output by lowering the app :ref:`CONFIG_LOG_DEFAULT_LEVEL` (the equivalent bootloader setting is :ref:`CONFIG_BOOTLOADER_LOG_LEVEL`). This also reduces the binary size, and saves some CPU time spent on string formatting.
    :not SOC_USB_OTG_SUPPORTED: - Increase the speed of logging output by increasing the :ref:`CONFIG_ESP_CONSOLE_UART_BAUDRATE`
    :SOC_USB_OTG_SUPPORTED: - Increase the speed of logging output by increasing the :ref:`CONFIG_ESP_CONSOLE_UART_BAUDRATE`. (Unless using internal USB-CDC for serial console, in which case the serial throughput doesn't depend on the configured baud rate.)

Not Recommended
^^^^^^^^^^^^^^^

The following options will also increase execution speed, but are not recommended as they also reduce the debuggability of the firmware application and may increase the severity of any bugs.

.. list::

   - Set :ref:`CONFIG_COMPILER_OPTIMIZATION_ASSERTION_LEVEL` to disabled. This also reduces firmware binary size by a small amount. However, it may increase the severity of bugs in the firmware including security-related bugs. If necessary to do this to optimize a particular function, consider adding ``#define NDEBUG`` in the top of that single source file instead.

.. _speed-targeted-optimizations:

Targeted Optimizations
----------------------

The following changes will increase the speed of a chosen part of the firmware application:

.. list::

    - Move frequently executed code to IRAM. By default, all code in the app is executed from flash cache. This means that it's possible for the CPU to have to wait on a "cache miss" while the next instructions are loaded from flash. Functions which are copied into IRAM are loaded once at boot time, and then will always execute at full speed.

      IRAM is a limited resource, and using more IRAM may reduce available DRAM, so a strategic approach is needed when moving code to IRAM. See :ref:`iram` for more information.

    -  Jump table optimizations can be re-enabled for individual source files that don't need to be placed in IRAM. For hot paths in large switch cases this will improve performance. For instructions on how to add the -fjump-tables -ftree-switch-conversion options when compiling individual source files, see :ref:`component_build_control`

Improving Startup Time
----------------------

In addition to the overall performance improvements shown above, the following options can be tweaked to specifically reduce startup time:

.. list::

   - Minimizing the :ref:`CONFIG_LOG_DEFAULT_LEVEL` and :ref:`CONFIG_BOOTLOADER_LOG_LEVEL` has a large impact on startup time. To enable more logging after the app starts up, set the :ref:`CONFIG_LOG_MAXIMUM_LEVEL` as well and then call :cpp:func:`esp_log_set_level` to restore higher level logs. The :example:`system/startup_time` main function shows how to do this.
   - If using deep sleep, setting :ref:`CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP` allows a faster wake from sleep. Note that if using Secure Boot this represents a security compromise, as Secure Boot validation will not be performed on wake.
   - Setting :ref:`CONFIG_BOOTLOADER_SKIP_VALIDATE_ON_POWER_ON` will skip verifying the binary on every boot from power-on reset. How much time this saves depends on the binary size and the flash settings. Note that this setting carries some risk if the flash becomes corrupt unexpectedly. Read the help text of the :ref:`config item <CONFIG_BOOTLOADER_SKIP_VALIDATE_ON_POWER_ON>` for an explanation and recommendations if using this option.
   - It's possible to save a small amount of time during boot by disabling RTC slow clock calibration. To do so, set :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_RTC_CLK_CAL_CYCLES` to 0. Any part of the firmware that uses RTC slow clock as a timing source will be less accurate as a result.

The example project :example:`system/startup_time` is pre-configured to optimize startup time. The files :example_file:`system/startup_time/sdkconfig.defaults` and :example_file:`system/startup_time/sdkconfig.defaults.{IDF_TARGET_PATH_NAME}` contain all of these settings. You can append these to the end of your project's own ``sdkconfig`` file to merge the settings, but please read the documentation for each setting first.

Task Priorities
---------------

As ESP-IDF FreeRTOS is a real-time operating system, it's necessary to ensure that high throughput or low latency tasks are granted a high priority in order to run immediately. Priority is set when calling :cpp:func:`xTaskCreate` or :cpp:func:`xTaskCreatePinnedToCore` and can be changed at runtime by calling :cpp:func:`vTaskPrioritySet`.

It's also necessary to ensure that tasks yield CPU (by calling :cpp:func:`vTaskDelay`, ``sleep()``, or by blocking on semaphores, queues, task notifications, etc) in order to not starve lower priority tasks and cause problems for the overall system. The :ref:`task-watchdog-timer` provides a mechanism to automatically detect if task starvation happens, however note that a Task WDT timeout does not always indicate a problem (sometimes the correct operation of the firmware requires some long-running computation). In these cases tweaking the Task WDT timeout or even disabling the Task WDT may be necessary.

.. _built-in-task-priorities:

Built-In Task Priorities
^^^^^^^^^^^^^^^^^^^^^^^^

ESP-IDF starts a number of system tasks at fixed priority levels. Some are automatically started during the boot process, some are started only if the application firmware initializes a particular feature. To optimize performance, structure application task priorities so that they are not delayed by system tasks, while also not starving system tasks and impacting other functions of the system.

This may require splitting up a particular task. For example, perform a time-critical operation in a high priority task or an interrupt handler and do the non-time-critical part in a lower priority task.

Header :idf_file:`components/esp_system/include/esp_task.h` contains macros for the priority levels used for built-in ESP-IDF tasks system.

Common priorities are:

.. Note: the following two lists should be kept the same, but the second list also shows CPU affinities

.. only:: CONFIG_FREERTOS_UNICORE

    .. list::

        - :ref:`Main task that executes app_main function <app-main-task>` has minimum priority (1).
        - :doc:`/api-reference/system/esp_timer` system task to manage timer events and execute callbacks has high priority (22, ``ESP_TASK_TIMER_PRIO``)
        - FreeRTOS Timer Task to handle FreeRTOS timer callbacks is created when the scheduler initializes and has minimum task priority (1, :ref:`configurable <CONFIG_FREERTOS_TIMER_TASK_PRIORITY>`).
        - :doc:`/api-guides/event-handling` system task to manage the default system event loop and execute callbacks has high priority (20, ``ESP_TASK_EVENT_PRIO``). This configuration is only used if the application calls :cpp:func:`esp_event_loop_create_default`, it's possible to call :cpp:func:`esp_event_loop_create` with a custom task configuration instead.
        - :doc:`/api-guides/lwip` TCP/IP task has high priority (18, ``ESP_TASK_TCPIP_PRIO``).
        - :doc:`Wi-Fi Driver </api-guides/wifi>` task has high priority (23).
        - Wi-Fi wpa_supplicant component may create dedicated tasks while the Wi-Fi Protected Setup (WPS), WPA2 EAP-TLS, Device Provisioning Protocol (DPP) or BSS Transition Management (BTM) features are in use. These tasks all have low priority (2).
        :SOC_BT_SUPPORTED: - :doc:`Bluetooth Controller </api-reference/bluetooth/index>` task has high priority (23, ``ESP_TASK_BT_CONTROLLER_PRIO``). The Bluetooth Controller needs to respond to requests with low latency, so it should always be close to the highest priority task in the system.
        :SOC_BT_SUPPORTED: - :doc:`NimBLE Bluetooth Host </api-reference/bluetooth/nimble/index>` host task has high priority (21).
        - The Ethernet driver creates a task for the MAC to receive Ethernet frames. If using the default config ``ETH_MAC_DEFAULT_CONFIG`` then the priority is medium-high (15). This setting can be changed by passing a custom :cpp:class:`eth_mac_config_t` struct when initializing the Ethernet MAC.
        - If using the :doc:`mDNS </api-reference/protocols/mdns>` component, it creates a task with default low priority 1 (:ref:`configurable<CONFIG_MDNS_TASK_PRIORITY>`.
        - If using the :doc:`MQTT </api-reference/protocols/mqtt>` component, it creates a task with default priority 5 (:ref:`configurable<CONFIG_MQTT_TASK_PRIORITY>`, depends on :ref:`CONFIG_MQTT_USE_CUSTOM_CONFIG` (also configurable runtime by ``task_prio`` field in the :cpp:class:`esp_mqtt_client_config_t`)

.. only :: not CONFIG_FREERTOS_UNICORE

    .. list::

        - :ref:`Main task that executes app_main function <app-main-task>` has minimum priority (1). This task is pinned to Core 0 by default (:ref:`configurable<CONFIG_ESP_MAIN_TASK_AFFINITY>`).
        - :doc:`/api-reference/system/esp_timer` system task to manage high precision timer events and execute callbacks has high priority (22, ``ESP_TASK_TIMER_PRIO``). This task is pinned to Core 0.
        - FreeRTOS Timer Task to handle FreeRTOS timer callbacks is created when the scheduler initializes and has minimum task priority (1, :ref:`configurable <CONFIG_FREERTOS_TIMER_TASK_PRIORITY>`). This task is pinned to Core 0.
        - :doc:`/api-guides/event-handling` system task to manage the default system event loop and execute callbacks has high priority (20, ``ESP_TASK_EVENT_PRIO``) and pinned to Core 0. This configuration is only used if the application calls :cpp:func:`esp_event_loop_create_default`, it's possible to call :cpp:func:`esp_event_loop_create` with a custom task configuration instead.
        - :doc:`/api-guides/lwip` TCP/IP task has high priority (18, ``ESP_TASK_TCPIP_PRIO``) and is not pinned to any core (:ref:`configurable<CONFIG_LWIP_TCPIP_TASK_AFFINITY>`).
        - :doc:`Wi-Fi Driver </api-guides/wifi>` task has high priority (23) and is pinned to Core 0 by default (:ref:`configurable<CONFIG_ESP32_WIFI_TASK_CORE_ID>`).
        - Wi-Fi wpa_supplicant component may create dedicated tasks while the Wi-Fi Protected Setup (WPS), WPA2 EAP-TLS, Device Provisioning Protocol (DPP) or BSS Transition Management (BTM) features are in use. These tasks all have low priority (2) and are not pinned to any core.
        :SOC_BT_SUPPORTED: - :doc:`Bluetooth Controller </api-reference/bluetooth/index>` task has high priority (23, ``ESP_TASK_BT_CONTROLLER_PRIO``) and is pinned to Core 0 by default (:ref:`configurable <{IDF_TARGET_CONTROLLER_CORE_CONFIG}>`). The Bluetooth Controller needs to respond to requests with low latency, so it should always be close to the highest priority task assigned to a single CPU.
        :SOC_BT_SUPPORTED: - :doc:`NimBLE Bluetooth Host </api-reference/bluetooth/nimble/index>` host task has high priority (21) and is pinned to Core 0 by default (:ref:`configurable <CONFIG_BT_NIMBLE_PINNED_TO_CORE_CHOICE>`).
        :esp32: - :doc:`Bluedroid Bluetooth Host </api-reference/bluetooth/index>` creates multiple tasks when used:
               - Stack event callback task ("BTC") has high priority (19).
               - Stack BTU layer task has high priority (20).
               - Host HCI host task has high priority (22).

               All Bluedroid Tasks are pinned to the same core, which is Core 0 by default (:ref:`configurable <CONFIG_BT_BLUEDROID_PINNED_TO_CORE_CHOICE>`).
        - The Ethernet driver creates a task for the MAC to receive Ethernet frames. If using the default config ``ETH_MAC_DEFAULT_CONFIG`` then the priority is medium-high (15) and the task is not pinned to any core. These settings can be changed by passing a custom :cpp:class:`eth_mac_config_t` struct when initializing the Ethernet MAC.
        - If using the :doc:`mDNS </api-reference/protocols/mdns>` component, it creates a task with default low priority 1 (:ref:`configurable <CONFIG_MDNS_TASK_PRIORITY>`) and pinned to CPU0 (:ref:`configurable <CONFIG_MDNS_TASK_AFFINITY>`).
        - If using the :doc:`MQTT </api-reference/protocols/mqtt>` component, it creates a task with default priority 5 (:ref:`configurable <CONFIG_MQTT_TASK_PRIORITY>`, depends on :ref:`CONFIG_MQTT_USE_CUSTOM_CONFIG`) and not pinned to any core (:ref:`configurable <CONFIG_MQTT_TASK_CORE_SELECTION_ENABLED>`).

Choosing application task priorities
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: CONFIG_FREERTOS_UNICORE

    In general, it's not recommended to set task priorities higher than the built-in {IDF_TARGET_RF_TYPE} operations as starving them of CPU may make the system unstable. For very short timing-critical operations that don't use the network, use an ISR or a very restricted task (very short bursts of runtime only) at highest priority (24). Choosing priority 19 will allow lower layer {IDF_TARGET_RF_TYPE} functionality to run without delays, but still preempts the lwIP TCP/IP stack and other less time-critical internal functionality - this is the best option for time-critical tasks that don't perform network operations. Any task that does TCP/IP network operations should run at lower priority than the lwIP TCP/IP task (18) to avoid priority inversion issues.

.. only:: not CONFIG_FREERTOS_UNICORE

    With a few exceptions (most importantly the lwIP TCP/IP task), in the default configuration most built-in tasks are pinned to Core 0. This makes it quite easy for the application to place high priority tasks on Core 1. Using priority 19 or higher will guarantee an application task can run on Core 1 without being preempted by any built-in task. To further isolate the tasks running on each CPU, configure the :ref:`lwIP task <CONFIG_LWIP_TCPIP_TASK_AFFINITY>` to only run on Core 0 instead of either core (this may reduce total TCP/IP throughput depending on what other tasks are running).

    In general, it's not recommended to set task priorities on Core 0 higher than the built-in {IDF_TARGET_RF_TYPE} operations as starving them of CPU may make the system unstable. Choosing priority 19 and Core 0 will allow lower layer {IDF_TARGET_RF_TYPE} functionality to run without delays, but still pre-empts the lwIP TCP/IP stack and other less time-critical internal functionality - this is an option for time-critical tasks that don't perform network operations. Any task that does TCP/IP network operations should run at lower priority than the lwIP TCP/IP task (18) to avoid priority inversion issues.

    .. note::

        Setting a task to always run in preference to built-in ESP-IDF tasks does not require pinning to Core 1. The task can be left unpinned - at priority 17 or lower - to optionally run on Core 0 as well, if no higher priority built-in task is running there. Using unpinned tasks can improve the overall CPU utilization, however it makes reasoning about task scheduling more complex.

.. note::

    Task execution is always completely suspended when writing to the built-in SPI flash chip. Only :ref:`iram-safe-interrupt-handlers` will continue executing.

Improving Interrupt Performance
-------------------------------

ESP-IDF supports dynamic :doc:`/api-reference/system/intr_alloc` with interrupt preemption. Each interrupt in the system has a priority, and higher priority interrupts will preempt lower priority ones.

Interrupt handlers will execute in preference to any task (provided the task is not inside a critical section). For this reason, it's important to minimize the amount of time spent executing in an interrupt handler.

To obtain the best performance for a particular interrupt handler:

.. list::

    - Assign more important interrupts a higher priority using a flag such as ``ESP_INTR_FLAG_LEVEL2`` or ``ESP_INTR_FLAG_LEVEL3`` when calling :cpp:func:`esp_intr_alloc`.
    :not CONFIG_FREERTOS_UNICORE: - Assign the interrupt on a CPU where built-in {IDF_TARGET_RF_TYPE} tasks are not configured to run (this means assigning on Core 1 by default, see :ref:`built-in-task-priorities`). Interrupts are assigned on the same CPU where the :cpp:func:`esp_intr_alloc` function call is made.
    - If you're sure the entire interrupt handler can run from IRAM (see :ref:`iram-safe-interrupt-handlers`) then set the ``ESP_INTR_FLAG_IRAM`` flag when calling :cpp:func:`esp_intr_alloc` to assign the interrupt. This prevents it being temporarily disabled if the application firmware writes to the internal SPI flash.
    - Even if the interrupt handler is not IRAM safe, if it is going to be executed frequently then consider moving the handler function to IRAM anyhow. This minimizes the chance of a flash cache miss when the interrupt code is executed (see :ref:`speed-targeted-optimizations`). It's possible to do this without adding the ``ESP_INTR_FLAG_IRAM`` flag to mark the interrupt as IRAM-safe, if only part of the handler is guaranteed to be in IRAM.

Improving Network Speed
-----------------------

* For Wi-Fi, see :ref:`How-to-improve-Wi-Fi-performance` and :ref:`wifi-buffer-usage`
* For lwIP TCP/IP (Wi-Fi and Ethernet), see :ref:`lwip-performance`
* The :example:`wifi/iperf` example contains a configuration that is heavily optimized for Wi-Fi TCP/IP throughput. Append the contents of the files :example_file:`wifi/iperf/sdkconfig.defaults`, :example_file:`wifi/iperf/sdkconfig.defaults.{IDF_TARGET_PATH_NAME}` and :example_file:`wifi/iperf/sdkconfig.ci.99` to your project ``sdkconfig`` file in order to add all of these options. Note that some of these options may have trade-offs in terms of reduced debuggability, increased firmware size, increased memory usage, or reduced performance of other features. To get the best result, read the documentation pages linked above and use this information to determine exactly which options are best suited for your app.
