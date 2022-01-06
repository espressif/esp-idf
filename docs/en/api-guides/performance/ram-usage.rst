Minimizing RAM Usage
====================

{IDF_TARGET_STATIC_MEANS_HEAP:default="Wi-Fi library, Bluetooth controller", esp32s2="Wi-Fi library"}

In some cases, a firmware application's available RAM may run low or run out entirely. In these cases, it's necessary to tune the memory usage of the firmware application.

In general, firmware should aim to leave some "headroom" of free internal RAM in order to deal with extraordinary situations or changes in RAM usage in future updates.

Background
----------

Before optimizing ESP-IDF RAM usage, it's necessary to understand the basics of {IDF_TARGET_NAME} memory types, the difference between static and dynamic memory usage in C, and the way ESP-IDF uses stack and heap. This information can all be found in :doc:`/api-reference/system/mem_alloc`.

Measuring Static Memory Usage
-----------------------------

The :ref:`idf.py` tool can be used to generate reports about the static memory usage of an application. Refer to :ref:`the Binary Size chapter for more information <idf.py-size>`.

Measuring Dynamic Memory Usage
------------------------------

ESP-IDF contains a range of heap APIs for measuring free heap at runtime. See :doc:`/api-reference/system/heap_debug`.

.. note::

   In embedded systems, heap fragmentation can be a significant issue alongside total RAM usage. The heap measurement APIs provide ways to measure the "largest free block". Monitoring this value along with the total number of free bytes can give a quick indication of whether heap fragmentation is becoming an issue.

Reducing Static Memory Usage
----------------------------

- Reducing the static memory usage of the application increases the amount of RAM available for heap at runtime, and vice versa.
- Generally speaking, minimizing static memory usage requires monitoring the .data and .bss sizes. For tools to do this, see :ref:`idf.py-size`.
- Internal ESP-IDF functions do not make heavy use of static RAM allocation in C. In many instances (including: {IDF_TARGET_STATIC_MEANS_HEAP}) "static" buffers are still allocated from heap, but the allocation is done once when the feature is initialized and will be freed if the feature is deinitialized. This is done in order to maximize the amount of free memory at different points in the application life-cycle.

To minimize static memory use:

.. list::

   - Declare structures, buffers, or other variables ``const`` whenever possible. Constant data can be stored in flash not RAM. This may require changing functions in the firmware to take ``const *`` arguments instead of mutable pointer arguments. These changes can also reduce the stack usage of some functions.
   :SOC_BT_SUPPORTED: - If using Bluedroid, setting the option :ref:`CONFIG_BT_BLE_DYNAMIC_ENV_MEMORY` will cause Bluedroid to allocate memory on initialization and free it on deinitialization. This doesn't necessarily reduce the peak memory usage, but changes it from static memory usage to runtime memory usage.

.. _optimize-stack-sizes:

Reducing Stack Sizes
--------------------

In FreeRTOS, task stacks are usually allocated from the heap. The stack size for each task is fixed (passed as an argument to :cpp:func:`xTaskCreate`). Each task can use up to its allocated stack size, but using more than this will cause an otherwise valid program to crash with a stack overflow or heap corruption.

Therefore, determining the optimum sizes of each task stack can substantially reduce RAM usage.

To determine optimum task stack sizes:

- Combine tasks. The best task stack size is 0 bytes, achieved by combining a task with another existing task. Anywhere that the firmware can be structured to perform multiple functions sequentially in a single task will increase free memory. In some cases, using a "worker task" pattern where jobs are serialized into a FreeRTOS queue (or similar) and then processed by generic worker tasks may help.
- Consolidate task functions. String formatting functions (like ``printf``) are particularly heavy users of stack, so any task which doesn't ever call these can usually have its stack size reduced.
- Enabling :ref:`newlib-nano-formatting` will reduce the stack usage of any task that calls ``printf()`` or other C string formatting functions.
- Avoid allocating large variables on the stack. In C, any large struct or array allocated as an "automatic" variable (i.e. default scope of a C declaration) will use space on the stack. Minimize the sizes of these, allocate them statically and/or see if you can save memory by allocating them from the heap only when they are needed.
- Avoid deep recursive function calls. Individual recursive function calls don't always add a lot of stack usage each time they are called, but if each function includes large stack-based variables then the overhead can get quite high.
- At runtime, call the function :cpp:func:`uxTaskGetStackHighWaterMark` with the handle of any task where you think there is unused stack memory. This function returns the minimum lifetime free stack memory in bytes. The easiest time to call this is from the task itself: call ``uxTaskGetStackHighWaterMark(NULL)`` to get the current task's high water mark after the time that the task has achieved its peak stack usage (i.e. if there is a main loop, execute the main loop a number of times with all possible states and then call :cpp:func:`uxTaskGetStackHighWaterMark`). Often, it's possible to subtract almost the entire value returned here from the total stack size of a task, but allow some safety margin to account for unexpected small increases in stack usage at runtime.
- Call :cpp:func:`uxTaskGetSystemState` at runtime to get a summary of all tasks in the system. This includes their individual stack "high watermark" values.
- When debugger watchpoints are not being used, set the :ref:`CONFIG_FREERTOS_WATCHPOINT_END_OF_STACK` option to trigger an immediate panic if a task writes the word at the end of its assigned stack. This is slightly more reliable than the default :ref:`CONFIG_FREERTOS_CHECK_STACKOVERFLOW` option of "Check using canary bytes", because the panic happens immediately, not on the next RTOS context switch. Neither option is perfect, it's possible in some cases for stack pointer to skip the watchpoint or canary bytes and corrupt another region of RAM, instead.

