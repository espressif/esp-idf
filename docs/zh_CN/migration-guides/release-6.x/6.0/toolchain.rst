工具链
==========

:link_to_translation:`en:[English]`

GCC 版本更新说明
-------------------

原先使用的 GCC 版本 14.2.0 现已统一升级为 15.1.0，适用于所有芯片系列。升级至 ESP-IDF v6.0 时，需将代码适配至 GCC 15.1.0。请参阅 `Porting to GCC 15 <https://gcc.gnu.org/gcc-15/porting_to.html>`_。

警告信息
---------

升级到 GCC 15.1.0 后，引入了新的编译警告，同时也增强了已有警告的检测能力。警告详情可参考 `GCC Warning Options <https://gcc.gnu.org/onlinedocs/gcc-15.1.0/gcc/Warning-Options.html>`_。建议认真检查代码并尽可能修复警告。根据具体警告的类型及用户代码的复杂程度不同，某些警告可能属于误报，消除这些误报也需进行较为复杂的代码修改。此时，可通过多种方式屏蔽警告。下文列出了常见的警告及其对应的解决方案。

如需全局屏蔽所有新增警告，请启用配置项 :ref:`CONFIG_COMPILER_DISABLE_GCC15_WARNINGS`。

``-Wno-unterminated-string-initialization``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

当使用字符串字面量来初始化字符数组时，如果该数组的大小不足以容纳字符串末尾的空字符，编译器就会发出这个警告。声明 nonstring 属性时除外。

.. code-block:: c

    #include "esp_attr.h"

    char arr[3] = "foo"; /* 触发警告  */
    NONSTRING_ATTR char arr2[3] = "bar"; /* 无警告  */

``-Wno-header-guard``
^^^^^^^^^^^^^^^^^^^^^^^

头文件中的宏定义和引用防护 (include guard) 不一致时触发警告（即 #ifndef 与 #define 使用的宏名称不一致时）。

.. code-block:: c

    #ifndef WHEADER_GUARD_2
    #define WHEADERGUARD2 /* 触发警告。此处需改为 WHEADER_GUARD_2  */
    /* ... */
    #endif

``-Wno-self-move（仅限 C++）``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

当一个值通过 `std::move` 转移到它自身时触发警告（此 `std::move` 操作通常没有意义）。

.. code-block:: cpp

    struct T {
      /* ... */
    };
    void fn()
    {
      T t;
      /* ... */
      t = std::move (t); /* 触发警告。可删除该行代码  */
    }


``-Wno-template-body（仅限 C++）``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

在模板解析阶段禁用错误诊断，仅在模板实例化时报告错误。

.. code-block:: cpp

    template<class T>
    void f() {
      const int n = 42;
      ++n; /* 只读变量 'n' */
    }

``-Wno-dangling-reference（仅限 C++）``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

当引用绑定到生命周期已结束的临时对象时触发警告，防止悬垂引用。

.. code-block:: cpp

    int n = 1;
    const int& r = std::max(n - 1, n + 1); /* r 成为悬垂引用  */

``-Wno-defaulted-function-deleted（仅限 C++）``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

当显式默认函数被编译器隐式删除时触发警告（通常因函数声明类型与隐式声明不匹配导致）。

.. code-block:: cpp

    template<typename>
    struct C {
       C();
       C(const C&&) = default; /* 被隐式删除  */
    };

``sys/dirent.h`` 不再包含一些函数原型
-------------------------------------------------------

问题
^^^^^^

使用旧工具链的代码可能会出现编译错误。例如：

.. code-block:: c

    #include <sys/dirent.h>
    /* .... */
    DIR* dir = opendir("test_dir");
    /* .... */
    /**
     * Compile error:
     * test.c: In function 'test_opendir':
     * test.c:100:16: error: implicit declaration of function 'opendir' [-Werror=implicit-function-declaration]
     *   100 |     DIR* dir = opendir(path);
     *       |                ^~~~~~~
     */

解决方法
^^^^^^^^^

包含正确的头文件即可解决此问题。请将代码重构如下：

.. code-block:: c

    #include <dirent.h>
    /* .... */
    DIR* dir = opendir("test_dir");

Picolibc
--------

启用 :ref:`CONFIG_LIBC_PICOLIBC<CONFIG_LIBC_PICOLIBC>` 配置后需注意以下变更：

``移除 sys/signal.h 头文件``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Picolibc 已移除 ``<sys/signal.h>`` 头文件。为确保跨 libc 实现的兼容性，请改用标准 C 头文件 ``<signal.h>``。

.. code-block:: c

    #include <sys/signal.h> /* 严重错误：sys/signal.h：没有此文件或目录 */
    #include <signal.h>     /* 正确：标准且可移植的写法 */

.. only:: CONFIG_ESP_ROM_HAS_SUBOPTIMAL_NEWLIB_ON_MISALIGNED_MEMORY

    RISC-V 芯片与 LibC 函数中的非对齐内存访问
    -----------------------------------------

    乐鑫的 RISC-V 芯片在执行非对齐内存访问时，相比对齐访问仅有较小的性能损耗。

    之前，当传入的指针不是按字对齐时，LibC 中涉及内存操作的函数（如拷贝或比较函数）会采用逐字节操作实现。现在，这些函数会尽可能采用字（4 字节）加载/存储操作，从而实现性能大幅提升。这些优化的实现通过 :ref:`CONFIG_LIBC_OPTIMIZED_MISALIGNED_ACCESS` 默认启用，但会减少应用大约 800–1000 字节的内存预算 (IRAM)。

    下表展示了在 ESP32-C3 芯片上使用 4096 字节的 buffer 进行基准测试的结果：

    .. list-table:: 基准测试结果
       :header-rows: 1
       :widths: 20 20 20 20

       * - 函数
         - 旧版（CPU 周期）
         - 优化版（CPU 周期）
         - 改进 (%)
       * - memcpy
         - 32873
         - 4200
         - 87.2
       * - memcmp
         - 57436
         - 14722
         - 74.4
       * - memmove
         - 49336
         - 9237
         - 81.3
       * - strcpy
         - 28678
         - 16659
         - 41.9
       * - strcmp
         - 36867
         - 11146
         - 69.8

    .. note::

        上述结果适用于非对齐内存操作。对齐内存操作的性能保持不变。

    性能得到提升的函数
    ^^^^^^^^^^^^^^^^^^

    - ``memcpy``
    - ``memcmp``
    - ``memmove``
    - ``strcpy``
    - ``strncpy``
    - ``strcmp``
    - ``strncmp``
