Power Management
================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

Power management algorithm included in ESP-IDF can adjust the advanced peripheral bus (APB) frequency, CPU frequency, and put the chip into light sleep mode to run an application at smallest possible power consumption, given the requirements of application components.

Application components can express their requirements by creating and acquiring power management locks.

For example:

- Driver for a peripheral clocked from APB can request the APB frequency to be set to 80 MHz while the peripheral is used.
- RTOS can request the CPU to run at the highest configured frequency while there are tasks ready to run.
- A peripheral driver may need interrupts to be enabled, which means it will have to request disabling light sleep.

Since requesting higher APB or CPU frequencies or disabling light sleep causes higher current consumption, please keep the usage of power management locks by components to a minimum.

Configuration
-------------

Power management can be enabled at compile time, using the option :ref:`CONFIG_PM_ENABLE`.

Enabling power management features comes at the cost of increased interrupt latency. Extra latency depends on a number of factors, such as the CPU frequency, single/dual core mode, whether or not frequency switch needs to be done. Minimum extra latency is 0.2 us (when the CPU frequency is 240 MHz and frequency scaling is not enabled). Maximum extra latency is 40 us (when frequency scaling is enabled, and a switch from 40 MHz to 80 MHz is performed on interrupt entry).

Dynamic frequency scaling (DFS) and automatic light sleep can be enabled in an application by calling the function :cpp:func:`esp_pm_configure`. Its argument is a structure defining the frequency scaling settings, :cpp:class:`esp_pm_config_esp32_t`. In this structure, three fields need to be initialized:

- ``max_freq_mhz``: Maximum CPU frequency in MHz, i.e., the frequency used when the ``ESP_PM_CPU_FREQ_MAX`` lock is acquired. This field will usually be set to :ref:`CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ`.
- ``min_freq_mhz``: Minimum CPU frequency in MHz, i.e., the frequency used when only the ``ESP_PM_APB_FREQ_MAX`` lock is acquired. This field can be set to the XTAL frequency value, or the XTAL frequency divided by an integer. Note that 10 MHz is the lowest frequency at which the default REF_TICK clock of 1 MHz can be generated.
- ``light_sleep_enable``: Whether the system should automatically enter light sleep when no locks are acquired (``true``/``false``).

Alternatively, if you enable the option :ref:`CONFIG_PM_DFS_INIT_AUTO` in menuconfig, the maximum CPU frequency will be determined by the :ref:`CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ` setting, and the minimum CPU frequency will be locked to the XTAL frequency.

.. note::

  Automatic light sleep is based on FreeRTOS Tickless Idle functionality. If automatic light sleep is requested while the option :ref:`CONFIG_FREERTOS_USE_TICKLESS_IDLE` is not enabled in menuconfig, :cpp:func:`esp_pm_configure` will return the error `ESP_ERR_NOT_SUPPORTED`.

.. note::

  In light sleep, peripherals are clock gated, and interrupts (from GPIOs and internal peripherals) will not be generated. A wakeup source described in the :doc:`sleep_modes` documentation can be used to trigger wakeup from the light sleep state. For example, the EXT0 and EXT1 wakeup sources can be used to wake up the chip via a GPIO.


Power Management Locks
----------------------

Applications have the ability to acquire/release locks in order to control the power management algorithm. When an application acquires a lock, the power management algorithm operation is restricted in a way described below. When the lock is released, such restrictions are removed.

Power management locks have acquire/release counters. If the lock has been acquired a number of times, it needs to be released the same number of times to remove associated restrictions.

ESP32 supports three types of locks described in the table below.

============================  ======================================================
Lock                          Description
============================  ======================================================
``ESP_PM_CPU_FREQ_MAX``       Requests CPU frequency to be at the maximum value set with :cpp:func:`esp_pm_configure`. For ESP32, this value can be set to 80 MHz, 160 MHz, or 240 MHz.

``ESP_PM_APB_FREQ_MAX``       Requests the APB frequency to be at the maximum supported value. For ESP32, this is 80 MHz.

``ESP_PM_NO_LIGHT_SLEEP``     Disables automatic switching to light sleep.
============================  ======================================================


ESP32 Power Management Algorithm
--------------------------------

The table below shows how CPU and APB frequencies will be switched if dynamic frequency scaling is enabled. You can specify the maximum CPU frequency with either :cpp:func:`esp_pm_configure` or :ref:`CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ`.


