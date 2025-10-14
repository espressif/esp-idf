Introduction to Low Power Mode for Systemic Power Management
==============================================================

:link_to_translation:`zh_CN:[中文]`

The {IDF_TARGET_NAME} supports various low power modes. From a systemic perspective on power management, the typical modes include DFS, Light-sleep mode, and Deep-sleep mode. These modes reduce power consumption by lowering clock frequencies (DFS) or entering sleep states without affecting system functionality. During sleep, unnecessary power domains are shut down, or clock gating is applied to peripherals not in use. Sleep modes are further classified into Light-sleep mode and Deep-sleep mode based on whether powering down domains would disrupt program execution context.

.. only:: SOC_WIFI_SUPPORTED or SOC_BLE_SUPPORTED

  Furthermore, for common use cases of the {IDF_TARGET_NAME} such as Wi-Fi/Bluetooth operation, ESP-IDF segment the modes above and optimize them specifically, which will be introduced in subsequent sections.

This section will first introduce low power modes from a systemic perspective, without considering specific use cases.

DFS
------

Dynamic Frequency Scaling (DFS) is a fundamental feature of the power management mechanism integrated into ESP-IDF. DFS adjusts the Advanced Peripheral Bus (APB) frequency and CPU frequency based on the application's holding of power locks. When holding a high-performance lock, it utilizes high frequency, while in idle states without holding power locks, it switches to low frequency to reduce power consumption, thereby minimizing the power consumption of running applications as much as possible.

The frequency adjustment mechanism of DFS operates based on the maximum frequency demand dictated by held power locks. Additionally, the values of :ref:`CONFIG_FREERTOS_HZ` also influence the frequency adjustments of DFS. Higher values lead to a higher frequency of task scheduling, then the system can also more quickly re-adjust the clock frequencies according to the system requirements. For further details regarding the frequency adjustment mechanism, please refer to :doc:`Power Management <../../api-reference/system/power_management>`.

The following graph illustrates the ideal current situation during the operation of the DFS mechanism.

.. code-block:: text

                        Hold CPU and APB MAX lock
                                  │
                                  │    Release CPU MAX lock
                         ▲        │      /
                Current  │        ▼     /
                         │   ──────────┐       Release APB MAX lock
                         │             │           /
                         │             │          /
                         │             └─────────┐
                         │             ▲         │
                         │             │         │
                         │   m-th tick │         └───────────
                         │                       ▲
                         │                       │
                         │             n-th tick │
                         └──────────────────────────────────────►
                                                            Time
              Ideal DFS Mechanism Frequency Adjustment Current Graph

DFS is suitable for scenarios where the CPU must remain active but low power consumption is required. Therefore, DFS is often activated with other low power modes, as will be detailed in the following sections.

.. _Light-sleep Mode:

Light-sleep Mode
---------------------

Light-sleep mode is a low power mode preset in the {IDF_TARGET_NAME}. Users can switch to Light-sleep mode by calling :cpp:func:`esp_light_sleep_start` interface. Upon entering sleep, the chip will shut down unnecessary power domains and apply clock gating to modules not in use, based on the current operational states of peripherals. The {IDF_TARGET_NAME} supports various wake-up sources. Please refer to :doc:`Sleep Modes <../../api-reference/system/sleep_modes>` for more information. When the chip wakes up from Light-sleep mode, the CPU continues running from the context it was in before entering sleep, and the operational states of peripherals remain unaffected. To effectively reduce chip power consumption under Light-sleep mode, it is highly recommended that users utilize Auto Light-sleep mode described below.

Auto Light-sleep mode is a low power mode provided by ESP-IDF :doc:`Power Management <../../api-reference/system/power_management>` component that leverages FreeRTOS's Tickless IDLE feature. When the application releases all power locks and all FreeRTOS tasks are in a blocked or suspended state, the system automatically calculates the next time point when an event will wake the operating system. If this calculated time point exceeds a set duration (:ref:`CONFIG_FREERTOS_IDLE_TIME_BEFORE_SLEEP`), the ``esp_pm`` component automatically configures the timer wake-up source and enters light sleep to reduce power consumption. To enable this mode, users need to set the ``light_sleep_enable`` field to true in :cpp:type:`esp_pm_config_t` when configuring DFS. For more details, please refer to `DFS Configuration`_.

