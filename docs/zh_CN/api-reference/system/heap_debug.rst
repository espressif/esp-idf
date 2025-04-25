堆内存调试
=====================

:link_to_translation:`en:[English]`

概述
--------

ESP-IDF 集成了用于请求 :ref:`堆内存信息 <heap-information>`、:ref:`堆内存损坏检测 <heap-corruption>` 和 :ref:`堆内存跟踪 <heap-tracing>` 的工具，有助于跟踪内存相关错误。

有关堆内存分配器的基本信息，请参阅 :doc:`堆内存分配 </api-reference/system/mem_alloc>`。

.. _heap-information:

堆内存信息
----------------

要获取堆内存状态的相关信息，请调用以下函数：

- :cpp:func:`heap_caps_get_free_size` 返回不同属性内存的当前空闲内存。
- :cpp:func:`heap_caps_get_largest_free_block` 返回堆中最大的空闲块，也是当前可分配的最大内存块。跟踪此值并将其与总空闲堆对比，可以检测堆碎片化情况。
- :cpp:func:`heap_caps_get_minimum_free_size` 可以跟踪堆启动以来的“低水位”。
- :cpp:func:`heap_caps_get_info` 返回一个 :cpp:class:`multi_heap_info_t` 结构体，包含上述函数的信息，以及一些额外的特定堆内存数据（分配数量等）。
- :cpp:func:`heap_caps_print_heap_info` 将 :cpp:func:`heap_caps_get_info` 返回的信息摘要打印到标准输出。
- :cpp:func:`heap_caps_dump` 和 :cpp:func:`heap_caps_dump_all` 输出堆中每个内存块结构的详细信息。注意，这可能会产生大量输出。


.. _heap-allocation-free:

堆内存分配与释放钩子函数
---------------------------------------

你可以使用堆内存分配和释放检测钩子，接收每次成功分配或释放堆内存操作的通知：

- 定义 :cpp:func:`esp_heap_trace_alloc_hook` 获取堆内存分配操作成功的提示。
- 定义 :cpp:func:`esp_heap_trace_free_hook` 获取堆内存释放操作成功的提示。

要启用此功能，请设置 :ref:`CONFIG_HEAP_USE_HOOKS` 选项。:cpp:func:`esp_heap_trace_alloc_hook` 和 :cpp:func:`esp_heap_trace_free_hook` 具有弱声明（即 ``__attribute__((weak))``），因此无需为这两个钩子提供声明。鉴于从 ISR 中分配和释放堆内存在技术上是可行的（**但强烈不建议**），:cpp:func:`esp_heap_trace_alloc_hook` 和 :cpp:func:`esp_heap_trace_free_hook` 可能会从 ISR 中调用。

不建议在钩子函数中执行（或调用 API 函数执行）阻塞操作或堆内存分配与释放。一般而言，最好保持代码简洁，避免在钩子函数中进行复杂计算。

要定义堆内存分配及释放钩子，请参阅如下示例:

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


.. _heap-allocation-failed:

内存分配失败钩子
------------------

用户可以使用 :cpp:func:`heap_caps_register_failed_alloc_callback` 注册回调函数，每次内存分配操作失败时都会调用该函数。

此外，若启用 :ref:`CONFIG_HEAP_ABORT_WHEN_ALLOCATION_FAILS` 选项，可以在任何分配操作失败时，自动中止系统。

要注册内存分配失败的回调函数，请参阅如下示例：

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


.. _heap-corruption:

检测堆内存损坏
------------------

堆内存损坏检测可检测到各类堆内存错误，包括：

- 越界写入和缓冲区溢出
- 对已释放内存的写操作
- 对已释放或未初始化内存的读操作

堆内存损坏检测共有三个级别，在下方列表中，后一级别比前一级别提供更精细的检测：

.. list::
    - `基本模式（无 canary 标记）`_
    - `轻量级模式`_
    - `全面检测模式`_

断言
^^^^^^^^^^

如 :component_file:`heap/multi_heap.c` 等堆的实现方式包含许多断言，堆内存损坏则断言失败。为高效检测堆内存损坏，请确保在项目配置中通过 :ref:`CONFIG_COMPILER_OPTIMIZATION_ASSERTION_LEVEL` 选项启用断言。

如果堆完整性断言失败，将打印一行类似 ``CORRUPT HEAP: multi_heap.c:225 detected at 0x3ffbb71c`` 的内容，打印的内存地址即内容损坏的堆结构地址。

调用 :cpp:func:`heap_caps_check_integrity_all` 或相关函数可手动检测堆的完整性。该函数可以检测所有指定的堆内存是否完整，在禁用断言时仍可生效。若此完整性检测发现错误，将打印相应错误及损坏的堆结构地址。

