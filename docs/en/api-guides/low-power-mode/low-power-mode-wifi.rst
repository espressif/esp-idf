Introduction to Low Power Mode in Wi-Fi Scenarios
==================================================

:link_to_translation:`zh_CN:[中文]`

After the previous introduction to low power mode from a systemic perspective, this section delves into low power mode in Wi-Fi scenarios. Due to the complexity of Wi-Fi scenarios, basic principles of Wi-Fi power saving will be introduced before specific low power mode. This section is focused on station mode.

.. todo - add sleep-current/esp32c61_summary.inc

.. only:: not esp32c61

  Choosing Low Power Mode in Wi-Fi Scenarios
  ---------------------------------------------

  To help users select appropriate low power mode, a summary table of low power modes in Wi-Fi scenarios is provided before detailed introduction.

  .. include:: ../sleep-current/{IDF_TARGET_PATH_NAME}_summary.inc

  .. note::

      All currents in the table are average currents, and the measured data are obtained by testing in a shielded box. The terms in the table are explained in the following text. Users can refer to them as needed.

.. _Basic Principles of Wi-Fi Power Saving:

Basic Principles of Wi-Fi Power Saving
----------------------------------------

Firstly, during the operation of a station, prolonged channel monitoring is required to avoid conflicts during transmission and reception. It leads to significant energy consumption as the RF module remains active, thus wasting power. Therefore, Wi-Fi protocols introduce power-saving modes.

The basic principle of power-saving mode is to reduce energy consumption by minimizing unnecessary monitoring time. Access points (APs) will cache packets for a station that has entered power-saving mode. At the same time, it will periodically send beacon frames containing Traffic Indication Map (TIM) information. TIM indicates the unicast packets cached by the AP. Within TIM, the Delivery Traffic Indication Message (DTIM) is special as it caches broadcast packets and sends them out periodically every n TIM intervals (determined by the AP). For stations, TIM is optional listening, while DTIM is mandatory listening. Therefore, station can choose to wake up only before each DTIM frame to power up Wi-Fi-related modules (RF modules) instead of constantly being in listening state. This effectively reduces power consumption.

.. code-block:: text

                          DTIM                DTIM
            TIM             │                   │
             │    │    │    │    │    │    │    │    │    │    │
             │    │    │    │    │    │    │    │    │    │    │
    AP    ───┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────


                         ┌────┐             ┌────┐
                         │    │             │    │
                         │    │             │    │
                         │    │             │    │
    Sta  ────────────────┴────┴─────────────┴────┴─────────────────

                      DTIM4 Power Saving Mode Diagram

Second, the time from powering up to powering down Wi-Fi related modules in a station also affects power consumption. Apart from the necessary time for data transmission processing, there are four configurations mainly affecting the duration:

  - Time offset caused by clock accuracy. The main reason is that clocks may deviate from ideal time to some extent, and the deviation can be positive or negative.
  - Time spent processing missed beacon frames, such as the duration of continuous listening after a missed beacon, the maximum allowable number of missed beacons, etc. The existence and duration of this period are uncertain but can be configured within a range.
  - Active time added to ensure the reception of burst data packets, which can be determined by configuration.
  - IDLE time is required for specific power-saving modes to meet entry conditions. Therefore, reducing the working time can improve power performance while meeting communication requirements.

.. code-block:: text

      ┌────────────┬────────────┬────────────┬────────────┬────────────┐
      │            │            │ time spent │            │            │
      │time offset │  necessary │ processing │ active time│ IDLE time  │
      │            │ processing │   missed   │            │            │
      │            │    time    │   beacon   │            │            │
      └────────────┴────────────┴────────────┴────────────┴────────────┘
                      Chip Operation Time Overview

Furthermore, when the station is not in a Wi-Fi transmission or reception state, other modules begin to affect the chip's power consumption. Different power-saving modes will configure different clock sources or dynamically adjust the operating frequencies of certain modules such as the CPU, while also shutting down varying numbers of functional modules, which effectively reduces the power consumption. Users can select suitable configurations according to their needs.

If time is plotted on the horizontal axis and current on the vertical axis, then the ideal current consumption graph of the chip under low power mode can be simplified as shown below:

