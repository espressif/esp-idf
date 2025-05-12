Speed Optimization
==================

:link_to_translation:`zh_CN:[中文]`

{IDF_TARGET_CONTROLLER_CORE_CONFIG:default="CONFIG_BT_CTRL_PINNED_TO_CORE", esp32="CONFIG_BTDM_CTRL_PINNED_TO_CORE_CHOICE", esp32s3="CONFIG_BT_CTRL_PINNED_TO_CORE_CHOICE"}
{IDF_TARGET_RF_TYPE:default="Wi-Fi/Bluetooth", esp32s2="Wi-Fi", esp32c6="Wi-Fi/Bluetooth/802.15.4", esp32c61="Wi-Fi/Bluetooth", esp32h2="Bluetooth/802.15.4", esp32h21="Bluetooth/802.15.4", esp32h4="Bluetooth/802.15.4", esp32c5="Wi-Fi/Bluetooth/802.15.4"}

Overview
--------

Optimizing execution speed is a key element of software performance. Code that executes faster can also have other positive effects, e.g., reducing overall power consumption. However, improving execution speed may have trade-offs with other aspects of performance such as :doc:`size`.

Choose What to Optimize
-----------------------

If a function in the application firmware is executed once per week in the background, it may not matter if that function takes 10 ms or 100 ms to execute. If a function is executed constantly at 10 Hz, it matters greatly if it takes 10 ms or 100 ms to execute.

Most kinds of application firmware only have a small set of functions that require optimal performance. Perhaps those functions are executed very often, or have to meet some application requirements for latency or throughput. Optimization efforts should be targeted at these particular functions.

Measuring Performance
---------------------

The first step to improving something is to measure it.

Basic Performance Measurements
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You may be able to measure directly the performance relative to an external interaction with the world, e.g., see the examples :example:`wifi/iperf` and :example:`ethernet/iperf` for measuring general network performance. Or you can use an oscilloscope or logic analyzer to measure the timing of an interaction with a device peripheral.

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

        printf("%u iterations took %llu milliseconds (%llu microseconds per invocation)\n",
               MEASUREMENTS, (end - start)/1000, (end - start)/MEASUREMENTS);
    }

Executing the target multiple times can help average out factors, e.g., RTOS context switches, overhead of measurements, etc.

- Using :cpp:func:`esp_timer_get_time` generates "wall clock" timestamps with microsecond precision, but has moderate overhead each time the timing functions are called.
- It is also possible to use the standard Unix ``gettimeofday()`` and ``utime()`` functions, although the overhead is slightly higher.
- Otherwise, including ``hal/cpu_hal.h`` and calling the HAL function ``cpu_hal_get_cycle_count()`` returns the number of CPU cycles executed. This function has lower overhead than the others, which is good for measuring very short execution times with high precision.

  .. only:: SOC_HP_CPU_HAS_MULTIPLE_CORES

      The CPU cycles are counted per-core, so only use this method from an interrupt handler, or a task that is pinned to a single core.

- While performing "microbenchmarks" (i.e., benchmarking only a very small routine of code that runs in less than 1-2 milliseconds), the flash cache performance can sometimes cause big variations in timing measurements depending on the binary. This happens because binary layout can cause different patterns of cache misses in a particular sequence of execution. If the test code is larger, then this effect usually averages out. Executing a small function multiple times when benchmarking can help reduce the impact of flash cache misses. Alternatively, move this code to IRAM (see :ref:`speed-targeted-optimizations`).

External Tracing
^^^^^^^^^^^^^^^^

The :doc:`/api-guides/app_trace` allows measuring code execution with minimal impact on the code itself.

Tasks
^^^^^

If the option :ref:`CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS` is enabled, then the FreeRTOS API :cpp:func:`vTaskGetRunTimeStats` can be used to retrieve runtime information about the processor time used by each FreeRTOS task.

:ref:`SEGGER SystemView <app_trace-system-behaviour-analysis-with-segger-systemview>` is an excellent tool for visualizing task execution and looking for performance issues or improvements in the system as a whole.

Improving Overall Speed
-----------------------

The following optimizations improve the execution of nearly all code, including boot times, throughput, latency, etc:

