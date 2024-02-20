Heap Memory Debugging
=====================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

ESP-IDF integrates tools for requesting :ref:`heap information <heap-information>`, :ref:`heap corruption detection <heap-corruption>`, and :ref:`heap tracing <heap-tracing>`. These can help track down memory-related bugs.

For general information about the heap memory allocator, see :doc:`Heap Memory Allocation </api-reference/system/mem_alloc>`.

.. _heap-information:

Heap Information
----------------

To obtain information about the state of the heap, call the following functions:

- :cpp:func:`heap_caps_get_free_size` can be used to return the current free memory for different memory capabilities.
- :cpp:func:`heap_caps_get_largest_free_block` can be used to return the largest free block in the heap, which is also the largest single allocation currently possible. Tracking this value and comparing it to the total free heap allows you to detect heap fragmentation.
- :cpp:func:`heap_caps_get_minimum_free_size` can be used to track the heap "low watermark" since boot.
- :cpp:func:`heap_caps_get_info` returns a :cpp:class:`multi_heap_info_t` structure, which contains the information from the above functions, plus some additional heap-specific data (number of allocations, etc.).
- :cpp:func:`heap_caps_print_heap_info` prints a summary of the information returned by :cpp:func:`heap_caps_get_info` to stdout.
- :cpp:func:`heap_caps_dump` and :cpp:func:`heap_caps_dump_all` output detailed information about the structure of each block in the heap. Note that this can be a large amount of output.


.. _heap-allocation-free:

Heap Allocation and Free Function Hooks
---------------------------------------

Heap allocation and free detection hooks allow you to be notified of every successful allocation and free operation:

- Providing a definition of :cpp:func:`esp_heap_trace_alloc_hook` allows you to be notified of every successful memory allocation operation
- Providing a definition of :cpp:func:`esp_heap_trace_free_hook` allows you to be notified of every successful memory-free operations

This feature can be enabled by setting the :ref:`CONFIG_HEAP_USE_HOOKS` option. :cpp:func:`esp_heap_trace_alloc_hook` and :cpp:func:`esp_heap_trace_free_hook` have weak declarations (e.g., ``__attribute__((weak))``), thus it is not necessary to provide declarations for both hooks. Given that it is technically possible to allocate and free memory from an ISR (**though strongly discouraged from doing so**), the :cpp:func:`esp_heap_trace_alloc_hook` and :cpp:func:`esp_heap_trace_free_hook` can potentially be called from an ISR.

It is not recommended to perform (or call API functions to perform) blocking operations or memory allocation/free operations in the hook functions. In general, the best practice is to keep the implementation concise and leave the heavy computation outside of the hook functions.

The example below shows how to define the allocation and free function hooks:

.. code-block:: c

  #include "esp_heap_caps.h"

  void esp_heap_trace_alloc_hook(void* ptr, size_t size, uint32_t caps)
  {
    ...
  }
  void esp_heap_trace_free_hook(void* ptr)
  {
    ...
  }

  void app_main()
  {
      ...
  }

.. _heap-corruption:

Heap Corruption Detection
-------------------------

Heap corruption detection allows you to detect various types of heap memory errors:

- Out-of-bound writes & buffer overflows
- Writes to freed memory
- Reads from freed or uninitialized memory

Assertions
^^^^^^^^^^

The heap implementation (:component_file:`heap/multi_heap.c`, etc.) includes numerous assertions that will fail if the heap memory is corrupted. To detect heap corruption most effectively, ensure that assertions are enabled in the project configuration via the :ref:`CONFIG_COMPILER_OPTIMIZATION_ASSERTION_LEVEL` option.

If a heap integrity assertion fails, a line will be printed like ``CORRUPT HEAP: multi_heap.c:225 detected at 0x3ffbb71c``. The memory address printed is the address of the heap structure that has corrupt content.

