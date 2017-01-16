General Notes About ESP-IDF Programming
=======================================

Application startup flow
------------------------

This note explains various steps which happen before ``app_main`` function of an ESP-IDF application is called.

The high level view of startup process is as follows:

1. First-stage bootloader in ROM loads second-stage bootloader image to RAM (IRAM & DRAM) from flash offset 0x1000.
2. Second-stage bootloader loads partition table and main app image from flash. Main app incorporates both RAM segments and read-only segments mapped via flash cache.
3. Main app image executes. At this point the second CPU and RTOS scheduler can be started.

This process is explained in detail in the following sections.

First stage bootloader
^^^^^^^^^^^^^^^^^^^^^^

After SoC reset, PRO CPU will start running immediately, executing reset vector code, while APP CPU will be held in reset. During startup process, PRO CPU does all the initialization. APP CPU reset is de-asserted in the ``call_start_cpu0`` function of application startup code. Reset vector code is located at address 0x40000400 in the mask ROM of the ESP32 chip and can not be modified.

Startup code called from the reset vector determines the boot mode by checking ``GPIO_STRAP_REG`` register for bootstrap pin states. Depending on the reset reason, the following takes place:

1. Reset from deep sleep: if the value in ``RTC_CNTL_STORE6_REG`` is non-zero, and CRC value of RTC memory in ``RTC_CNTL_STORE7_REG`` is valid, use ``RTC_CNTL_STORE6_REG`` as an entry point address and jump immediately to it. If ``RTC_CNTL_STORE6_REG`` is zero, or ``RTC_CNTL_STORE7_REG`` contains invalid CRC, or once the code called via ``RTC_CNTL_STORE6_REG`` returns, proceed with boot as if it was a power-on reset. **Note**: to run customized code at this point, a deep sleep stub mechanism is provided. Please see :doc:`deep sleep <deep-sleep-stub>` documentation for this.

2. For power-on reset, software SOC reset, and watchdog SOC reset: check the ``GPIO_STRAP_REG`` register if UART or SDIO download mode is requested. If this is the case, configure UART or SDIO, and wait for code to be downloaded. Otherwise, proceed with boot as if it was due to software CPU reset.

3. For software CPU reset and watchdog CPU reset: configure SPI flash based on EFUSE values, and attempt to load the code from flash. This step is described in more detail in the next paragraphs. If loading code from flash fails, unpack BASIC interpreter into the RAM and start it. Note that RTC watchdog is still enabled when this happens, so unless any input is received by the interpreter, watchdog will reset the SOC in a few hundred milliseconds, repeating the whole process. If the interpreter receives any input from the UART, it disables the watchdog.

Application binary image is loaded from flash starting at address 0x1000. First 4kB sector of flash is used to store secure boot IV and signature of the application image. Please check secure boot documentation for details about this. 

.. TODO: describe application binary image format, describe optional flash configuration commands.

Second stage bootloader
^^^^^^^^^^^^^^^^^^^^^^^

In ESP-IDF, the binary image which resides at offset 0x1000 in flash is the second stage bootloader. Second stage bootloader source code is available in components/bootloader directory of ESP-IDF. Note that this arrangement is not the only one possible with the ESP32 chip. It is possible to write a fully featured application which would work when flashed to offset 0x1000, but this is out of scope of this document. Second stage bootloader is used in ESP-IDF to add flexibility to flash layout (using partition tables), and allow for various flows associated with flash encryption, secure boot, and over-the-air updates (OTA) to take place.

When the first stage bootloader is finished checking and loading the second stage bootloader, it jumps to the second stage bootloader entry point found in the binary image header.

Second stage bootloader reads the partition table found at offset 0x8000. See :doc:`partition tables <partition-tables>` documentation for more information. The bootloader finds factory and OTA partitions, and decides which one to boot based on data found in *OTA info* partition. 

For the selected partition, second stage bootloader copies data and code sections which are mapped into IRAM and DRAM to their load addresses. For sections which have load addresses in DROM and IROM regions, flash MMU is configured to provide the correct mapping. Note that the second stage bootloader configures flash MMU for both PRO and APP CPUs, but it only enables flash MMU for PRO CPU. Reason for this is that second stage bootloader code is loaded into the memory region used by APP CPU cache. The duty of enabling cache for APP CPU is passed on to the application. Once code is loaded and flash MMU is set up, second stage bootloader jumps to the application entry point found in the binary image header.

Currently it is not possible to add application-defined hooks to the bootloader to customize application partition selection logic. This may be required to load different application image depending on a state of a GPIO, for example. Such customization features will be added to ESP-IDF in the future. For now, bootloader can be customized by copying bootloader component into application directory and making necessary changes there. ESP-IDF build system will compile the component in application directory instead of ESP-IDF components directory in this case.

Application startup
^^^^^^^^^^^^^^^^^^^

ESP-IDF application entry point is ``call_start_cpu0`` function found in ``components/esp32/cpu_start.c``. Two main things this function does are to enable heap allocator and to make APP CPU jump to its entry point, ``call_start_cpu1``. The code on PRO CPU sets the entry point for APP CPU, de-asserts APP CPU reset, and waits for a global flag to be set by the code running on APP CPU, indicating that it has started. Once this is done, PRO CPU jumps to ``start_cpu0`` function, and APP CPU jumps to ``start_cpu1`` function.