.. code-block:: text

                                        ┌────────┐
                                        │        │
                                        │  DFS   │
                                        │        │
                                        └───┬────┘
                                            │
                                            ▼
          ┌──────────┐   when idle      ┌──────────┐ exceed set time ┌──────────┐
          │          │  ─────────────►  │          │  ────────────►  │          │
          │          │                  │          │                 │   auto   │
          │  active  │                  │   IDLE   │                 │  light   │
          │          │                  │          │                 │   sleep  │
          │          │  ◄─────────────  │          │                 │          │
          └──────────┘     not idle     └──────────┘                 └──────┬───┘
            ▲                                                               │
            │                      configure wake-up source                 │
            └───────────────────────────────────────────────────────────────┘

                              Auto Light-sleep Mode Workflow

Based on the workflow of Auto Light-sleep mode, its ideal current graph can be obtained, with key nodes marked on the chart.

.. code-block:: text

                         ▲  DFS frequency reduced
                 Current |        │    exceed set idle time
                         |        ▼     / light sleep
                         |     ┌──┐    /      ┌──┐
                         |     │  └──┐        │  └──┐
                         |     |     |        |     |
                         |     │     │        │     │
                         |  ───┘     └────────┘     └────
                         │                     \
                         │              wake-up source wakes up
                         └───────────────────────────────►
                                                      Time
                   Ideal Current Graph of Auto Light-sleep Mode

.. note::

    - To better illustrate the main changes of Auto Light-sleep mode, the DFS frequency reduction process is omitted from the graph above.

    - Auto Light-sleep mode is suitable for scenarios where real-time response to external demands is not required.

    - Auto Light-sleep mode operates based on timer wake-up sources. Therefore, users should not manually configure timer wake-up sources in their application.


.. _Deep-sleep Mode:

Deep-sleep Mode
-----------------

Deep-sleep mode is designed to achieve better power performance by retaining only RTC/LP memory and peripherals during sleep, while all other modules are shut down. Similar to Light-sleep mode, Deep-sleep mode is entered through API calls and requires configuration of wake-up sources for awakening. Users can switch to Deep-sleep mode by calling :cpp:func:`esp_deep_sleep_start` interface.

Deep-sleep mode requires the configuration of wake-up sources. The {IDF_TARGET_NAME} supports multiple wake-up sources. For a complete list of wake-up sources, please refer to :doc:`Sleep Modes <../../api-reference/system/sleep_modes>`. These wake-up sources can also be combined so that any wake-up source can trigger the awakening. If no wake-up source is configured when entering deep sleep, the chip will remain in sleep state until an external reset occurs. Unlike Light-sleep mode, Deep-sleep mode upon awakening will lose the CPU's running context before, so the bootloader needs to be run again to enter the user program.

The workflow of Deep-sleep mode is shown as below:

.. code-block:: text

                  ┌───────┐  call API  ┌───────┐
                  │       ├───────────►│ deep  │
                  │active │            │ sleep │
                  │       │            │       │
                  └───────┘            └───┬───┘
                      ▲                    │
                      └────────────────────┘
                      wake-up source wakes up
                    Deep-sleep Mode Workflow

The primary application scenario of Deep-sleep mode determines that the system will awaken only after a long period and will return to deep sleep state after completing its task. The ideal current graph is as follows.

.. code-block:: text

               ▲
       Current |
               |               call API
               |                  │
               |                  ▼
               |               ┌────┐
               |               │    │
               | wake-up source|    |
               |  wakes up  \  |    |
               |             \ │    │
               │     ──────────┘    └────────────────
               │
               └──────────────────────────────────────────►
                                                      Time
              Ideal Current Graph of Deep-sleep Mode

Deep-sleep mode can be utilized in low power sensor applications or situations where data transmission is not required for most of the time, commonly referred to as standby mode.


.. only:: ESP_ROM_SUPPORT_DEEP_SLEEP_WAKEUP_STUB

  Devices can wake up periodically from deep sleep to measure and upload data, and then return to deep sleep. Alternatively, it can store multiple data sets in RTC memory and transmit them all at once upon the next wake-up. This feature can be implemented using the deep-sleep-stub functionality. For details, please refer to :doc:`Deep-sleep Wake Stubs <../../api-guides/deep-sleep-stub>`.

Low Power Mode Configuration on Pure System
----------------------------------------------

After introducing low power modes from a systemic perspective, this section will present common configuration options, recommended configuration options for each mode, and configurations steps.

Common Configuration Options
------------------------------------

.. note::

      The configuration options below are briefly introduced. For more detailed information, please click the link behind each option.