Internal Stack Sizes
^^^^^^^^^^^^^^^^^^^^

ESP-IDF allocates a number of internal tasks for housekeeping purposes or operating system functions. Some are created during the startup process, and some are created at runtime when particular features are initialized.

The default stack sizes for these tasks are usually set conservatively high, to allow all common usage patterns. Many of the stack sizes are configurable, and it may be possible to reduce them to match the real runtime stack usage of the task.

.. important::

   If internal task stack sizes are set too small, ESP-IDF will crash unpredictably. Even if the root cause is task stack overflow, this is not always clear when debugging. It is recommended that internal stack sizes are only reduced carefully (if at all), with close attention to "high water mark" free space under load. If reporting an issue that occurs when internal task stack sizes have been reduced, please always include this information and the specific configuration that is being used.

.. list::

   - :ref:`Main task that executes app_main function <app-main-task>` has stack size :ref:`CONFIG_ESP_MAIN_TASK_STACK_SIZE`.
   - :doc:`/api-reference/system/esp_timer` system task which executes callbacks has stack size :ref:`CONFIG_ESP_TIMER_TASK_STACK_SIZE`.
   - FreeRTOS Timer Task to handle FreeRTOS timer callbacks has stack size :ref:`CONFIG_FREERTOS_TIMER_TASK_STACK_DEPTH`.
   - :doc:`/api-guides/event-handling` system task to execute callbacks for the default system event loop has stack size :ref:`CONFIG_ESP_SYSTEM_EVENT_TASK_STACK_SIZE`.
   - :doc:`/api-guides/lwip` TCP/IP task has stack size :ref:`CONFIG_LWIP_TCPIP_TASK_STACK_SIZE`
   :SOC_BT_SUPPORTED: - :doc:`Bluedroid Bluetooth Host </api-reference/bluetooth/index>` have task stack sizes :ref:`CONFIG_BT_BTC_TASK_STACK_SIZE`, :ref:`CONFIG_BT_BTU_TASK_STACK_SIZE`.
   :SOC_BT_SUPPORTED: - :doc:`NimBLE Bluetooth Host </api-reference/bluetooth/nimble/index>` has task stack size :ref:`CONFIG_BT_NIMBLE_TASK_STACK_SIZE`
   - The Ethernet driver creates a task for the MAC to receive Ethernet frames. If using the default config ``ETH_MAC_DEFAULT_CONFIG`` then the task stack size is 4 KB. This setting can be changed by passing a custom :cpp:class:`eth_mac_config_t` struct when initializing the Ethernet MAC.
   - FreeRTOS idle task stack size is configured by :ref:`CONFIG_FREERTOS_IDLE_TASK_STACKSIZE`.
   - If using the :doc:`mDNS </api-reference/protocols/mdns>` and/or :doc:`MQTT </api-reference/protocols/mqtt>` components, they create tasks with stack sizes configured by :ref:`CONFIG_MDNS_TASK_STACK_SIZE` and :ref:`CONFIG_MQTT_TASK_STACK_SIZE`, respectively. MQTT stack size can also be configured using ``task_stack`` field of :cpp:class:`esp_mqtt_client_config_t`.

.. note::

   Aside from built-in system features such as esp-timer, if an ESP-IDF feature is not initialized by the firmware then no associated task is created. In those cases, the stack usage is zero and the stack size configuration for the task is not relevant.

Reducing Heap Usage
-------------------

For functions that assist in analyzing heap usage at runtime, see :doc:`/api-reference/system/heap_debug`.

Normally, optimizing heap usage consists of analyzing the usage and removing calls to ``malloc()`` that aren't being used, reducing the corresponding sizes, or freeing previously allocated buffers earlier.

There are some ESP-IDF configuration options that can reduce heap usage at runtime:

