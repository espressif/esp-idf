Power Management
================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

Power management algorithm included in ESP-IDF can adjust the advanced peripheral bus (APB) frequency, CPU frequency, and put the chip into Light-sleep mode to run an application at smallest possible power consumption, given the requirements of application components.

Application components can express their requirements by creating and acquiring power management locks.

For example:

- Driver for a peripheral clocked from APB can request the APB frequency to be set to 80 MHz while the peripheral is used.
- RTOS can request the CPU to run at the highest configured frequency while there are tasks ready to run.
- A peripheral driver may need interrupts to be enabled, which means it has to request disabling Light-sleep.

Since requesting higher APB or CPU frequencies or disabling Light-sleep causes higher current consumption, please keep the usage of power management locks by components to a minimum.

Configuration
-------------

Power management can be enabled at compile time, using the option :ref:`CONFIG_PM_ENABLE`.

Enabling power management features comes at the cost of increased interrupt latency. Extra latency depends on a number of factors, such as the CPU frequency, single/dual core mode, whether or not frequency switch needs to be done. Minimum extra latency is 0.2 us (when the CPU frequency is 240 MHz and frequency scaling is not enabled). Maximum extra latency is 40 us (when frequency scaling is enabled, and a switch from 40 MHz to 80 MHz is performed on interrupt entry).

Dynamic frequency scaling (DFS) and automatic Light-sleep can be enabled in an application by calling the function :cpp:func:`esp_pm_configure`. Its argument is a structure defining the frequency scaling settings, :cpp:class:`esp_pm_config_t`. In this structure, three fields need to be initialized:

.. list::

    - ``max_freq_mhz``: Maximum CPU frequency in MHz, i.e., the frequency used when the ``ESP_PM_CPU_FREQ_MAX`` lock is acquired. This field is usually set to the default CPU frequency.

    :esp32 or esp32s2: - ``min_freq_mhz``: Minimum CPU frequency in MHz, indicating the frequency used when not holding the power management lock. Note that 10 MHz is the minimum frequency required for generating a 1 MHz REF_TICK default clock.

    :not esp32 and not esp32s2: - ``min_freq_mhz``: Minimum CPU frequency in MHz, indicating the frequency used when not holding the power management lock.

    - ``light_sleep_enable``: Whether the system should automatically enter Light-sleep when no locks are acquired (``true``/``false``).


  Alternatively, if you enable the option :ref:`CONFIG_PM_DFS_INIT_AUTO` in menuconfig, the maximum CPU frequency will be determined by the :ref:`CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ` setting, and the minimum CPU frequency will be locked to the XTAL frequency.

.. note::

  Automatic Light-sleep is based on FreeRTOS Tickless Idle functionality. If automatic Light-sleep is requested while the option :ref:`CONFIG_FREERTOS_USE_TICKLESS_IDLE` is not enabled in menuconfig, :cpp:func:`esp_pm_configure` will return the error `ESP_ERR_NOT_SUPPORTED`.

.. note::

  In Light-sleep, peripherals are clock gated, and interrupts (from GPIOs and internal peripherals) will not be generated. A wakeup source described in the :doc:`sleep_modes` documentation can be used to trigger wakeup from the Light-sleep state.

.. only:: SOC_PM_SUPPORT_EXT0_WAKEUP or SOC_PM_SUPPORT_EXT1_WAKEUP

  For example, the EXT0 and EXT1 wakeup sources can be used to wake up the chip via a GPIO.


Power Management Locks
----------------------
{IDF_TARGET_MAX_CPU_FREQ: default="Not updated yet", esp32="80 MHz, 160 MHz, or 240 MHz", esp32s2="80 MHz, 160 MHz, or 240 MHz", esp32s3="80 MHz, 160 MHz, or 240 MHz", esp32c2="80 MHz or 120 MHz", esp32c3="80 MHz or 160 MHz", esp32c6="80 MHz or 160 MHz", esp32p4="360 MHz", esp32c5="80 MHz, 160 MHz or 240 MHz", esp32c61="80 MHz or 160 MHz"}

Applications have the ability to acquire/release locks in order to control the power management algorithm. When an application acquires a lock, the power management algorithm operation is restricted in a way described below. When the lock is released, such restrictions are removed.

