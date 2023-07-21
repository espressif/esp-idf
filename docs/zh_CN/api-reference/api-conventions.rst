API 约定
===============
:link_to_translation:`en:[English]`

.. highlight:: c

本文介绍了 ESP-IDF 应用程序编程接口 (API) 中常见的约定和假设。

ESP-IDF 提供了几种编程接口：

* 在 ESP-IDF 组件的公共头文件中声明的 C 函数、结构体、枚举、类型定义和预处理器宏。编程指南的 API 参考部分描述了这些函数、结构体和类型。
* 编译系统函数、预定义变量和选项，详情请参阅 :ref:`ESP-IDF CMake 构建系统 API <cmake_buildsystem_api>`。
* :doc:`Kconfig <kconfig>` 选项，可用于代码及编译系统文件 (``CMakeLists.txt``)。
* :doc:`主机工具 <../api-guides/tools/index>` 及其命令行参数。

ESP-IDF 由多个组件组成，组件中包含专门为 ESP 芯片编写的代码或第三方库（即第三方组件）。对于某些第三方库，ESP-IDF 提供专用的包装器和接口，以简化对第三方库的使用，或提高其与 ESP-IDF 其他功能的兼容性。某些情况下，第三方组件将直接呈现底层库的原始 API。

以下各节介绍了部分 ESP-IDF API 及其使用的相关内容。

错误处理
--------------

多数 ESP-IDF API 会返回由 :cpp:type:`esp_err_t` 类型定义的错误代码。有关出错处理的更多信息，请参阅 :doc:`错误处理 <../api-guides/error-handling>` 部分。有关 ESP-IDF 组件返回的错误代码列表，请参阅 :doc:`错误代码参考 <error-codes>`。

.. _api_reference_config_structures:

配置结构体
------------------------

.. important:: 为确保应用程序与未来 ESP-IDF 版本的兼容性，请正确初始化配置结构体。

多数 ESP-IDF 中的初始化、配置和安装函数（通常以 ``..._init()``、 ``..._config()`` 和 ``..._install()`` 命名）都需要一个指向配置结构体的指针作为参数。例如::

    const esp_timer_create_args_t my_timer_args = {
        .callback = &my_timer_callback,
        .arg = callback_arg,
        .name = "my_timer"
    };
    esp_timer_handle_t my_timer;
    esp_err_t err = esp_timer_create(&my_timer_args, &my_timer);

初始化函数不会存储指向配置结构体的指针，因此在栈上分配结构体是安全的。

应用程序必须初始化结构体的所有字段，以下为错误示例::

    esp_timer_create_args_t my_timer_args;
    my_timer_args.callback = &my_timer_callback;
    /* 错误! 字段 .arg 和 .name 未初始化 */
    esp_timer_create(&my_timer_args, &my_timer);

大多数 ESP-IDF 示例使用 C99 的 `指定初始化器`_ 来完成结构体初始化，从而以简洁的方式设置子集字段，并将剩余字段初始化为零::

    const esp_timer_create_args_t my_timer_args = {
        .callback = &my_timer_callback,
        /* 正确，字段 .arg 和 .name 已初始化为零 */
    };

C++ 语言同样支持指定初始化器语法，但初始化器必须遵循声明顺序。在 C++ 代码中使用 ESP-IDF API 时，可以考虑使用以下模式::

    /* 正确：.dispatch_method、.name 以及 .skip_unhandled_events 初始化为零 */
    const esp_timer_create_args_t my_timer_args = {
        .callback = &my_timer_callback,
        .arg = &my_arg,
    };

    ///* 错误：esp_timer_create_args_t 中，.arg 在 .callback 之后声明 */
    //const esp_timer_create_args_t my_timer_args = {
    //    .arg = &my_arg,
    //    .callback = &my_timer_callback,
    //};

了解指定初始化器的更多信息，请参见 :ref:`cplusplus_designated_initializers`。注意，C++20 之前的 C++ 语言不是当前 ESP-IDF 的默认版本，不支持指定初始化器。如需使用 C++20 之前的 C++ 标准编译代码，可以借助 GCC 扩展生成以下模式::

    esp_timer_create_args_t my_timer_args = {};
    /* 所有字段初始化为零 */
    my_timer_args.callback = &my_timer_callback;

默认初始化器
^^^^^^^^^^^^^^^^^^^^

ESP-IDF 为某些配置结构体提供了用于设置字段默认值的宏::

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    /* HTTPD_DEFAULT_CONFIG 扩展到一个指定的初始化器。此时，所有字段均已设置为默认值，且支持编辑：*/
    config.server_port = 8081;
    httpd_handle_t server;
    esp_err_t err = httpd_start(&server, &config);

