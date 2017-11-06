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

- :cpp:func:`xPortGetFreeHeapSize` is a FreeRTOS function which returns the number of free bytes in the (data memory) heap. This is equivalent to calling ``heap_caps_get_free_size(MALLOC_CAP_8BIT)``.
- :cpp:func:`heap_caps_get_free_size` can also be used to return the current free memory for different memory capabilities.
- :cpp:func:`heap_caps_get_largest_free_block` can be used to return the largest free block in the heap. This is the largest single allocation which is currently possible. Tracking this value and comparing to total free heap allows you to detect heap fragmentation.
- :cpp:func:`xPortGetMinimumEverFreeHeapSize` and the related :cpp:func:`heap_caps_get_minimum_free_size` can be used to track the heap "low water mark" since boot.
- :cpp:func:`heap_caps_get_info` returns a :cpp:class:`multi_heap_info_t` structure which contains the information from the above functions, plus some additional heap-specific data (number of allocations, etc.).
- :cpp:func:`heap_caps_print_heap_info` prints a summary to stdout of the information returned by :cpp:func:`heap_caps_get_info`.
- :cpp:func:`heap_caps_dump` and :cpp:func:`heap_caps_dump_all` will output detailed information about the structure of each block in the heap. Note that this can be large amount of output.


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

If a heap integrity assertion fails, a line will be printed like ``CORRUPT HEAP: multi_heap.c:225 detected at 0x3ffbb71c``. The memory address which is printed is the address of the heap structure which has corrupt content.

It's also possible to manually check heap integrity by calling :cpp:func:`heap_caps_check_integrity_all` or related functions. This function checks all of requested heap memory for integrity, and can be used even if assertions are disabled. If the integrity check prints an error, it will also contain the address(es) of corrupt heap structures.

Finding Heap Corruption
^^^^^^^^^^^^^^^^^^^^^^^

Memory corruption can be one of the hardest classes of bugs to find and fix, as one area of memory can be corrupted from a totally different place. Some tips:

- A crash with a ``CORRUPT HEAP:`` message will usually include a stack trace, but this stack trace is rarely useful. The crash is the symptom of memory corruption when the system realises the heap is corrupt, but usually the corruption happened elsewhere and earlier in time.
- Increasing the Heap memory debugging `Configuration`_ level to "Light impact" or "Comprehensive" can give you a more accurate message with the first corrupt memory address.
- Adding regular calls to :cpp:func:`heap_caps_check_integrity_all` or :cpp:func:`heap_caps_check_integrity_addr` in your code will help you pin down the exact time that the corruption happened. You can move these checks around to "close in on" the section of code that corrupted the heap.
- Based on the memory address which is being corrupted, you can use :ref:`JTAG debugging <jtag-debugging-introduction>` to set a watchpoint on this address and have the CPU halt when it is written to.
- If you don't have JTAG, but you do know roughly when the corruption happens, then you can set a watchpoint in software just beforehand via :cpp:func:`esp_set_watchpoint`. A fatal exception will occur when the watchpoint triggers. For example ``esp_set_watchpoint(0, (void *)addr, 4, ESP_WATCHPOINT_STORE``. Note that watchpoints are per-CPU and are set on the current running CPU only, so if you don't know which CPU is corrupting memory then you will need to call this function on both CPUs.
- For buffer overflows, `heap tracing`_ in ``HEAP_TRACE_ALL`` mode lets you see which callers are allocating which addresses from the heap. See `Heap Tracing To Find Heap Corruption`_ for more details. If you can find the function which allocates memory with an address immediately before the address which is corrupted, this will probably be the function which overflows the buffer.
- Calling :cpp:func:`heap_caps_dump` or :cpp:func:`heap_caps_dump_all` can give an indication of what heap blocks are surrounding the corrupted region and may have overflowed/underflowed/etc.

Configuration
^^^^^^^^^^^^^

Temporarily increasing the heap corruption detection level can give more detailed information about heap corruption errors.