Power management locks have acquire/release counters. If the lock has been acquired a number of times, it needs to be released the same number of times to remove associated restrictions.

{IDF_TARGET_NAME} supports three types of locks described in the table below.

.. list-table::
  :header-rows: 1
  :widths: 25 60

  * - Lock
    - Description
  * - ``ESP_PM_CPU_FREQ_MAX``
    - Requests CPU frequency to be at the maximum value set with :cpp:func:`esp_pm_configure`. For {IDF_TARGET_NAME}, this value can be set to {IDF_TARGET_MAX_CPU_FREQ}.
  * - ``ESP_PM_APB_FREQ_MAX``
    - Requests the APB frequency to be at the maximum supported value. For {IDF_TARGET_NAME}, this is 80 MHz.
  * - ``ESP_PM_NO_LIGHT_SLEEP``
    - Disables automatic switching to Light-sleep.

{IDF_TARGET_NAME} Power Management Algorithm
---------------------------------------

The table below shows how CPU and APB frequencies will be switched if dynamic frequency scaling is enabled. You can specify the maximum CPU frequency with either :cpp:func:`esp_pm_configure` or :ref:`CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ`.

.. include:: inc/power_management_{IDF_TARGET_PATH_NAME}.rst

If none of the locks are acquired, and Light-sleep is enabled in a call to :cpp:func:`esp_pm_configure`, the system will go into Light-sleep mode. The duration of Light-sleep will be determined by:

- FreeRTOS tasks blocked with finite timeouts
- Timers registered with :doc:`High resolution timer <esp_timer>` APIs

Light-sleep duration is chosen to wake up the chip before the nearest event (task being unblocked, or timer elapses).

To skip unnecessary wake-up, you can consider initializing an ``esp_timer`` with the ``skip_unhandled_events`` option as ``true``. Timers with this flag will not wake up the system and it helps to reduce consumption.

Automatic Light-sleep Time Compensation Mechanism
-------------------------------------------------

ESP-IDF uses a predictive time compensation mechanism for automatic Light-sleep. The system measures the actual wakeup overhead after each Light-sleep cycle and uses this measurement to predict the wakeup overhead for the next sleep cycle.

The system calculates sleep duration based on the next scheduled event and subtracts the predicted wakeup overhead (from the previous cycle) to set the wakeup timer. After wakeup, since FreeRTOS systick interrupts are suspended during sleep, the system needs to call :cpp:func:`vTaskStepTick()` to compensate for the ticks elapsed during sleep, maintaining the accuracy of FreeRTOS tick count. Meanwhile, it measures the actual overhead and stores it for the next prediction, creating a feedback loop that adapts to system behavior.

However, actual overhead can vary due to cache misses, CPU frequency changes, flash latency variations, or hardware state restoration time. When actual overhead exceeds prediction, the actual sleep time may exceed the expected value, causing :cpp:func:`vTaskStepTick()` to receive an excessive tick compensation value, triggering assertion failure.

The :ref:`CONFIG_PM_LIGHTSLEEP_TICK_OVERFLOW_PROTECTION` option provides a safety mechanism to prevent assertion failures when wakeup overhead exceeds prediction. When enabled, the system limits the tick compensation value to prevent overflow. This option can be enabled in menuconfig via ``Component config`` > ``Power Management`` > ``Enable light sleep tick overflow protection``.

When enabled, this option handles oversleep as follows:
- If oversleep is within tolerance (configurable via :ref:`CONFIG_PM_LIGHTSLEEP_TICK_OVERFLOW_TOLERANCE`, default: 2 ticks), the system silently limits ``slept_ticks`` to ``xExpectedIdleTime``, preventing assertion failure
- If oversleep exceeds tolerance (may indicate a bug), the system does not limit ticks, logs an error message, and assertion failure will occur
- In rare edge cases, it may lose ticks, causing FreeRTOS tick count (``xTickCount``) to lag behind real time (``esp_timer``). Tasks using :cpp:func:`vTaskDelay()` may experience slightly longer delays than expected, and FreeRTOS software timers may have reduced accuracy.

When disabled (default), the system provides accurate tick compensation with better precision for time-critical applications. In edge cases, if wakeup overhead estimation is insufficient causing Light-sleep oversleep, assertion failure and system crash may occur.