当特定配置结构体提供了默认初始化器宏时，推荐使用该默认初始化器宏。

.. _api_reference_private_apis:

私有 API
------------

在 ESP-IDF 中，某些头文件包含的 API 仅限于在 ESP-IDF 源代码中使用，不支持在应用程序中使用。此类头文件的名称或路径通常带有 ``private`` 或 ``esp_private``。某些组件（如 :doc:`hal <../api-guides/hardware-abstraction>`）则仅包含私有 API。

私有 API 可能在次要或补丁版本之间以不兼容的方式被删除或更改。

.. _api_reference_example_components:

示例项目组件
------------------------------

ESP-IDF 示例中提供了一系列演示 ESP-IDF API 使用方式的工程。为避免在各个示例中重复引用相同的代码片段，示例的常用组件中定义了一些通用辅助工具。这些常用组件包括 :example:`common_components` 目录下的组件和示例本身的部分组件，它们不属于 ESP-IDF API 的范畴。

不建议在自定义项目中通过 ``EXTRA_COMPONENT_DIRS`` 编译系统变量直接引用这些组件，因为在不同的 ESP-IDF 版本中，组件可能存在显著变化。基于 ESP-IDF 示例开始新项目时，需将项目及其所依赖的公共组件从 ESP-IDF 中复制出来，并将公共组件视为项目的一部分。请注意，公共组件是针对示例编写的，可能不包括生产应用程序所需的所有出错处理。在使用前，需阅读代码并判断它是否适用于所需用例。

API 稳定性
-------------

ESP-IDF 使用 `语义版本管理办法 <https://semver.org/>`_ ，详情请参阅 :ref:`版本管理 <versioning-scheme>`。

ESP-IDF 的次要版本和错误修复版本会保证与过往版本的兼容性。以下各节解释了兼容性的不同方面和限制。

源代码级别兼容性
^^^^^^^^^^^^^^^^^^^^^^^^^^

ESP-IDF 确保在 ESP-IDF 组件的公共头文件中声明的 C 函数、结构体、枚举、类型定义和预处理宏的源代码级别兼容性。源代码级别兼容性意味着应用程序无需修改即可在新版本的 ESP-IDF 上重新编译。

以下在次要版本之间的更改不会破坏源代码级别兼容性：

* 使用 ``deprecated`` 属性废弃函数、使用预处理器 ``#warning`` 废弃头文件。废弃功能已在 ESP-IDF 发布说明中列出。建议更新源代码以使用替换被废弃的函数或文件的新函数或文件。ESP-IDF 的主要版本将移除废弃的函数和文件。
* 重命名组件，在组件间移动源代码和头文件，但需确保编译系统仍可以找到正确的文件。
* 重命名 Kconfig 选项。Kconfig 系统的 :ref:`向后兼容性 <configuration-options-compatibility>` 确保应用程序在 ``sdkconfig`` 文件、CMake 文件和源代码中仍然可以使用原始的 Kconfig 选项名称。

缺少二进制兼容性
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ESP-IDF 无法确保版本间的二进制兼容性。这意味着，如果使用某个 ESP-IDF 版本构建了一个预编译库，在下一个次要或错误修复版本中，无法确保该库将以相同方式运行。以下更改可以保持源代码级别兼容性，但不保证二进制兼容性：

* 更改 C 枚举成员的数值。
* 添加新的结构体成员或更改成员顺序。关于有助于确保兼容性的提示，请参阅 :ref:`api_reference_config_structures`。
* 用具有相同签名的  ``static inline`` 函数替换 ``extern`` 函数，反之亦然。
* 用兼容的 C 函数替换类似于函数的宏。

其他不兼容情况
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

尽管我们致力于优化 ESP-IDF 版本升级，但是在次要版本之间，ESP-IDF 的某些部分可能会不兼容。如有不属于下列情况的意外重大更新，欢迎向我们发送报告：

* :ref:`api_reference_private_apis`。
* :ref:`api_reference_example_components`。
* 明确标记为 "beta"、"preview" 或 "experimental" 的功能。
* 为缓解安全问题做出的更改，或以更安全的行为取代不安全的默认行为的更改。
* 从未运行成功的功能。例如，如果某个函数或枚举值从未成功使用，则可能会以修复的形式将其重命名或删除。这包括依赖于非功能芯片硬件功能的软件功能。
* 未明确记录的意外或未定义行为可能会被修复或更改，如缺少参数范围验证。
* 在菜单配置中 :doc:`Kconfig <kconfig>` 选项的位置。
* 示例项目的位置和名称。

.. _指定初始化器: https://en.cppreference.com/w/c/language/struct_initialization
