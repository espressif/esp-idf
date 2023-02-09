GCC 
***

:link_to_translation:`en:[English]`


GCC 版本
========

ESP-IDF 之前使用的 GCC 版本为 11.2.0，现已针对所有芯片目标升级至 GCC 12.2.0。若需要将您的代码从 GCC 11.2.0 迁移到 GCC 12.2.0，请参考以下官方 GCC 迁移指南。

* `迁移至 GCC 9 <https://gcc.gnu.org/gcc-9/porting_to.html>`_
* `迁移至 GCC 10 <https://gcc.gnu.org/gcc-10/porting_to.html>`_
* `迁移至 GCC 11 <https://gcc.gnu.org/gcc-11/porting_to.html>`_


警告
====

升级至 GCC 12.2.0 后会触发新警告，或是导致原有警告内容发生变化。所有 GCC 警告的详细内容，请参考 `GCC 警告选项 <https://gcc.gnu.org/onlinedocs/gcc-12.2.0/gcc/Warning-Options.html>`_。建议用户仔细检查代码，并设法解决这些警告。但由于某些警告的特殊性及用户代码的复杂性，有些警告可能为误报，需要进行关键修复。在这种情况下，用户可以采取多种方式来抑制这些警告。本节介绍了用户可能遇到的常见警告及如何抑制这些警告。


``-Wuse-after-free``
--------------------

Typically, this warning should not produce false-positives for release-level code. But may appear in test cases. There is an example of how it was fixed in IDF's test_realloc.c 

.. code-block:: c

    void *x = malloc(64);
    void *y = realloc(x, 48);
    TEST_ASSERT_EQUAL_PTR(x, y);

Pointers may be converted to int to avoid warning ``-Wuse-after-free``

.. code-block:: c

    int x = (int) malloc(64);
    int y = (int) realloc((void *) x, 48);
    TEST_ASSERT_EQUAL_UINT32((uint32_t) x, (uint32_t) y);

``-Waddress``
-------------

GCC 12.2.0 introduces an enhanced version of the ``-Waddress`` warning option, which is now more eager in detecting the checking of pointers to array in if-statements.

The following code will trigger the warning:

.. code-block:: c

    char array[8];
    ...
    if (array)
        memset(array, 0xff, sizeof(array));


Eliminating unnecessary check resolves the warning.

.. code-block:: c

    char array[8];
    ...
    memset(array, 0xff, sizeof(array));


RISCV Builds Outside of IDF
===========================

The RISCV extensions ``zicsr`` and ``zifencei`` have been separated from the ``I`` extension. GCC 12 reflects this change, and as a result, when building for RISCV ESP32 chips outside of the IDF framework, you must include the ``_zicsr_zifencei`` postfix when specifying the -march option in your build system.

Example:

.. code-block:: bash

  riscv32-esp-elf-gcc main.c -march=rv32imac

Is now replaced with:

.. code-block:: bash

  riscv32-esp-elf-gcc main.c -march=rv32imac_zicsr_zifencei
