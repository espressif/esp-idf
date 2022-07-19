GCC 
***

Previous GCC version was 8.4.0


Official GNU porting guides for your code
=========================================


* https://gcc.gnu.org/gcc-9/porting_to.html

* https://gcc.gnu.org/gcc-10/porting_to.html

* https://gcc.gnu.org/gcc-11/porting_to.html


Espressif Toolchain changes
===========================


``int32_t`` and ``uint32_t`` for Xtensa compiler
------------------------------------------------

The types ``int32_t`` and ``uint32_t`` have been changed from ``int`` and ``unsigned int`` to ``long`` and ``unsigned long``. Upstream GCC uses ``long`` integers for int32_t/uint32_t on Xtensa, RISC-V and other architectures.

+---------+--------------------------+-----------------+
|         | 2021r2 and older, GCC 8  | 2022r1, GCC 11  |
+=========+==========================+=================+
| xtensa  | (unsigned) int           | (unsigned) long |
+---------+--------------------------+-----------------+
| riscv32 | (unsigned) long          | (unsigned) long |
+---------+--------------------------+-----------------+


The most cases in code are related to the formatting. Using ``%i``, ``%x``, etc., should be replaced to ``PRIi32``, ``PRIxx``, and others from ``<inttypes.h>``.

In other cases it should be noted that enums have ``int`` type.

In common, ``int32_t`` and ``int`` are different types, as well as ``uint32_t`` and ``unsigned int``.


Removing of ``CONFIG_COMPILER_DISABLE_GCC8_WARNINGS`` build option
------------------------------------------------------------------

``CONFIG_COMPILER_DISABLE_GCC8_WARNINGS`` option was introduced to help transition from rigid GCC 5 toolchain to new ones with helping build ancient code. Enough time has passed to fix the warnings.

For now in GCC 11, the suggestion is to review your own code to comply compiler warnings.


Common cases in code
====================


``-Wstringop-overflow``, ``-Wstringop-overread``, ``-Wstringop-truncation``, and ``-Warray-bounds`` warnings
------------------------------------------------------------------------------------------------------------

Warning details: https://gcc.gnu.org/onlinedocs/gcc-11.2.0/gcc/Warning-Options.html

Double check your code then fix please. Unfortunately, not all seemingly simple ways to satisfy the compiler will work.
You can supress such warnings if the compiler worried for nothing.

.. code-block:: c

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstringop-overflow"
    #pragma GCC diagnostic ignored "-Warray-bounds"
        memset(RTC_SLOW_MEM, 0, CONFIG_ULP_COPROC_RESERVE_MEM); // <<-- This line leads to warnings
    #pragma GCC diagnostic pop


.. code-block:: c

    #pragma GCC diagnostic push
    #if     __GNUC__ >= 11
    #pragma GCC diagnostic ignored "-Wstringop-overread" // <<-- This key had been introduced since GCC 11
    #endif
    #pragma GCC diagnostic ignored "-Warray-bounds"
                memcpy(backup_write_data, (void *)EFUSE_PGM_DATA0_REG, sizeof(backup_write_data)); // <<-- This line leads to warnings
    #pragma GCC diagnostic pop



``-Waddress-of-packed-member`` warning
--------------------------------------

Warning details: https://gcc.gnu.org/onlinedocs/gcc-11.2.0/gcc/Warning-Options.html

Double check your code then fix please.

Unaligned pointer value for data doesn't have penalty for xtensa and riscv32 Espressif chips so we can ignore it in most cases.

.. code-block:: none

    components/bt/host/bluedroid/btc/profile/std/gatt/btc_gatt_util.c: In function 'btc_to_bta_gatt_id':
    components/bt/host/bluedroid/btc/profile/std/gatt/btc_gatt_util.c:105:21: warning: taking address of packed member of 'struct <anonymous>' may result in an unaligned pointer value [-Waddress-of-packed-member]
      105 |     btc_to_bta_uuid(&p_dest->uuid, &p_src->uuid);
          |                     ^~~~~~~~~~~~~


on CMake level for tons of cases:

.. code-block:: cmake

    set_source_files_properties(
        "host/bluedroid/bta/gatt/bta_gattc_act.c"
        "host/bluedroid/bta/gatt/bta_gattc_cache.c"
        "host/bluedroid/btc/profile/std/gatt/btc_gatt_util.c"
        "host/bluedroid/btc/profile/std/gatt/btc_gatts.c"
        PROPERTIES COMPILE_FLAGS -Wno-address-of-packed-member)

or on code level:

.. code-block:: c

    #pragma GCC diagnostic push
    #if     __GNUC__ >= 9
    #pragma GCC diagnostic ignored "-Waddress-of-packed-member" <<-- This key had been introduced since GCC 9
    #endif
        uint32_t* reg_ptr = (uint32_t*)src;
    #pragma GCC diagnostic pop



``llabs()`` for 64-bit integers
-------------------------------

The function ``abs()`` from stdlib.h takes ``int`` argument. Please use ``llabs()`` for types that intended to be 64-bit. In particular it's important for ``time_t``.