Both ``start_cpu0`` and ``start_cpu1`` are weak functions, meaning that they can be overridden in the application, if some application-specific change to initialization sequence is needed. Default implementation of ``start_cpu0`` enables or initializes components depending on choices made in ``menuconfig``. Please see source code of this function in ``components/esp32/cpu_start.c`` for an up to date list of steps performed. Note that any C++ global constructors present in the application will be called at this stage. Once all essential components are initialized, *main task* is created and FreeRTOS scheduler is started. 

While PRO CPU does initialization in ``start_cpu0`` function, APP CPU spins in ``start_cpu1`` function, waiting for the scheduler to be started on the PRO CPU. Once the scheduler is started on the PRO CPU, code on the APP CPU starts the scheduler as well.

Main task is the task which runs ``app_main`` function. Main task stack size and priority can be configured in ``menuconfig``. Application can use this task for initial application-specific setup, for example to launch other tasks. Application can also use main task for event loops and other general purpose activities. If ``app_main`` function returns, main task is deleted.


Application memory layout
-------------------------

ESP32 chip has flexible memory mapping features. This section describes how ESP-IDF uses these features by default.

Application code in ESP-IDF can be placed into one of the following memory regions.

IRAM (instruction RAM)
^^^^^^^^^^^^^^^^^^^^^^

ESP-IDF allocates part of `Internal SRAM0` region (defined in the Technical Reference Manual) for instruction RAM. Except for the first 64 kB block which is used for PRO and APP CPU caches, the rest of this memory range (i.e. from ``0x40080000`` to ``0x400A0000``) is used to store parts of application which need to run from RAM.

A few components of ESP-IDF and parts of WiFi stack are placed into this region using the linker script.

If some application code needs to be placed into IRAM, it can be done using ``IRAM_ATTR`` define::

	#include "esp_attr.h"
	
	void IRAM_ATTR gpio_isr_handler(void* arg)
	{
		// ...		
	}

Here are the cases when parts of application may or should be placed into IRAM.

- ISR handlers must always be placed into IRAM. Furthermore, ISR handlers may only call functions placed into IRAM or functions present in ROM. *Note 1:* all FreeRTOS APIs are currently placed into IRAM, so are safe to call from ISR handlers. *Note 1:* all constant data used by ISR handlers and functions called from ISR handlers (including, but not limited to, ``const char`` arrays), must be placed into DRAM using ``DRAM_ATTR``.

- Some timing critical code may be placed into IRAM to reduce the penalty associated with loading the code from flash. ESP32 reads code and data from flash via a 32 kB cache. In some cases, placing a function into IRAM may reduce delays caused by a cache miss.

IROM (code executed from Flash)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If a function is not explicitly placed into IRAM or RTC memory, it is placed into flash. The mechanism by which Flash MMU is used to allow code execution from flash is described in the Technical Reference Manual. ESP-IDF places the code which should be executed from flash starting from the beginning of ``0x400D0000 — 0x40400000`` region. Upon startup, second stage bootloader initializes Flash MMU to map the location in flash where code is located into the beginning of this region. Access to this region is transparently cached using two 32kB blocks in ``0x40070000`` — ``0x40080000`` range.

Note that the code outside ``0x40000000 — 0x40400000`` region may not be reachable with Window ABI ``CALLx`` instructions, so special care is required if ``0x40400000 — 0x40800000`` or ``0x40800000 — 0x40C00000`` regions are used by the application. ESP-IDF doesn't use these regions by default.

RTC fast memory
^^^^^^^^^^^^^^^

The code which has to run after wake-up from deep sleep mode has to be placed into RTC memory. Please check detailed description in :doc:`deep sleep <deep-sleep-stub>` documentation.

DRAM (data RAM)
^^^^^^^^^^^^^^^

Non-constant static data and zero-initialized data is placed by the linker into the 256 kB ``0x3FFB0000 — 0x3FFF0000`` region. Note that this region is reduced by 64kB (by shifting start address to ``0x3FFC0000``) if Bluetooth stack is used. Length of this region is also reduced by 16 kB or 32kB if trace memory is used. All space which is left in this region after placing static data there is used for the runtime heap.

Constant data may also be placed into DRAM, for example if it is used in an ISR handler (see notes in IRAM section above). To do that, ``DRAM_ATTR`` define can be used::

	DRAM_ATTR const char[] format_string = "%p %x";
	char buffer[64];
	sprintf(buffer, format_string, ptr, val);

Needless to say, it is not advised to use ``printf`` and other output functions in ISR handlers. For debugging purposes, use ``ESP_EARLY_LOGx`` macros when logging from ISR handlers. Make sure that both ``TAG`` and format string are placed into ``DRAM`` in that case.

DROM (data stored in Flash)
^^^^^^^^^^^^^^^^^^^^^^^^^^^

By default, constant data is placed by the linker into a 4 MB region (``0x3F400000 — 0x3F800000``) which is used to access external flash memory via Flash MMU and cache. Exceptions to this are literal constants which are embedded by the compiler into application code.

RTC slow memory
^^^^^^^^^^^^^^^

Global and static variables used by code which runs from RTC memory (i.e. deep sleep stub code) must be placed into RTC slow memory. Please check detailed description in :doc:`deep sleep <deep-sleep-stub>` documentation.