定位堆内存损坏
^^^^^^^^^^^^^^^^^^^^^^^

内存损坏可能是最难定位和修复的错误类型之一，因为导致内存损坏的原因可能与问题的表现毫不相干。以下是有关定位堆内存损坏的一些提示：

- 如果系统崩溃并提示 ``CORRUPT HEAP:``，打印信息中通常包含栈跟踪，但此栈跟踪往往无效，因为系统会在检测到堆内存损坏后崩溃，但实际的损坏通常发生在其他位置，且损坏时间早于系统发现的时间。
- 将堆内存调试 `配置项`_ 级别增加到“轻度影响”或“全面”可以得到更准确的信息，定位首个内存损坏的地址。
- 在代码中定期调用 :cpp:func:`heap_caps_check_integrity_all` 或 :cpp:func:`heap_caps_check_integrity_addr` 可以定位内存损坏发生的确切时间。可以反复调整检测函数位置，以定位导致堆内存损坏的代码块。
- 根据损坏的内存地址，按照 :ref:`JTAG 调试 <jtag-debugging-introduction>` 在此地址上设置监视点，并在写入时使 CPU 暂停。
- 如果没有 JTAG，但大致了解损坏发生的时间，则可以通过 :cpp:func:`esp_cpu_set_watchpoint` 在软件中提前设置监视点，触发监视点将导致致命错误。函数使用示例为 ``esp_cpu_set_watchpoint(0, (void *)addr, 4, ESP_WATCHPOINT_STORE)``。注意，监视点在各个 CPU 独立存在，并且仅设置在当前运行的 CPU 上，因此，若无法确定哪个 CPU 破坏了内存，则需要在两个 CPU 上分别调用此函数。
- 对于缓冲区溢出，以 ``HEAP_TRACE_ALL`` 模式进行 `堆内存跟踪`_ 可以看到哪些调用函数正在从堆中分配哪些地址，详情请参阅 `堆内存跟踪定位堆内存损坏`_。如果可以找到在已损坏地址的前一地址分配内存的函数，这些函数很可能就是使缓冲区溢出的函数。
- 调用 :cpp:func:`heap_caps_dump` 或 :cpp:func:`heap_caps_dump_all` 提示损坏区域周围堆块的情况，并了解哪些堆块可能已经溢出或下溢等。

配置项
^^^^^^^^^^^^^

暂时提高堆内存损坏检测级别，可以进一步获取有关堆内存损坏错误的详细信息。

在项目配置菜单中，可以在 ``Component config`` 下找到 ``Heap memory debugging`` 菜单，其中的 :ref:`CONFIG_HEAP_CORRUPTION_DETECTION` 选项可以设置为以下三种级别：


基本模式（无 canary 标记）
++++++++++++++++++++++++++++++++

此为默认级别，默认情况下，不会启用任何特殊的堆内存损坏检测功能。但会启用提供的断言。如果堆的任何内部数据结构出现覆盖或损坏，就会打印出一个堆内存损坏错误。这通常表示缓冲区溢出或越界写入。

启用断言时，如果出现重复释放相同内存的情况（即双重释放），则会触发断言。

在基本模式调用 :cpp:func:`heap_caps_check_integrity`，可以检查所有堆结构的完整性，并在出错时打印错误信息。


轻量级模式
+++++++++++++++++

此级别包含“基本模式”中的所有检测功能。此外，每个分配的内存块都会在头部和尾部添加 canary 字节进行“污染”标记。如果应用程序写入了这些 canary 字节，则会被视为已损坏，导致完整性检查失败。

头 canary 字的值为 ``0xABBA1234`` （按字节顺序为 ``3412BAAB``），尾 canary 字的值为 ``0xBAAD5678`` （按字节顺序为 ``7856ADBA``）。

基本模式下的堆内存损坏检测可以检测到大多数越界写入，在检测到错误前的越界字节数取决于堆属性。但轻量级模式更精确，可以检测到单个字节的越界写入。

启用轻量级模式检测会增加内存使用量，因为每次内存分配都会使用额外的元数据字节。

在轻量级模式下，每次调用 :cpp:func:`heap_caps_free` 时，都会检查要释放的缓冲区头尾 canary 字节是否匹配预期值。

调用 :cpp:func:`heap_caps_check_integrity` 或 :cpp:func:`heap_caps_check_integrity_all` 时，会检查所有已分配的堆内存块的 canary 字节是否匹配预期值。

以上两种情况检查的是，在缓冲区返回给用户之前，已分配块的前 4 个字节是否为 ``0xABBA1234``，以及在缓冲区返回给用户之后，最后 4 个字节是否为 ``0xBAAD5678``。