.. only:: esp32 or esp32s3

  - single/dual core operation mode (:ref:`CONFIG_FREERTOS_UNICORE`)
      For multi-core chips, the single core operation mode can be selected.


  - RTOS Tick rate (Hz) (:ref:`CONFIG_FREERTOS_HZ`)
      This parameter denotes the frequency of the system's periodic task scheduling.


DFS Configuration
------------------------

DFS offers the following configurable options:

- ``max_freq_mhz``
    This parameter denotes the maximum CPU frequency (MHz), i.e., the frequency at which the CPU operates at its highest performance level. It is typically set to the maximum value specified by the chip parameters.

- ``min_freq_mhz``
    This parameter denotes the minimum CPU frequency (MHz), i.e., the CPU's operating frequency when the system is in an idle state. This field can be set to the crystal oscillator (XTAL) frequency value or the XTAL frequency value divided by an integer.

- ``light_sleep_enable``
    Enabling this option allows the system to automatically enter the light sleep during idle periods, i.e., enabling Auto Light-sleep mode, as detailed earlier.

Specific configuration steps are as follows:

1. Enable :ref:`CONFIG_PM_ENABLE`
2. Configure ``max_freq_mhz`` and ``min_freq_mhz`` as follows:

  ::

    esp_pm_config_t pm_config = {
            .max_freq_mhz = CONFIG_EXAMPLE_MAX_CPU_FREQ_MHZ,
            .min_freq_mhz = CONFIG_EXAMPLE_MIN_CPU_FREQ_MHZ,
            .light_sleep_enable = false
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));

Recommended Configuration
++++++++++++++++++++++++++++

.. list-table::
  :header-rows: 1
  :widths: 40 20

  * - Configuration Name
    - Configuration Status

  * - Enable power management component (:ref:`CONFIG_PM_ENABLE`)
    - ON

  * - RTOS Tick rate (Hz) (:ref:`CONFIG_FREERTOS_HZ`)
    - 1000

  * - ``max_freq_mhz``
    - 160

  * - ``min_freq_mhz``
    - 40

  * - ``light_sleep_enable``
    - false

.. note::
    Configurations not mentioned in the above table are set to default.


Light-sleep Mode Configuration
---------------------------------

This section introduces the recommended configuration and configuration steps for Auto Light-sleep mode.

.. note::

      The configuration options below are briefly introduced. For more detailed information, please click the link behind each option.

- Minimum IDLE Tick count before entering sleep state (:ref:`CONFIG_FREERTOS_IDLE_TIME_BEFORE_SLEEP`)
- Put light sleep related codes in IRAM (:ref:`CONFIG_PM_SLP_IRAM_OPT`)
- Put RTOS IDLE related codes in IRAM (:ref:`CONFIG_PM_RTOS_IDLE_OPT`)
- RTC slow clock source (:ref:`CONFIG_RTC_CLK_SRC`)

  .. list-table::
    :header-rows: 1
    :widths: 40 40 20

    * - Clock Source
      - Timer Accuracy
      - Frequency Offset

    * - RTC_CLK_SRC_INT_RC
      - High
      - Large

    * - RTC_CLK_SRC_EXT_CRYS
      - Low
      - Small

- Disable all GPIO when chip at sleep (:ref:`CONFIG_PM_SLP_DISABLE_GPIO`)

.. only:: SOC_PM_SUPPORT_MAC_BB_PD

    - Power down MAC and baseband (:ref:`CONFIG_ESP_PHY_MAC_BB_PD`)


.. only:: SOC_PM_SUPPORT_CPU_PD

    - Power down CPU (:ref:`CONFIG_PM_POWER_DOWN_CPU_IN_LIGHT_SLEEP`)


.. only:: SOC_PM_SUPPORT_TAGMEM_PD

    - Power down I/D-cache tag memory (:ref:`CONFIG_PM_RESTORE_CACHE_TAGMEM_AFTER_LIGHT_SLEEP`)


.. only:: SOC_PM_SUPPORT_VDDSDIO_PD

    - Power down flash in light sleep (:ref:`CONFIG_ESP_SLEEP_POWER_DOWN_FLASH`)

        .. only:: SOC_SPIRAM_SUPPORTED

          Due to the shared power pins between flash and PSRAM, cutting power to PSRAM would result in data loss. Therefore, to ensure light sleep does not disrupt program execution, enabling this option requires that the system does not utilize PSRAM.

