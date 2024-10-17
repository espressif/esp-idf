C++ 支持
===========

:link_to_translation:`en:[English]`

.. highlight:: cpp

ESP-IDF 主要使用 C 语言编写，并提供 C 语言 API。但 ESP-IDF 也支持使用 C++ 开发应用程序，与 C++ 开发相关的各种主题在本文档中列出。

ESP-IDF 支持以下 C++ 功能：

- :ref:`cplusplus_exceptions`
- :ref:`cplusplus_multithreading`
- :ref:`cplusplus_rtti`
- :doc:`thread-local-storage` （``thread_local`` 关键字）
- 除部分 :ref:`cplusplus_limitations`，所有由 GCC 实现的 C++ 功能均受支持。有关由 GCC 所实现功能的详细信息，请参阅 `GCC 文档 <https://gcc.gnu.org/projects/cxx-status.html>`_。


``esp-idf-cxx`` 组件
-------------------------

`esp-idf-cxx <https://github.com/espressif/esp-idf-cxx>`_ 组件为一些 ESP-IDF 中的功能提供了更高级别的 C++ API，该组件可以从 `乐鑫组件注册表 <https://components.espressif.com/components/espressif/esp-idf-cxx>`_ 中获取。


C++ 语言标准
---------------------

默认情况下，ESP-IDF 使用 C++23 语言标准和 GNU 扩展 (``-std=gnu++23``) 编译 C++ 代码。

要使用其他语言标准编译特定组件的源代码，请按以下步骤，在组件的 CMakeLists.txt 文件中设置所需的编译器标志：

.. code-block:: cmake

    idf_component_register( ... )
    target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++11)

如果组件的公共头文件也需要以该语言标准编译，请使用 ``PUBLIC`` 而非 ``PRIVATE``。


.. _cplusplus_multithreading:

多线程
--------------

支持 C++ 线程，互斥锁和条件变量。C++ 线程基于 pthread 构建，而 pthread 封装了 FreeRTOS 任务。

有关在 C++ 中创建线程的示例，请参阅 :example:`cxx/pthread`。该示例演示了如何使用 ESP-pthread 组件修改 C++ 线程的堆栈大小、优先级、名称和内核亲和性。

.. note::

    `std::jthread <https://en.cppreference.com/w/cpp/thread/jthread>`_ 的析构函数只能从 :ref:`posix_thread_api` 或 `C++ 线程库 API <https://en.cppreference.com/w/cpp/thread>`_ 创建的任务中安全地调用。


.. _cplusplus_exceptions:

异常处理
------------------

ESP-IDF 默认禁用对 C++ 异常处理的支持，可以用 :ref:`CONFIG_COMPILER_CXX_EXCEPTIONS` 选项启用该支持。

如果抛出了异常处理，却没有相应的 ``catch`` 块，程序将由 ``abort`` 函数终止，并打印回溯信息。有关回溯信息的更多信息，请参见 :doc:`fatal-errors`。

C++ 异常处理应 **仅** 应用于异常情况，即意外情况及罕见情况，如发生频率低于 1% 的事件。**请勿** 将 C++ 异常处理用于流程控制，详情请参阅下文的资源使用部分。有关使用 C++ 异常处理的更多详情，请参阅 `ISO C++ FAQ <https://isocpp.org/wiki/faq/exceptions>`_ 和 `CPP 核心指南 <https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-errors>`_。

有关 C++ 异常处理的示例，请参阅 :example:`cxx/exceptions`。该示例演示了如何在 {IDF_TARGET_NAME} 中启用和使用 C++ 异常，示例中声明了一个类，当提供的参数等于 0 时，这个类会在构造函数中抛出异常。

C++ 异常处理及所需资源
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

启用异常处理后，应用程序的二进制文件通常会增加几个 KB。

此外，可能需要为异常处理应急内存池保留一部分 RAM。如果无法从堆内存中分配异常处理对象，则会使用该池中的内存。

使用 :ref:`CONFIG_COMPILER_CXX_EXCEPTIONS_EMG_POOL_SIZE` 变量可以设置异常处理应急内存池的内存量。

当且仅当 C++ 异常抛出时，会使用额外的栈内存（约 200 字节），从而从栈内存顶部调用函数，启动异常处理。

使用 C++ 异常处理的代码的运行时间取决于运行时实际发生的情况。

- 如果没有抛出异常，则异常处理的代码运行速度会更快，因为无需检查错误代码。
- 如果抛出异常，异常处理代码的运行时间会比返回错误代码的代码长几个数量级。

如果抛出异常，解开栈代码的速度要比返回错误代码慢好几个数量级。所增加的运行时长取决于应用程序的要求和错误处理的实现方式（例如，是否需要用户输入或发送消息到云端）。因此，在实时关键的代码路径中，不应使用会抛出异常的代码。


.. _cplusplus_rtti:

运行时类型信息 (RTTI)
-------------------------------

ESP-IDF 默认禁用对 RTTI 的支持，可以用 :ref:`CONFIG_COMPILER_CXX_RTTI` 选项启用该支持。