.. list::

    :esp32: - Set :ref:`CONFIG_ESPTOOLPY_FLASHFREQ` to 80 MHz. This is double the 40 MHz default value and doubles the speed at which code is loaded or executed from flash. You should verify that the board or module that connects the {IDF_TARGET_NAME} to the flash chip is rated for 80 MHz operation at the relevant temperature ranges before changing this setting. This information is contained in the hardware datasheet(s).
    - Set :ref:`CONFIG_ESPTOOLPY_FLASHMODE` to QIO or QOUT mode (Quad I/O). Both almost double the speed at which code is loaded or executed from flash compared to the default DIO mode. QIO is slightly faster than QOUT if both are supported. Note that both the flash chip model, and the electrical connections between the {IDF_TARGET_NAME} and the flash chip must support quad I/O modes or the SoC will not work correctly.
    - Set :ref:`CONFIG_COMPILER_OPTIMIZATION` to ``Optimize for performance (-O2)`` . This may slightly increase binary size compared to the default setting, but almost certainly increases the performance of some code. Note that if your code contains C or C++ Undefined Behavior, then increasing the compiler optimization level may expose bugs that otherwise are not seen.
    :SOC_ASSIST_DEBUG_SUPPORTED: - Set :ref:`CONFIG_ESP_SYSTEM_HW_STACK_GUARD` to disabled. This may slightly increase the performance of some code, especially in cases where a lot of interrupts occur on the device.
    :esp32: - If the application uses PSRAM and is based on ESP32 rev. 3 (ECO3), setting :ref:`CONFIG_ESP32_REV_MIN` to ``3`` disables PSRAM bug workarounds, reducing the code size and improving overall performance.
    :SOC_CPU_HAS_FPU: - Avoid using floating point arithmetic ``float``. Even though {IDF_TARGET_NAME} has a single precision hardware floating point unit, floating point calculations are always slower than integer calculations. If possible then use fixed point representations, a different method of integer representation, or convert part of the calculation to be integer only before switching to floating point.
    :not SOC_CPU_HAS_FPU: - Avoid using floating point arithmetic ``float``. On {IDF_TARGET_NAME} these calculations are emulated in software and are very slow. If possible, use fixed point representations, a different method of integer representation, or convert part of the calculation to be integer only before switching to floating point.
    - Avoid using double precision floating point arithmetic ``double``. These calculations are emulated in software and are very slow. If possible then use an integer-based representation, or single-precision floating point.
    :CONFIG_ESP_ROM_HAS_SUBOPTIMAL_NEWLIB_ON_MISALIGNED_MEMORY: - Avoid misaligned 4-byte memory accesses in performance-critical code sections. For potential performance improvements, consider enabling :ref:`CONFIG_LIBC_OPTIMIZED_MISALIGNED_ACCESS`, which requires approximately 190 bytes of IRAM and 870 bytes of flash memory. Note that properly aligned memory operations will always execute at full speed without performance penalties.


.. only:: esp32s2 or esp32s3 or esp32p4

    Change cache size
    ^^^^^^^^^^^^^^^^^

    On {IDF_TARGET_NAME}, increasing the overall speed can be achieved to some degree by increasing the size of cache and thus potentially decreasing the frequency of "cache misses" through the Kconfig option(s) listed below.

    .. list::

        :esp32s2: - :ref:`CONFIG_ESP32S2_INSTRUCTION_CACHE_SIZE`.
        :esp32s2: - :ref:`CONFIG_ESP32S2_DATA_CACHE_SIZE`.
        :esp32s3: - :ref:`CONFIG_ESP32S3_INSTRUCTION_CACHE_SIZE`.
        :esp32s3: - :ref:`CONFIG_ESP32S3_DATA_CACHE_SIZE`.
        :esp32p4: - :ref:`CONFIG_CACHE_L2_CACHE_SIZE`.


    .. note::

        Increasing the cache size will also result in reducing the amount of available RAM.


.. only:: SOC_CACHE_L2_CACHE_SIZE_CONFIGURABLE

    .. note::

        On {IDF_TARGET_NAME}, the L2 cache size is configurable via the Kconfig option :ref:`CONFIG_CACHE_L2_CACHE_SIZE`.
        Setting the L2 cache size to its smallest value will maximize the available RAM while also potentially augmenting the frequency of "cache misses".
        Setting the L2 cache size to its largest value will potentially lower the frequency of "cache misses" at the cost of reducing the available RAM.

Reduce Logging Overhead
^^^^^^^^^^^^^^^^^^^^^^^

