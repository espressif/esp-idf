Memory Management for MMU Supported Memory
******************************************

:link_to_translation:`zh_CN:[中文]`

.. toctree::
    :maxdepth: 1

Introduction
============

{IDF_TARGET_NAME} Memory Management Unit (MMU) is relatively simple. It can do memory address translation between physical memory addresses and virtual memory addresses. So CPU can access physical memories via virtual addresses. There are multiple types of virtual memory addresses, which have different capabilities.

ESP-IDF provides a memory mapping driver that manages the relation between these physical memory addresses and virtual memory addresses, so as to achieve some features such as reading from SPI flash via a pointer.

Memory mapping driver is actually a capabilities-based virtual memory address allocator that allows applications to make virtual memory address allocations for different purposes. In the following chapters, we call this driver ``esp_mmap`` driver.

ESP-IDF also provides a memory synchronization driver which can be used for potential memory desynchronization scenarios.

Physical Memory Types
=====================

Memory mapping driver currently supports mapping to following physical memory type(s):

.. list::

    - SPI flash
    :SOC_SPIRAM_SUPPORTED and not esp32: - PSRAM


Virtual Memory Capabilities
===========================

.. list::

    - :cpp:enumerator:`MMU_MEM_CAP_EXEC`: This capability indicates that the virtual memory address has the execute permission. Note this permission scope is within the MMU hardware.
    - :cpp:enumerator:`MMU_MEM_CAP_READ`: This capability indicates that the virtual memory address has the read permission. Note this permission scope is within the MMU hardware.
    - :cpp:enumerator:`MMU_MEM_CAP_WRITE`: This capability indicates that the virtual memory address has the write permission. Note this permission scope is within the MMU hardware.
    - :cpp:enumerator:`MMU_MEM_CAP_32BIT`: This capability indicates that the virtual memory address allows for 32 bits or multiples of 32 bits access.
    - :cpp:enumerator:`MMU_MEM_CAP_8BIT`: This capability indicates that the virtual memory address allows for 8 bits or multiples of 8 bits access.


.. only:: esp32


    8 MB external memory addresses (from 0x40400000 to 0x40C00000) which have the :cpp:enumerator:`MMU_MEM_CAP_EXEC` and :cpp:enumerator:`MMU_MEM_CAP_READ` capabilities are not available for users to allocate, due to hardware limitations.


.. only:: esp32s2

    4 MB external memory addresses (from 0x40400000 to 0x40800000) which have the :cpp:enumerator:`MMU_MEM_CAP_EXEC` and :cpp:enumerator:`MMU_MEM_CAP_READ` capabilities are not available for users to allocate, due to hardware limitations.


You can call :cpp:func:`esp_mmu_map_get_max_consecutive_free_block_size` to know the largest consecutive mappable block size with certain capabilities.


Memory Management Drivers
=========================


Driver Concept
--------------


Terminology
^^^^^^^^^^^

The virtual memory pool is made up with one or multiple virtual memory regions, see below figure:

.. image:: /../_static/diagrams/mmu/mem_pool.png
    :scale: 80 %
    :align: center


- A virtual memory pool stands for the whole virtual address range that can be mapped to physical memory.
- A virtual memory region is a range of virtual address with same attributes.
- A virtual memory block is a piece of virtual address range that is dynamically mapped.
- A slot is the virtual address range between two virtual memory blocks.
- A physical memory block is a piece of physical address range that is to-be-mapped or already mapped to a virtual memory block.
- Dynamical mapping is done by calling ``esp_mmap`` driver API :cpp:func:`esp_mmu_map`. This API maps the given physical memory block to a virtual memory block which is allocated by the ``esp_mmap`` driver.


Relation Between Memory Blocks
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When mapping a physical memory block A, block A can have one of the following relations with another previously mapped physical memory block B:

- Enclosed: block A is completely enclosed within block B, see figure below:

  .. image:: /../_static/diagrams/mmu/enclosed.png
    :scale: 80 %
    :align: center