In ``make menuconfig``, under ``Component config`` there is a menu ``Heap memory debugging``. The setting :ref:`CONFIG_HEAP_CORRUPTION_DETECTION` can be set to one of three levels:

Basic (no poisoning)
++++++++++++++++++++

This is the default level. No special heap corruption features are enabled, but provided assertions are enabled (the default configuration) then a heap corruption error will be printed if any of the heap's internal data structures appear overwritten or corrupted. This usually indicates a buffer overrun or out of bounds write.

If assertions are enabled, an assertion will also trigger if a double-free occurs (the same memory is freed twice).

Calling :cpp:func:`heap_caps_check_integrity` in Basic mode will check the integrity of all heap structures, and print errors if any appear to be corrupted.

Light Impact
++++++++++++

At this level, heap memory is additionally "poisoned" with head and tail "canary bytes" before and after each block which is allocated. If an application writes outside the bounds of allocated buffers, the canary bytes will be corrupted and the integrity check will fail.

The head canary word is 0xABBA1234 (3412BAAB in byte order), and the tail canary word is 0xBAAD5678 (7856ADBA in byte order).

"Basic" heap corruption checks can also detect most out of bounds writes, but this setting is more precise as even a single byte overrun can be detected. With Basic heap checks, the number of overrun bytes before a failure is detected will depend on the properties of the heap.

Enabling "Light Impact" checking increases memory usage, each individual allocation will use 9 to 12 additional bytes of memory (depending on alignment).

Each time ``free()`` is called in Light Impact mode, the head and tail canary bytes of the buffer being freed are checked against the expected values.

When :cpp:func:`heap_caps_check_integrity` is called, all allocated blocks of heap memory have their canary bytes checked against the expected values.

In both cases, the check is that the first 4 bytes of an allocated block (before the buffer returned to the user) should be the word 0xABBA1234. Then the last 4 bytes of the allocated block (after the buffer returned to the user) should be the word 0xBAAD5678.

Different values usually indicate buffer underrun or overrun, respectively.


Comprehensive
+++++++++++++

This level incorporates the "light impact" detection features plus additional checks for uninitialised-access and use-after-free bugs. In this mode, all freshly allocated memory is filled with the pattern 0xCE, and all freed memory is filled with the pattern 0xFE.

Enabling "Comprehensive" detection has a substantial runtime performance impact (as all memory needs to be set to the allocation patterns each time a malloc/free completes, and the memory also needs to be checked each time.) However it allows easier detection of memory corruption bugs which are much more subtle to find otherwise. It is recommended to only enable this mode when debugging, not in production.

Crashes in Comprehensive Mode
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If an application crashes reading/writing an address related to 0xCECECECE in Comprehensive mode, this indicates it has read uninitialized memory. The application should be changed to either use calloc() (which zeroes memory), or initialize the memory before using it. The value 0xCECECECE may also be seen in stack-allocated automatic variables, because in IDF most task stacks are originally allocated from the heap and in C stack memory is uninitialized by default.

If an application crashes and the exception register dump indicates that some addresses or values were 0xFEFEFEFE, this indicates it is reading heap memory after it has been freed (a "use after free bug".) The application should be changed to not access heap memory after it has been freed.

If a call to malloc() or realloc() causes a crash because it expected to find the pattern 0xFEFEFEFE in free memory and a different pattern was found, then this indicates the app has a use-after-free bug where it is writing to memory which has already been freed.

Manual Heap Checks in Comprehensive Mode
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Calls to :cpp:func:`heap_caps_check_integrity` may print errors relating to 0xFEFEFEFE, 0xABBA1234 or 0xBAAD5678. In each case the checker is expecting to find a given pattern, and will error out if this is not found:

- For free heap blocks, the checker expects to find all bytes set to 0xFE. Any other values indicate a use-after-free bug where free memory has been incorrectly overwritten.
- For allocated heap blocks, the behaviour is the same as for `Light Impact` mode. The canary bytes 0xABBA1234 and 0xBAAD5678 are checked at the head and tail of each allocated buffer, and any variation indicates a buffer overrun/underrun.

