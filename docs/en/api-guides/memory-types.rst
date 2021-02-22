.. _memory-layout:

Memory Types
------------

{IDF_TARGET_NAME} chip has flexible memory mapping features. This section describes how ESP-IDF uses these features by default.

Application code in ESP-IDF can be placed into one of the following memory regions.

.. _iram:

IRAM (instruction RAM)
^^^^^^^^^^^^^^^^^^^^^^

.. only:: esp32

    ESP-IDF allocates part of `Internal SRAM0` region for instruction RAM. The region is defined in *{IDF_TARGET_NAME} Technical Reference Manual* > *System and Memory* > *Embedded Memory* [`PDF <{IDF_TARGET_TRM_EN_URL}#sysmem>`__]. Except for the first 64 kB block which is used for PRO and APP CPU caches, the rest of this memory range (i.e. from ``0x40080000`` to ``0x400A0000``) is used to store parts of application which need to run from RAM.

.. only:: esp32s2

    ESP-IDF allocates part of `Internal SRAM0` region for instruction RAM. The region is defined in *{IDF_TARGET_NAME} Technical Reference Manual* > *System and Memory* > *Internal Memory* [`PDF <{IDF_TARGET_TRM_EN_URL}#sysmem>`__]. Except for the first 64 kB block which is used for PRO and APP CPU caches, the rest of this memory range (i.e. from ``0x40080000`` to ``0x400A0000``) is used to store parts of application which need to run from RAM.

A few components of ESP-IDF and parts of WiFi stack are placed into this region using the linker script.

If some application code needs to be placed into IRAM, it can be done by using the :doc:`linker-script-generation` feature and adding a linker script framgnet file to your project that targets entire source files or functions with the ``noflash`` placement. See the :doc:`linker-script-generation` docs for more information.

Alternatively, it's possible to specify IRAM placement in the source code using the ``IRAM_ATTR`` macro::

	#include "esp_attr.h"

	void IRAM_ATTR gpio_isr_handler(void* arg)
	{
		// ...
	}

This method is less comprehensive than the linker script method (for example: strings inside this function may not be placed in RAM automatically, if GCC optimizations generate a jump table then this will be placed in flash). Using a linker script fragment instead will resolve these issues.

Here are the cases when parts of application may or should be placed into IRAM.

- Interrupt handlers must be placed into IRAM if ``ESP_INTR_FLAG_IRAM`` is used when registering the interrupt handler. In this case, ISR may only call functions placed into IRAM or functions present in ROM. *Note 1:* all FreeRTOS APIs are currently placed into IRAM, so are safe to call from interrupt handlers. If the ISR is placed into IRAM, all constant data used by the ISR and functions called from ISR (including, but not limited to, ``const char`` arrays), must be placed into DRAM using ``DRAM_ATTR``.

- Some timing critical code may be placed into IRAM to reduce the penalty associated with loading the code from flash. {IDF_TARGET_NAME} reads code and data from flash via a 32 kB cache. In some cases, placing a function into IRAM may reduce delays caused by a cache miss.

.. _irom:

IROM (code executed from Flash)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: SOC_SDIO_SLAVE_SUPPORTED

    If a function is not explicitly placed into IRAM or RTC memory, it is placed into flash. The mechanism by which Flash MMU is used to allow code execution from flash is described in *{IDF_TARGET_NAME} Technical Reference Manual* > *Memory Management and Protection Units (MMU, MPU)* [`PDF <{IDF_TARGET_TRM_EN_URL}#mpummu>`__]. ESP-IDF places the code which should be executed from flash starting from the beginning of ``0x400D0000 — 0x40400000`` region. Upon startup, second stage bootloader initializes Flash MMU to map the location in flash where code is located into the beginning of this region. Access to this region is transparently cached using two 32kB blocks in ``0x40070000`` — ``0x40080000`` range.

.. only:: not SOC_SDIO_SLAVE_SUPPORTED

    If a function is not explicitly placed into IRAM or RTC memory, it is placed into flash. The mechanism by which Flash MMU is used to allow code execution from flash is described in `{IDF_TARGET_NAME} Technical Reference Manual <{IDF_TARGET_TRM_EN_URL}>`__. ESP-IDF places the code which should be executed from flash starting from the beginning of ``0x400D0000 — 0x40400000`` region. Upon startup, second stage bootloader initializes Flash MMU to map the location in flash where code is located into the beginning of this region. Access to this region is transparently cached using two 32kB blocks in ``0x40070000`` — ``0x40080000`` range.