+---------------+---------------------------------------+-------------------------------------+
| Max CPU       |            Lock Acquisition           | CPU and APB Frequncies              |
| Frequency Set |                                       |                                     |
+---------------+---------------------------------------+-------------------------------------+
|      240      | | Any of ``ESP_PM_CPU_FREQ_MAX``      |                                     |
|               | | or ``ESP_PM_APB_FREQ_MAX`` acquired | | CPU: 240 MHz                      |
|               |                                       | | APB: 80 MHz                       |
+               +---------------------------------------+-------------------------------------+
|               |                  None                 | Min values for both frequencies set |
|               |                                       | with :cpp:func:`esp_pm_configure`   |
+---------------+---------------------------------------+-------------------------------------+
|      160      | ``ESP_PM_CPU_FREQ_MAX`` acquired      | | CPU: 160 MHz                      |
|               |                                       | | APB: 80 MHz                       |
+               +---------------------------------------+-------------------------------------+
|               | ``ESP_PM_CPU_FREQ_MAX`` acquired,     | | CPU: 80 MHz                       |
|               | ``ESP_PM_APB_FREQ_MAX`` not acquired  | | APB: 80 MHz                       |
+               +---------------------------------------+-------------------------------------+
|               |                  None                 | Min values for both frequencies set |
|               |                                       | with :cpp:func:`esp_pm_configure`   |
+---------------+---------------------------------------+-------------------------------------+
|       80      | | Any of ``ESP_PM_CPU_FREQ_MAX``      | | CPU: 80 MHz                       |
|               | | or ``ESP_PM_APB_FREQ_MAX`` acquired | | APB: 80 MHz                       |
+               +---------------------------------------+-------------------------------------+
|               |                  None                 | Min values for both frequencies set |
|               |                                       | with :cpp:func:`esp_pm_configure`   |
+---------------+---------------------------------------+-------------------------------------+


If none of the locks are acquired, and light sleep is enabled in a call to :cpp:func:`esp_pm_configure`, the system will go into light sleep mode. The duration of light sleep will be determined by:

- FreeRTOS tasks blocked with finite timeouts
- Timers registered with :doc:`High resolution timer <esp_timer>` APIs

Light sleep duration will be chosen to wake up the chip before the nearest event (task being unblocked, or timer elapses).


Dynamic Frequency Scaling and Peripheral Drivers
------------------------------------------------

When DFS is enabled, the APB frequency can be changed multiple times within a single RTOS tick. The APB frequency change does not affect the work of some peripherals, while other peripherals may have issues. For example, Timer Group peripheral timers will keep counting, however, the speed at which they count will change proportionally to the APB frequency.

The following peripherals work normally even when the APB frequency is changing:

- **UART**: if REF_TICK is used as a clock source. See `use_ref_tick` member of :cpp:class:`uart_config_t`.
- **LEDC**: if REF_TICK is used as a clock source. See :cpp:func:`ledc_timer_config` function.
- **RMT**: if REF_TICK is used as a clock source. Although the driver does not support REF_TICK, this feature can be enabled by clearing the ``RMT_REF_ALWAYS_ON_CHx`` bit for the respective channel.

Currently, the following peripheral drivers are aware of DFS and will use the ``ESP_PM_APB_FREQ_MAX`` lock for the duration of the transaction:

- SPI master
- I2C
- I2S (If the APLL clock is used, then it will use the ``ESP_PM_NO_LIGHT_SLEEP`` lock)
- SDMMC

The following drivers will hold the ``ESP_PM_APB_FREQ_MAX`` lock while the driver is enabled:

- **SPI slave**: between calls to :cpp:func:`spi_slave_initialize` and :cpp:func:`spi_slave_free`.
- **Ethernet**: between calls to :cpp:func:`esp_eth_enable` and :cpp:func:`esp_eth_disable`.
- **WiFi**: between calls to :cpp:func:`esp_wifi_start` and :cpp:func:`esp_wifi_stop`. If modem sleep is enabled, the lock will be released for the periods of time when radio is disabled.
- **Bluetooth**: between calls to :cpp:func:`esp_bt_controller_enable` and :cpp:func:`esp_bt_controller_disable`. If Bluetooth modem sleep is enabled, the ``ESP_PM_APB_FREQ_MAX`` lock will be released for the periods of time when radio is disabled. However the ``ESP_PM_NO_LIGHT_SLEEP`` lock will still be held, unless :ref:`CONFIG_BTDM_LOW_POWER_CLOCK` option is set to "External 32kHz crystal".
- **CAN**: between calls to :cpp:func:`can_driver_install` and :cpp:func:`can_driver_uninstall`.

The following peripheral drivers are not aware of DFS yet. Applications need to acquire/release locks themselves, when necessary:

- MCPWM
- PCNT
- Sigma-delta
- Timer group


API Reference
-------------

.. include:: /_build/inc/esp_pm.inc
.. include:: /_build/inc/pm.inc