It is also possible to manually check heap integrity by calling :cpp:func:`heap_caps_check_integrity_all` or related functions. This function checks all of the requested heap memory for integrity and can be used even if assertions are disabled. If the integrity checks detects an error, it will print the error along with the address(es) of corrupt heap structures.

Memory Allocation Failed Hook
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Users can use :cpp:func:`heap_caps_register_failed_alloc_callback` to register a callback that is invoked every time an allocation operation fails.

Additionally, users can enable the :ref:`CONFIG_HEAP_ABORT_WHEN_ALLOCATION_FAILS`, which will automatically trigger a system abort if any allocation operation fails.

The example below shows how to register an allocation failure callback:

.. code-block:: c

  #include "esp_heap_caps.h"

  void heap_caps_alloc_failed_hook(size_t requested_size, uint32_t caps, const char *function_name)
  {
    printf("%s was called but failed to allocate %d bytes with 0x%X capabilities. \n",function_name, requested_size, caps);
  }

  void app_main()
  {
      ...
      esp_err_t error = heap_caps_register_failed_alloc_callback(heap_caps_alloc_failed_hook);
      ...
      void *ptr = heap_caps_malloc(allocation_size, MALLOC_CAP_DEFAULT);
      ...
  }

Finding Heap Corruption
^^^^^^^^^^^^^^^^^^^^^^^

Memory corruption can be one of the hardest classes of bugs to find and fix, as the source of the corruption could be completely unrelated to the symptoms of the corruption. Here are some tips:

- A crash with a ``CORRUPT HEAP:`` message usually includes a stack trace, but this stack trace is rarely useful. The crash is the symptom of memory corruption when the system realizes the heap is corrupt. But usually, the corruption happens elsewhere and earlier in time.
- Increasing the heap memory debugging `Configuration`_ level to "Light impact" or "Comprehensive" gives you a more accurate message with the first corrupt memory address.
- Adding regular calls to :cpp:func:`heap_caps_check_integrity_all` or :cpp:func:`heap_caps_check_integrity_addr` in your code helps you pin down the exact time that the corruption happened. You can move these checks around to "close in on" the section of code that corrupted the heap.
- Based on the memory address that has been corrupted, you can use :ref:`JTAG debugging <jtag-debugging-introduction>` to set a watchpoint on this address and have the CPU halt when it is written to.
- If you do not have JTAG, but you do know roughly when the corruption happens, set a watchpoint in software just beforehand via :cpp:func:`esp_cpu_set_watchpoint`. A fatal exception will occur when the watchpoint triggers. The following is an example of how to use the function - ``esp_cpu_set_watchpoint(0, (void *)addr, 4, ESP_WATCHPOINT_STORE)``. Note that watchpoints are per-CPU and are set on the current running CPU only. So if you do not know which CPU is corrupting memory, call this function on both CPUs.
- For buffer overflows, `heap tracing`_ in ``HEAP_TRACE_ALL`` mode tells which callers are allocating which addresses from the heap. See `Heap Tracing To Find Heap Corruption`_ for more details. You can try to find the function that allocates memory with an address immediately before the corrupted address, since it is probably the function that overflows the buffer.
- Calling :cpp:func:`heap_caps_dump` or :cpp:func:`heap_caps_dump_all` can give an indication of what heap blocks are surrounding the corrupted region and may have overflowed or underflowed, etc.

Configuration
^^^^^^^^^^^^^

Temporarily increasing the heap corruption detection level can give more detailed information about heap corruption errors.

In the project configuration menu, under ``Component config``, there is a menu ``Heap memory debugging``. The option :ref:`CONFIG_HEAP_CORRUPTION_DETECTION` can be set to one of the following three levels:

Basic (No Poisoning)
++++++++++++++++++++

This is the default level. By default, no special heap corruption features are enabled, but the provided assertions are enabled. A heap corruption error will be printed if any of the heap's internal data structures appear overwritten or corrupted. This usually indicates a buffer overrun or out-of-bounds write.

If assertions are enabled, an assertion will also trigger if a double-free occurs (the same memory is freed twice).

