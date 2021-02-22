Application Startup Flow
========================
:link_to_translation:`zh_CN:[中文]`

{IDF_TARGET_BOOTLOADER_OFFSET:default="0x0", esp32="0x1000", esp32s2="0x1000"}

This note explains various steps which happen before ``app_main`` function of an ESP-IDF application is called.

The high level view of startup process is as follows:

1. First-stage bootloader in ROM loads second-stage bootloader image to RAM (IRAM & DRAM) from flash offset {IDF_TARGET_BOOTLOADER_OFFSET}.
2. Second-stage bootloader loads partition table and main app image from flash. Main app incorporates both RAM segments and read-only segments mapped via flash cache.
3. Main app image executes. At this point the second CPU and RTOS scheduler can be started.

This process is explained in detail in the following sections.

First stage bootloader
^^^^^^^^^^^^^^^^^^^^^^

.. only:: not CONFIG_FREERTOS_UNICORE

    After SoC reset, PRO CPU will start running immediately, executing reset vector code, while APP CPU will be held in reset. During startup process, PRO CPU does all the initialization. APP CPU reset is de-asserted in the ``call_start_cpu0`` function of application startup code. Reset vector code is located in the mask ROM of the {IDF_TARGET_NAME} chip and cannot be modified.

.. only:: CONFIG_FREERTOS_UNICORE

    After SoC reset, the CPU will start running immediately to perform initialization. The reset vector code is located in the mask ROIm of the {IDF_TARGET_NAME} chip and cannot be modified.

Startup code called from the reset vector determines the boot mode by checking ``GPIO_STRAP_REG`` register for bootstrap pin states. Depending on the reset reason, the following takes place:

1. Reset from deep sleep: if the value in ``RTC_CNTL_STORE6_REG`` is non-zero, and CRC value of RTC memory in ``RTC_CNTL_STORE7_REG`` is valid, use ``RTC_CNTL_STORE6_REG`` as an entry point address and jump immediately to it. If ``RTC_CNTL_STORE6_REG`` is zero, or ``RTC_CNTL_STORE7_REG`` contains invalid CRC, or once the code called via ``RTC_CNTL_STORE6_REG`` returns, proceed with boot as if it was a power-on reset. **Note**: to run customized code at this point, a deep sleep stub mechanism is provided. Please see :doc:`deep sleep <deep-sleep-stub>` documentation for this.

2. For power-on reset, software SOC reset, and watchdog SOC reset: check the ``GPIO_STRAP_REG`` register if a custom boot mode (such as UART Download Mode) is requested. If this is the case, this custom loader mode is executed from ROM. Otherwise, proceed with boot as if it was due to software CPU reset. Consult {IDF_TARGET_NAME} datasheet for a description of SoC boot modes and how to execute them.

3. For software CPU reset and watchdog CPU reset: configure SPI flash based on EFUSE values, and attempt to load the code from flash. This step is described in more detail in the next paragraphs.

.. note::

    During normal boot modes the RTC watchdog is enabled when this happens, so if the process is interrupted or stalled then the watchdog will reset the SOC automatically and repeat the boot process. This may cause the SoC to strap into a new boot mode, if the stapping GPIOs have changed.

.. only:: esp32

    Second stage bootloader binary image is loaded from flash starting at address 0x1000. If :doc:`secure-boot-v1` is in use then the first 4kB sector of flash is used to store secure boot IV and digest of the bootloader image. Otherwise, this sector is unused.

.. only:: esp32s2

    Second stage bootloader binary image is loaded from flash starting at address 0x1000. The 4kB sector of flash before this address is unused.

.. only:: not (esp32 or esp32s2)

    SAecond stage bootloader binary image is loaded from the start of flash at offset 0x0.

.. TODO: describe application binary image format, describe optional flash configuration commands.

Second stage bootloader
^^^^^^^^^^^^^^^^^^^^^^^

In ESP-IDF, the binary image which resides at offset {IDF_TARGET_BOOTLOADER_OFFSET} in flash is the second stage bootloader. Second stage bootloader source code is available in :idf_file:`components/bootloader` directory of ESP-IDF. Second stage bootloader is used in ESP-IDF to add flexibility to flash layout (using partition tables), and allow for various flows associated with flash encryption, secure boot, and over-the-air updates (OTA) to take place.

When the first stage bootloader is finished checking and loading the second stage bootloader, it jumps to the second stage bootloader entry point found in the binary image header.

Second stage bootloader reads the partition table found by default at offset 0x8000 (:ref:`<CONFIG_PARTITION_TABLE_OFFSET> configurable value`). See :doc:`partition tables <partition-tables>` documentation for more information. The bootloader finds factory and OTA app partitions. If OTA app partitions are found in the partition table, the bootloader consults the ``otadata`` partition to determine which one should be booted. See :doc:`/api-reference/system/ota` for more information.

For a full description of the configuration options available for the ESP-IDF bootloader, see :doc:`bootloader`.

For the selected partition, second stage bootloader reads the binary image from flash one segment at a time:

- For segments with load addresses in internal :ref:`iram` or :ref:`dram`, the contents are copied from flash to the load address.
- For segments which have load addresses in :ref:`drom` or :ref:`irom` regions, the3 flash MMU is configured to provide the correct mapping from the flash to the load address.

.. only:: not CONFIG_FREERTOS_UNICORE

    Note that the second stage bootloader configures flash MMU for both PRO and APP CPUs, but it only enables flash MMU for PRO CPU. Reason for this is that second stage bootloader code is loaded into the memory region used by APP CPU cache. The duty of enabling cache for APP CPU is passed on to the application.

Once all segments are processed - meaning code is loaded and flash MMU is set up, second stage bootloader verifies the integrity of the application and then jumps to the application entry point found in the binary image header.

Application startup
^^^^^^^^^^^^^^^^^^^

ESP-IDF application entry point is ``call_start_cpu0`` function found in :idf_file:`components/esp_system/port/cpu_start.c`. This function is executed by the second stage bootloader, and never returns.

Two main things this function does are to enable heap allocator and to make APP CPU jump to its entry point, ``call_start_cpu1``. The code on PRO CPU sets the entry point for APP CPU, de-asserts APP CPU reset, and waits for a global flag to be set by the code running on APP CPU, indicating that it has started. Once this is done, PRO CPU jumps to ``start_cpu0`` function, and APP CPU jumps to ``start_cpu1`` function.

Both ``start_cpu0`` and ``start_cpu1`` are weak functions, meaning that they can be overridden in the application, if some application-specific change to initialization sequence is needed. Default implementation of ``start_cpu0`` enables or initializes components depending on choices made in ``menuconfig``. Please see source code of this function in ``components/{IDF_TARGET_PATH_NAME}/cpu_start.c`` for an up to date list of steps performed. Note that any C++ global constructors present in the application will be called at this stage. Once all essential components are initialized, *main task* is created and FreeRTOS scheduler is started.

While PRO CPU does initialization in ``start_cpu0`` function, APP CPU spins in ``start_cpu1`` function, waiting for the scheduler to be started on the PRO CPU. Once the scheduler is started on the PRO CPU, code on the APP CPU starts the scheduler as well.

Main task is the task which runs ``app_main`` function. Main task stack size and priority can be configured in ``menuconfig``. Application can use this task for initial application-specific setup, for example to launch other tasks. Application can also use main task for event loops and other general purpose activities. If ``app_main`` function returns, main task is deleted.
