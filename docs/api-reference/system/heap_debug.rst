Heap Memory Debugging
=====================

Overview
--------

ESP-IDF integrates tools for requesting `heap information`_, `detecting heap corruption <heap corruption detection>`_, and `tracing memory leaks <heap tracing>`_. These can help track down memory-related bugs.

For general information about the heap memory allocator, see the :doc:`Heap Memory Allocation </api-reference/system/mem_alloc>` page.

.. _heap-information:

Heap Information
----------------

To obtain information about the state of the heap:

- ``xPortGetFreeHeapSize()`` is a FreeRTOS function which returns the number of free bytes in the (data memory) heap. This is equivalent to ``heap_caps_get_free_size(MALLOC_CAP_8BIT)``.
- ``heap_caps_get_free_size()`` can also be used to return the current free memory for different memory capabilities.
- ``heap_caps_get_largest_free_block()`` can be used to return the largest free block in the heap. This is the largest single allocation which is currently possible. Tracking this value and comparing to total free heap allows you to detect heap fragmentation.
- ``xPortGetMinimumEverFreeHeapSize()`` and the related ``heap_caps_get_minimum_free_size()`` can be used to track the heap "low water mark" since boot.
- ``heap_caps_get_info`` returns a ``multi_heap_info_t`` structure which contains the information from the above functions, plus some additional heap-specific data (number of allocations, etc.)


.. _heap-corruption:

Heap Corruption Detection
-------------------------

Heap corruption detection allows you to detect various types of heap memory errors:

- Out of bounds writes & buffer overflow.
- Writes to freed memory.
- Reads from freed or uninitialized memory,

Assertions
^^^^^^^^^^

The heap implementation (``multi_heap.c``, etc.) includes a lot of assertions which will fail if the heap memory is corrupted. To detect heap corruption most effectively, ensure that assertions are enabled in ``make menuconfig`` under ``Compiler options``.

It's also possible to manually check heap integrity by calling the ``heap_caps_check_integrity()`` function (see below). This function checks all of requested heap memory for integrity, and can be used even if assertions are disabled.

Configuration
^^^^^^^^^^^^^

In ``make menuconfig``, under ``Component config`` there is a menu ``Heap memory debugging``. The setting ``Heap corruption detection`` can be set to one of three levels:

Basic (no poisoning)
^^^^^^^^^^^^^^^^^^^^

This is the default level. No special heap corruption features are enabled, but checks will fail if any of the heap's internal data structures are overwritten or corrupted. This usually indicates a buffer overrun or out of bounds write.

If assertions are enabled, an assertion will also trigger if a double-free occurs (ie the same memory is freed twice).

Light impact
^^^^^^^^^^^^

At this level, heap memory is additionally "poisoned" with head and tail "canary bytes" before and after each block which is allocated. If an application writes outside the bounds of the allocated buffer, the canary bytes will be corrupted and the integrity check will fail.

"Basic" heap corruption checks can also detect out of bounds writes, but this setting is more precise as even a single byte overrun will always be detected. With Basic heap checks, the number of overrun bytes before a failure is detected will depend on the properties of the heap.

Similar to other heap checks, these "canary bytes" are checked via assertion whenever memory is freed and can also be checked manually via ``heap_caps_check_integrity()``.

This level increases memory usage, each individual allocation will use 9 to 12 additional bytes of memory (depending on alignment).

Comprehensive
^^^^^^^^^^^^^

This level incorporates the "light impact" detection features plus additional checks for uninitialised-access and use-after-free bugs. In this mode, all freshly allocated memory is filled with the pattern 0xCE, and all freed memory is filled with the pattern 0xFE.

If an application crashes reading/writing an address related to 0xCECECECE when this setting is enabled, this indicates it has read uninitialized memory. The application should be changed to either use calloc() (which zeroes memory), or initialize the memory before using it. The value 0xCECECECE may also be seen in stack-allocated automatic variables, because in IDF most task stacks are originally allocated from the heap and in C stack memory is uninitialized by default.

If an application crashes reading/writing an address related to 0xFEFEFEFE, this indicates it is reading heap memory after it has been freed (a "use after free bug".) The application should be changed to not access heap memory after it has been freed.

If the IDF heap allocator fails because the pattern 0xFEFEFEFE was not found in freed memory then this indicates the app has a use-after-free bug where it is writing to memory which has already been freed.

Enabling "Comprehensive" detection has a substantial runtime performance impact (as all memory needs to be set to the allocation patterns each time a malloc/free completes, and the memory also needs to be checked each time.)

Finding Heap Corruption
^^^^^^^^^^^^^^^^^^^^^^^

Memory corruption can be one of the hardest classes of bugs to find and fix, as one area of memory can be corrupted from a totally different place. Some tips:

- If you can find the address (in memory) which is being corrupted, you can set a watchpoint on this address via JTAG to have the CPU halt when it is written to.
- If you don't have JTAG, but you do know roughly when the corruption happens, then you can set a watchpoint in software. A fatal exception will occur when the watchpoint triggers. For example ``esp_set_watchpoint(0, (void *)addr, 4, ESP_WATCHPOINT_STORE``. Note that the watchpoint is set on the current running CPU only, so if you don't know which CPU is corrupting memory then you will need to call this function on both CPUs.
- For buffer overflows, `heap tracing`_ in ``HEAP_TRACE_ALL`` mode lets you see which callers allocate the memory address(es) immediately before the address which is being corrupted. There is a strong chance this is the code which overflows the buffer.