如果检查到字节与上述值不同，通常表示缓冲区越界或下溢。其中越界表示在写入内存时，写入的数据超过了所分配内存的大小，导致写入到了未分配的内存区域；下溢表示在读取内存时，读取的数据超出了所分配内存的范围，读取了未分配的内存区域的数据。


全面检测模式
+++++++++++++++++++

此级别包含了轻量级模式的检测功能，此外还会检查未初始化访问和使用已释放内存产生的错误。此模式会将所有新分配的内存填充为 ``0xCE``，将所有已释放的内存填充为 ``0xFE``。

启用全面检测模式会对运行性能产生实质影响，因为每次 :cpp:func:`heap_caps_malloc` 或 :cpp:func:`heap_caps_free` 操作完成时，都需要将所有内存设置为分配模式，并检查内存。但是，此模式更容易检测到其他方式难以发现的内存损坏错误。建议只在调试时启用此模式，请勿在生产环境中启用。

在调用 :cpp:func:`heap_caps_check_integrity` 或 :cpp:func:`heap_caps_check_integrity_all` 时，还会对已分配和释放模式（分别为 ``0xCE`` 和 ``0xFE``）进行检查。

全面检测模式下程序崩溃
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

全面检测模式下，如果应用程序在读取或写入与 ``0xCECECECE`` 相关地址时崩溃，表示它读取了未初始化内存。此时，应修改应用程序，使用 :cpp:func:`heap_caps_calloc` 将内存清零，或在使用前初始化内存。在栈分配的自动变量中也可能存在 ``0xCECECECE`` 的值，因为 ESP-IDF 中的大多数任务栈最初由堆分配，而在 C 中，栈内存默认未初始化。

如果应用程序崩溃，且异常寄存器转储指示某些地址或值为 ``0xFEFEFEFE``，表示它读取了已释放的堆内存。此时，应修改应用程序，避免访问已释放的堆内存。

调用 :cpp:func:`heap_caps_malloc` 或 :cpp:func:`heap_caps_realloc` 时，如果在已释放的内存中找到了不同于 ``0xFEFEFEFE`` 的值，将导致应用程序崩溃。这表示应用程序写入了已经释放的内存，从而产生错误。

全面检测模式下手动堆内存检测
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

调用 :cpp:func:`heap_caps_check_integrity` 或 :cpp:func:`heap_caps_check_integrity_all` 会打印与 ``0xFEFEFEFE``、``0xABBA1234``、或 ``0xBAAD5678`` 相关的错误。在不同情况下，检测器均会检测给定模式，若未找到，则输出相应错误：

- 对于已释放的堆内存块，检测器会检查是否所有字节都设置为 ``0xFE``，检测到任何其他值都表示错误写入了已释放内存。
- 对于已分配的堆内存块，检测器的检查模式与轻量级模式相同，即在每个分配的缓冲区头部和尾部检查 canary 字节 ``0xABBA1234`` 和 ``0xBAAD5678``，检测到任何其他字节都表示缓冲区越界或下溢。


.. _heap-task-tracking:

堆任务跟踪
------------------

堆任务跟踪可获取堆内存分配的各任务信息，应用程序须指定计划跟踪堆分配的堆属性。

示例代码可参考 :example:`system/heap_task_tracking`。


.. _heap-tracing:

堆内存跟踪
----------------

堆内存跟踪支持跟踪用于分配或释放内存的代码，且支持以下两种跟踪模式：

- 独立模式。此模式下，跟踪数据保存在设备上（因此收集的信息大小受指定缓冲区限制），并由设备上的代码完成分析。部分 API 可访问和转储收集的信息。
- 主机模式。此模式不受独立模式所受限制，其跟踪数据使用 app_trace 库通过 JTAG 连接发送到主机，随后使用特殊工具完成分析。

堆内存跟踪具有以下两种功能：

- 泄漏检测：检测已分配但未释放的内存。
- 堆内存使用分析：显示在跟踪运行期间所有分配或释放内存的函数。

如何判断内存泄漏
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

如果怀疑存在内存泄漏，首先要找出程序中存在泄漏的部分。调用 :cpp:func:`heap_caps_get_free_size` 或 :ref:`堆内存信息 <heap-information>` 中的其他相关函数，跟踪应用程序的内存使用情况，尝试将泄漏范围缩小到某个或一系列空闲内存始终减少而没有恢复的函数。


独立模式
^^^^^^^^^^^^^

确定存在泄漏的代码后，请执行以下步骤：