.. code-block:: text

               ▲
       Current |
               |
               |            interval                period
               |        │ ◄──────────► │           │ ◄─► │
               |     ┌─────┐        ┌─────┐        ┌─────┐
               |     │     │        │     │        │     │
               |     |     |        |     |        |     | base current
               |     |     |        |     |        |     |   |
               |     |     |        |     |        |     |   ▼
               |  ───┘     └────────┘     └────────┘     └──────
               |
               └───────────────────────────────────────────────►
                                                          Time
            Ideal Current Graph of the Chip Under Low Power Mode

When the station needs to engage in Wi-Fi communication, the Wi-Fi-related modules (PHY) are activated, causing a significant increase in current. The current remains at a relatively high level until the task is completed. After that, the chip will deactivate the Wi-Fi-related modules, causing the current to decrease to a lower level.

Three main factors affect power consumption performance: interval, period, and base current.

  - **Interval** refers to the interval at which the station's Wi-Fi-related modules operate. It can be customized by low power mode or determined by the DTIM interval according to Wi-Fi protocol power-saving mechanisms (see first part in :ref:`Basic Principles of Wi-Fi Power Saving`). Generally, a larger interval leads to better power performance under the same conditions. But it also results in slower response times, affecting communication timeliness.

  - **Period** can be seen as the duration of each time the station's Wi-Fi operates, which also affects power performance. The period is not fixed (see second part in :ref:`Basic Principles of Wi-Fi Power Saving`). In ensuring normal Wi-Fi communication, a shorter period leads to better power performance. However, reducing the period will inevitably affect communication reliability.

  - **Base current** refers to the current of the chip when the Wi-Fi-related modules are not active. It is influenced by various factors. Different power-saving modes have different sleep strategies. Therefore, optimizing the configuration to reduce the base current can improve power performance. But closing other modules will affect related functions and the wake-up time of the chip.

Therefore, power consumption can be reduced by considering the three aspects. Next, Modem-sleep mode and Auto Light-sleep mode will be introduced. The main difference between the two modes lies in the optimization of these three factors.


Modem-sleep Mode
----------------

The main principle of Modem-sleep mode is based on the DTIM mechanism. In this mode, the chip periodically wakes up for Wi-Fi-related tasks, and enters sleep state between intervals to power down PHY (RF module) to reduce power consumption. Besides, through the DTIM mechanism, the station can maintain Wi-Fi connection and data transmission with the AP.

Modem-sleep mode automatically enters sleep after the Wi-Fi task ends without the need to call an API. During sleep, only the Wi-Fi-related modules (PHY) are closed, while other modules remain in power-up state.

Modem-sleep mode will wake up according to the DTIM interval or listen interval (as introduced below in :ref:`Modem-sleep Mode Configuration`), acting as if the system has automatically set a Wi-Fi wake-up source. Therefore, users do not need to configure a wake-up source. The system can also wake up when actively sending packets.

Modem-sleep mode is a toggle mode that automatically runs after calling the API to activate it. Its workflow is very clear, as shown in the diagram below.

.. code-block:: text

          ┌───────────┐      Wi-Fi task finished     ┌───────────┐
          │           ├─────────────────────────────►│   modem   │
          │   active  │                              │   sleep   │
          │           │◄─────────────────────────────┤           │
          └───────────┘   DTIM interval activated    └───────────┘
                              / sending packets

                          Modem-sleep Mode Workflow

Based on the base current graph provided above and combined with the operating principle of Modem-sleep mode, an ideal current graph can be derived, taking Min Modem-sleep mode (as introduced below in :ref:`Modem-sleep Mode Configuration`) as an example.

.. code-block:: text

              ▲
       Current|
              |               DTIM interval
              |             │ ◄──────────► │
              |          ┌─────┐        ┌─────┐        ┌─────┐
              |  before  │     │        │     │        │     │
              |   DTIM   |     |        |     |        |     |  Wi-Fi task
              |  interval|     |        |     |        |     | finished
              |activated |     |        |     |        |     | /
              |        \ │     │        │     │        │     │/
              |  ────────┘     └────────┘     └────────┘     └──────
              |
              └─────────────────────────────────────────────────────►
                                                                Time
                    Ideal Current Graph of Min Modem-sleep Mode