启用此选项，将以启用了 RTTI 支持的方式编译所有的 C++ 文件，并支持使用 ``dynamic_cast`` 转换和 ``typeid`` 运算符。启用此选项通常会增加几十 KB 的二进制文件大小。

有关在 ESP-IDF 中使用 RTTI 的示例，请参阅 :example:`cxx/rtti`。该示例演示了如何在 ESP-IDF 中使用 RTTI 功能，启用编译时对 RTTI 的支持，并展示了如何打印对象和函数的去混淆类型名称，以及 dynamic_cast 在两个继承自同一基类的对象上如何表现。

在 C++ 中进行开发
-----------------

以下部分提供了在 C++ 中开发 ESP-IDF 应用程序的一些技巧。


组合 C 和 C++ 代码
^^^^^^^^^^^^^^^^^^^^^^^^

当应用程序的不同部分使用 C 和 C++ 开发时，理解 `语言链接性 <https://en.cppreference.com/w/cpp/language/language_linkage>`_ 的概念非常重要。

为了能够从 C 代码中调用 C++ 函数，该 C++ 函数必须使用 C 链接 (``extern "C"``) 进行 **声明** 和 **定义**：

.. code-block:: cpp

    // 在 .h 文件中声明：
    #ifdef __cplusplus
    extern "C" {
    #endif

    void my_cpp_func(void);

    #ifdef __cplusplus
    }
    #endif

    // 在 .cpp 文件中进行定义：
    extern "C" void my_cpp_func(void) {
        // ...
    }


为了能够从 C++ 中调用 C 函数，该 C 函数必须使用 C 链接 **声明**：

.. code-block:: C

    // 在 .h 文件中声明：
    #ifdef __cplusplus
    extern "C" {
    #endif

    void my_c_func(void);

    #ifdef __cplusplus
    }
    #endif

    // 在 .c 文件中进行定义：
    void my_c_func(void) {
        // ...
    }


在 C++ 中定义 ``app_main``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ESP-IDF 希望应用程序入口点 ``app_main`` 以 C 链接定义。当 ``app_main`` 在 .cpp 源文件中定义时，必须以 ``extern "C"`` 标识：

.. code-block:: cpp

    extern "C" void app_main()
    {
    }


.. _cplusplus_designated_initializers:

指定初始化器
^^^^^^^^^^^^^^^^^^^^^^^

许多 ESP-IDF 组件会以 :ref:`api_reference_config_structures` 作为初始化函数的参数。用 C 编写的 ESP-IDF 示例通常使用 `指定初始化器 <https://en.cppreference.com/w/c/language/struct_initialization>`_，以可读且可维护的方式填充有关结构体。

C 和 C++ 语言对于指定初始化器有不同的规则。例如，C++23（当前在 ESP-IDF 中默认使用）不支持无序指定初始化、嵌套指定初始化、混合使用指定初始化器和常规初始化器，而对数组进行指定初始化。因此，当将 ESP-IDF 的 C 示例移植到 C++ 时，可能需要对结构体初始化器进行一些更改。详细信息请参阅 `C++ aggregate initialization reference <https://en.cppreference.com/w/cpp/language/aggregate_initialization>`_。


``iostream``
^^^^^^^^^^^^

ESP-IDF 支持 ``iostream`` 功能，但应注意：

1. ESP-IDF 在构建过程中通常会删除未使用的代码。然而，在使用 iostreams 的情况下，仅在其中一个源文件包含 ``<iostream>`` 头文件就会使二进制文件增加大约 200 kB。
2. ESP-IDF 默认使用简单的非阻塞机制来处理标准输入流 (``stdin``)。要获得 ``std::cin`` 的常规行为，应用程序必须初始化 UART 驱动程序，并启用阻塞模式，详情请参阅 :example_file:`common_components/protocol_examples_common/stdin_out.c`。


.. _cplusplus_limitations:

限制
-----------

- 链接脚本生成器不支持将具有 C++ 链接的函数单独放置在内存的特定位置。
- 当与模板函数一起使用时，会忽略各种节属性（例如 ``IRAM_ATTR``）。
- vtable 位于 flash 中，在禁用 flash 缓存时无法访问。因此，在 :ref:`iram-safe-interrupt-handlers` 中应避免调用虚拟函数。目前尚无法使用链接器脚本生成器调整 vtable 的放置位置。
- 不支持 C++ 文件系统 (``std::filesystem``) 功能。


注意事项
-------------

请勿在 C++ 中使用 ``setjmp``/``longjmp``。``longjmp`` 会在不调用任何析构函数的情况下盲目跳出堆栈，容易引起未定义的行为和内存泄漏。请改用 C++ 异常处理，这类程序可以确保正确调用析构函数。如果无法使用 C++ 异常处理，请使用其他替代方案（ ``setjmp``/``longjmp`` 除外），如简单的返回码。

应用示例
--------------------

- :example:`cxx/pthread`

- :example:`cxx/exceptions` 演示了如何在 {IDF_TARGET_NAME} 中启用和使用 C++ 异常。该示例声明了一个类，当提供的参数等于 0 时，这个类会在构造函数中抛出异常。

- :example:`cxx/rtti`