.. _heap-tracing:

Heap Tracing
------------

Heap Tracing allows tracing of code which allocates/frees memory.

Note: Heap tracing "standalone" mode is currently implemented, meaning that tracing does not require any external hardware but uses internal memory to hold trace data. Heap tracing via JTAG trace port is also planned.

Heap tracing can perform two functions:

- Leak checking: find memory which is allocated and never freed.
- Heap use analysis: show all functions that are allocating/freeing memory while the trace is running.

How To Diagnose Memory Leaks
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you suspect a memory leak, the first step is to figure out which part of the program is leaking memory. Use the ``xPortGetFreeHeapSize()``, ``heap_caps_get_free()``, and related functions to track memory use over the life of the application. Try to narrow the leak down to a single function or sequence of functions where free memory always decreases and never recovers.

Once you've identified the code which you think is leaking:

- Under ``make menuconfig``, navigate to ``Component settings`` -> ``Heap Memory Debugging`` and set ``Enable heap tracing``.
- Call the function ``heap_trace_init_standalone()`` early in the program, to register a buffer which can be used to record the memory trace.
- Call the function ``heap_trace_start()`` to begin recording all mallocs/frees in the system. Call this immediately before the piece of code which you suspect is leaking memory.
- Call the function ``heap_trace_stop()`` to stop the trace once the suspect piece of code has finished executing.
- Call the function ``heap_trace_dump()`` to dump the results of the heap trace.

An example::

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

The output from the heap trace will look something like this::

  2 allocations trace (100 entry buffer)
  32 bytes (@ 0x3ffaf214) allocated CPU 0 ccount 0x2e9b7384 caller 0x400d276d:0x400d27c1
  0x400d276d: leak_some_memory at /path/to/idf/examples/get-started/blink/main/./blink.c:27

  0x400d27c1: blink_task at /path/to/idf/examples/get-started/blink/main/./blink.c:52

  8 bytes (@ 0x3ffaf804) allocated CPU 0 ccount 0x2e9b79c0 caller 0x400d2776:0x400d27c1
  0x400d2776: leak_some_memory at /path/to/idf/examples/get-started/blink/main/./blink.c:29

  0x400d27c1: blink_task at /path/to/idf/examples/get-started/blink/main/./blink.c:52

  40 bytes 'leaked' in trace (2 allocations)
  total allocations 2 total frees 0

(Above example output is using :doc:`IDF Monitor </get-started/idf-monitor>` to automatically decode PC addresses to their source files & line number.)

The first line indicates how many allocation entries are in the buffer, compared to its total size.

In ``HEAP_TRACE_LEAKS`` mode, for each traced memory allocation which has not already been freed a line is printed with:

- ``XX bytes`` is number of bytes allocated
- ``@ 0x...`` is the heap address returned from malloc/calloc.
- ``CPU x`` is the CPU (0 or 1) running when the allocation was made.
- ``ccount 0x...`` is the CCOUNT (CPU cycle count) register value when the allocation was mode. Is different for CPU 0 vs CPU 1.
- ``caller 0x...`` gives the call stack of the call to malloc()/free(), as a list of PC addresses.
  These can be decoded to source files and line numbers, as shown above.

The depth of the call stack recorded for each trace entry can be configured in ``make menuconfig``, under ``Heap Memory Debugging`` -> ``Enable heap tracing`` -> ``Heap tracing stack depth``. Up to 10 stack frames can be recorded for each allocation (the default is 2). Each additional stack frame increases the memory usage of each ``heap_trace_record_t`` record by eight bytes.

Finally, the total number of 'leaked' bytes (bytes allocated but not freed while trace was running) is printed, and the total number of allocations this represents.

A warning will be printed if the trace buffer was not large enough to hold all the allocations which happened. If you see this warning, consider either shortening the tracing period or increasing the number of records in the trace buffer.

Performance Impact
^^^^^^^^^^^^^^^^^^

Enabling heap tracing in menuconfig increases the code size of your program, and has a very small negative impact on performance of heap allocation/free operations even when heap tracing is not running.

When heap tracing is running, heap allocation/free operations are substantially slower than when heap tracing is stopped. Increasing the depth of stack frames recorded for each allocation (see above) will also increase this performance impact.

False-Positive Memory Leaks
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Not everything printed by ``heap_trace_dump()`` is necessarily a memory leak. Among things which may show up here, but are not memory leaks:

- Any memory which is allocated after ``heap_trace_start()`` but then freed after ``heap_trace_stop()`` will appear in the leak dump.
- Allocations may be made by other tasks in the system. Depending on the timing of these tasks, it's quite possible this memory is freed after ``heap_trace_stop()`` is called.
- The first time a task uses stdio - for example, when it calls printf() - a lock (RTOS mutex semaphore) is allocated by the libc. This allocation lasts until the task is deleted.
- The Bluetooth, WiFi, and TCP/IP libraries will allocate heap memory buffers to handle incoming or outgoing data. These memory buffers are usually short lived, but some may be shown in the heap leak trace if the data was received/transmitted by the lower levels of the network while the leak trace was running.
- TCP connections will continue to use some memory after they are closed, because of the ``TIME_WAIT`` state. After the ``TIME_WAIT`` period has completed, this memory will be freed.

One way to differentiate between "real" and "false positive" memory leaks is to call the suspect code multiple times while tracing is running, and look for patterns (multiple matching allocations) in the heap trace output.

API Reference - Heap Tracing
----------------------------

.. include:: /_build/inc/esp_heap_trace.inc
