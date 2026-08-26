GCC
***

:link_to_translation:`en:[English]`


GCC 版本
========

ESP-IDF 之前使用的 GCC 版本为 8.4.0，现已针对所有芯片目标升级至 GCC 11.2.0。若需要将代码从 GCC 8.4.0 迁移到 GCC 11.2.0，请参考以下官方 GCC 迁移指南。

* `迁移至 GCC 9 <https://gcc.gnu.org/gcc-9/porting_to.html>`_
* `迁移至 GCC 10 <https://gcc.gnu.org/gcc-10/porting_to.html>`_
* `迁移至 GCC 11 <https://gcc.gnu.org/gcc-11/porting_to.html>`_


警告
====

升级至 GCC 11.2.0 后会触发新警告，或是导致原有警告内容发生变化。所有 GCC 警告的详细内容，请参考 `GCC 警告选项 <https://gcc.gnu.org/onlinedocs/gcc-11.2.0/gcc/Warning-Options.html>`_。建议用户仔细检查代码，并设法解决这些警告。但由于某些警告的特殊性及用户代码的复杂性，有些警告可能为误报，需要进行关键修复。在这种情况下，用户可以采取多种方式来抑制这些警告。本节介绍了用户可能遇到的常见警告及如何抑制这些警告。

.. 注意::

    建议用户在抑制警告之前仔细确认该警告是否确实为误报。


``-Wstringop-overflow``、 ``-Wstringop-overread``、 ``-Wstringop-truncation`` 和 ``-Warray-bounds``
------------------------------------------------------------------------------------------------------------------

如果编译器不能准确判断内存或字符串的大小，使用 memory/string copy/compare 函数的用户会遇到某种 ``-Wstringop`` 警告。下文展示了触发这些警告的代码，并介绍了如何抑制这些警告。

.. code-block:: c

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstringop-overflow"
    #pragma GCC diagnostic ignored "-Warray-bounds"
        memset(RTC_SLOW_MEM, 0, CONFIG_ULP_COPROC_RESERVE_MEM); // <<-- 此行触发了警告
    #pragma GCC diagnostic pop


.. code-block:: c

    #pragma GCC diagnostic push
    #if __GNUC__ >= 11
    #pragma GCC diagnostic ignored "-Wstringop-overread" // <<-- 此键从 GCC 11 开始引入
    #endif
    #pragma GCC diagnostic ignored "-Warray-bounds"（-Warray-bounds）。
                memcpy(backup_write_data, (void *)EFUSE_PGM_DATA0_REG, sizeof(backup_write_data)); // <<-- 此行触发了警告
    #pragma GCC diagnostic pop


``-Waddress-of-packed-member``
---------------------------------

当访问打包 ``struct`` 中的某个未对齐成员时，由于非对齐内存访问会对性能产生影响，GCC 会触发 ``-Waddress-of-packed-member`` 警告。然而，所有基于 Xtensa 或 RISC-V 架构的 ESP 芯片都允许非对齐内存访问，并且不会产生额外的性能影响。因此，在大多数情况下，可以忽略此问题。

.. code-block:: none

    components/bt/host/bluedroid/btc/profile/std/gatt/btc_gatt_util.c: In function 'btc_to_bta_gatt_id':
    components/bt/host/bluedroid/btc/profile/std/gatt/btc_gatt_util.c:105:21: warning: taking address of packed member of 'struct <anonymous>' may result in an unaligned pointer value [-Waddress-of-packed-member]
      105 |     btc_to_bta_uuid(&p_dest->uuid, &p_src->uuid);
          |                     ^~~~~~~~~~~~~


如果该警告在多个源文件中多次出现，可以在 CMake 级别抑制该警告，如下所示。

.. code-block:: cmake

    set_source_files_properties(
        "host/bluedroid/bta/gatt/bta_gattc_act.c"
        "host/bluedroid/bta/gatt/bta_gattc_cache.c"
        "host/bluedroid/btc/profile/std/gatt/btc_gatt_util.c"
        "host/bluedroid/btc/profile/std/gatt/btc_gatts.c"
        PROPERTIES COMPILE_FLAGS -Wno-address-of-packed-member)

但如果只有一或两处警告，可以直接在源代码中进行抑制，如下所示。

.. code-block:: c

    #pragma GCC diagnostic push
    #if     __GNUC__ >= 9
    #pragma GCC diagnostic ignored "-Waddress-of-packed-member" <<-- 此键从 GCC 11 开始引入
    #endif
        uint32_t* reg_ptr = (uint32_t*)src;
    #pragma GCC diagnostic pop


``llabs()`` 用于 64 位整数
-------------------------------

stdlib.h 中的函数 ``abs()`` 需要使用 ``int`` 参数。请在计划为 64 位的类型中使用 ``llabs()``，尤其是 ``time_t``。


乐鑫工具链更新
=================

Xtensa 编译器中的 ``int32_t`` 和 ``uint32_t``
---------------------------------------------------

在 Xtensa 编译器中，``int32_t`` 和 ``uint32_t`` 类型已分别从 ``int`` 和 ``unsigned int`` 更新为 ``long`` 和 ``unsigned long``。此更新现与上游 GCC 相匹配，上游 GCC 在 Xtensa、RISC-V 和其他架构上使用 ``long`` 整数来表示 ``int32_t`` 和 ``uint32_t``。


.. list-table::
   :widths: 20 45 35
   :header-rows: 1

   * -
     - 2021r2 及以上版本，GCC 8
     - 2022r1，GCC 11
   * - Xtensa
     - (unsigned) int
     - (unsigned) long
   * - riscv32
     - (unsigned) long
     - (unsigned) long


若代码中使用了 ``<inttypes.h>`` 提供的类型来格式化字符串，则这些代码会受到上述变化的影响。使用这些宽度固定的类型（例如 ``uint32_t``）时，请使用 ``PRIi32``、``PRIx32`` 来分别替换 ``%i``、``%x`` 等占位符。**只有** 在 ``<inttypes.h>`` 中定义的类型（例如 ``int``）需要这种特殊格式。

在其他情况下，请注意枚举支持 ``int`` 类型。

通常，``int32_t`` 和 ``int`` 为不同的类型。同样，``uint32_t`` 和 ``unsigned int`` 也为不同的类型。

如果用户在其应用中没有对格式化字符串进行上述更新，程序会报错，如下所示：

.. code-block:: none

    /Users/name/esp/esp-rainmaker/components/esp-insights/components/esp_diagnostics/include/esp_diagnostics.h:238:29: error: format '%u' expects argument of type 'unsigned int', but argument 3 has type 'uint32_t' {aka 'long unsigned int'} [-Werror=format=]
    238 |     esp_diag_log_event(tag, "EV (%u) %s: " format, esp_log_timestamp(), tag, ##__VA_ARGS__); \
        |                             ^~~~~~~~~~~~~~         ~~~~~~~~~~~~~~~~~~~
        |                                                    |
        |                                                    uint32_t {aka long unsigned int}
                                                  uint32_t {aka long unsigned int}

移除构建选项 ``CONFIG_COMPILER_DISABLE_GCC8_WARNINGS``
----------------------------------------------------------

原有的 ``CONFIG_COMPILER_DISABLE_GCC8_WARNINGS`` 选项用于构建使用现已僵化的 GCC 5 工具链编写的陈旧代码。但由于已经过去较长时间，现在可以对警告进行修复，因此该选项已移除。

目前，在 GCC 11 中，建议用户仔细检查代码，尽量解决编译器警告。