Calling :cpp:func:`heap_caps_check_integrity` in Basic mode checks the integrity of all heap structures, and print errors if any appear to be corrupted.

Light Impact
++++++++++++

At this level, heap memory is additionally "poisoned" with head and tail "canary bytes" before and after each block that is allocated. If an application writes outside the bounds of allocated buffers, the canary bytes will be corrupted, and the integrity check will fail.

The head canary word is ``0xABBA1234`` (``3412BAAB`` in byte order), and the tail canary word is ``0xBAAD5678`` (``7856ADBA`` in byte order).

With basic heap corruption checks, most out-of-bound writes can be detected and the number of overrun bytes before a failure is detected depends on the properties of the heap. However, the Light Impact mode is more precise as even a single-byte overrun can be detected.

Enabling light-impact checking increases the memory usage. Each individual allocation uses 9 to 12 additional bytes of memory depending on alignment.

Each time :cpp:func:`heap_caps_free` is called in Light Impact mode, the head and tail canary bytes of the buffer being freed are checked against the expected values.

When :cpp:func:`heap_caps_check_integrity` is called, all allocated blocks of heap memory have their canary bytes checked against the expected values.

In both cases, the functions involve checking that the first 4 bytes of an allocated block (before the buffer is returned to the user) should be the word ``0xABBA1234``, and the last 4 bytes of the allocated block (after the buffer is returned to the user) should be the word ``0xBAAD5678``.

Different values usually indicate buffer underrun or overrun. Overrun indicates that when writing to memory, the data written exceeds the size of the allocated memory, resulting in writing to an unallocated memory area; underrun indicates that when reading memory, the data read exceeds the allocated memory and reads data from an unallocated memory area.

Comprehensive
+++++++++++++

This level incorporates the "light impact" detection features plus additional checks for uninitialized-access and use-after-free bugs. In this mode, all freshly allocated memory is filled with the pattern ``0xCE``, and all freed memory is filled with the pattern ``0xFE``.

Enabling Comprehensive mode has a substantial impact on runtime performance, as all memory needs to be set to the allocation patterns each time a :cpp:func:`heap_caps_malloc` or :cpp:func:`heap_caps_free` completes, and the memory also needs to be checked each time. However, this mode allows easier detection of memory corruption bugs which are much more subtle to find otherwise. It is recommended to only enable this mode when debugging, not in production.

Crashes in Comprehensive Mode
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If an application crashes when reading or writing an address related to ``0xCECECECE`` in Comprehensive mode, it indicates that it has read uninitialized memory. The application should be changed to either use :cpp:func:`heap_caps_calloc` (which zeroes memory), or initialize the memory before using it. The value ``0xCECECECE`` may also be seen in stack-allocated automatic variables, because, in ESP-IDF, most task stacks are originally allocated from the heap, and in C, stack memory is uninitialized by default.

If an application crashes, and the exception register dump indicates that some addresses or values were ``0xFEFEFEFE``, this indicates that it is reading heap memory after it has been freed, i.e., a "use-after-free bug". The application should be changed to not access heap memory after it has been freed.

If a call to :cpp:func:`heap_caps_malloc` or :cpp:func:`heap_caps_realloc` causes a crash because it was expected to find the pattern ``0xFEFEFEFE`` in free memory and a different pattern was found, it indicates that the app has a use-after-free bug where it is writing to memory that has already been freed.

Manual Heap Checks in Comprehensive Mode
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Calls to :cpp:func:`heap_caps_check_integrity` may print errors relating to ``0xFEFEFEFE``, ``0xABBA1234``, or ``0xBAAD5678``. In each case the checker is expected to find a given pattern, and will error out if not found:

- For free heap blocks, the checker expects to find all bytes set to ``0xFE``. Any other values indicate a use-after-free bug where free memory has been incorrectly overwritten.
- For allocated heap blocks, the behavior is the same as for the Light Impact mode. The canary bytes ``0xABBA1234`` and ``0xBAAD5678`` are checked at the head and tail of each allocated buffer, and any variation indicates a buffer overrun or underrun.