.. only:: esp32c2

    - To reduce the on-chip RAM usage of the Power Management module (:ref:CONFIG_PM_SLEEP_FUNC_IN_IRAM), the options in the table below are used to control whether individual components of the Power Management module enable or disable on-chip RAM optimization when :ref:CONFIG_PM_SLEEP_FUNC_IN_IRAM is disabled.

    .. list-table::
      :header-rows: 1
      :widths: 30 60

      * - Configuration Name
        - Configuration Description

      * - :ref:`CONFIG_PM_SLP_IRAM_OPT`
        - When this option is enabled, the software flow for entering and exiting light/deep sleep is linked to on-chip RAM, which shortens the sleep transition time but consumes more on-chip RAM resources. When this option is disabled, the light/deep sleep software flow is compiled into flash memory instead, resulting in longer sleep transition time but saving on-chip RAM resources.

      * - :ref:`CONFIG_PM_RTOS_IDLE_OPT`
        - In auto light sleep mode, when this option is enabled, the FreeRTOS system tick, IDLE task hook functions, and the FreeRTOS tickless idle software context are linked to on-chip RAM. Otherwise, they will be linked to flash memory.

      * - :ref:`CONFIG_ESP_PERIPH_CTRL_FUNC_IN_IRAM`
        - When this option is enabled, the software implementation related to peripheral clock and reset control will be linked to on-chip RAM.  Otherwise, it will be linked to flash memory.

      * - :ref:`CONFIG_ESP_REGI2C_CTRL_FUNC_IN_IRAM`
        - When this option is enabled, the software implementation related to Analog I2C read/write access will be linked to on-chip RAM. Otherwise, it will be linked to flash memory.

Configuration Steps:

1. Configure wake-up sources (refer to :doc:`Sleep Modes <../../api-reference/system/sleep_modes>` for details)
2. Enable :ref:`CONFIG_PM_ENABLE`
3. Enable :ref:`CONFIG_FREERTOS_USE_TICKLESS_IDLE`
4. Configure DFS parameters
5. ``light_sleep_enable`` = true, detailed as follows:

  ::

    esp_pm_config_t pm_config = {
      .max_freq_mhz = CONFIG_EXAMPLE_MAX_CPU_FREQ_MHZ,
      .min_freq_mhz = CONFIG_EXAMPLE_MIN_CPU_FREQ_MHZ,
      #if CONFIG_FREERTOS_USE_TICKLESS_IDLE
      .light_sleep_enable = true
      #endif
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));

6. Additional relevant parameters for configuration introduction

Recommended Configuration
+++++++++++++++++++++++++++++

.. only:: esp32c3 or esp32s3

  .. list-table::
   :header-rows: 1
   :widths: 30 15

   * - Configuration Name
     - Configuration Status

   * - Enable power management component (:ref:`CONFIG_PM_ENABLE`)
     - ON

   * - Enable RTOS Tickless IDLE mode (:ref:`CONFIG_FREERTOS_USE_TICKLESS_IDLE`)
     - ON

   * - RTOS Tick rate (Hz) (:ref:`CONFIG_FREERTOS_HZ`)
     - 1000

   * - Minimum IDLE Tick count before entering sleep mode (:ref:`CONFIG_FREERTOS_IDLE_TIME_BEFORE_SLEEP`)
     - 3

   * - Put light sleep related codes in IRAM (:ref:`CONFIG_PM_SLP_IRAM_OPT`)
     - OFF

   * - Put RTOS IDLE related codes in IRAM (:ref:`CONFIG_PM_RTOS_IDLE_OPT`)
     - OFF

   * - RTC slow clock source (:ref:`CONFIG_RTC_CLK_SRC`)
     - Internal 150 kHz OSC

   * - Disable all GPIO when chip at sleep (:ref:`CONFIG_PM_SLP_DISABLE_GPIO`)
     - ON

   * - Power down MAC and baseband (:ref:`CONFIG_ESP_PHY_MAC_BB_PD`)
     - ON

   * - Power down CPU (:ref:`CONFIG_PM_POWER_DOWN_CPU_IN_LIGHT_SLEEP`)
     - ON

.. only:: SOC_PM_SUPPORT_TAGMEM_PD

   * - Power down I/D-cache tag memory (:ref:`CONFIG_PM_RESTORE_CACHE_TAGMEM_AFTER_LIGHT_SLEEP`)
     - ON


   * - Power down flash in light sleep (:ref:`CONFIG_ESP_SLEEP_POWER_DOWN_FLASH`)
     - OFF

   * - ``max_freq_mhz``
     - 160

   * - ``min_freq_mhz``
     - 40

   * - ``light_sleep_enable``
     - true

  .. note::
      Configurations not mentioned in the above table are set to default.