.. list::

   - lwIP documentation has a section to configure :ref:`lwip-ram-usage`.
   - :ref:`wifi-buffer-usage` describes options to either reduce numbers of "static" buffers or reduce the maximum number of "dynamic" buffers in use, in order to minimize memory usage at possible cost of performance. Note that "static" Wi-Fi buffers are still allocated from heap when Wi-Fi is initialized and will be freed if Wi-Fi is deinitialized.
   :esp32: - The Ethernet driver allocates DMA buffers for the internal Ethernet MAC when it is initialized - configuration options are :ref:`CONFIG_ETH_DMA_BUFFER_SIZE`, :ref:`CONFIG_ETH_DMA_RX_BUFFER_NUM`, :ref:`CONFIG_ETH_DMA_TX_BUFFER_NUM`.
   - mbedTLS TLS session memory usage can be minimized by enabling the ESP-IDF feature :ref:`CONFIG_MBEDTLS_DYNAMIC_BUFFER`.
   :esp32: - In single core mode only, it's possible to use IRAM as byte accessible memory (added to the regular heap) by enabling :ref:`CONFIG_ESP32_IRAM_AS_8BIT_ACCESSIBLE_MEMORY`. Note that this option carries a performance penalty and the risk of security issues caused by executable data. If this option is enabled then it's possible to set other options to prefer certain buffers be allocated from this memory: :ref:`mbedTLS <CONFIG_MBEDTLS_MEM_ALLOC_MODE>`, :ref:`NimBLE <CONFIG_BT_NIMBLE_MEM_ALLOC_MODE>`.
   :esp32: - Reduce :ref:`CONFIG_BTDM_CTRL_BLE_MAX_CONN` if using BLE.
   :esp32: - Reduce :ref:`CONFIG_BTDM_CTRL_BR_EDR_MAX_ACL_CONN` if using Bluetooth Classic.

.. note::

   There are other configuration options that will increase heap usage at runtime if changed from the defaults. These are not listed here, but the help text for the configuration item will mention if there is some memory impact.

.. _optimize-iram-usage:

Optimizing IRAM Usage
---------------------

.. only:: not esp32

   The available DRAM at runtime (for heap usage) is also reduced by the static IRAM usage. Therefore, one way to increase available DRAM is to reduce IRAM usage.

If the app allocates more static IRAM than is available then the app will fail to build and linker errors such as ``section `.iram0.text' will not fit in region `iram0_0_seg'``, ``IRAM0 segment data does not fit`` and ``region `iram0_0_seg' overflowed by 84 bytes`` will be seen. If this happens, it is necessary to find ways to reduce static IRAM usage in order to link the application.

To analyze the IRAM usage in the firmware binary, use :ref:`idf.py-size`. If the firmware failed to link, steps to analyze are shown at :ref:`idf-size-linker-failed`.

The following options will reduce IRAM usage of some ESP-IDF features:

.. list::

    - Enable :ref:`CONFIG_FREERTOS_PLACE_FUNCTIONS_INTO_FLASH`. Provided these functions are not (incorrectly) used from ISRs, this option is safe to enable in all configurations.
    - Enable :ref:`CONFIG_FREERTOS_PLACE_SNAPSHOT_FUNS_INTO_FLASH`. Enabling this option will place snapshot-related functions, such as ``vTaskGetSnapshot`` or ``uxTaskGetSnapshotAll``, in flash.
    - Disable Wi-Fi options :ref:`CONFIG_ESP32_WIFI_IRAM_OPT` and/or :ref:`CONFIG_ESP32_WIFI_RX_IRAM_OPT`. Disabling these options will free available IRAM at the cost of Wi-Fi performance.
    :esp32c3 or esp32s3: - :ref:`CONFIG_SPI_FLASH_ROM_IMPL` enabling this option will free some IRAM but will mean that esp_flash bugfixes and new flash chip support is not available.
    :esp32: - :ref:`CONFIG_SPI_FLASH_ROM_DRIVER_PATCH` disabling this option will free some IRAM but is only available in some flash configurations (see the configuration item help text).
    - Disabling :ref:`CONFIG_ESP_EVENT_POST_FROM_IRAM_ISR` prevents posting ``esp_event`` events from :ref:`iram-safe-interrupt-handlers` but will save some IRAM.
    - Disabling :ref:`CONFIG_SPI_MASTER_ISR_IN_IRAM` prevents spi_master interrupts from being serviced while writing to flash, and may otherwise reduce spi_master performance, but will save some IRAM.
    - Setting :ref:`CONFIG_HAL_DEFAULT_ASSERTION_LEVEL` to disable assertion for HAL component will save some IRAM especially for HAL code who calls `HAL_ASSERT` a lot and resides in IRAM.