.. _heap-tracing:

Heap Tracing
------------

Heap Tracing allows tracing of code which allocates/frees memory.

.. note::

   Heap tracing "standalone" mode is currently implemented, meaning that tracing does not require any external hardware but uses internal memory to hold trace data. Heap tracing via JTAG trace port is also planned.

Heap tracing can perform two functions:

- Leak checking: find memory which is allocated and never freed.
- Heap use analysis: show all functions that are allocating/freeing memory while the trace is running.

How To Diagnose Memory Leaks
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you suspect a memory leak, the first step is to figure out which part of the program is leaking memory. Use the :cpp:func:`xPortGetFreeHeapSize`, :cpp:func:`heap_caps_get_free`, and related functions to track memory use over the life of the application. Try to narrow the leak down to a single function or sequence of functions where free memory always decreases and never recovers.

Once you've identified the code which you think is leaking:

- Under ``make menuconfig``, navigate to ``Component settings`` -> ``Heap Memory Debugging`` and set :ref:`CONFIG_HEAP_TRACING`.
- Call the function :cpp:func:`heap_trace_init_standalone` early in the program, to register a buffer which can be used to record the memory trace.
- Call the function :cpp:func:`heap_trace_start` to begin recording all mallocs/frees in the system. Call this immediately before the piece of code which you suspect is leaking memory.
- Call the function :cpp:func:`heap_trace_stop` to stop the trace once the suspect piece of code has finished executing.
- Call the function :cpp:func:`heap_trace_dump` to dump the results of the heap trace.

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

Heap Tracing To Find Heap Corruption
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When a region in heap is corrupted, it may be from some other part of the program which allocated memory at a nearby address.

If you have some idea at what time the corruption occured, enabling heap tracing in ``HEAP_TRACE_ALL`` mode allows you to record all of the functions which allocated memory, and the addresses where they were corrupted.

Using heap tracing in this way is very similar to memory leak detection as described above. For memory which is allocated and not freed, the output 

Heap tracing can also be used to help track down heap corruption. By using 

Performance Impact
^^^^^^^^^^^^^^^^^^

Enabling heap tracing in menuconfig increases the code size of your program, and has a very small negative impact on performance of heap allocation/free operations even when heap tracing is not running.

When heap tracing is running, heap allocation/free operations are substantially slower than when heap tracing is stopped. Increasing the depth of stack frames recorded for each allocation (see above) will also increase this performance impact.

False-Positive Memory Leaks
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Not everything printed by :cpp:func:`heap_trace_dump` is necessarily a memory leak. Among things which may show up here, but are not memory leaks:

- Any memory which is allocated after :cpp:func:`heap_trace_start` but then freed after :cpp:func:`heap_trace_stop` will appear in the leak dump.
- Allocations may be made by other tasks in the system. Depending on the timing of these tasks, it's quite possible this memory is freed after :cpp:func:`heap_trace_stop` is called.
- The first time a task uses stdio - for example, when it calls ``printf()`` - a lock (RTOS mutex semaphore) is allocated by the libc. This allocation lasts until the task is deleted.
- The Bluetooth, WiFi, and TCP/IP libraries will allocate heap memory buffers to handle incoming or outgoing data. These memory buffers are usually short lived, but some may be shown in the heap leak trace if the data was received/transmitted by the lower levels of the network while the leak trace was running.
- TCP connections will continue to use some memory after they are closed, because of the ``TIME_WAIT`` state. After the ``TIME_WAIT`` period has completed, this memory will be freed.

One way to differentiate between "real" and "false positive" memory leaks is to call the suspect code multiple times while tracing is running, and look for patterns (multiple matching allocations) in the heap trace output.

API Reference - Heap Tracing
----------------------------

.. include:: /_build/inc/esp_heap_trace.inc