- Identical: block A is completely the same as block B, see figure below:

  .. image:: /../_static/diagrams/mmu/identical.png
    :scale: 80 %
    :align: center

  Note that ``esp_mmap`` driver considers the identical scenario **the same as the enclosed scenario**.

- Overlapped: block A is overlapped with block B, see figure below:

  .. image:: /../_static/diagrams/mmu/overlapped.png
    :scale: 80 %
    :align: center

  There is a special condition, when block A entirely encloses block B, see figure below:

  .. image:: /../_static/diagrams/mmu/inversed_enclosed.png
    :scale: 70 %
    :align: center

  Note that ``esp_mmap`` driver considers this scenario **the same as the overlapped scenario**.


Driver Behaviour
----------------

Memory Map
^^^^^^^^^^

You can call :cpp:func:`esp_mmu_map` to do a dynamical mapping. This API can allocate a certain size of virtual memory block according to the virtual memory capabilities you selected, then map this virtual memory block to the physical memory block as you requested. The ``esp_mmap`` driver supports mapping to one or more types of physical memory, so you should specify the physical memory target when mapping.

By default, physical memory blocks and virtual memory blocks are one-to-one mapped. This means, when calling :cpp:func:`esp_mmu_map`:

* If it is the enclosed scenario, this API will return an :c:macro:`ESP_ERR_INVALID_STATE`. The ``out_ptr`` will be assigned to the start virtual memory address of the previously mapped one which encloses the to-be-mapped one.
* If it is the identical scenario, this API will behaves exactly the same as the enclosed scenario.
* If it is the overlapped scenario, this API will by default return an :c:macro:`ESP_ERR_INVALID_ARG`. This means, ``esp_mmap`` driver by default does not allow mapping a physical memory address to multiple virtual memory addresses.

Specially, you can use :c:macro:`ESP_MMU_MMAP_FLAG_PADDR_SHARED`. This flag stands for one-to-multiple mapping between a physical address and multiple virtual addresses:

* If it is the overlapped scenario, this API will allocate a new virtual memory block as requested, then map to the given physical memory block.


Memory Unmap
^^^^^^^^^^^^

You can call :cpp:func:`esp_mmu_unmap` to unmap a previously mapped memory block. This API returns an :c:macro:`ESP_ERR_NOT_FOUND` if you are trying to unmap a virtual memory block that is not mapped to any physical memory block yet.


Memory Address Conversion
^^^^^^^^^^^^^^^^^^^^^^^^^

The ``esp_mmap`` driver provides two helper APIs to do the conversion between virtual memory address and physical memory address：

* :cpp:func:`esp_mmu_vaddr_to_paddr` converts virtual address to physical address.
* :cpp:func:`esp_mmu_paddr_to_vaddr` converts physical address to virtual address.


Memory Synchronization
^^^^^^^^^^^^^^^^^^^^^^

MMU supported physical memories can be accessed by one or multiple methods.

SPI flash can be accessed by SPI1 (ESP-IDF ``esp_flash`` driver APIs), or by pointers. ESP-IDF ``esp_flash`` driver APIs have already considered the memory synchronization, so users do not need to worry about this.

.. only:: SOC_SPIRAM_SUPPORTED

    PSRAM can be accessed by pointers, hardware guarantees the data consistency when PSRAM is only accessed via pointers.

.. only:: esp32s3

    PSRAM can also be accessed by EDMA. Data desynchronization may happen because hardware does not guarantee the data consistency under such condition. You should call :cpp:func:`esp_cache_msync` to synchronize the Cache and the PSRAM.

    See :doc:`/api-reference/system/mm_sync` for more details.


Thread Safety
=============

APIs in ``esp_mmu_map.h`` are not guaranteed to be thread-safe.

APIs in ``esp_cache.h`` are guaranteed to be thread-safe.


API Reference
=============

API Reference - ESP MMAP Driver
-------------------------------

.. include-build-file:: inc/esp_mmu_map.inc
