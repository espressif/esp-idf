Power Management
================

Overview
--------

Power management algorithm included in ESP-IDF can adjust APB frequency, CPU frequency, and put the chip into light sleep mode to run the application
at smallest possible power consumption, given the requirements of application components.

Application components can express their requirements by creating and acquiring power management locks.

For instance, a driver for a peripheral clocked from APB can request the APB frequency to be set to 80 MHz, for the duration while the peripheral is used. Another example is that the RTOS will request the CPU to run at the highest configured frequency while there are tasks ready to run. Yet another example is a peripheral driver which needs interrupts to be enabled. Such driver can request light sleep to be disabled.

Naturally, requesting higher APB or CPU frequency or disabling light sleep causes higher current consumption. Components should try to limit usage of power management locks to the shortest amount of time possible.

Configuration
-------------

Power management can be enabled at compile time, using :ref:`CONFIG_PM_ENABLE` option.

Enabling power management features comes at the cost of increased interrupt latency. Extra latency depends on a number of factors, among which are CPU frequency, single/dual core mode, whether frequency switch needs to be performed or not. Minimal extra latency is 0.2us (when CPU frequency is 240MHz, and frequency scaling is not enabled), maximum extra latency is 40us (when frequency scaling is enabled, and a switch from 40MHz to 80MHz is performed on interrupt entry).

Dynamic frequency scaling (DFS) can be enabled in the application by calling :cpp:func:`esp_pm_configure` function. Its argument is a structure defining frequency scaling settings (for ESP32, minimum and maximum CPU frequencies). Alternatively, :ref:`CONFIG_PM_DFS_INIT_AUTO` option can be enabled in menuconfig.  If enabled, maximal CPU frequency is determined by :ref:`CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ` setting, and minimal CPU frequency is set to the XTAL frequency.

.. note::

	:cpp:func:`esp_pm_configure` function also has provisions for enabling automatic light sleep mode. However this feature is not fully supported yet, so `esp_pm_configure` will return an `ESP_ERR_NOT_SUPPORTED` if automatic light sleep is requested.

Power Management Locks
----------------------

As mentioned in the overview, applications can acquire/release locks to control the power management algorithm. When application takes a lock, power management algorithm operation is restricted in a way described below, for each lock. When the lock is released, such restriction is removed.

Different parts of the application can take the same lock. In this case, lock mush be released the same number of times as it was acquired, in order for power managment algorithm to resume.

In ESP32, three types of locks are supported:

``ESP_PM_CPU_FREQ_MAX``
  Requests CPU frequency to be at the maximal value set via :cpp:func:`esp_pm_configure`. For ESP32, this value can be set to 80, 160, or 240MHz.

``ESP_PM_APB_FREQ_MAX``
  Requests APB frequency to be at the maximal supported value. For ESP32, this is 80 MHz.

``ESP_PM_NO_LIGHT_SLEEP``
  Prevents automatic light sleep from being used. Note: currently taking this lock has no effect, as automatic light sleep is never used.


Power Management Algorithm for the ESP32
----------------------------------------

When dynamic frequency scaling is enabled, CPU frequency will be switched as follows:

- If maximal CPU frequency (set using :cpp:func:`esp_pm_configure` or :ref:`CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ`) is 240 MHz:

  1. When ``ESP_PM_CPU_FREQ_MAX`` or ``ESP_PM_APB_FREQ_MAX``  locks are acquired, CPU frequency will be 240 MHz, and APB frequency will be 80 MHz.

  2. Otherwise, frequency will be switched to the minimal value set using :cpp:func:`esp_pm_configure` (usually, XTAL).

- If maximal CPU frequency is 160 MHz:

  1. When ``ESP_PM_CPU_FREQ_MAX`` is acquired, CPU frequency is set to 160 MHz, and APB frequency to 80 MHz.

  2. When ``ESP_PM_CPU_FREQ_MAX`` is not acquired, but ``ESP_PM_APB_FREQ_MAX`` is, CPU and APB frequencies are set to 80 MHz.

  3. Otherwise, frequency will be switched to the minimal value set using :cpp:func:`esp_pm_configure` (usually, XTAL).

- If maximal CPU frequency is 80 MHz:

  1. When ``ESP_PM_CPU_FREQ_MAX`` or ``ESP_PM_APB_FREQ_MAX``  locks are acquired, CPU and APB frequencies will be 80 MHz.

  2. Otherwise, frequency will be switched to the minimal value set using :cpp:func:`esp_pm_configure` (usually, XTAL).


Dynamic Frequency Scaling and Peripheral Drivers
------------------------------------------------

When DFS is enabled, APB frequency can be changed several times within a single RTOS tick. Some peripherals can work normally even when APB frequency changes; some can not.

The following peripherals can work even when APB frequency is changing:

- UART: if REF_TICK is used as clock source (see `use_ref_tick` member of `uart_config_t`).

- LEDC: if REF_TICK is used as clock source (see :cpp:func:`ledc_timer_config` function).

- RMT: if REF_TICK is used as clock source. Currently the driver does not support REF_TICK, but it can be enabled by clearing ``RMT_REF_ALWAYS_ON_CHx`` bit for the respective channel.

Currently, the following peripheral drivers are aware of DFS and will use ``ESP_PM_APB_FREQ_MAX`` lock for the duration of the transaction:

- SPI master

- SDMMC

The following drivers will hold ``ESP_PM_APB_FREQ_MAX`` lock while the driver is enabled:

- SPI slave — between calls to :cpp:func:`spi_slave_initialize` and cpp:func:`spi_slave_free`.

- Ethernet — between calls to :cpp:func:`esp_eth_enable` and :cpp:func:`esp_eth_disable`.

- WiFi — between calls to :cpp:func:`esp_wifi_start` and :cpp:func:`esp_wifi_stop`. If modem sleep is enabled, lock will be released for thte periods of time when radio is disabled.

- Bluetooth — between calls to :cpp:func:`esp_bt_controller_enable` and :cpp:func:`esp_bt_controller_disable`.

The following peripheral drivers are not aware of DFS yet. Applications need to acquire/release locks when necessary:

- I2C

- I2S

- MCPWM

- PCNT

- Sigma-delta

- Timer group


API Reference
-------------

.. include:: /_build/inc/esp_pm.inc
.. include:: /_build/inc/pm.inc