- 启用 :ref:`CONFIG_HEAP_TRACING_DEST` 选项。
- 在程序早期调用函数 :cpp:func:`heap_trace_init_standalone` 注册一个可用于记录内存跟踪的缓冲区。
- 在有内存泄漏之嫌的代码块前，调用函数 :cpp:func:`heap_trace_start` 记录系统中的所有内存分配和释放操作。
- 在可疑代码执行完毕后调用 :cpp:func:`heap_trace_stop` 函数可停止跟踪内存的分配和释放。
- 调用 :cpp:func:`heap_trace_alloc_pause` 函数暂停跟踪新的内存分配，同时继续跟踪内存释放。在可能泄漏内存的代码后立即调用该函数，防止记录任何新的内存分配。
- 调用函数 :cpp:func:`heap_trace_dump` 导出内存跟踪结果。

应用程序代码初始化、启动和停止堆内存跟踪的一般过程，见以下代码片段示例：

.. code-block:: c

  #include "esp_heap_trace.h"

  #define NUM_RECORDS 100
  static heap_trace_record_t trace_record[NUM_RECORDS]; // 该缓冲区必须在内部 RAM 中

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

堆内存跟踪会输出类似以下格式的内容：

.. only:: CONFIG_IDF_TARGET_ARCH_RISCV

  .. code-block:: none

    ====== Heap Trace: 8 records (8 capacity) ======
        3 bytes (@ 0x3fcb26f8, Internal) allocated CPU 0 ccount 0x1e7af728 freed
        6 bytes (@ 0x3fcb4ff0, Internal) allocated CPU 0 ccount 0x1e7afc38 freed
        9 bytes (@ 0x3fcb5000, Internal) allocated CPU 0 ccount 0x1e7b01d4 freed
        12 bytes (@ 0x3fcb5010, Internal) allocated CPU 0 ccount 0x1e7b0778 freed
        15 bytes (@ 0x3fcb5020, Internal) allocated CPU 0 ccount 0x1e7b0d18 freed
        18 bytes (@ 0x3fcb5034, Internal) allocated CPU 0 ccount 0x1e7b12b8 freed
        21 bytes (@ 0x3fcb504c, Internal) allocated CPU 0 ccount 0x1e7b1858 freed
        24 bytes (@ 0x3fcb5068, Internal) allocated CPU 0 ccount 0x1e7b1dfc freed
    ====== Heap Trace Summary ======
    Mode: Heap Trace All
    0 bytes alive in trace (0/8 allocations)
    records: 8 (8 capacity, 8 high water mark)
    total allocations: 8
    total frees: 8
    ================================

  或者，启用了 ``CONFIG_ESP_SYSTEM_USE_FRAME_POINTER`` 选项并且正确配置了栈深度，会输出以下内容：