.. only:: esp32 or esp32s2

  .. list-table::
    :header-rows: 1
    :widths: 30 15

    * - Configuration Name
      - Configuration Status

    * - Enable power management component (:ref:`CONFIG_PM_ENABLE`)
      - ON

    * - Enable RTOS Tickless IDLE mode (:ref:`CONFIG_FREERTOS_USE_TICKLESS_IDLE`)
      - ON

    * - RTOS Tick rate (Hz) (:ref:`CONFIG_FREERTOS_HZ`)
      - 1000

    * - Minimum IDLE Tick count before entering sleep mode (:ref:`CONFIG_FREERTOS_IDLE_TIME_BEFORE_SLEEP`)
      - 3

    * - Put light sleep related codes in IRAM (:ref:`CONFIG_PM_SLP_IRAM_OPT`)
      - OFF

    * - Put RTOS IDLE related codes in IRAM (:ref:`CONFIG_PM_RTOS_IDLE_OPT`)
      - OFF

    * - RTC slow clock source (:ref:`CONFIG_RTC_CLK_SRC`)
      - Internal 150 kHz OSC

    * - Disable all GPIO when chip at sleep (:ref:`CONFIG_PM_SLP_DISABLE_GPIO`)
      - ON

    * - ``max_freq_mhz``
      - 160

    * - ``min_freq_mhz``
      - 40

    * - ``light_sleep_enable``
      - true

  .. note::
      Configurations not mentioned in the above table are set to default.

.. only:: esp32c2

  .. list-table::
   :header-rows: 1
   :widths: 30 15

   * - Configuration Name
     - Configuration Status

   * - Enable power management component (:ref:`CONFIG_PM_ENABLE`)
     - ON

   * - Enable RTOS Tickless IDLE mode (:ref:`CONFIG_FREERTOS_USE_TICKLESS_IDLE`)
     - ON

   * - RTOS Tick rate (Hz) (:ref:`CONFIG_FREERTOS_HZ`)
     - 1000

   * - Minimum IDLE Tick count before entering sleep mode (:ref:`CONFIG_FREERTOS_IDLE_TIME_BEFORE_SLEEP`)
     - 3

   * - Put light sleep related codes in IRAM (:ref:`CONFIG_PM_SLP_IRAM_OPT`)
     - OFF

   * - Put RTOS IDLE related codes in IRAM (:ref:`CONFIG_PM_RTOS_IDLE_OPT`)
     - OFF

   * - RTC slow clock source (:ref:`CONFIG_RTC_CLK_SRC`)
     - Internal 150 kHz OSC

   * - Disable all GPIO when chip at sleep (:ref:`CONFIG_PM_SLP_DISABLE_GPIO`)
     - ON

   * - Power down flash in light sleep (:ref:`CONFIG_ESP_SLEEP_POWER_DOWN_FLASH`)
     - OFF

   * - ``max_freq_mhz``
     - 120

   * - ``min_freq_mhz``
     - 40

   * - ``light_sleep_enable``
     - true

  .. note::
      Configurations not mentioned in the above table are set to default.

Deep-sleep Mode Configuration
---------------------------------

For Deep-sleep mode, other configurations are of minimal significance except wake-up source-related configurations.

Configuration Steps:

1. Configure wake-up sources (refer to :doc:`Sleep Modes <../../api-reference/system/sleep_modes>` for details)
2. Call the API, as follows

.. code-block:: none

   /* Enter deep sleep */
   esp_deep_sleep_start();

Users can keep specific modules powered on during sleep using the following configuration options:

- Power up External 40 MHz XTAL
    In some special applications, certain modules require high clock accuracy and stability during sleep (e.g., BT). In such cases, it is recommended to enable the External 40 MHz XTAL during sleep.
    Code to enable and disable, as follows::

      ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_ON));
      ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_OFF));

- Power up Internal 8 MHz OSC
    In some special applications, certain modules (e.g., LEDC) use the Internal 8 MHz OSC as a clock source and need to function normally during light sleep. In such cases, it is recommended to enable the Internal 8 MHz OSC during sleep.
    Code to enable and disable, as follows::

      ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_RTC8M, ESP_PD_OPTION_ON));
      ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_RTC8M, ESP_PD_OPTION_OFF));
