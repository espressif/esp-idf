System
======

:link_to_translation:`zh_CN:[中文]`

SPIRAM DMA Reserve Pool
-----------------------

When :ref:`CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL` is enabled, the capability priorities of the reserved internal DMA pool have changed:

- **Before**: medium priority for ``MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL``; low priority for ``MALLOC_CAP_DEFAULT``.
- **After**: medium priority for ``MALLOC_CAP_DMA`` only; low priority for ``MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL``.

Allocations that request ``MALLOC_CAP_INTERNAL`` without ``MALLOC_CAP_DMA``, such as FreeRTOS task stacks and TCBs, no longer prefer the reserve pool and use it only after other internal heaps are exhausted. Explicit ``MALLOC_CAP_DMA`` requests still prefer the pool.

With a non-zero :ref:`CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL`, the internal-first attempt of ``malloc()`` now skips heaps that provide ``MALLOC_CAP_DEFAULT`` only at low priority: the reserve pool, and on some targets RTC RAM and SPM (for example, SPM and RTC RAM on ESP32-P4). Small ``malloc()`` allocations that previously landed in these heaps are now placed in PSRAM first; these heaps are used only after PSRAM is exhausted. Buffers that must stay in internal memory should be allocated with ``heap_caps_malloc()`` and ``MALLOC_CAP_INTERNAL``.