.. code-block:: none

    ====== Heap Trace: 8 records (8 capacity) ======
        6 bytes (@ 0x3fc9f620, Internal) allocated CPU 0 ccount 0x1a31ac84 caller 0x40376321:0x40376379
    0x40376321: heap_caps_malloc at /path/to/idf/examples/components/heap/heap_caps.c:84
    0x40376379: heap_caps_malloc_default at /path/to/idf/examples/components/heap/heap_caps.c:110

    freed by 0x403839e4:0x42008096
    0x403839e4: free at /path/to/idf/examples/components/newlib/heap.c:40
    0x42008096: test_func_74 at /path/to/idf/examples/components/heap/test_apps/heap_tests/main/test_heap_trace.c:104 (discriminator 3)

        9 bytes (@ 0x3fc9f630, Internal) allocated CPU 0 ccount 0x1a31b618 caller 0x40376321:0x40376379
    0x40376321: heap_caps_malloc at /path/to/idf/examples/components/heap/heap_caps.c:84
    0x40376379: heap_caps_malloc_default at /path/to/idf/examples/components/heap/heap_caps.c:110

    freed by 0x403839e4:0x42008096
    0x403839e4: free at /path/to/idf/examples/components/newlib/heap.c:40
    0x42008096: test_func_74 at /path/to/idf/examples/components/heap/test_apps/heap_tests/main/test_heap_trace.c:104 (discriminator 3)

        12 bytes (@ 0x3fc9f640, Internal) allocated CPU 0 ccount 0x1a31bfac caller 0x40376321:0x40376379
    0x40376321: heap_caps_malloc at /path/to/idf/examples/components/heap/heap_caps.c:84
    0x40376379: heap_caps_malloc_default at /path/to/idf/examples/components/heap/heap_caps.c:110

    freed by 0x403839e4:0x42008096
    0x403839e4: free at /path/to/idf/examples/components/newlib/heap.c:40
    0x42008096: test_func_74 at /path/to/idf/examples/components/heap/test_apps/heap_tests/main/test_heap_trace.c:104 (discriminator 3)

        15 bytes (@ 0x3fc9f650, Internal) allocated CPU 0 ccount 0x1a31c940 caller 0x40376321:0x40376379
    0x40376321: heap_caps_malloc at /path/to/idf/examples/components/heap/heap_caps.c:84
    0x40376379: heap_caps_malloc_default at /path/to/idf/examples/components/heap/heap_caps.c:110

    freed by 0x403839e4:0x42008096
    0x403839e4: free at /path/to/idf/examples/components/newlib/heap.c:40
    0x42008096: test_func_74 at /path/to/idf/examples/components/heap/test_apps/heap_tests/main/test_heap_trace.c:104 (discriminator 3)

        18 bytes (@ 0x3fc9f664, Internal) allocated CPU 0 ccount 0x1a31d2d4 caller 0x40376321:0x40376379
    0x40376321: heap_caps_malloc at /path/to/idf/examples/components/heap/heap_caps.c:84
    0x40376379: heap_caps_malloc_default at /path/to/idf/examples/components/heap/heap_caps.c:110

    freed by 0x403839e4:0x42008096
    0x403839e4: free at /path/to/idf/examples/components/newlib/heap.c:40
    0x42008096: test_func_74 at /path/to/idf/examples/components/heap/test_apps/heap_tests/main/test_heap_trace.c:104 (discriminator 3)

        21 bytes (@ 0x3fc9f67c, Internal) allocated CPU 0 ccount 0x1a31dc68 caller 0x40376321:0x40376379
    0x40376321: heap_caps_malloc at /path/to/idf/examples/components/heap/heap_caps.c:84
    0x40376379: heap_caps_malloc_default at /path/to/idf/examples/components/heap/heap_caps.c:110

    freed by 0x403839e4:0x42008096
    0x403839e4: free at /path/to/idf/examples/components/newlib/heap.c:40
    0x42008096: test_func_74 at /path/to/idf/examples/components/heap/test_apps/heap_tests/main/test_heap_trace.c:104 (discriminator 3)

        24 bytes (@ 0x3fc9f698, Internal) allocated CPU 0 ccount 0x1a31e600 caller 0x40376321:0x40376379
    0x40376321: heap_caps_malloc at /path/to/idf/examples/components/heap/heap_caps.c:84
    0x40376379: heap_caps_malloc_default at /path/to/idf/examples/components/heap/heap_caps.c:110

    freed by 0x403839e4:0x42008096
    0x403839e4: free at /path/to/idf/examples/components/newlib/heap.c:40
    0x42008096: test_func_74 at /path/to/idf/examples/components/heap/test_apps/heap_tests/main/test_heap_trace.c:104 (discriminator 3)

        6 bytes (@ 0x3fc9f6b4, Internal) allocated CPU 0 ccount 0x1a320698 caller 0x40376321:0x40376379
    0x40376321: heap_caps_malloc at /path/to/idf/examples/components/heap/heap_caps.c:84
    0x40376379: heap_caps_malloc_default at /path/to/idf/examples/components/heap/heap_caps.c:110

    ====== Heap Trace Summary ======
    Mode: Heap Trace All
    6 bytes alive in trace (1/8 allocations)
    records: 8 (8 capacity, 8 high water mark)
    total allocations: 9
    total frees: 8
    ================================

.. note::

    以上示例输出使用 :doc:`IDF 监视器 </api-guides/tools/idf-monitor>`，自动将 PC 地址解码为其源文件和行号。

    如果记录列表溢出，将输出 ``(NB: Internal Buffer has overflowed, so trace data is incomplete.)``。如果看到此日志，请考虑缩短追踪周期或增加追踪缓冲区中的记录数量。

    如果在调用 :cpp:func:`heap_trace_dump` 或 :cpp:func:`heap_trace_dump_caps` 时有新条目被追踪，将输出 ``(NB: New entries were traced while dumping, so trace dump may have duplicate entries.)``。

在 ``HEAP_TRACE_LEAKS`` 或 ``HEAP_TRACE_ALL`` 模式下，对跟踪的每个未释放的已分配内存，打印的信息中都会包含以下内容：

.. list::

    - ``XX bytes``：分配的字节数。
    - ``@ 0x...``：从 :cpp:func:`heap_caps_malloc` 或 :cpp:func:`heap_caps_calloc` 返回的堆地址。
    - ``Internal`` 或 ``PSRAM``：分配内存的一般位置。
    - ``CPU x``：分配过程中运行的 CPU（CPU 0 或 CPU 1）。
    - ``ccount 0x...``：分配时的 CCOUNT（CPU 循环计数器）寄存器值，CPU 0 与 CPU 1 中的这一值不同。
    :CONFIG_IDF_TARGET_ARCH_XTENSA: - ``caller 0x...`` 以 PC 地址列表的形式，提供 :cpp:func:`heap_caps_malloc` 的调用栈信息。这些地址可以解码为源文件和行号，如上文所示。