.. _heap-task-tracking:

Heap Task Tracking
------------------

Heap Task Tracking can be used to get per-task info for heap memory allocation. The application has to specify the heap capabilities for which the heap allocation is to be tracked.

Example code is provided in :example:`system/heap_task_tracking`.

.. _heap-tracing:

Heap Tracing
------------

Heap Tracing allows the tracing of code which allocates or frees memory. Two tracing modes are supported:

- Standalone. In this mode, traced data are kept on-board, so the size of the gathered information is limited by the buffer assigned for that purpose, and the analysis is done by the on-board code. There are a couple of APIs available for accessing and dumping collected info.
- Host-based. This mode does not have the limitation of the standalone mode, because traced data are sent to the host over JTAG connection using app_trace library. Later on, they can be analyzed using special tools.

Heap tracing can perform two functions:

- Leak checking: find memory that is allocated and never freed.
- Heap use analysis: show all functions that are allocating or freeing memory while the trace is running.

How to Diagnose Memory Leaks
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you suspect a memory leak, the first step is to figure out which part of the program is leaking memory. Use the :cpp:func:`heap_caps_get_free_size` or related functions in :ref:`heap information <heap-information>` to track memory use over the life of the application. Try to narrow the leak down to a single function or sequence of functions where free memory always decreases and never recovers.


Standalone Mode
+++++++++++++++

Once you have identified the code which you think is leaking:

- Enable the :ref:`CONFIG_HEAP_TRACING_DEST` option.
- Call the function :cpp:func:`heap_trace_init_standalone` early in the program, to register a buffer that can be used to record the memory trace.
- Call the function :cpp:func:`heap_trace_start` to begin recording all mallocs or frees in the system. Call this immediately before the piece of code which you suspect is leaking memory.
- Call the function :cpp:func:`heap_trace_stop` to stop the trace once the suspect piece of code has finished executing.
- Call the function :cpp:func:`heap_trace_dump` to dump the results of the heap trace.

The following code snippet demonstrates how application code would typically initialize, start, and stop heap tracing:

.. code-block:: c

  #include "esp_heap_trace.h"

  #define NUM_RECORDS 100
  static heap_trace_record_t trace_record[NUM_RECORDS]; // This buffer must be in internal RAM

  ...

  void app_main()
  {
      ...
      ESP_ERROR_CHECK( heap_trace_init_standalone(trace_record, NUM_RECORDS) );
      ...
  }

  void some_function()
  {
      ESP_ERROR_CHECK( heap_trace_start(HEAP_TRACE_LEAKS) );

      do_something_you_suspect_is_leaking();

      ESP_ERROR_CHECK( heap_trace_stop() );
      heap_trace_dump();
      ...
  }

The output from the heap trace has a similar format to the following example:

.. only:: CONFIG_IDF_TARGET_ARCH_XTENSA

    .. code-block:: none

        2 allocations trace (100 entry buffer)
        32 bytes (@ 0x3ffaf214) allocated CPU 0 ccount 0x2e9b7384 caller 0x400d276d:0x400d27c1
        0x400d276d: leak_some_memory at /path/to/idf/examples/get-started/blink/main/./blink.c:27

        0x400d27c1: blink_task at /path/to/idf/examples/get-started/blink/main/./blink.c:52

        8 bytes (@ 0x3ffaf804) allocated CPU 0 ccount 0x2e9b79c0 caller 0x400d2776:0x400d27c1
        0x400d2776: leak_some_memory at /path/to/idf/examples/get-started/blink/main/./blink.c:29

        0x400d27c1: blink_task at /path/to/idf/examples/get-started/blink/main/./blink.c:52

        40 bytes 'leaked' in trace (2 allocations)
        total allocations 2 total frees 0