.. only:: esp32

   When compiling for ESP32 revisions older than ECO3 (:ref:`CONFIG_ESP32_REV_MIN`), PSRAM cache bug workaround (:ref:`CONFIG_SPIRAM_CACHE_WORKAROUND`) option is enabled, and the C library functions normally located in ROM are recompiled with the workaround and placed into IRAM instead. For most applications, it is safe to move many of the C library functions into Flash, reclaiming some IRAM. Corresponding options include:

   .. list::

       - :ref:`CONFIG_SPIRAM_CACHE_LIBJMP_IN_IRAM`: affects the functions ``longjmp`` and ``setjump``.
       - :ref:`CONFIG_SPIRAM_CACHE_LIBMATH_IN_IRAM`: affects the functions ``abs``, ``div``, ``labs``, ``ldiv``, ``quorem``, ``fpclassify`` and ``nan``.
       - :ref:`CONFIG_SPIRAM_CACHE_LIBNUMPARSER_IN_IRAM`: affects the functions ``utoa``, ``itoa``, ``atoi``, ``atol``, ``strtol``, and ``strtoul``.
       - :ref:`CONFIG_SPIRAM_CACHE_LIBIO_IN_IRAM`: affects the functions ``wcrtomb``, ``fvwrite``, ``wbuf``, ``wsetup``, ``fputwc``, ``wctomb_r``, ``ungetc``, ``makebuf``, ``fflush``, ``refill``, and ``sccl``.
       - :ref:`CONFIG_SPIRAM_CACHE_LIBTIME_IN_IRAM`: affects the functions ``asctime``, ``asctime_r``, ``ctime``, ``ctime_r``, ``lcltime``, ``lcltime_r``, ``gmtime``, ``gmtime_r``, ``strftime``, ``mktime``, ``tzset_r``, ``tzset``, ``time``, ``gettzinfo``, ``systimes``, ``month_lengths``, ``timelocal``, ``tzvars``, ``tzlock``, ``tzcalc_limits``, and ``strptime``.
       - :ref:`CONFIG_SPIRAM_CACHE_LIBCHAR_IN_IRAM`: affects the functions ``ctype_``, ``toupper``, ``tolower``, ``toascii``, ``strupr``, ``bzero``, ``isalnum``, ``isalpha``, ``isascii``, ``isblank``, ``iscntrl``, ``isdigit``, ``isgraph``, ``islower``, ``isprint``, ``ispunct``, ``isspace``, and ``isupper``.
       - :ref:`CONFIG_SPIRAM_CACHE_LIBMEM_IN_IRAM`: affects the functions ``memccpy``, ``memchr``, ``memmove``, and ``memrchr``.
       - :ref:`CONFIG_SPIRAM_CACHE_LIBSTR_IN_IRAM`: affects the functions ``strcasecmp``, ``strcasestr``, ``strchr``, ``strcoll``, ``strcpy``, ``strcspn``, ``strdup``, ``strdup_r``, ``strlcat``, ``strlcpy``, ``strlen``, ``strlwr``, ``strncasecmp``, ``strncat``, ``strncmp``, ``strncpy``, ``strndup``, ``strndup_r``, ``strrchr``, ``strsep``, ``strspn``, ``strstr``, ``strtok_r, and ``strupr``.
       - :ref:`CONFIG_SPIRAM_CACHE_LIBRAND_IN_IRAM`: affects the functions ``srand``, ``rand``, and ``rand_r``.
       - :ref:`CONFIG_SPIRAM_CACHE_LIBENV_IN_IRAM`: affects the functions ``environ``, ``envlock``, and ``getenv_r``.
       - :ref:`CONFIG_SPIRAM_CACHE_LIBFILE_IN_IRAM`: affects the functions lock``, ``isatty``, ``fclose``, ``open``, ``close``, ``creat``, ``read``, ``rshift``, ``sbrk``, ``stdio``, ``syssbrk``, ``sysclose``, ``sysopen``, ``creat``, ``sysread``, ``syswrite``, ``impure``, ``fwalk``, and ``findfp``.
       - :ref:`CONFIG_SPIRAM_CACHE_LIBMISC_IN_IRAM`: affects the functions ``raise`` and ``system``.

   The exact amount of IRAM saved will depend on how much C library code is actually used by the application. In addition to these, the following options may be used to move more of the C library code into Flash, however note that this may result in reduced performance. Also take care to not use corresponding C library functions from interrupts which may be called while cache is disabled (allocated with :c:macro:`ESP_INTR_FLAG_IRAM` flag), refer to :ref:`iram-safe-interrupt-handlers` for more details. For these reasons, the functions ``itoa``, ``memcmp``, ``memcpy``, ``memset``, ``strcat``, ``strcmp``, and ``strlen`` are always put in IRAM.

.. note::

    Moving frequently-called functions from IRAM to flash may increase their execution time.

.. note::

    Other configuration options exist that will increase IRAM usage by moving some functionality into IRAM, usually for performance, but the default option is not to do this. These are not listed here. The IRAM size impact of enabling these options is usually noted in the configuration item help text.