It is recommended to keep this option disabled by default to maintain tick accuracy. Enable it only when you experience assertion failures related to :cpp:func:`vTaskStepTick()` and can accept slight inaccuracy of RTOS tick time compared to real time.

Debugging and Profiling
-----------------------

The power management subsystem provides several functions to help debug and profile power management lock usage in applications:

- :cpp:func:`esp_pm_dump_locks` - Dumps a list of all currently created locks to a specified stream, showing their types, names, and current acquisition status.
- :cpp:func:`esp_pm_get_lock_stats_all` - Retrieves statistics for all PM lock types, including the number of locks created and the number currently acquired.
- :cpp:func:`esp_pm_lock_get_stats` - Gets detailed statistics for a specific lock instance, including acquisition count and (if profiling is enabled) the number of times taken and total time held.

These functions are particularly useful for:

1. Identifying leaks where locks are acquired but never released
2. Understanding which components are preventing power savings
3. Optimizing power consumption by analyzing lock usage patterns
4. Debugging issues related to lock management in applications

To enable profiling features (timing information for individual locks), enable the :ref:`CONFIG_PM_PROFILING` option in menuconfig.

Dynamic Frequency Scaling and Peripheral Drivers
------------------------------------------------

When DFS is enabled, the APB frequency can be changed multiple times within a single RTOS tick. The APB frequency change does not affect the operation of some peripherals, while other peripherals may have issues. For example, Timer Group peripheral timers keeps counting, however, the speed at which they count changes proportionally to the APB frequency.

Peripheral clock sources such as ``REF_TICK``, ``XTAL``, ``RC_FAST`` (i.e., ``RTC_8M``), their frequencies will not be inflenced by APB frequency. And therefore, to ensure the peripheral behaves consistently during DFS, it is recommended to select one of these clocks as the peripheral clock source. For more specific guidelines, please refer to the "Power Management" section of each peripheral's "API Reference > Peripherals API" page.

Currently, the following peripheral drivers are aware of DFS and use the ``ESP_PM_APB_FREQ_MAX`` lock for the duration of the transaction:

.. list::

  - SPI master
  - I2C
  :SOC_I2S_HW_VERSION_1 or not SOC_I2S_SUPPORTS_APLL: - I2S
  :not SOC_I2S_HW_VERSION_1 and SOC_I2S_SUPPORTS_APLL: - I2S (if the APLL clock is used, then it will use the ``ESP_PM_NO_LIGHT_SLEEP`` lock)
  - SDMMC

The following drivers hold the ``ESP_PM_APB_FREQ_MAX`` lock while the driver is enabled:

.. list::

    - **SPI slave**: between calls to :cpp:func:`spi_slave_initialize` and :cpp:func:`spi_slave_free`.
    - **GPTimer**: between calls to :cpp:func:`gptimer_enable` and :cpp:func:`gptimer_disable`.
    - **Ethernet**: between calls to :cpp:func:`esp_eth_driver_install` and :cpp:func:`esp_eth_driver_uninstall`.
    :SOC_WIFI_SUPPORTED: - **WiFi**: between calls to :cpp:func:`esp_wifi_start` and :cpp:func:`esp_wifi_stop`. If modem sleep is enabled, the lock will be released for the periods of time when radio is disabled.
    :SOC_TWAI_SUPPORTED: - **TWAI**: between calls to :cpp:func:`twai_driver_install` and :cpp:func:`twai_driver_uninstall` (only when the clock source is set to :cpp:enumerator:`TWAI_CLK_SRC_APB`).
    :SOC_BT_SUPPORTED and esp32: - **Bluetooth**: between calls to :cpp:func:`esp_bt_controller_enable` and :cpp:func:`esp_bt_controller_disable`. If Bluetooth Modem-sleep is enabled, the ``ESP_PM_APB_FREQ_MAX`` lock will be released for the periods of time when radio is disabled. However the ``ESP_PM_NO_LIGHT_SLEEP`` lock will still be held, unless :ref:`CONFIG_BTDM_CTRL_LOW_POWER_CLOCK` option is set to "External 32 kHz crystal".
    :SOC_BT_SUPPORTED and not esp32: - **Bluetooth**: between calls to :cpp:func:`esp_bt_controller_enable` and :cpp:func:`esp_bt_controller_disable`. If Bluetooth Modem-sleep is enabled, the ``ESP_PM_APB_FREQ_MAX`` lock will be released for the periods of time when radio is disabled. However the ``ESP_PM_NO_LIGHT_SLEEP`` lock will still be held.
    :SOC_PCNT_SUPPORTED: - **PCNT**: between calls to :cpp:func:`pcnt_unit_enable` and :cpp:func:`pcnt_unit_disable`.
    :SOC_SDM_SUPPORTED: - **Sigma-delta**: between calls to :cpp:func:`sdm_channel_enable` and :cpp:func:`sdm_channel_disable`.
    :SOC_MCPWM_SUPPORTED: - **MCPWM**: between calls to :cpp:func:`mcpwm_timer_enable` and :cpp:func:`mcpwm_timer_disable`, as well as :cpp:func:`mcpwm_capture_timer_enable` and :cpp:func:`mcpwm_capture_timer_disable`.