在 ``HEAP_TRACE_LEAKS`` 模式下，释放内存后，相关的记录会被丢弃。

在 ``HEAP_TRACE_ALL`` 模式下：

.. list::

  :CONFIG_IDF_TARGET_ARCH_RISCV: - 释放内存后，相关记录会保留在列表中，其字段 ``freed`` 设置为 true。
  :CONFIG_IDF_TARGET_ARCH_XTENSA: - 释放内存后，相关记录的字段 ``freed by`` 会填充为 :cpp:func:`heap_caps_free` 的调用栈（以 PC 地址列表的形式）。这些地址可以解码为源文件和行号，如上所示。
  - 在达到最大记录数后，会丢弃旧记录，用新记录替换。

.. only:: CONFIG_IDF_TARGET_ARCH_XTENSA

    每个跟踪条目记录的调用栈深度可以在项目配置菜单下进行配置，选择 ``Heap Memory Debugging`` > ``Enable heap tracing`` > :ref:`CONFIG_HEAP_TRACING_STACK_DEPTH`。每个内存分配最多可以记录 32 个栈帧（默认为 2），每增加一个栈帧，每个 ``heap_trace_record_t`` 记录的内存使用量将增加 8 个字节。

.. only:: CONFIG_IDF_TARGET_ARCH_RISCV

    默认情况下，每个跟踪条目记录的调用栈深度为 0，所以只能检索到内存分配函数的直接调用者。但是，启用 ``CONFIG_ESP_SYSTEM_USE_FRAME_POINTER`` 选项后，可以在项目配置菜单下配置此调用栈深度，选择 ``Heap Memory Debugging`` > ``Enable heap tracing`` > :ref:`CONFIG_HEAP_TRACING_STACK_DEPTH`。每个内存分配最多可以记录 32 个栈帧（默认为 2），每增加一个栈帧，每个 ``heap_trace_record_t`` 记录的内存使用量将增加 8 个字节。

最后，将打印“泄漏”的总字节数（即在跟踪期间分配但未释放的总字节数），以及它所代表的总分配次数。

使用哈希表提高性能
++++++++++++++++++++

默认情况下，堆追踪使用一个静态分配的双向链表来存储追踪记录。这种方式的缺点是，当链表中的记录条目数量增加时，查找特定记录的耗时也会随之增加，从而导致运行性能下降。因此，在需要存储大量记录时，双向链表的使用效率很低（甚至可能导致功能无法使用，因为从列表中检索条目所需的时间会阻碍应用程序的正常运行）。

为了解决这个问题，可以前往 ``Component config`` > ``Heap Memory Debugging`` 配置菜单 > 启用 :ref:`CONFIG_HEAP_TRACE_HASH_MAP` 选项，使用哈希表机制来存储记录。这样就可以在不严重影响性能的情况下追踪大量记录。

每个哈希表条目是一个单向链表，用于存储具有相同哈希 ID 的记录。

每条记录的哈希 ID 是基于它们追踪的内存指针计算的。使用的哈希函数基于修改后的 Fowler-Noll-Vo 哈希函数，确保了所有记录在范围 [0, 哈希表大小) 内均匀分布。其中哈希表大小可以前往项目配置菜单 ``Component config`` > ``Heap Memory Debugging`` > 设置 :ref:`CONFIG_HEAP_TRACE_HASH_MAP_SIZE` 来定义。

.. note::

  .. list::

    - 选项 :ref:`CONFIG_HEAP_TRACE_HASH_MAP_SIZE` 定义了哈希表中的条目数量。记录的总数量仍由用户在调用 :cpp:func:`heap_trace_init_standalone` 时定义。如果最大记录数为 ``N``，而哈希表的条目数为 ``H``，那么每个条目最多可包含 ``N / H`` 条记录。
    - 哈希表是对双向链表的补充，而无法替代双向链表。因为使用哈希表可能会导致显著的内存开销。
    :SOC_SPIRAM_SUPPORTED: - 存储哈希表所用的内存是动态分配的（默认分配在内部内存中），但用户可以通过前往 ``Component config`` > ``Heap Memory Debugging`` > 设置:ref:`CONFIG_HEAP_TRACE_HASH_MAP_IN_EXT_RAM` 选项，将哈希表强制存储在外部内存中（此选项仅在启用了 :ref:`CONFIG_SPIRAM` 的条件下可用）。

主机模式
^^^^^^^^^^^^^^^

确定存在泄漏的代码后，请执行以下步骤：