Modem-sleep mode is generally used in scenarios where the CPU needs to remain active and maintain a Wi-Fi connection. For example, it is utilized to realize local voice wake-up by the {IDF_TARGET_NAME}, where the CPU continuously collects and processes audio data.

DFS + Modem-sleep Mode
------------------------

In Modem-sleep mode, the CPU remains active while the DFS mechanism primarily adjusts the CPU and APB operating frequencies to reduce power consumption. Therefore, combining DFS with Modem sleep mode can further optimize power performance. Additionally, as the Wi-Fi task requests the :cpp:enumerator:`ESP_PM_CPU_FREQ_MAX` power lock to ensure the rapid execution of Wi-Fi tasks, frequency adjustment by DFS and Modem-sleep mode only occurs during the base current phase, which is after the Wi-Fi task ends.

In Wi-Fi scenarios, to help users understand main changes, the state of DFS can be simplified. Specifically, although DFS primarily adjusts frequencies based on the maximum demands of the CPU and APB locks, in Wi-Fi scenarios, the CPU frequency needs to be maximized for operation. Besides, after the Wi-Fi task ends, it can be ideally assumed that no other tasks need to be completed, and that after some time, both locks are released to enter IDLE state. This simplified situation also ignores any current variations caused by changes in the locks during this time.

In Wi-Fi scenarios, the flowchart of DFS csn be simplified as follows:

.. code-block:: text

                                      ┌────────┐
                                      │        │
                                      │  DFS   │
                                      │        │
                                      └───┬────┘
                                          │
                                          ▼
        ┌──────────┐   when IDLE      ┌──────────┐
        │          │  ─────────────►  │          │
        │          │                  │          │
        │  active  │                  │   IDLE   │
        │          │                  │          │
        │          │  ◄─────────────  │          │
        └──────────┘     not IDLE     └──────────┘

      Simplified Flowchart of DFS in the Wi-Fi Scenario

The system transitions between active state and IDLE state in Wi-Fi scenarios. After the Wi-Fi task is completed, the system releases all locks after a period of time and enters the IDLE state. At this point, the DFS mechanism reduces the frequency to the set minimum value, ignoring the frequency adjustment actions during the state transition, which facilitates understanding.

The DFS + Modem-sleep mode's ideal current graph is simplified as below:

.. code-block:: text

               ▲               Wi-Fi task finished
       Current |                  /  modem sleep
               |                 /
               |          ┌─────┐                 ┌─────┐
               |          │     │                 │     │
               |          |     |                 |     |   lock released
               |          |     |                 |     |   DFS frequency
               |          |     |                 |     |      reduced
               |          |     |                 |     |    /  IDLE
               |          |     |                 |     |   /
               |          |     │                 |     │  /
               |          │     └─┐               │     └─┐
               |  ────────┘       └───────────────┘       └─────────
               |
               └─────────────────────────────────────────────────────►
                                                                  Time
                    DFS + Modem-sleep Mode's Ideal Current Graph

Auto Light-sleep Mode + Wi-Fi Scenario
----------------------------------------

Auto Light-sleep mode combines the ESP-IDF power management mechanism, the DTIM mechanism, and Light-sleep mode in Wi-Fi scenarios. Enabling power management is a prerequisite of this mode, and its auto aspect is demonstrated by the system automatically entering Light-sleep after being in the IDLE state for a set duration. Additionally, auto Auto Light-sleep mode adheres to the DTIM mechanism. The system will automatically wake up to maintain Wi-Fi connection with AP.

In the Wi-Fi environment, the sleep mechanism of Auto Light-sleep mode remains consistent with that of the pure system. It still relies on the power management mechanism, where the condition for entering sleep is when the system has been IDLE for a duration exceeding the set time. The system will assess if the IDLE time meets the conditions, and if so, it will directly enter sleep. This process is automatic. During sleep, RF, the 8 MHz oscillator, the 40 MHz high-speed crystal oscillator, PLL, and gated digital core clock are automatically turned off, and CPU operation is suspended.

In the Wi-Fi environment, the Auto Light-sleep mode follows the DTIM mechanism. The system will automatically wake up before the arrival of DTIM frames, as if a Wi-Fi wake-up source has been set. Therefore, there is no need for configuration. Additionally, the system can be awakened when actively sending packets.

