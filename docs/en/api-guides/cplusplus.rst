C++ Support
===========

.. highlight:: cpp

ESP-IDF is primarily written in C and provides C APIs. However, ESP-IDF supports development of applications in C++. This document covers various topics relevant to C++ development.

The following C++ features are supported:

- :ref:`cplusplus_exceptions`
- :ref:`cplusplus_multithreading`
- :ref:`cplusplus_rtti`
- :doc:`thread-local-storage` (``thread_local`` keyword)
- All C++ features implemented by GCC, except for some :ref:`limitations <cplusplus_limitations>`. See `GCC documentation <https://gcc.gnu.org/projects/cxx-status.html>`_ for details on features implemented by GCC.

esp-idf-cxx Component
---------------------

`esp-idf-cxx <https://github.com/espressif/esp-idf-cxx>`_ component provides higher-level C++ APIs for some of the ESP-IDF features. This component is available from the `IDF Component Registry <https://components.espressif.com/components/espressif/esp-idf-cxx>`_.

C++ language standard
---------------------

By default, ESP-IDF compiles C++ code with C++23 language standard with GNU extensions (``-std=gnu++23``).

To compile the source code of a certain component using a different language standard, set the desired compiler flag in the component CMakeLists.txt file:

.. code-block:: cmake

    idf_component_register( ... )
    target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++11)

Use ``PUBLIC`` instead of ``PRIVATE`` if the public header files of the component also need to be compiled with the same language standard.

.. _cplusplus_multithreading:

Multithreading
--------------

C++ threads, mutexes, and condition variables are supported. C++ threads are built on top of pthreads, which in turn wrap FreeRTOS tasks.

See :example:`cxx/pthread` for an example of creating threads in C++.

.. note::

    The destructor of `std::jthread <https://en.cppreference.com/w/cpp/thread/jthread>`_ can only safely be called from a task that has been created by :ref:`the IDF implementation of pthread_create() <posix_thread_api>` or by the `C++ threading library API <https://en.cppreference.com/w/cpp/thread>`_.

.. _cplusplus_exceptions:

Exception handling
------------------

Support for C++ Exceptions in ESP-IDF is disabled by default, but can be enabled using the :ref:`CONFIG_COMPILER_CXX_EXCEPTIONS` option.

If an exception is thrown, but there is no ``catch`` block, the program will be terminated by the ``abort`` function, and the backtrace will be printed. See :doc:`Fatal Errors <fatal-errors>` for more information about backtraces.

C++ Exceptions should *only* be used for exceptional cases, something happening unexpectedly and that is quite rare, e.g. an event that happens less frequently than 1 every 100 times. *Do not* use them for control flow (see also the section about resource usage below)! For more information on how to use C++ Exceptions, see the `ISO C++ FAQ <https://isocpp.org/wiki/faq/exceptions>`_ and `CPP Core Guidelines <https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-errors>`_.

See :example:`cxx/exceptions` for an example of C++ exception handling.

C++ Exception Handling and Resource Usage 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Enabling exception handling normally increases application binary size by a few KB.

Additionally, it may be necessary to reserve some amount of RAM for exception emergency pool. Memory from this pool will be used if it is not possible to allocate exception object from the heap. The amount of memory in the emergency pool can be set using the :ref:`CONFIG_COMPILER_CXX_EXCEPTIONS_EMG_POOL_SIZE` variable. Some additional stack memory (around 200 bytes) will also be used if and only if a C++ Exception is actually thrown, because it requires calling some functions from the top of the stack to initiate exception handling.

The run time of code using C++ exceptions depends on what actually happens at run time. If no exception is thrown, the code tends to be somewhat faster since there is no need to check error codes. If an exception is thrown, the run time of the code that handles exceptions will be orders of magnitude slower than code returning an error code. This increase may or may not be significant, however, in the entire application, in particular if the error handling requires additional action, such as a user input or messaging to a cloud. But exception-throwing code should never be used in real-time critical code paths.

.. _cplusplus_rtti:

Runtime Type Information (RTTI)
-------------------------------

