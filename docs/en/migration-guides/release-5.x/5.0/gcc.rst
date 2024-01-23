GCC
***

:link_to_translation:`zh_CN:[中文]`


GCC Version
===========

The previous GCC version was GCC 8.4.0. This has now been upgraded to GCC 11.2.0 on all targets. Users that need to port their code from GCC 8.4.0 to 11.2.0 should refer to the series of official GCC porting guides listed below:

* `Porting to GCC 9 <https://gcc.gnu.org/gcc-9/porting_to.html>`_
* `Porting to GCC 10 <https://gcc.gnu.org/gcc-10/porting_to.html>`_
* `Porting to GCC 11 <https://gcc.gnu.org/gcc-11/porting_to.html>`_


Warnings
========

The upgrade to GCC 11.2.0 has resulted in the addition of new warnings, or enhancements to existing warnings. The full details of all GCC warnings can be found in `GCC Warning Options <https://gcc.gnu.org/onlinedocs/gcc-11.2.0/gcc/Warning-Options.html>`_. Users are advised to double-check their code, then fix the warnings if possible. Unfortunately, depending on the warning and the complexity of the user's code, some warnings will be false positives that require non-trivial fixes. In such cases, users can choose to suppress the warning in multiple ways. This section outlines some common warnings that users are likely to encounter, and ways to suppress them.

.. warning::

    Users are advised to check that a warning is indeed a false positive before attempting to suppress them it.


``-Wstringop-overflow``, ``-Wstringop-overread``, ``-Wstringop-truncation``, and ``-Warray-bounds``
--------------------------------------------------------------------------------------------------------

Users that use memory/string copy/compare functions will run into one of the ``-Wstringop`` warnings if the compiler cannot properly determine the size of the memory/string. The examples below demonstrate code that triggers these warnings and how to suppress them.

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


``-Waddress-of-packed-member``
--------------------------------

GCC will issue this warning when accessing an unaligned member of a packed ``struct`` due to the incurred penalty of unaligned memory access. However, all ESP chips (on both Xtensa and RISC-V architectures) allow for unaligned memory access and incur no extra penalty. Thus, this warning can be ignored in most cases.

.. code-block:: none

    components/bt/host/bluedroid/btc/profile/std/gatt/btc_gatt_util.c: In function 'btc_to_bta_gatt_id':
    components/bt/host/bluedroid/btc/profile/std/gatt/btc_gatt_util.c:105:21: warning: taking address of packed member of 'struct <anonymous>' may result in an unaligned pointer value [-Waddress-of-packed-member]
      105 |     btc_to_bta_uuid(&p_dest->uuid, &p_src->uuid);
          |                     ^~~~~~~~~~~~~


If the warning occurs in multiple places across multiple source files, users can suppress the warning at the CMake level as demonstrated below.

.. code-block:: cmake

    set_source_files_properties(
        "host/bluedroid/bta/gatt/bta_gattc_act.c"
        "host/bluedroid/bta/gatt/bta_gattc_cache.c"
        "host/bluedroid/btc/profile/std/gatt/btc_gatt_util.c"
        "host/bluedroid/btc/profile/std/gatt/btc_gatts.c"
        PROPERTIES COMPILE_FLAGS -Wno-address-of-packed-member)

However, if there are only one or two instances, users can suppress the warning directly in the source code itself as demonstrated below.

.. code-block:: c

    #pragma GCC diagnostic push
    #if     __GNUC__ >= 9
    #pragma GCC diagnostic ignored "-Waddress-of-packed-member" <<-- This key had been introduced since GCC 9
    #endif
        uint32_t* reg_ptr = (uint32_t*)src;
    #pragma GCC diagnostic pop


``llabs()`` for 64-bit Integers
-------------------------------

The function ``abs()`` from stdlib.h takes ``int`` argument. Please use ``llabs()`` for types that are intended to be 64-bit. It is particularly important for ``time_t``.


Espressif Toolchain Changes
===========================

``int32_t`` and ``uint32_t`` for Xtensa Compiler
------------------------------------------------

The types ``int32_t`` and ``uint32_t`` have been changed from the previous ``int`` and ``unsigned int`` to ``long`` and ``unsigned long`` respectively for the Xtensa compiler. This change now matches upstream GCC which ``long`` integers for ``int32_t`` and ``uint32_t`` on Xtensa, RISC-V, and other architectures.


.. list-table::
   :widths: 20 45 35
   :header-rows: 1

   * -
     - 2021r2 and older, GCC 8
     - 2022r1, GCC 11
   * - Xtensa
     - (unsigned) int
     - (unsigned) long
   * - riscv32
     - (unsigned) long
     - (unsigned) long


The change mostly affects code that formats strings using types provided by ``<inttypes.h>``. When using these fixed-width types (e.g., ``uint32_t``), users will need to replace placeholders such as ``%i`` and ``%x`` with ``PRIi32`` and ``PRIx32`` respectively. Types *not* defined in ``<inttypes.h>`` (e.g., ``int``) do *not* need this special formatting.

In other cases, it should be noted that enums have the ``int`` type.

In common, ``int32_t`` and ``int``, as well as ``uint32_t`` and ``unsigned int``, are different types.

If users do not make the aforementioned updates to format strings in their applications, the following error will be reported during compilation:

.. code-block:: none

    /Users/name/esp/esp-rainmaker/components/esp-insights/components/esp_diagnostics/include/esp_diagnostics.h:238:29: error: format '%u' expects argument of type 'unsigned int', but argument 3 has type 'uint32_t' {aka 'long unsigned int'} [-Werror=format=]
    238 |     esp_diag_log_event(tag, "EV (%u) %s: " format, esp_log_timestamp(), tag, ##__VA_ARGS__); \
        |                             ^~~~~~~~~~~~~~         ~~~~~~~~~~~~~~~~~~~
        |                                                    |
        |                                                    uint32_t {aka long unsigned int}
                                                  uint32_t {aka long unsigned int}

Removing ``CONFIG_COMPILER_DISABLE_GCC8_WARNINGS`` Build Option
------------------------------------------------------------------

``CONFIG_COMPILER_DISABLE_GCC8_WARNINGS`` option was introduced to allow building of legacy code dating from the rigid GCC 5 toolchain. However, enough time has passed to allow for the warnings to be fixed, thus this option has been removed.

For now in GCC 11, users are advised to review their code and fix the compiler warnings where possible.