Although standard output is buffered, it is possible for an application to be limited by the rate at which it can print data to log output once buffers are full. This is particularly relevant for startup time if a lot of output is logged, but such problem can happen at other times as well. There are multiple ways to solve this problem:

.. list::

    - Reduce the volume of log output by lowering the app :ref:`CONFIG_LOG_DEFAULT_LEVEL` (the equivalent bootloader setting is :ref:`CONFIG_BOOTLOADER_LOG_LEVEL`). This also reduces the binary size, and saves some CPU time spent on string formatting.
    :not SOC_USB_OTG_SUPPORTED: - Increase the speed of logging output by increasing the :ref:`CONFIG_ESP_CONSOLE_UART_BAUDRATE`.
    :SOC_USB_OTG_SUPPORTED: - Increase the speed of logging output by increasing the :ref:`CONFIG_ESP_CONSOLE_UART_BAUDRATE`. However, if you are using internal USB-CDC, the serial throughput is not dependent on the configured baud rate.
    - If your application does not require dynamic log level changes and you do not need to control logs per module using tags, consider disabling :ref:`CONFIG_LOG_DYNAMIC_LEVEL_CONTROL` and changing :ref:`CONFIG_LOG_TAG_LEVEL_IMPL`. It helps to reduce memory usage and also contributes to speeding up log operations in your application about 10 times.

Not Recommended
^^^^^^^^^^^^^^^

The following options also increase execution speed, but are not recommended as they also reduce the debuggability of the firmware application and may increase the severity of any bugs.

.. list::

   - Set :ref:`CONFIG_COMPILER_OPTIMIZATION_ASSERTION_LEVEL` to disabled. This also reduces firmware binary size by a small amount. However, it may increase the severity of bugs in the firmware including security-related bugs. If it is necessary to do this to optimize a particular function, consider adding ``#define NDEBUG`` at the top of that single source file instead.

.. _speed-targeted-optimizations:


Targeted Optimizations
----------------------

The following changes increase the speed of a chosen part of the firmware application:

.. list::

    - Move frequently executed code to IRAM. By default, all code in the app is executed from flash cache. This means that it is possible for the CPU to have to wait on a "cache miss" while the next instructions are loaded from flash. Functions which are copied into IRAM are loaded once at boot time, and then always execute at full speed.

      IRAM is a limited resource, and using more IRAM may reduce available DRAM, so a strategic approach is needed when moving code to IRAM. See :ref:`iram` for more information.

    -  Jump table optimizations can be re-enabled for individual source files that do not need to be placed in IRAM. For hot paths in large ``switch cases``, this improves performance. For instructions on how to add the ``-fjump-tables`` and ``-ftree-switch-conversion`` options when compiling individual source files, see :ref:`component_build_control`

Improving Startup Time
----------------------

In addition to the overall performance improvements shown above, the following options can be tweaked to specifically reduce startup time:

.. list::

   - Minimizing the :ref:`CONFIG_LOG_DEFAULT_LEVEL` and :ref:`CONFIG_BOOTLOADER_LOG_LEVEL` has a large impact on startup time. To enable more logging after the app starts up, set the :ref:`CONFIG_LOG_MAXIMUM_LEVEL` as well, and then call :cpp:func:`esp_log_level_set` to restore higher level logs. The :example:`system/startup_time` main function shows how to do this.
   :SOC_RTC_FAST_MEM_SUPPORTED: - If using Deep-sleep mode, setting :ref:`CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP` allows a faster wake from sleep. Note that if using Secure Boot, this represents a security compromise, as Secure Boot validation are not be performed on wake.
   - Setting :ref:`CONFIG_BOOTLOADER_SKIP_VALIDATE_ON_POWER_ON` skips verifying the binary on every boot from the power-on reset. How much time this saves depends on the binary size and the flash settings. Note that this setting carries some risk if the flash becomes corrupt unexpectedly. Read the help text of the :ref:`config item <CONFIG_BOOTLOADER_SKIP_VALIDATE_ON_POWER_ON>` for an explanation and recommendations if using this option.
   - It is possible to save a small amount of time during boot by disabling RTC slow clock calibration. To do so, set :ref:`CONFIG_RTC_CLK_CAL_CYCLES` to 0. Any part of the firmware that uses RTC slow clock as a timing source will be less accurate as a result.
   :SOC_SPIRAM_SUPPORTED: - When external memory is used (:ref:`CONFIG_SPIRAM` enabled), enabling memory test on the external memory (:ref:`CONFIG_SPIRAM_MEMTEST`) can have a large impact on startup time (approximately 1 second per 4 MB of memory tested). Disabling the memory tests will reduce startup time at the expense of testing the external memory.
   :SOC_SPIRAM_SUPPORTED: - When external memory is used (:ref:`CONFIG_SPIRAM` enabled), enabling comprehensive poisoning will increase the startup time (approximately 300 milliseconds per 4 MiB of memory set) since all the memory used as heap (including the external memory) will be set to a default value.

