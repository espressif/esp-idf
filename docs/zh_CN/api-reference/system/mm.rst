基于 MMU 的存储管理
*******************

:link_to_translation:`en:[English]`

.. toctree::
    :maxdepth: 1

简介
====

{IDF_TARGET_NAME} 的存储管理单元 (MMU) 相对简单。它可以在物理存储地址和虚拟存储地址之间进行存储地址转换，因此，CPU 可以通过虚拟存储地址来访问物理存储。虚拟存储地址有多种类型，分别具有不同的功能。

ESP-IDF 提供了一个存储器映射驱动程序，用于管理这些物理存储地址和虚拟存储地址之间的关系，实现一些功能。例如，可以通过指针从 SPI flash 中读取内容。

存储映射驱动程序实际上是一个基于属性的虚拟存储地址分配器，允许应用程序为不同的目的分配虚拟存储地址。下文将此驱动程序称为 ``esp_mmap`` 驱动程序。

ESP-IDF 还提供了一个存储器同步驱动程序来处理潜在的不同步的情况。

物理存储类型
============

存储映射驱动程序目前支持映射到以下物理存储类型：

.. list::

    - SPI flash
    :SOC_SPIRAM_SUPPORTED and not esp32: - PSRAM


虚拟存储的功能
==============

.. list::

    - :cpp:enumerator:`MMU_MEM_CAP_EXEC`：此功能表示虚拟存储地址具有执行权限。注意，此权限的范围仅限 MMU 硬件内部。
    - :cpp:enumerator:`MMU_MEM_CAP_READ`：此功能表示虚拟存储地址具有读取权限。注意，此权限的范围仅限 MMU 硬件内部。
    - :cpp:enumerator:`MMU_MEM_CAP_WRITE`：此功能表示虚拟存储地址具有写入权限。注意，此权限的范围仅限 MMU 硬件内部。
    - :cpp:enumerator:`MMU_MEM_CAP_32BIT`：此功能表示允许访问 32 位或 32 位倍数的虚拟存储。
    - :cpp:enumerator:`MMU_MEM_CAP_8BIT`：此功能表示允许访问 8 位或 8 位倍数的虚拟存储。


.. only:: esp32


    由于硬件限制，不能分配具有 :cpp:enumerator:`MMU_MEM_CAP_EXEC` 和 :cpp:enumerator:`MMU_MEM_CAP_READ` 功能的 8 MB 外部存储地址（从 0x40400000 到 0x40C00000）。


.. only:: esp32s2

    由于硬件限制，不能分配具有 :cpp:enumerator:`MMU_MEM_CAP_EXEC` 和 :cpp:enumerator:`MMU_MEM_CAP_READ` 功能的 4 MB 外部存储地址（从 0x40400000 到 0x40800000）。


如需了解具有特定功能的最大连续可映射块大小，请调用 :cpp:func:`esp_mmu_map_get_max_consecutive_free_block_size`。


存储管理驱动程序
================


驱动程序
--------


术语解释
^^^^^^^^

虚拟存储池由一个或多个虚拟存储区域组成，如下图所示：

.. image:: /../_static/diagrams/mmu/mem_pool.png
    :scale: 80 %
    :align: center


- 一个虚拟存储池是指可以映射到物理存储的整个虚拟地址范围。
- 一个虚拟存储区域是指具有相同属性的虚拟地址范围。
- 一个虚拟存储块是指一块动态映射的虚拟地址范围。
- 一个间隙是指两个虚拟存储块之间的虚拟地址范围。
- 一个物理存储块是指一块待映射或已映射到虚拟存储块的物理地址范围。
- 动态映射是指调用 ``esp_mmap`` 驱动程序 API :cpp:func:`esp_mmu_map` 进行的映射，此 API 会将给定的物理存储块映射到 ``esp_mmap`` 驱动程序分配的虚拟存储块中。


存储块的关系
^^^^^^^^^^^^

在映射物理存储块 A 时，存储块 A 与此前已映射的另一个物理存储块 B 之间可能存在以下关系之一：

- 包含：块 A 被完全包含在块 B 内，如下图所示：

  .. image:: /../_static/diagrams/mmu/enclosed.png
    :scale: 80 %
    :align: center