.. only:: CONFIG_IDF_TARGET_ARCH_RISCV

    .. code-block:: none

        2 allocations trace (100 entry buffer)
        32 bytes (@ 0x3ffaf214) allocated CPU 0 ccount 0x2e9b7384 caller
        8 bytes (@ 0x3ffaf804) allocated CPU 0 ccount 0x2e9b79c0 caller
        40 bytes 'leaked' in trace (2 allocations)
        total allocations 2 total frees 0

.. note::

    The above example output uses :doc:`IDF Monitor </api-guides/tools/idf-monitor>` to automatically decode PC addresses to their source files and line numbers.

The first line indicates how many allocation entries are in the buffer, compared to its total size.

In ``HEAP_TRACE_LEAKS`` mode, for each traced memory allocation that has not already been freed, a line is printed with:

.. list::

    - ``XX bytes`` is the number of bytes allocated.
    - ``@ 0x...`` is the heap address returned from :cpp:func:`heap_caps_malloc` or :cpp:func:`heap_caps_calloc` .
    - ``Internal`` or ``PSRAM`` is the general location of the allocated memory.
    - ``CPU x`` is the CPU (0 or 1) running when the allocation was made.
    - ``ccount 0x...`` is the CCOUNT (CPU cycle count) register value the allocation was made. The value is different for CPU 0 vs CPU 1.
    :CONFIG_IDF_TARGET_ARCH_XTENSA: - ``caller 0x...`` gives the call stack of the call to :cpp:func:`heap_caps_malloc` or :cpp:func:`heap_caps_free` , as a list of PC addresses. These can be decoded to source files and line numbers, as shown above.

.. only:: not CONFIG_IDF_TARGET_ARCH_RISCV

    The depth of the call stack recorded for each trace entry can be configured in the project configuration menu, under ``Heap Memory Debugging`` > ``Enable heap tracing`` > :ref:`CONFIG_HEAP_TRACING_STACK_DEPTH`. Up to 32 stack frames can be recorded for each allocation (the default is 2). Each additional stack frame increases the memory usage of each ``heap_trace_record_t`` record by eight bytes.

Finally, the total number of the 'leaked' bytes (bytes allocated but not freed while the trace is running) is printed together with the total number of allocations it represents.

A warning will be printed if the trace buffer was not large enough to hold all the allocations happened. If you see this warning, consider either shortening the tracing period or increasing the number of records in the trace buffer.


Host-Based Mode
+++++++++++++++

Once you have identified the code which you think is leaking:

- In the project configuration menu, navigate to ``Component settings`` > ``Heap Memory Debugging`` > :ref:`CONFIG_HEAP_TRACING_DEST` and select ``Host-Based``.
- In the project configuration menu, navigate to ``Component settings`` > ``Application Level Tracing`` > :ref:`CONFIG_APPTRACE_DESTINATION1` and select ``Trace memory``.
- In the project configuration menu, navigate to ``Component settings`` > ``Application Level Tracing`` > ``FreeRTOS SystemView Tracing`` and enable :ref:`CONFIG_APPTRACE_SV_ENABLE`.
- Call the function :cpp:func:`heap_trace_init_tohost` early in the program, to initialize the JTAG heap tracing module.
- Call the function :cpp:func:`heap_trace_start` to begin recording all memory allocation and free calls in the system. Call this immediately before the piece of code which you suspect is leaking memory.

  In host-based mode, the argument to this function is ignored, and the heap tracing module behaves like ``HEAP_TRACE_ALL`` is passed, i.e., all allocations and deallocations are sent to the host.

- Call the function :cpp:func:`heap_trace_stop` to stop the trace once the suspect piece of code has finished executing.

The following code snippet demonstrates how application code would typically initialize, start, and stop host-based mode heap tracing:

.. code-block:: c

  #include "esp_heap_trace.h"

  ...

  void app_main()
  {
      ...
      ESP_ERROR_CHECK( heap_trace_init_tohost() );
      ...
  }

  void some_function()
  {
      ESP_ERROR_CHECK( heap_trace_start(HEAP_TRACE_LEAKS) );

      do_something_you_suspect_is_leaking();

      ESP_ERROR_CHECK( heap_trace_stop() );
      ...
  }