The example project :example:`system/startup_time` is pre-configured to optimize startup time. The file :example_file:`system/startup_time/sdkconfig.defaults` contain all of these settings. You can append these to the end of your project's own ``sdkconfig`` file to merge the settings, but please read the documentation for each setting first.

Task Priorities
---------------

As ESP-IDF FreeRTOS is a real-time operating system, it is necessary to ensure that high-throughput or low-latency tasks are granted a high priority in order to run immediately. Priority is set when calling :cpp:func:`xTaskCreate` or :cpp:func:`xTaskCreatePinnedToCore` and can be changed at runtime by calling :cpp:func:`vTaskPrioritySet`.

It is also necessary to ensure that tasks yield CPU (by calling :cpp:func:`vTaskDelay`, ``sleep()``, or by blocking on semaphores, queues, task notifications, etc) in order to not starve lower-priority tasks and cause problems for the overall system. The :ref:`task-watchdog-timer` provides a mechanism to automatically detect if task starvation happens. However, note that a TWDT timeout does not always indicate a problem, because sometimes the correct operation of the firmware requires some long-running computation. In these cases, tweaking the TWDT timeout or even disabling the TWDT may be necessary.

.. _built-in-task-priorities:

Built-in Task Priorities
^^^^^^^^^^^^^^^^^^^^^^^^

ESP-IDF starts a number of system tasks at fixed priority levels. Some are automatically started during the boot process, while some are started only if the application firmware initializes a particular feature. To optimize performance, structure the task priorities of your application properly to ensure the tasks are not delayed by the system tasks, while also not starving system tasks and impacting other functions of the system.

This may require splitting up a particular task. For example, perform a time-critical operation in a high-priority task or an interrupt handler and do the non-time-critical part in a lower-priority task.

Header :idf_file:`components/esp_system/include/esp_task.h` contains macros for the priority levels used for built-in ESP-IDF tasks system. See :ref:`freertos_system_tasks` for more details about the system tasks.

Common priorities are:

.. Note: the following two lists should be kept the same, but the second list also shows CPU affinities

.. only:: not SOC_HP_CPU_HAS_MULTIPLE_CORES

    .. list::

        - :ref:`app-main-task` that executes app_main function has minimum priority (1).
        - :doc:`/api-reference/system/esp_timer` system task to manage timer events and execute callbacks has high priority (22, ``ESP_TASK_TIMER_PRIO``)
        - FreeRTOS Timer Task to handle FreeRTOS timer callbacks is created when the scheduler initializes and has minimum task priority (1, :ref:`configurable <CONFIG_FREERTOS_TIMER_TASK_PRIORITY>`).
        - :doc:`/api-reference/system/esp_event` system task to manage the default system event loop and execute callbacks has high priority (20, ``ESP_TASK_EVENT_PRIO``). This configuration is only used if the application calls :cpp:func:`esp_event_loop_create_default`. It is possible to call :cpp:func:`esp_event_loop_create` with a custom task configuration instead.
        - :doc:`/api-guides/lwip` TCP/IP task has high priority (18, ``ESP_TASK_TCPIP_PRIO``).
        :SOC_WIFI_SUPPORTED: - :doc:`/api-guides/wifi` task has high priority (23).
        :SOC_WIFI_SUPPORTED: - Wi-Fi wpa_supplicant component may create dedicated tasks while the Wi-Fi Protected Setup (WPS), WPA2 EAP-TLS, Device Provisioning Protocol (DPP) or BSS Transition Management (BTM) features are in use. These tasks all have low priority (2).
        :SOC_BT_SUPPORTED: - :doc:`/api-reference/bluetooth/controller_vhci` task has high priority (23, ``ESP_TASK_BT_CONTROLLER_PRIO``). The Bluetooth Controller needs to respond to requests with low latency, so it should always be among the highest priority task in the system.
        :SOC_BT_SUPPORTED: - :doc:`/api-reference/bluetooth/nimble/index` task has high priority (21).
        - The Ethernet driver creates a task for the MAC to receive Ethernet frames. If using the default config ``ETH_MAC_DEFAULT_CONFIG`` then the priority is medium-high (15). This setting can be changed by passing a custom :cpp:class:`eth_mac_config_t` struct when initializing the Ethernet MAC.
        - If using the :doc:`/api-reference/protocols/mqtt` component, it creates a task with default priority 5 (:ref:`configurable<CONFIG_MQTT_TASK_PRIORITY>`), depending on :ref:`CONFIG_MQTT_USE_CUSTOM_CONFIG`, and also configurable at runtime by ``task_prio`` field in the :cpp:class:`esp_mqtt_client_config_t`)
        - To see what is the task priority for ``mDNS`` service, please check `Performance Optimization <https://docs.espressif.com/projects/esp-protocols/mdns/docs/latest/en/index.html#performance-optimization>`__.

