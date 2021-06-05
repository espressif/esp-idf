.. _memory-layout:

Memory Types
------------

{IDF_TARGET_NAME} chip has multiple memory types and flexible memory mapping features. This section describes how ESP-IDF uses these features by default.

ESP-IDF distinguishes between instruction memory bus (IRAM, IROM, RTC FAST memory) and data memory bus (DRAM, DROM). Instruction memory is executable, and can only be read or written via 4-byte aligned words. Data memory is not executable and can be accessed via individual byte operations. For more information about the different memory buses consult the {IDF_TARGET_NAME} Technical Reference Manual* > *System and Memory*  [`PDF <{IDF_TARGET_TRM_EN_URL}#sysmem>`__].

.. _dram:

DRAM (Data RAM)
^^^^^^^^^^^^^^^

Non-constant static data (.data) and zero-initialized data (.bss) is placed by the linker into Internal SRAM as data memory. Remaining space in this region is used for the runtime heap.

.. only:: esp32

   The available size of the internal DRAM region is reduced by 64kB (by shifting start address to ``0x3FFC0000``) if Bluetooth stack is used. Length of this region is also reduced by 16 kB or 32kB if trace memory is used. Due to some memory fragmentation issues caused by ROM, it is also not possible to use all available DRAM for static allocations - however the remaining DRAM is still available as heap at runtime.

.. only:: not esp32

   .. note::

          The maximum statically allocated DRAM size is reduced by the :ref:`iram` size of the compiled application. The available heap memory at runtime is reduced by the total static IRAM and DRAM usage of the application.

Constant data may also be placed into DRAM, for example if it is used in an non-flash-safe ISR (see explanation under :ref:`how-to-place-code-in-iram`).

"noinit" DRAM
=============

The macro ``__NOINIT_ATTR`` can be used as attribute to place data into ``.noinit`` section. The values placed into this section will not be initialized at startup and should keep its value after software restart.

Example::

    __NOINIT_ATTR uint32_t noinit_data;

.. _iram:

IRAM (Instruction RAM)
^^^^^^^^^^^^^^^^^^^^^^

.. only:: esp32

    ESP-IDF allocates part of Internal SRAM0 region for instruction RAM. The region is defined in *{IDF_TARGET_NAME} Technical Reference Manual* > *System and Memory* > *Embedded Memory* [`PDF <{IDF_TARGET_TRM_EN_URL}#sysmem>`__]. Except for the first 64 kB block which is used for PRO and APP MMU caches, the rest of this memory range (i.e. from ``0x40080000`` to ``0x400A0000``) is used to store parts of the application which need to run from RAM.

.. only:: esp32s2

    ESP-IDF allocates part of Internal SRAM region for instruction RAM. The region is defined in *{IDF_TARGET_NAME} Technical Reference Manual* > *System and Memory* > *Internal Memory* [`PDF <{IDF_TARGET_TRM_EN_URL}#sysmem>`__]. Except for the first block (up to 32 kB) which is used for MMU cache, the rest of this memory range is used to store parts of application which need to run from RAM.

.. only:: not esp32

    .. note:: Any internal SRAM which is not used for Instruction RAM will be made available as :ref:`dram` for static data and dynamic allocation (heap).

Why place code in IRAM
======================

Cases when parts of application should be placed into IRAM:

- Interrupt handlers must be placed into IRAM if ``ESP_INTR_FLAG_IRAM`` is used when registering the interrupt handler. For more information, see :ref:`iram-safe-interrupt-handlers`.

- Some timing critical code may be placed into IRAM to reduce the penalty associated with loading the code from flash. {IDF_TARGET_NAME} reads code and data from flash via the MMU cache. In some cases, placing a function into IRAM may reduce delays caused by a cache miss and significantly improve that function's performance.

.. _how-to-place-code-in-iram:

How to place code in IRAM
=========================

Some code is automatically placed into the IRAM region using the linker script.

If some specific application code needs to be placed into IRAM, it can be done by using the :doc:`linker-script-generation` feature and adding a linker script fragment file to your component that targets entire source files or functions with the ``noflash`` placement. See the :doc:`linker-script-generation` docs for more information.

Alternatively, it's possible to specify IRAM placement in the source code using the ``IRAM_ATTR`` macro::

	#include "esp_attr.h"

	void IRAM_ATTR gpio_isr_handler(void* arg)
	{
		// ...
	}

There are some possible issues with placement in IRAM, that may cause problems with IRAM-safe interrupt handlers:

* Strings or constants inside an ``IRAM_ATTR`` function may not be placed in RAM automatically. It's possible to use ``DRAM_ATTR`` attributes to mark these, or using the linker script method will cause these to be automatically placed correctly.

  .. code-block:: c

    void IRAM_ATTR gpio_isr_handler(void* arg)
    {
       const static DRAM_ATTR uint8_t INDEX_DATA[] = { 45, 33, 12, 0 };
       const static char *MSG = DRAM_STR("I am a string stored in RAM");
    }

  Note that knowing which data should be marked with ``DRAM_ATTR`` can be hard, the compiler will sometimes recognize that a variable or expression is constant (even if it is not marked ``const``) and optimize it into flash, unless it is marked with ``DRAM_ATTR``.