.. only:: SOC_PM_SUPPORT_TOP_PD

    Light-sleep Peripheral Power Down
    ---------------------------------

        {IDF_TARGET_NAME} supports power-down peripherals during Light-sleep.

        If :ref:`CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP` is enabled, when the driver initializes the peripheral, the driver will register the working register context of the peripheral to the sleep retention link. Before entering sleep, the ``REG_DMA`` peripheral reads the configuration in the sleep retention link, and back up the register context to memory according to the configuration. ``REG_DMA`` also restores context from memory to peripheral registers on wakeup.

        Currently ESP-IDF supports Light-sleep context retention for the following peripherals. Their context is automatically restored, or they provide some option for the user to enable this feature and goes into peripheral power down mode.

        .. list::

            - INT_MTX
            - TEE/APM
            - IO_MUX / GPIO
            - MSPI (SPI0/1)
            - SYSTIMER
            :SOC_TIMER_SUPPORT_SLEEP_RETENTION: - GPTimer
            :SOC_RMT_SUPPORT_SLEEP_RETENTION: - RMT
            :SOC_ETM_SUPPORT_SLEEP_RETENTION: - ETM
            :SOC_LEDC_SUPPORT_SLEEP_RETENTION: - LEDC
            :SOC_I2C_SUPPORT_SLEEP_RETENTION: - I2C
            :SOC_I2S_SUPPORT_SLEEP_RETENTION: - I2S
            :SOC_MCPWM_SUPPORT_SLEEP_RETENTION: - MCPWM
            :SOC_UART_SUPPORT_SLEEP_RETENTION: - All UARTs
            :SOC_TEMPERATURE_SENSOR_SUPPORT_SLEEP_RETENTION: - Temperature Sensor
            :SOC_TWAI_SUPPORT_SLEEP_RETENTION: - All TWAIs
            :SOC_PARLIO_SUPPORT_SLEEP_RETENTION: - PARL_IO
            :SOC_SPI_SUPPORT_SLEEP_RETENTION: - All GPSPIs
            :SOC_EMAC_SUPPORT_SLEEP_RETENTION: - EMAC

        Some peripherals haven't support Light-sleep context retention, or it cannot survive from the register lose. They will prevent the power-down of peripherals even when the feature is enabled.

        .. list::

            :SOC_SDIO_SLAVE_SUPPORTED: - SDIO Slave
            :SOC_PCNT_SUPPORTED: - PCNT

        The following peripherals (and those not listed in any group of this section) are not yet supported. If your application uses these peripherals, they may not work well after waking up from sleep.

        .. list::

            - ASSIST_DEBUG
            - Trace
            - Crypto: AES/ECC/HMAC/RSA/SHA/DS/XTA_AES/ECDSA
            - USB-Serial-JTAG
            - SARADC

        .. note::

            When the peripheral power domain is powered down during sleep, both the IO_MUX and GPIO modules are inactive, meaning the chip pins' state is not maintained by these modules. To preserve the state of an IO during sleep, it's essential to call :cpp:func:`gpio_hold_dis` and :cpp:func:`gpio_hold_en` before and after configuring the GPIO state. This action ensures that the IO configuration is latched and prevents the IO from becoming floating while in sleep mode.


API Reference
-------------

.. include-build-file:: inc/esp_pm.inc