Note that the code outside ``0x40000000 — 0x40400000`` region may not be reachable with Window ABI ``CALLx`` instructions, so special care is required if ``0x40400000 — 0x40800000`` or ``0x40800000 — 0x40C00000`` regions are used by the application. ESP-IDF doesn't use these regions by default.

RTC fast memory
^^^^^^^^^^^^^^^

The code which has to run after wake-up from deep sleep mode has to be placed into RTC memory. Please check detailed description in :doc:`deep sleep <deep-sleep-stub>` documentation.

.. _dram:

DRAM (data RAM)
^^^^^^^^^^^^^^^

Non-constant static data and zero-initialized data is placed by the linker into the 256 kB ``0x3FFB0000 — 0x3FFF0000`` region. Note that this region is reduced by 64kB (by shifting start address to ``0x3FFC0000``) if Bluetooth stack is used. Length of this region is also reduced by 16 kB or 32kB if trace memory is used. All space which is left in this region after placing static data there is used for the runtime heap.

Constant data may also be placed into DRAM, for example if it is used in an ISR (see notes in IRAM section above). To do that, ``DRAM_ATTR`` define can be used::

	DRAM_ATTR const char[] format_string = "%p %x";
	char buffer[64];
	sprintf(buffer, format_string, ptr, val);

Needless to say, it is not advised to use ``printf`` and other output functions in ISRs. For debugging purposes, use ``ESP_EARLY_LOGx`` macros when logging from ISRs. Make sure that both ``TAG`` and format string are placed into ``DRAM`` in that case.

The macro ``__NOINIT_ATTR`` can be used as attribute to place data into ``.noinit`` section. The values placed into this section will not be initialized at startup and keep its value after software restart.

Example::

    __NOINIT_ATTR uint32_t noinit_data;

.. _irom:

DROM (data stored in Flash)
^^^^^^^^^^^^^^^^^^^^^^^^^^^

By default, constant data is placed by the linker into a 4 MB region (``0x3F400000 — 0x3F800000``) which is used to access external flash memory via Flash MMU and cache. Exceptions to this are literal constants which are embedded by the compiler into application code.

RTC slow memory
^^^^^^^^^^^^^^^

Global and static variables used by code which runs from RTC memory (i.e. deep sleep stub code) must be placed into RTC slow memory. Please check detailed description in :doc:`deep sleep <deep-sleep-stub>` documentation.

The attribute macro named ``RTC_NOINIT_ATTR`` can be used to place data into this type of memory. The values placed into this section keep their value after waking from deep sleep.

Example::

    RTC_NOINIT_ATTR uint32_t rtc_noinit_data;

DMA Capable Requirement
-----------------------

Most DMA controllers (e.g. SPI, sdmmc, etc.) have requirements that sending/receiving buffers should be placed in DRAM
and word-aligned. We suggest to place DMA buffers in static variables rather than in the stack. Use macro ``DMA_ATTR``
to declare global/local static variables like::

    DMA_ATTR uint8_t buffer[]="I want to send something";

    void app_main()
    {
        // initialization code...
        spi_transaction_t temp = {
            .tx_buffer = buffer,
            .length = 8*sizeof(buffer),
        };
        spi_device_transmit( spi, &temp );
        // other stuff
    }

Or::

    void app_main()
    {
        DMA_ATTR static uint8_t buffer[]="I want to send something";
        // initialization code...
        spi_transaction_t temp = {
            .tx_buffer = buffer,
            .length = 8*sizeof(buffer),
        };
        spi_device_transmit( spi, &temp );
        // other stuff
    }

Placing DMA buffers in the stack is still allowed, though you have to keep in mind:

.. list::

    :SOC_SPIRAM_SUPPORTED: - Never try to do this if the stack is in the pSRAM. If the stack of a task is placed in the pSRAM, several steps have to be taken as described in :doc:`external-ram` (at least ``SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY`` option enabled in the menuconfig). Make sure your task is not in the pSRAM.
    - Use macro ``WORD_ALIGNED_ATTR`` in functions before variables to place them in proper positions like::

        void app_main()
        {
            uint8_t stuff;
            WORD_ALIGNED_ATTR uint8_t buffer[]="I want to send something";   //or the buffer will be placed right after stuff.
            // initialization code...
            spi_transaction_t temp = {
                .tx_buffer = buffer,
                .length = 8*sizeof(buffer),
            };
            spi_device_transmit( spi, &temp );
            // other stuff
        }