The operation workflow of Auto Light-sleep mode in the Wi-Fi environment is relatively complex, but it is entirely automated throughout. Specific details are illustrated in the diagram.

.. code-block:: text

                                  ┌────────┐
                                  │        │
                                  │  DFS   │
                                  │        │
                                  └───┬────┘
                                      │
                                      ▼
    ┌──────────┐   when IDLE      ┌──────────┐ exceed set time ┌──────────┐
    │          │  ─────────────►  │          │  ────────────►  │          │
    │          │                  │          │                 │   auto   │
    │  active  │                  │   IDLE   │                 │  light   │
    │          │                  │          │                 │   sleep  │
    │          │  ◄─────────────  │          │                 │          │
    └──────────┘     not IDLE     └──────────┘                 └──────┬───┘
      ▲                                                               │
      │          DTIM interval activated / sending packets            │
      └───────────────────────────────────────────────────────────────┘

                      Auto Light-sleep Mode Workflow

In Wi-Fi scenarios, Auto Light-sleep mode is often enabled simultaneously with Modem-sleep mode. Here, an ideal current graph of Modem + Auto Light-sleep mode is provided, with key nodes marked on the graph.

.. code-block:: text

                                  DTIM interval
              ▲            │ ◄───────────────────► │
      Current |          ┌─────┐                 ┌─────┐
              |          │     │ \               │     │
              |          |     |  \              |     |
              |          |     |  Wi-Fi task     |     |
              |          |     |   finished      |     |
              |          |     |   modem sleep   |     |
              |  before  |     |                 |     |          IDLE time
              |  DTIM    |     |     IDLE        |     |       exceeds set time
              | activated|     │  DFS frequency  |     │       light sleep
              |       \  |     └─┐    / reduced  |     └─┐    /
              |        \ │       └─┐ /           │       └─┐ /
              |  ────────┘         └─────────────┘         └─────────
              |
              └─────────────────────────────────────────────────────►
                                                                Time
            Ideal Current Graph of Modem + Auto Light-sleep Mode

In the Wi-Fi environment, Auto Light-sleep mode can be utilized to maintain Wi-Fi connection and respond promptly to data sent by AP. Additionally, the CPU can remain IDLE when no commands are received. For example, in applications such as Wi-Fi switches, the CPU is mostly IDLE until it receives a control command to operate on GPIO.


Deep-sleep Mode + Wi-Fi Scenario
-----------------------------------

The Deep-sleep mode in Wi-Fi scenarios is essentially the same as in a pure system. For details, please refer to :ref:`Deep-sleep Mode`. Here, it will not be further discussed.


Low Power Mode Configuration in Wi-Fi Scenarios
-----------------------------------------------------

After introducing the low power mode in Wi-Fi scenarios, this section will cover common configuration options, unique configuration options for each mode, and instructions for using the corresponding low power mode APIs. Additionally, recommendations for the respective modes' configurations (including recommended configurations for pure systems) will be provided, along with specific performance details.

.. note::

      The configuration options below are briefly introduced. For more detailed information, please click the link behind each option.

Common Configuration Options
++++++++++++++++++++++++++++++++

- Power consumption related:

  - Max Wi-Fi TX power (dBm) (:ref:`CONFIG_ESP_PHY_MAX_WIFI_TX_POWER`)

- Speed optimization related:

  - Wi-Fi IRAM speed optimization (:ref:`CONFIG_ESP_WIFI_IRAM_OPT`)
  - Wi-Fi RX IRAM speed optimization (:ref:`CONFIG_ESP_WIFI_RX_IRAM_OPT`)
  - Wi-Fi Sleep IRAM speed optimization (:ref:`CONFIG_ESP_WIFI_SLP_IRAM_OPT`)


- Wi-Fi Protocol related:

  - Minimum active time (:ref:`CONFIG_ESP_WIFI_SLP_DEFAULT_MIN_ACTIVE_TIME`)

  - Maximum keep alive time (:ref:`CONFIG_ESP_WIFI_SLP_DEFAULT_MAX_ACTIVE_TIME`)

  - Send gratuitous ARP periodically (:ref:`CONFIG_LWIP_ESP_GRATUITOUS_ARP`)

  - Wi-Fi sleep optimize when beacon lost (:ref:`CONFIG_ESP_WIFI_SLP_BEACON_LOST_OPT`)


