C 支持
======

:link_to_translation:`en:[English]`

ESP-IDF 主要使用 C 语言编写，并提供 C API。ESP-IDF 可以使用以下 C 标准函数库的实现之一：

- `Newlib <https://sourceware.org/newlib/>`_ (默认)
- `Picolibc <https://keithp.com/picolibc/>`_ (通过 :ref:`CONFIG_LIBC_PICOLIBC<CONFIG_LIBC_PICOLIBC>` Kconfig 选项启用)

Newlib 的版本号记录在 :component_file:`newlib/sbom.yml` 文件中。

一般来说，除非在 :ref:`unsupported_c_features` 特别说明，当前编译器（目前是 GCC）支持的所有 C 语言功能在 ESP-IDF 中均可使用。

.. _c_version:

C 版本
------

**ISO C17 的 GNU 方言** (``--std=gnu17``) 是 ESP-IDF 中当前默认的 C 语言版本。

使用不同的语言标准编译某个组件的源代码，请在组件的 ``CMakeLists.txt`` 文件中设置所需的编译器标志：

.. code-block:: cmake

    idf_component_register( ... )
    target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu11)

如果组件的公共头文件也需要使用相同的语言标准编译，请将标志 ``PRIVATE`` 替换为 ``PUBLIC``。

.. _unsupported_c_features:

不支持的 C 功能
---------------

ESP-IDF 不支持以下功能。

嵌套函数的指针
^^^^^^^^^^^^^^

**ISO C17 的 GNU 方言** 支持 `嵌套函数 <https://gcc.gnu.org/onlinedocs/gcc/Nested-Functions.html>`_，但 ESP-IDF 不支持将嵌套函数引用为指针。这是因为 GCC 编译器在引用嵌套函数的指针时，会在栈上生成一个 `跳板 <https://gcc.gnu.org/onlinedocs/gccint/Trampolines.html>`_ （即，一小段可执行代码），ESP-IDF 不允许从栈执行代码，因此不支持使用指向嵌套函数的指针。