- 相同：块 A 与块 B 完全相同，如下图所示：

  .. image:: /../_static/diagrams/mmu/identical.png
    :scale: 80 %
    :align: center

  注意，``esp_mmap`` 驱动程序将相同 **视为包含**。

- 重叠：块 A 与块 B 重叠，如下图所示：

  .. image:: /../_static/diagrams/mmu/overlapped.png
    :scale: 80 %
    :align: center

  特殊情况下，当块 A 完全包围块 B 时，如下图所示：

  .. image:: /../_static/diagrams/mmu/inversed_enclosed.png
    :scale: 70 %
    :align: center

  注意，``esp_mmap`` 驱动程序将这种情况 **视为重叠**。


驱动程序行为
------------

存储映射
^^^^^^^^

可以调用 :cpp:func:`esp_mmu_map` 进行动态映射。该 API 会根据你所选择的虚拟存储的属性分配一定大小的虚拟存储块，然后按照要求将此虚拟存储块映射到物理存储块中。``esp_mmap`` 驱动程序支持映射到一种或多种物理存储，因此，应在映射时指定物理存储目标。

默认情况下，物理存储块和虚拟存储块是一对一映射。因此，当调用 :cpp:func:`esp_mmu_map` 时，如果存储关系不同，API 的行为也有所不同：

* 如果是“包含”的情形，此 API 将返回 :c:macro:`ESP_ERR_INVALID_STATE`。此时，由于之前已映射的虚拟存储包含待映射的虚拟存储，因此返回的 ``out_ptr`` 会指向之前映射的虚拟存储的起始地址。
* 如果是“相同”的情形，此 API 的行为与“包含”的情况完全相同。
* 如果是“重叠”的情形，此 API 默认返回 :c:macro:`ESP_ERR_INVALID_ARG`。这表明，``esp_mmap`` 驱动程序在默认情况下，不允许将一个物理存储地址映射到多个虚拟存储地址。

但是，可以使用 :c:macro:`ESP_MMU_MMAP_FLAG_PADDR_SHARED` flag，代表在物理地址和虚拟地址之间的一对多映射：

* 如果是“重叠”的情形，此 API 将按照要求分配一个新的虚拟存储块，并将其映射到给定的物理存储块中。


取消存储映射
^^^^^^^^^^^^

可以调用 :cpp:func:`esp_mmu_unmap` 来取消先前存储块的映射。如果尝试取消映射的虚拟存储块实际尚未映射到任何物理存储块，此 API 将返回 :c:macro:`ESP_ERR_NOT_FOUND`。


存储地址转换
^^^^^^^^^^^^

``esp_mmap`` 驱动程序提供了两个辅助 API，用于虚拟存储地址和物理存储地址之间的转换：

* :cpp:func:`esp_mmu_vaddr_to_paddr` 可将虚拟存储地址转换为物理存储地址。
* :cpp:func:`esp_mmu_paddr_to_vaddr` 可将物理存储地址转换为虚拟存储地址。


存储同步
^^^^^^^^

可以通过一种或多种方法来访问支持 MMU 的物理存储。

SPI flash 可以通过 SPI1（ESP-IDF ``spi_flash`` 驱动 API）或指针进行访问。ESP-IDF ``spi_flash`` 驱动 API 中已经考虑了存储同步，因此在使用时无需额外考虑存储同步。

.. only:: SOC_SPIRAM_SUPPORTED

    PSRAM 可以通过指针进行访问。当只通过指针访问 PSRAM 时，硬件可以保证数据的一致性。

.. only:: esp32s3

    PSRAM 也可以通过 EDMA 进行访问，但有可能发生数据不同步的问题。因为在这种情况下，硬件不能保证数据的一致性。应调用 :cpp:func:`esp_cache_msync` 来同步 cache 和 PSRAM。

    如需了解更多内容，请参考 :doc:`/api-reference/system/mm_sync`。


线程安全
========

``esp_mmu_map.h`` 中的 API 不能确保线程的安全性。

``esp_cache.h`` 中的 API 能够确保线程的安全性。


API 参考
========

API 参考 - ESP MMAP 驱动
-------------------------------

.. include-build-file:: inc/esp_mmu_map.inc
