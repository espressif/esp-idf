.. _app_trace-function-tracing:

Compiler-Instrumented Function Tracing
======================================

:link_to_translation:`zh_CN:[中文]`

Function tracing records every function entry and exit automatically, without manual trace points. When a source file is built with the GCC flag ``-finstrument-functions``, the compiler inserts a call at the start and end of each function. These calls go to hooks provided by the :doc:`esp_trace <index>` component, which forward the events to the active trace encoder (for example SystemView). The events carry the raw function and call-site addresses. SystemView records them as raw addresses. Resolving the addresses to function names is done separately against the ELF file (for example with ``addr2line`` or a custom tool).

Enabling Function Tracing
-------------------------

Enable it under ``Component config`` > ``ESP Trace Configuration`` > ``Function Tracing``. The following options control function tracing:

- :ref:`CONFIG_ESP_TRACE_FUNCTION_TRACE` - build the function-trace hooks and runtime.
- :ref:`CONFIG_ESP_TRACE_FUNCTION_TRACE_AUTO_START` - when enabled (default), recording follows the encoder's state and begins as soon as the host starts the session. When disabled, no events are emitted until the application calls :cpp:func:`esp_trace_function_trace_start`; an active host session alone is not enough.

Enabling the option compiles the hooks and runtime into the build. It does not add ``-finstrument-functions`` to any code, so on its own it produces no events. To trace a component or file, add the flag from its own ``CMakeLists.txt`` for the sources you want traced:

.. code-block:: cmake

    if(CONFIG_ESP_TRACE_FUNCTION_TRACE)
        target_compile_options(${COMPONENT_LIB} PRIVATE -finstrument-functions)
    endif()

Use the GCC ``-finstrument-functions-exclude-file-list`` and ``-finstrument-functions-exclude-function-list`` flags to skip specific files or functions. Keep instrumentation scoped to your own components, and do not instrument code that runs with the flash cache disabled (IRAM ISRs, SPI flash operations).

Decoding Function-Trace Events
------------------------------

After collecting a SystemView capture (see :doc:`sysview`), decode the function-trace events with the processing script:

.. code-block:: bash

    $IDF_PATH/tools/esp_app_trace/sysviewtrace_proc.py -i func -b </path/to/program.elf> -t {IDF_TARGET_TOOLCHAIN_PREFIX}- file:///path/to/trace.svdat

The ``-i func`` option selects the function-trace event stream. The script uses the ELF file and toolchain prefix to resolve addresses, then prints a per-function report listing each traced function with its address, entry and exit counts, and source location.

Application Example
-------------------

- :example:`system/function_tracing` demonstrates compiler-instrumented function entry/exit tracing and how to control which code is instrumented.
