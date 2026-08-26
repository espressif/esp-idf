GCC
***

:link_to_translation:`zh_CN:[中文]`


GCC Version
===========

The previous GCC version was GCC 11.2.0. This has now been upgraded to GCC 12.2.0 on all targets. Users that need to port their code from GCC 11.2.0 to 12.2.0 should refer to the series of official GCC porting guides listed below:

* `Porting to GCC 12 <https://gcc.gnu.org/gcc-12/porting_to.html>`_


Warnings
========

The upgrade to GCC 12.2.0 has resulted in the addition of new warnings, or enhancements to existing warnings. The full details of all GCC warnings can be found in `GCC Warning Options <https://gcc.gnu.org/onlinedocs/gcc-12.2.0/gcc/Warning-Options.html>`_. Users are advised to double-check their code, then fix the warnings if possible. Unfortunately, depending on the warning and the complexity of the user's code, some warnings will be false positives that require non-trivial fixes. In such cases, users can choose to suppress the warning in multiple ways. This section outlines some common warnings that users are likely to encounter and ways to fix them.


``-Wuse-after-free``
--------------------

Typically, this warning should not produce false-positives for release-level code. But this may appear in test cases. There is an example of how it was fixed in ESP-IDF's ``test_realloc.c``.

.. code-block:: c

    void *x = malloc(64);
    void *y = realloc(x, 48);
    TEST_ASSERT_EQUAL_PTR(x, y);

Pointers may be converted to int to avoid warning ``-Wuse-after-free``.

.. code-block:: c

    int x = (int) malloc(64);
    int y = (int) realloc((void *) x, 48);
    TEST_ASSERT_EQUAL_UINT32((uint32_t) x, (uint32_t) y);

``-Waddress``
-------------

GCC 12.2.0 introduces an enhanced version of the ``-Waddress`` warning option, which is now more eager in detecting the checking of pointers to an array in if-statements.

The following code triggers the warning:

.. code-block:: c

    char array[8];
    ...
    if (array)
        memset(array, 0xff, sizeof(array));


Eliminating unnecessary checks resolves the warning.

.. code-block:: c

    char array[8];
    ...
    memset(array, 0xff, sizeof(array));


RISC-V Builds Outside of ESP-IDF
================================

The RISC-V extensions ``zicsr`` and ``zifencei`` have been separated from the ``I`` extension. GCC 12 reflects this change, and as a result, when building for RISC-V ESP32 chips outside of the ESP-IDF framework, you must include the ``_zicsr_zifencei`` postfix when specifying the -march option in your build system.

Example:

.. code-block:: bash

  riscv32-esp-elf-gcc main.c -march=rv32imac

Now is replaced with:

.. code-block:: bash

  riscv32-esp-elf-gcc main.c -march=rv32imac_zicsr_zifencei