.. only:: SOC_HP_CPU_HAS_MULTIPLE_CORES

    .. list::

        - :ref:`app-main-task` that executes app_main function has minimum priority (1). This task is pinned to Core 0 by default (:ref:`configurable<CONFIG_ESP_MAIN_TASK_AFFINITY>`).
        - :doc:`/api-reference/system/esp_timer` system task to manage high precision timer events and execute callbacks has high priority (22, ``ESP_TASK_TIMER_PRIO``). This task is pinned to Core 0.
        - FreeRTOS Timer Task to handle FreeRTOS timer callbacks is created when the scheduler initializes and has minimum task priority (1, :ref:`configurable <CONFIG_FREERTOS_TIMER_TASK_PRIORITY>`). This task is pinned to Core 0.
        - :doc:`/api-reference/system/esp_event` system task to manage the default system event loop and execute callbacks has high priority (20, ``ESP_TASK_EVENT_PRIO``) and it is pinned to Core 0. This configuration is only used if the application calls :cpp:func:`esp_event_loop_create_default`, it is possible to call :cpp:func:`esp_event_loop_create` with a custom task configuration instead.
        - :doc:`/api-guides/lwip` TCP/IP task has high priority (18, ``ESP_TASK_TCPIP_PRIO``) and is not pinned to any core (:ref:`configurable<CONFIG_LWIP_TCPIP_TASK_AFFINITY>`).
        :SOC_WIFI_SUPPORTED: - :doc:`/api-guides/wifi` task has high priority (23) and is pinned to Core 0 by default (:ref:`configurable<CONFIG_ESP_WIFI_TASK_CORE_ID>`).
        :SOC_WIFI_SUPPORTED: - Wi-Fi wpa_supplicant component may create dedicated tasks while the Wi-Fi Protected Setup (WPS), WPA2 EAP-TLS, Device Provisioning Protocol (DPP) or BSS Transition Management (BTM) features are in use. These tasks all have low priority (2) and are not pinned to any core.
        :SOC_BT_SUPPORTED: - :doc:`/api-reference/bluetooth/controller_vhci` task has high priority (23, ``ESP_TASK_BT_CONTROLLER_PRIO``) and is pinned to Core 0 by default (:ref:`configurable <{IDF_TARGET_CONTROLLER_CORE_CONFIG}>`). The Bluetooth Controller needs to respond to requests with low latency, so it should always be among the highest priority task assigned to a single CPU.
        :SOC_BT_SUPPORTED: - :doc:`/api-reference/bluetooth/nimble/index` task has high priority (21) and is pinned to Core 0 by default (:ref:`configurable <CONFIG_BT_NIMBLE_PINNED_TO_CORE_CHOICE>`).
        :esp32: - :doc:`/api-reference/bluetooth/index` creates multiple tasks when used:
               - Stack event callback task ("BTC") has high priority (19).
               - Stack BTU layer task has high priority (20).
               - Host HCI host task has high priority (22).

               All Bluedroid Tasks are pinned to the same core, which is Core 0 by default (:ref:`configurable <CONFIG_BT_BLUEDROID_PINNED_TO_CORE_CHOICE>`).

        - The Ethernet driver creates a task for the MAC to receive Ethernet frames. If using the default config ``ETH_MAC_DEFAULT_CONFIG`` then the priority is medium-high (15) and the task is not pinned to any core. These settings can be changed by passing a custom :cpp:class:`eth_mac_config_t` struct when initializing the Ethernet MAC.
        - If using the :doc:`/api-reference/protocols/mqtt` component, it creates a task with default priority 5 (:ref:`configurable <CONFIG_MQTT_TASK_PRIORITY>`, depending on :ref:`CONFIG_MQTT_USE_CUSTOM_CONFIG`) and not pinned to any core (:ref:`configurable <CONFIG_MQTT_TASK_CORE_SELECTION_ENABLED>`).
        - To see what is the task priority for ``mDNS`` service, please check `Performance Optimization <https://docs.espressif.com/projects/esp-protocols/mdns/docs/latest/en/index.html#performance-optimization>`__.