- 在项目配置菜单中，前往 ``Component config`` > ``Heap Memory Debugging`` > :ref:`CONFIG_HEAP_TRACING_DEST` 并选择 ``Host-Based``。
- 在项目配置菜单中，前往 ``Component config`` > ``Application Level Tracing`` > :ref:`CONFIG_APPTRACE_DESTINATION1` 并选择 ``Trace memory``。
- 在项目配置菜单中，前往 ``Component config`` > ``Application Level Tracing`` > ``FreeRTOS SystemView Tracing`` 并启用 :ref:`CONFIG_APPTRACE_SV_ENABLE`。
- 在程序早期，调用函数 :cpp:func:`heap_trace_init_tohost`，初始化 JTAG 堆内存跟踪模块。
- 在有内存泄漏之嫌的代码块前，调用函数 :cpp:func:`heap_trace_start` 开始记录系统中的内存分配和释放操作。

  主机模式忽略该函数参数，堆内存跟踪模块以 ``HEAP_TRACE_ALL`` 传递后的方式运行，即所有的内存分配和释放都发送到主机。

- 在有内存泄露之嫌的代码块后，调用函数 :cpp:func:`heap_trace_stop` 停止跟踪。

应用程序代码初始化、启动和停止基于主机模式堆内存跟踪的一般过程，请参阅以下代码片段示例：

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

要收集并分析堆内存跟踪结果，请在主机上完成以下操作：

1.  构建程序并将其下载到目标设备，详情请参阅 :ref:`第五步：开始使用 ESP-IDF 吧 <get-started-build>`。

2.  运行 OpenOCD（请参阅 :doc:`JTAG 调试 </api-guides/jtag-debugging/index>`）。

.. note::

    使用此功能需要 ``v0.10.0-esp32-20181105`` 或更高版本的 OpenOCD。

3. 使用 GDB 可以自动启动和/或停止跟踪，为此应准备特殊的 ``gdbinit`` 文件：

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

使用此文件，GDB 将连接到目标设备、重置该设备，在程序触发 :cpp:func:`heap_trace_start` 断点时开始跟踪，在程序触发 :cpp:func:`heap_trace_stop` 断点时停止跟踪。跟踪数据将保存至 ``/tmp/heap_log.svdat``。

4. 使用命令 ``{IDF_TARGET_TOOLCHAIN_PREFIX}-gdb -x gdbinit </path/to/program/elf>`` 运行 GDB

5. 调用 :cpp:func:`heap_trace_stop` 函数使程序停止运行时，退出 GDB，跟踪数据将保存至 ``/tmp/heap.svdat``

6. 运行处理脚本 ``$IDF_PATH/tools/esp_app_trace/sysviewtrace_proc.py -p -b </path/to/program/elf> /tmp/heap_log.svdat``

堆内存跟踪堆输出将类似以下格式的内容:

.. code-block::

  Parse trace from '/tmp/heap.svdat'...
  Stop parsing trace. (Timeout 0.000000 sec while reading 1 byte!)
  Process events from '['/tmp/heap.svdat']'...
  [0.002244575] HEAP: Allocated 1 byte @ 0x3ffaffd8 from task "alloc" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:47
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.002258425] HEAP: Allocated 2 bytes @ 0x3ffaffe0 from task "alloc" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:48
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.002563725] HEAP: Freed bytes @ 0x3ffaffe0 from the task "free" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:31 (discriminator 9)
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.002782950] HEAP: Freed bytes @ 0x3ffb40b8 from the task "main" on core 0 by:
  /home/user/projects/esp/esp-idf/components/freertos/tasks.c:4590
  /home/user/projects/esp/esp-idf/components/freertos/tasks.c:4590

  [0.002798700] HEAP: Freed bytes @ 0x3ffb50bc from the task "main" on core 0 by:
  /home/user/projects/esp/esp-idf/components/freertos/tasks.c:4590
  /home/user/projects/esp/esp-idf/components/freertos/tasks.c:4590

  [0.102436025] HEAP: Allocated 2 bytes @ 0x3ffaffe0 from the task "alloc" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:47
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.102449800] HEAP: Allocated 4 bytes @ 0x3ffaffe8 from the task "alloc" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:48
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.102666150] HEAP: Freed bytes @ 0x3ffaffe8 from the task "free" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:31 (discriminator 9)
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.202436200] HEAP: Allocated 3 bytes @ 0x3ffaffe8 from the task "alloc" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:47
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.202451725] HEAP: Allocated 6 bytes @ 0x3ffafff0 from the task "alloc" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:48
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.202667075] HEAP: Freed bytes @ 0x3ffafff0 from the task "free" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:31 (discriminator 9)
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.302436000] HEAP: Allocated 4 bytes @ 0x3ffafff0 from the task "alloc" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:47
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.302451475] HEAP: Allocated 8 bytes @ 0x3ffb40b8 from the task "alloc" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:48
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.302667500] HEAP: Freed bytes @ 0x3ffb40b8 from the task "free" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:31 (discriminator 9)
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  Processing completed.

  Processed 1019 events

  =============== HEAP TRACE REPORT ===============

  Processed 14 heap events.

  [0.002244575] HEAP: Allocated 1 bytes @ 0x3ffaffd8 from the task "alloc" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:47
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.102436025] HEAP: Allocated 2 bytes @ 0x3ffaffe0 from the task "alloc" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:47
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.202436200] HEAP: Allocated 3 bytes @ 0x3ffaffe8 from the task "alloc" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:47
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  [0.302436000] HEAP: Allocated 4 bytes @ 0x3ffafff0 from the task "alloc" on core 0 by:
  /home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:47
  /home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

  Found 10 leaked bytes in 4 blocks.