Support for RTTI is disabled by default, but can be enabled using :ref:`CONFIG_COMPILER_CXX_RTTI` option.

Enabling this option compiles all C++ files with RTTI support enabled, which allows using ``dynamic_cast`` conversion and ``typeid`` operator. Enabling this option typically increases the binary size by tens of kB.

See :example:`cxx/rtti` for an example of using RTTI in ESP-IDF.


Developing in C++
-----------------

The following sections provide tips on developing ESP-IDF applications in C++.

Combining C and C++ code
^^^^^^^^^^^^^^^^^^^^^^^^

When part of the application is developed in C and part in C++, it is important to understand the concept of `language linkage <https://en.cppreference.com/w/cpp/language/language_linkage>`_.

In order for a C++ function to be callable from C code, it has to be both *declared* and *defined* with C linkage (``extern "C"``)::

    // declaration in the header file:
    #ifdef __cplusplus
    extern "C" {
    #endif

    void my_cpp_func(void);

    #ifdef __cplusplus
    }
    #endif

    // definition in a .cpp file:
    extern "C" void my_cpp_func(void) {
        // ...
    }


In order for a C function to be callable from C++, it has to be *declared* with C linkage::

    // declaration in the header file:
    #ifdef __cplusplus
    extern "C" {
    #endif

    void my_c_func(void);

    #ifdef __cplusplus
    }
    #endif

    // definition in a .c file:
    void my_c_func(void) {
        // ...
    }


Defining ``app_main`` in C++
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ESP-IDF expects the application entry point, ``app_main``, to be defined with C linkage. When ``app_main`` is defined in a .cpp source file, it has to be designated as ``extern "C"``::

    extern "C" void app_main()
    {
    }

.. _cplusplus_designated_initializers:

Designated initializers
^^^^^^^^^^^^^^^^^^^^^^^

Many of the ESP-IDF components use :ref:`configuration structures <api_reference_config_structures>` as arguments to the initialization functions. ESP-IDF examples written in C routinely use `designated initializers <https://en.cppreference.com/w/c/language/struct_initialization>`_ to fill these structures in a readable and a maintainable way.

C and C++ languages have different rules with regards to the designated initializers. For example, C++ language version C++23, currently the default in ESP-IDF, does not support out-of-order designated initialization, nested designated initialization, mixing of designated initializers and regular initializers, and designated initialization of arrays. Therefore, when porting ESP-IDF C examples to C++, some changes to the structure initializers may be necessary. See the `C++ aggregate initialization reference <https://en.cppreference.com/w/cpp/language/aggregate_initialization>`_ for more details.


iostream
^^^^^^^^

``iostream`` functionality is supported in ESP-IDF, with a couple of caveats:

1. Normally ESP-IDF build process eliminates the unused code. However in the case of iostreams, simply including ``<iostream>`` header in one of the source files significantly increases the binary size (by about 200 kB).
2. By default, ESP-IDF uses a simple non-blocking implementation of the standard input stream (``stdin``). To get the usual behavior of ``std::cin``, the application has to initialize the UART driver and enable the blocking mode as shown in :example_file:`common_components/protocol_examples_common/stdin_out.c`.

.. _cplusplus_limitations:

Limitations
-----------

- Linker script generator doesn't support function level placements for functions with C++ linkage.
- Various section attributes (such as ``IRAM_ATTR``) are ignored when used with template functions.
- Vtables are placed into Flash and are not accessible when the flash cache is disabled. Therefore, virtual function calls should be avoided in :ref:`IRAM-safe interrupt handlers <iram-safe-interrupt-handlers>`. Placement of Vtables cannot be adjusted using the linker script generator, yet.
- C++ filesystem (``std::filesystem``) features are not supported.

What to Avoid
-------------

Do not use ``setjmp``/``longjmp`` in C++! ``longjmp`` blindly jumps up the stack without calling any destructors, easily introducing undefined behavior and memory leaks. Use C++ exceptions instead, they will guarantee correctly calling destructors. If you cannot use C++ exceptions, use alternatives (except ``setjmp``/``longjmp`` themselves) such as simple return codes.