* GCC optimizations that automatically generate jump tables or switch/case lookup tables place these tables in flash. There are two possible ways to resolve this issue:

  - Use a :doc:`linker script fragment <linker-script-generation>` to mark the entire source file as ``noflash``
  - Pass specific flags to the compiler to disable these optimizations in the relevant source files. For CMake, place the following in the component CMakeLists.txt file:

    .. code-block:: cmake

        set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/relative/path/to/file" PROPERTIES
                                    COMPILE_FLAGS "-fno-jump-tables -fno-tree-switch-conversion")

.. _irom:

IROM (code executed from Flash)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If a function is not explicitly placed into :ref:`iram` or RTC memory, it is placed into flash. The mechanism by which Flash MMU is used to allow code execution from flash is described in *{IDF_TARGET_NAME} Technical Reference Manual* > *Memory Management and Protection Units (MMU, MPU)* [`PDF <{IDF_TARGET_TRM_EN_URL}#mpummu>`__]. As IRAM is limited, most of an application's binary code must be placed into IROM instead.

During :doc:`startup`, the bootloader (which runs from IRAM) configures the MMU flash cache to map the app's instruction code region to the instruction space. Flash accessed via the MMU is cached using some internal SRAM and accessing cached flash data is as fast as accessing other types of internal memory.

RTC fast memory
^^^^^^^^^^^^^^^

The same region of RTC fast memory can be accessed as both instruction and data memory. Code which has to run after wake-up from deep sleep mode has to be placed into RTC memory. Please check detailed description in :doc:`deep sleep <deep-sleep-stub>` documentation.

.. only:: esp32

     RTC fast memory can only be accessed by the PRO CPU.

     In single core mode, remaining RTC fast memory is added to the heap unless the option :ref:`CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP` is disabled. This memory can be used interchangeably with :ref:`DRAM`, but is slightly slower to access and not DMA capable.

.. only:: not esp32

     Remaining RTC fast memory is added to the heap unless the option :ref:`CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP` is disabled. This memory can be used interchangeably with :ref:`DRAM`, but is slightly slower to access.

.. _drom:

DROM (data stored in Flash)
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. highlight:: c

By default, constant data is placed by the linker into a region mapped to the MMU flash cache. This is the same as the :ref:`irom` section, but is for read-only data not executable code.

The only constant data not placed into into this memory type by default are literal constants which are embedded by the compiler into application code. These are placed as the surrounding function's executable instructions.

The ``DRAM_ATTR`` attribute can be used to force constants from DROM into the :ref:`dram` section (see above).

.. only:: SOC_RTC_SLOW_MEM_SUPPORTED

    RTC slow memory
    ^^^^^^^^^^^^^^^

    Global and static variables used by code which runs from RTC memory must be placed into RTC slow memory. For example :doc:`deep sleep <deep-sleep-stub>` variables can be placed here instead of RTC fast memory, or code and variables accessed by the :doc:`/api-guides/ulp`.

    The attribute macro named ``RTC_NOINIT_ATTR`` can be used to place data into this type of memory. The values placed into this section keep their value after waking from deep sleep.

    Example::

        RTC_NOINIT_ATTR uint32_t rtc_noinit_data;

DMA Capable Requirement
^^^^^^^^^^^^^^^^^^^^^^^

.. highlight:: c

Most peripheral DMA controllers (e.g. SPI, sdmmc, etc.) have requirements that sending/receiving buffers should be placed in DRAM
and word-aligned. We suggest to place DMA buffers in static variables rather than in the stack. Use macro ``DMA_ATTR``
to declare global/local static variables like::

    DMA_ATTR uint8_t buffer[]="I want to send something";

    void app_main()
    {
        // initialization code...
        spi_transaction_t temp = {
            .tx_buffer = buffer,
            .length = 8 * sizeof(buffer),
        };
        spi_device_transmit(spi, &temp);
        // other stuff
    }

Or::

    void app_main()
    {
        DMA_ATTR static uint8_t buffer[] = "I want to send something";
        // initialization code...
        spi_transaction_t temp = {
            .tx_buffer = buffer,
            .length = 8 * sizeof(buffer),
        };
        spi_device_transmit(spi, &temp);
        // other stuff
    }

It is also possible to allocate DMA-capable memory buffers dynamically by using the :ref:`MALLOC_CAP_DMA <dma-capable-memory>` capabilities flag.

DMA Buffer in the stack
^^^^^^^^^^^^^^^^^^^^^^^

Placing DMA buffers in the stack is possible but discouraged. If doing so, pay attention to the following:

.. list::

    :SOC_SPIRAM_SUPPORTED: - Placing DRAM buffers on the stack is not recommended if if the stack may be in PSRAM. If the stack of a task is placed in the PSRAM, several steps have to be taken as described in :doc:`external-ram`.
    - Use macro ``WORD_ALIGNED_ATTR`` in functions before variables to place them in proper positions like::

        void app_main()
        {
            uint8_t stuff;
            WORD_ALIGNED_ATTR uint8_t buffer[] = "I want to send something";   //or the buffer will be placed right after stuff.
            // initialization code...
            spi_transaction_t temp = {
                .tx_buffer = buffer,
                .length = 8 * sizeof(buffer),
            };
            spi_device_transmit(spi, &temp);
            // other stuff
        }