To gather and analyze heap trace, do the following on the host:

1.  Build the program and download it to the target as described in :ref:`Step 5. First Steps on ESP-IDF <get-started-build>`.

2.  Run OpenOCD (see :doc:`JTAG Debugging </api-guides/jtag-debugging/index>`).

.. note::

    In order to use this feature, you need OpenOCD version ``v0.10.0-esp32-20181105`` or later.

3. You can use GDB to start and/or stop tracing automatically. To do this you need to prepare a special ``gdbinit`` file:

.. code-block:: c

    target remote :3333

    mon reset halt
    maintenance flush register-cache

    tb heap_trace_start
    commands
    mon esp sysview start file:///tmp/heap.svdat
    c
    end

    tb heap_trace_stop
    commands
    mon esp sysview stop
    end

    c

Using this file GDB can connect to the target, reset it, and start tracing when the program hits breakpoint at :cpp:func:`heap_trace_start`. Tracing will be stopped when the program hits breakpoint at :cpp:func:`heap_trace_stop`. Traced data will be saved to ``/tmp/heap_log.svdat``.

4. Run GDB using ``{IDF_TARGET_TOOLCHAIN_PREFIX}-gdb -x gdbinit </path/to/program/elf>``.

5. Quit GDB when the program stops at :cpp:func:`heap_trace_stop`. Traced data are saved in ``/tmp/heap.svdat``.

6. Run processing script ``$IDF_PATH/tools/esp_app_trace/sysviewtrace_proc.py -p -b </path/to/program/elf> /tmp/heap_log.svdat``.

The output from the heap trace has a similar format to the following example:

.. code-block::

  Parse trace from '/tmp/heap.svdat'...
  Stop parsing trace. (Timeout 0.000000 sec while reading 1 bytes!)
  Process events from '['/tmp/heap.svdat']'...
  [0.002244575] HEAP: Allocated 1 bytes @ 0x3ffaffd8 from task "alloc" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:47
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.002258425] HEAP: Allocated 2 bytes @ 0x3ffaffe0 from task "alloc" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:48
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.002563725] HEAP: Freed bytes @ 0x3ffaffe0 from task "free" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:31 (discriminator 9)
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.002782950] HEAP: Freed bytes @ 0x3ffb40b8 from task "main" on core 0 by:
  /home/user/projects/esp/esp-idf/components/freertos/tasks.c:4590
  /home/user/projects/esp/esp-idf/components/freertos/tasks.c:4590

  [0.002798700] HEAP: Freed bytes @ 0x3ffb50bc from task "main" on core 0 by:
  /home/user/projects/esp/esp-idf/components/freertos/tasks.c:4590
  /home/user/projects/esp/esp-idf/components/freertos/tasks.c:4590

  [0.102436025] HEAP: Allocated 2 bytes @ 0x3ffaffe0 from task "alloc" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:47
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.102449800] HEAP: Allocated 4 bytes @ 0x3ffaffe8 from task "alloc" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:48
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.102666150] HEAP: Freed bytes @ 0x3ffaffe8 from task "free" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:31 (discriminator 9)
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.202436200] HEAP: Allocated 3 bytes @ 0x3ffaffe8 from task "alloc" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:47
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.202451725] HEAP: Allocated 6 bytes @ 0x3ffafff0 from task "alloc" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:48
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.202667075] HEAP: Freed bytes @ 0x3ffafff0 from task "free" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:31 (discriminator 9)
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.302436000] HEAP: Allocated 4 bytes @ 0x3ffafff0 from task "alloc" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:47
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.302451475] HEAP: Allocated 8 bytes @ 0x3ffb40b8 from task "alloc" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:48
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.302667500] HEAP: Freed bytes @ 0x3ffb40b8 from task "free" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:31 (discriminator 9)
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  Processing completed.

  Processed 1019 events

  =============== HEAP TRACE REPORT ===============

  Processed 14 heap events.

  [0.002244575] HEAP: Allocated 1 bytes @ 0x3ffaffd8 from task "alloc" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:47
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.102436025] HEAP: Allocated 2 bytes @ 0x3ffaffe0 from task "alloc" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:47
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.202436200] HEAP: Allocated 3 bytes @ 0x3ffaffe8 from task "alloc" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:47
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.302436000] HEAP: Allocated 4 bytes @ 0x3ffafff0 from task "alloc" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:47
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  Found 10 leaked bytes in 4 blocks.