堆内存跟踪定位堆内存损坏
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

堆内存跟踪也是一种定位堆内存损坏位置的方法。当堆中的某个区域损坏时，可能是因为程序中的其他部分在相邻地址分配内存。

如果大致了解堆内存损坏发生的时间范围，启用 ``HEAP_TRACE_ALL`` 模式的堆内存跟踪，可以记录分配内存的所有函数及其相应地址。

以此方法使用堆内存跟踪与上文描述的内存泄漏检测类似，对已分配但未释放的内存输出结果相同，但还会显示已释放内存的记录。

性能影响
^^^^^^^^^^^^^^^^^^

在 menuconfig 中启用堆内存跟踪，会增加程序代码的大小，即便未运行堆内存跟踪，也会对堆内存分配或释放操作的性能产生较小的负面影响。

运行堆内存跟踪时，堆内存分配或释放操作的速度明显变慢。增加为各内存分配的栈帧深度（见上文）也会造成这种性能影响。

为减轻堆内存跟踪运行时的性能损失，请启用 :ref:`CONFIG_HEAP_TRACE_HASH_MAP`。此时，将使用哈希映射机制处理堆内存跟踪记录，减少堆内存分配或释放操作的执行时长。设置 :ref:`CONFIG_HEAP_TRACE_HASH_MAP_SIZE` 的值可以调整哈希映射的大小。

.. only:: SOC_SPIRAM_SUPPORTED

  默认情况下，哈希映射会放置在内部 RAM 中，启用 :ref:`CONFIG_HEAP_TRACE_HASH_MAP_IN_EXT_RAM` 时也可将其放置在外部 RAM 中。要启用此配置，请确保已启用 :ref:`CONFIG_SPIRAM` 和 :ref:`CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY`。

内存泄漏误报
^^^^^^^^^^^^^^^^^^^^^^^^^^^

并非所有由 :cpp:func:`heap_trace_dump` 打印的信息都是内存泄漏，以下情况也可能打印信息：

- 在调用 :cpp:func:`heap_trace_start` 后分配且在调用 :cpp:func:`heap_trace_stop` 后才释放的内存都会出现在泄漏信息中。
- 系统中的其他任务也可能进行内存分配。根据这些任务的时间安排，报错的这部分内存很可能在调用 :cpp:func:`heap_trace_stop` 后释放。
- 当任务第一次使用 stdio，如调用 :cpp:func:`heap_caps_printf` 时，libc 会分配一个锁，即 RTOS 互斥信号量，该分配将持续至任务删除。
- 进行打印浮点数等调用 :cpp:func:`heap_caps_printf` 的操作时，会根据需要，从堆中分配一些内存，这些分配将持续至任务删除。
- 蓝牙、Wi-Fi 和 TCP/IP 库会分配堆内存缓冲区，处理传入或传出的数据，这些内存缓冲区通常持续时间较短。但如果在运行堆内存泄漏跟踪期间，网络底层接收或发送了数据，一些缓冲区可能会出现在堆内存泄漏跟踪输出中。
- 由于存在 ``TIME_WAIT`` 状态，TCP 连接在关闭后仍会使用一些内存，``TIME_WAIT`` 状态结束后将释放这些内存。

要区分“真实”和“误报”的内存泄漏，可以在堆内存跟踪运行时多次调用可疑代码，并在堆内存跟踪输出中查找重复出现的内存分配情况。

应用示例
--------------------

- :example:`system/heap_task_tracking/basic` 演示了堆任务跟踪的概览功能，用于导出每个任务在堆内存使用方面的统计信息概要。
- :example:`system/heap_task_tracking/advanced` 演示了堆任务跟踪中统计信息函数的用法，用于访问每个任务在堆内存使用方面的完整统计数据。

API 参考 - 堆内存跟踪
----------------------------

.. include-build-file:: inc/esp_heap_trace.inc