.. _Modem-sleep Mode Configuration:

Modem-sleep Mode Configuration
++++++++++++++++++++++++++++++++

- Configurable Options

  - Min Modem
      This parameter indicates that the station operates according to the DTIM cycle. It wakes up before each DTIM to receive beacon frames, which ensures that broadcast information is not missed. However, the DTIM cycle is determined by the AP. If the DTIM cycle is short, the power saving effect will be reduced.

  - Max Modem
      This parameter indicates that the station customizes a listen interval and wakes up to receive beacon frames at intervals defined by the listen interval. This approach saves power when the listen interval is large but may lead to missed DTIMs and broadcast data.


- Configuration Steps

  - Call the API and select the mode parameters

  .. code-block:: none

        typedef enum {
            WIFI_PS_NONE,
            WIFI_PS_MIN_MODEM,
            WIFI_PS_MAX_MODEM,
        } wifi_ps_type_t;
        esp_err_t esp_wifi_set_ps(wifi_ps_type_t type);

    If WIFI_PS_MAX_MODEM is selected, the listen interval also needs to be configured. An example is provided below::

          #define LISTEN_INTERVAL 3
          wifi_config_t wifi_config = {
              .sta = {
              .ssid = "SSID",
              .password = "Password",
              .listen_interval = LISTEN_INTERVAL,
            },
          };
          ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
          ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
          ESP_ERROR_CHECK(esp_wifi_start());

- Recommended Configuration

    The recommended configuration provided here is for Min Modem-sleep mode + DFS.

    .. list-table::
        :header-rows: 1
        :widths: 20 15

        * - Configuration Name
          - Configuration Status

        * - WIFI_PS_MIN_MODEM
          - ON

        * - CONFIG_PM_ENABLE
          - ON

        * - RTOS Tick rate (Hz)
          - 1000

        * - ``max_freq_mhz``
          - The maximum CPU frequency supported by {IDF_TARGET_NAME}

        * - ``min_freq_mhz``
          - {CONFIG_XTAL_FREQ}

        * - ``light_sleep_enable``
          - false

    .. todo - add sleep-current/esp32c61_modem_sleep.inc

    .. only:: not esp32c61

      - Configuration Performance

          .. include:: ../sleep-current/{IDF_TARGET_PATH_NAME}_modem_sleep.inc


Auto Light-sleep Mode + Wi-Fi Scenario Configuration
+++++++++++++++++++++++++++++++++++++++++++++++++++++++

Auto Light-sleep mode in Wi-Fi scenarios does not require wake-up source configuration compared with a pure system. But the remaining part of configuration is basically the same in the two operation scenarios. Therefore, detailed introduction of configurable options, configuration steps, and recommended configurations can be found in the previous section :ref:`Deep-sleep Mode`, with the Wi-Fi-related configurations set to default.

.. todo - add eep-current/esp32c61_light_sleep.inc

.. only:: not esp32c61

  - Configuration Performance

      The configuration performance mirrors that of the recommended Auto Light-sleep mode configuration in a pure system, combined with the default Wi-Fi-related configurations in the Wi-Fi environment.

      .. include:: ../sleep-current/{IDF_TARGET_PATH_NAME}_light_sleep.inc

Deep-sleep Mode + Wi-Fi Scenario Configuration
++++++++++++++++++++++++++++++++++++++++++++++++

Deep-sleep mode configuration in Wi-Fi scenarios is essentially the same as in a pure system. Therefore, detailed introduction of configurable options, configuration steps, and recommended configurations can be found in the previous section :ref:`Deep-sleep Mode`, with the Wi-Fi-related configurations set to default.

- Configuration Performance

    The performance of this configuration mirrors that of the recommended Deep-sleep mode configuration in a pure system, combined with the default Wi-Fi-related configurations in the Wi-Fi environment.

    .. only:: esp32

      Average current approximately 5.0 μA

    .. only:: esp32s2

      Average current approximately 5.0 μA

    .. only:: esp32s3

      Average current approximately 6.9 μA

    .. only:: esp32c3

      Average current approximately 4.8 μA

    .. only:: esp32c2

      Average current approximately 4.9 μA

    .. only:: esp32c6

      Average current approximately 6.7 μA

    .. only:: esp32c5

      Average current approximately 10.0 μA