Heap Tracing To Find Heap Corruption
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Heap tracing can also be used to help track down heap corruption. When a region in the heap is corrupted, it may be from some other part of the program that allocated memory at a nearby address.

If you have an approximate idea of when the corruption occurred, enabling heap tracing in ``HEAP_TRACE_ALL`` mode allows you to record all the memory allocation functions used and the corresponding allocation addresses.

Using heap tracing in this way is very similar to memory leak detection as described above. For memories that are allocated and not freed, the output is the same. However, records will also be shown for memory that has been freed.

Performance Impact
^^^^^^^^^^^^^^^^^^

Enabling heap tracing in menuconfig increases the code size of your program, and has a very small negative impact on the performance of heap allocation or free operations even when heap tracing is not running.

When heap tracing is running, heap allocation or free operations are substantially slower than when heap tracing is stopped. Increasing the depth of stack frames recorded for each allocation (see above) also increases this performance impact.

To mitigate the performance loss when the heap tracing is enabled and active, enable :ref:`CONFIG_HEAP_TRACE_HASH_MAP`. With this configuration enabled, a hash map mechanism will be used to handle the heap trace records, thus considerably decreasing the heap allocation or free execution time. The size of the hash map can be modified by setting the value of :ref:`CONFIG_HEAP_TRACE_HASH_MAP_SIZE`.

.. only:: SOC_SPIRAM_SUPPORTED

  By default, the hash map is placed into internal RAM. It can also be placed into external RAM if :ref:`CONFIG_HEAP_TRACE_HASH_MAP_IN_EXT_RAM` is enabled. In order to enable this configuration, make sure to enable :ref:`CONFIG_SPIRAM` and :ref:`CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY`.

False-Positive Memory Leaks
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Not everything printed by :cpp:func:`heap_trace_dump` is necessarily a memory leak. The following cases may also be printed:

- Any memory that is allocated after :cpp:func:`heap_trace_start` but freed after :cpp:func:`heap_trace_stop` appears in the leaked dump.
- Allocations may be made by other tasks in the system. Depending on the timing of these tasks, it is quite possible that this memory is freed after :cpp:func:`heap_trace_stop` is called.
- The first time a task uses stdio - e.g., when it calls :cpp:func:`heap_caps_printf` - a lock, i.e., RTOS mutex semaphore, is allocated by the libc. This allocation lasts until the task is deleted.
- Certain uses of :cpp:func:`heap_caps_printf`, such as printing floating point numbers and allocating some memory from the heap on demand. These allocations last until the task is deleted.
- The Bluetooth, Wi-Fi, and TCP/IP libraries allocate heap memory buffers to handle incoming or outgoing data. These memory buffers are usually short-lived, but some may be shown in the heap leak trace if the data has been received or transmitted by the lower levels of the network during the heap tracing.
- TCP connections retain some memory even after they are closed due to the ``TIME_WAIT`` state. Once the ``TIME_WAIT`` period is completed, this memory will be freed.

One way to differentiate between "real" and "false positive" memory leaks is to call the suspect code multiple times while tracing is running, and look for patterns (multiple matching allocations) in the heap trace output.

API Reference - Heap Tracing
----------------------------

.. include-build-file:: inc/esp_heap_trace.inc
