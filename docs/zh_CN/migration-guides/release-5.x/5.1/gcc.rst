GCC
***

:link_to_translation:`en:[English]`


GCC 版本
========

ESP-IDF 之前使用的 GCC 版本为 11.2.0，现已针对所有芯片目标升级至 GCC 12.2.0。若需要将代码从 GCC 11.2.0 迁移到 GCC 12.2.0，请参考以下 GCC 官方迁移指南。

* `迁移至 GCC 12 <https://gcc.gnu.org/gcc-12/porting_to.html>`_


警告
====

升级至 GCC 12.2.0 后会触发新警告，或是导致原有警告内容发生变化。了解所有 GCC 警告的详细内容，请参考 `GCC 警告选项 <https://gcc.gnu.org/onlinedocs/gcc-12.2.0/gcc/Warning-Options.html>`_。建议用户仔细检查代码，并尽量解决这些警告。但由于某些警告的特殊性及用户代码的复杂性，有些警告可能为误报，需要进行关键修复。在这种情况下，用户可以采取多种方式来抑制警告。本节介绍了用户可能遇到的常见警告及如何修复这些警告。


``-Wuse-after-free``
--------------------

一般而言，此警告不会针对发布版本的代码产生误报，但是这种情况可能出现在测试用例中。以下示例为如何在 ESP-IDF 的 test_realloc.c 中修复该警告。

.. code-block:: c

    void *x = malloc(64);
    void *y = realloc(x, 48);
    TEST_ASSERT_EQUAL_PTR(x, y);

将指针转换为 int 可以避免出现 ``-Wuse-after-free`` 警告。

.. code-block:: c

    int x = (int) malloc(64);
    int y = (int) realloc((void *) x, 48);
    TEST_ASSERT_EQUAL_UINT32((uint32_t) x, (uint32_t) y);

``-Waddress``
-------------

GCC 12.2.0 引入了增强版 ``-Waddress`` 警告选项，该选项对 if 语句中的数组指针检查更加敏感。

以下代码将触发警告：

.. code-block:: c

    char array[8];
    ...
    if (array)
        memset(array, 0xff, sizeof(array));


删去不必要的检查可以消除警告。

.. code-block:: c

    char array[8];
    ...
    memset(array, 0xff, sizeof(array));


在 ESP-IDF 框架之外构建 RISC-V
================================

RISC-V 的 ``zicsr`` 和 ``zifencei`` 扩展现已独立于 ``I`` 扩展，这一变化在 GCC 12 中也有所体现。因此，在 ESP-IDF 框架之外构建 RISC-V ESP32 芯片时，请在构建系统中指定 -march 选项时添加 ``_zicsr_zifencei`` 后缀。示例如下。

原为：

.. code-block:: bash

  riscv32-esp-elf-gcc main.c -march=rv32imac

现为：

.. code-block:: bash

  riscv32-esp-elf-gcc main.c -march=rv32imac_zicsr_zifencei