Choosing Task Priorities of the Application
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: not SOC_HP_CPU_HAS_MULTIPLE_CORES

    .. only:: SOC_WIFI_SUPPORTED or SOC_BT_SUPPORTED or SOC_IEEE802154_SUPPORTED

        In general, it is not recommended to set task priorities higher than the built-in {IDF_TARGET_RF_TYPE} operations as starving them of CPU may make the system unstable.

    For very short timing-critical operations that do not use the network, use an ISR or a very restricted task (with very short bursts of runtime only) at the highest priority (24).

    .. only:: SOC_WIFI_SUPPORTED or SOC_BT_SUPPORTED or SOC_IEEE802154_SUPPORTED

        Choosing priority 19 allows lower-layer {IDF_TARGET_RF_TYPE} functionality to run without delays, but still preempts the lwIP TCP/IP stack and other less time-critical internal functionality - this is the best option for time-critical tasks that do not perform network operations.

    Any task that does TCP/IP network operations should run at a lower priority than the lwIP TCP/IP task (18) to avoid priority-inversion issues.

.. only:: not SOC_HP_CPU_HAS_MULTIPLE_CORES

    With a few exceptions, most importantly the lwIP TCP/IP task, in the default configuration most built-in tasks are pinned to Core 0. This makes it quite easy for the application to place high priority tasks on Core 1. Using priority 19 or higher guarantees that an application task can run on Core 1 without being preempted by any built-in task. To further isolate the tasks running on each CPU, configure the :ref:`lwIP task <CONFIG_LWIP_TCPIP_TASK_AFFINITY>` to only run on Core 0 instead of either core, which may reduce total TCP/IP throughput depending on what other tasks are running.

    .. only:: SOC_WIFI_SUPPORTED or SOC_BT_SUPPORTED or SOC_IEEE802154_SUPPORTED

        In general, it is not recommended to set task priorities on Core 0 higher than the built-in {IDF_TARGET_RF_TYPE} operations as starving them of CPU may make the system unstable. Choosing priority 19 and Core 0 allows lower-layer {IDF_TARGET_RF_TYPE} functionality to run without delays, but still pre-empts the lwIP TCP/IP stack and other less time-critical internal functionality. This is an option for time-critical tasks that do not perform network operations. Any task that does TCP/IP network operations should run at lower priority than the lwIP TCP/IP task (18) to avoid priority-inversion issues.

    .. note::

        Setting a task to always run in preference to built-in ESP-IDF tasks does not require pinning the task to Core 1. Instead, the task can be left unpinned and assigned a priority of 17 or lower. This allows the task to optionally run on Core 0 if there are no higher-priority built-in tasks running on that core. Using unpinned tasks can improve the overall CPU utilization, however it makes reasoning about task scheduling more complex.

.. note::

    Task execution is always completely suspended when writing to the built-in SPI flash chip. Only :ref:`iram-safe-interrupt-handlers` continues executing.

Improving Interrupt Performance
-------------------------------

ESP-IDF supports dynamic :doc:`/api-reference/system/intr_alloc` with interrupt preemption. Each interrupt in the system has a priority, and higher-priority interrupts preempts lower priority ones.

Interrupt handlers execute in preference to any task, provided the task is not inside a critical section. For this reason, it is important to minimize the amount of time spent in executing an interrupt handler.

To obtain the best performance for a particular interrupt handler:

.. list::

    - Assign more important interrupts a higher priority using a flag such as ``ESP_INTR_FLAG_LEVEL2`` or ``ESP_INTR_FLAG_LEVEL3`` when calling :cpp:func:`esp_intr_alloc`.
    :SOC_HP_CPU_HAS_MULTIPLE_CORES: - Assign the interrupt on a CPU where built-in {IDF_TARGET_RF_TYPE} tasks are not configured to run, which means assigning the interrupt on Core 1 by default, see :ref:`built-in-task-priorities`. Interrupts are assigned on the same CPU where the :cpp:func:`esp_intr_alloc` function call is made.
    - If you are sure the entire interrupt handler can run from IRAM (see :ref:`iram-safe-interrupt-handlers`) then set the ``ESP_INTR_FLAG_IRAM`` flag when calling :cpp:func:`esp_intr_alloc` to assign the interrupt. This prevents it being temporarily disabled if the application firmware writes to the internal SPI flash.
    - Even if the interrupt handler is not IRAM-safe, if it is going to be executed frequently then consider moving the handler function to IRAM anyhow. This minimizes the chance of a flash cache miss when the interrupt code is executed (see :ref:`speed-targeted-optimizations`). It is possible to do this without adding the ``ESP_INTR_FLAG_IRAM`` flag to mark the interrupt as IRAM-safe, if only part of the handler is guaranteed to be in IRAM.

.. _improve-network-speed:

Improving Network Speed
-----------------------

.. list::

    :SOC_WIFI_SUPPORTED: * For Wi-Fi, see :ref:`How-to-improve-Wi-Fi-performance` and :ref:`wifi-buffer-usage`
    * For lwIP TCP/IP, see :ref:`lwip-performance`
    :SOC_WIFI_SUPPORTED: * The :example:`wifi/iperf` example contains a configuration that is heavily optimized for Wi-Fi TCP/IP throughput, usually at the expense of higher RAM usage. Append the contents of the files :example_file:`wifi/iperf/sdkconfig.defaults`, :example_file:`wifi/iperf/sdkconfig.defaults.{IDF_TARGET_PATH_NAME}` and :example_file:`wifi/iperf/sdkconfig.ci.99` to the ``sdkconfig`` file in your project in order to add all of these options. Note that some of these options may have trade-offs in terms of reduced debuggability, increased firmware size, increased memory usage, or reduced performance of other features. To get the best result, read the documentation pages linked above and use related information to determine exactly which options are best suited for your app.
    :SOC_EMAC_SUPPORTED: * The :example:`ethernet/iperf` example contains a configuration that is heavily optimized for Ethernet TCP/IP throughput, usually at the expense of higher RAM usage. Examine :example_file:`ethernet/iperf/sdkconfig.defaults` for more details. Note that some of these options may have trade-offs in terms of reduced debuggability, increased firmware size, increased memory usage, or reduced performance of other features. To get the best result, read the documentation pages linked above and use related information to determine exactly which options are best suited for your app.

Improving I/O Performance
-------------------------

Using standard C library functions like ``fread`` and ``fwrite`` instead of platform-specific unbuffered syscalls such as ``read`` and ``write``, may result in slower performance.

The ``fread`` and ``fwrite`` functions are designed for portability rather than speed, introducing some overhead due to their buffered nature. Check the example :example:`storage/fatfs/getting_started` to see how to use these two functions.

In contrast, the ``read`` and ``write`` functions are standard POSIX APIs that can be used directly when working with FatFs through VFS, with ESP-IDF handling the underlying implementation. Check the example :example:`storage/fatfs/fs_operations` to see how to use the two functions.

Additional tips are provided below, and further details can be found in :doc:`/api-reference/storage/fatfs`.

.. list::

    - The maximum size of a read/write request is equal to the FatFS cluster size (allocation unit size).
    - For better performance, prefer using ``read`` and ``write`` over ``fread`` and ``fwrite``.
    - To improve the speed of buffered reading functions like ``fread`` and ``fgets``, consider increasing the file buffer size. The default size in Newlib is 128 bytes, but you can increase it to 4096, 8192, or 16384 bytes. This can be made locally using the ``setvbuf`` function for a specific file pointer or globally by modifying the ``CONFIG_FATFS_VFS_FSTAT_BLKSIZE`` setting.

        .. note::

            Increasing the buffer size will also increase heap memory usage.
