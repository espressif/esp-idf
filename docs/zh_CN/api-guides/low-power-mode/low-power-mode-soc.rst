系统低功耗模式介绍
===================

:link_to_translation:`en:[English]`

{IDF_TARGET_NAME} 支持多种低功耗模式，从系统功耗管理角度来看，主要有 DFS、Light-sleep 模式和 Deep-sleep 模式。这些模式降低功耗的主要策略是在不影响系统功能的前提下，尽可能降低时钟频率 (DFS) 或进入睡眠状态，在睡眠状态下关闭不必要的电源域或对不使用的外设进行时钟门控以降低功耗。根据关闭电源域的等级是否会破坏程序运行上下文又细分为 Light-sleep 模式和 Deep-sleep 模式。

.. only:: SOC_WIFI_SUPPORTED or SOC_BLE_SUPPORTED

  其次地，对于 {IDF_TARGET_NAME} 常用的工作场景，如 Wi-Fi/蓝牙工作时，基于上述的几种低功耗模式，ESP-IDF 还提供了进行过针对性优化的细分低功耗模式，将在后续章节介绍。

本节将首先从纯系统角度介绍，不涉及具体工作场景的低功耗模式。

DFS
------

Dynamic Frequency Scaling (DFS) 即动态频率切换，是 ESP-IDF 中集成的电源管理机制的基础功能。DFS 可以根据应用程序持有电源锁的情况，调整外围总线 (APB) 频率和 CPU 频率。持有高性能锁就使用高频，空闲状态不持有电源锁时则使用低频来降低功耗，以此来尽可能减少运行应用程序的功耗。

DFS 的调频机制即根据持有电源锁的最大频率需求来调整频率，同时，`CONFIG_FREERTOS_HZ` 的数值也会对 DFS 调频产生影响。:ref:`CONFIG_FREERTOS_HZ` 值越大，即系统任务调度的频率越高，意味着系统能更及时地根据需求调整频率。有关调频机制的详细信息，请参见 :doc:`电源管理 <../../api-reference/system/power_management>`。

下图为 DFS 调频机制运行的理想电流情况。

.. code-block:: text

                    持有 CPU 和 APB MAX 锁
                               │
                               │        释放 CPU MAX 锁
                      ▲        │       /
              电流大小 │        ▼      /
                      │   ──────────┐             释放 APB MAX 锁
                      │             │            /
                      │             │           /
                      │             └─────────┐
                      │             ▲         │
                      │             │         │
                      │ 第 m 个 tick │         └───────────
                      │                       ▲
                      │                       │
                      │           第 n 个 tick │
                      └──────────────────────────────────────►
                                                         时间
                              理想 DFS 机制调频电流图

DFS 适用于 CPU 必须处于工作状态但是对低功耗有需求的场景，因此 DFS 通常与其他低功耗模式共同开启，下文会详细介绍。

.. _Light-sleep 模式:

Light-sleep 模式
-------------------

Light-sleep 模式是 {IDF_TARGET_NAME} 预设的一种低功耗模式，用户可以通过调用 :cpp:func:`esp_light_sleep_start` 接口主动切换至 Light-sleep 模式，进入睡眠后，芯片将根据当前各外设的工作状态，关闭不必要的电源域和对睡眠期间不使用的模块进行时钟门控。{IDF_TARGET_NAME} 支持多种唤醒源，详见 :doc:`睡眠模式 <../../api-reference/system/sleep_modes>`，当芯片从 Light-sleep 模式唤醒后，CPU 会接着入睡时的上下文继续运行，外设的工作状态也不会被破坏。为充分利用 Light-sleep 模式以降低芯片功耗，更推荐用户使用下述的 Auto Light-sleep 模式。

Auto Light-sleep 模式是 ESP-IDF :doc:`电源管理 <../../api-reference/system/power_management>` 组件基于 FreeRTOS 的 Tickless IDLE 功能提供的一种低功耗模式。当应用程序释放所有电源锁，FreeRTOS 的所有任务都进入阻塞态或挂起态时，系统会自动获取下一个有就绪事件需要唤醒操作系统的时间点，当判定此时间点距当前超过设定时间（:ref:`CONFIG_FREERTOS_IDLE_TIME_BEFORE_SLEEP`）后，``esp_pm`` 组件会自动配置定时器唤醒源并进入 light sleep 以降低功耗。用户在配置 DFS 时置真 :cpp:type:`esp_pm_config_t` 中的 ``light_sleep_enable`` 字段即可启用该模式，详见下文中的 `DFS 配置`_。

.. code-block:: text

                                        ┌────────┐
                                        │        │
                                        │  DFS   │
                                        │        │
                                        └───┬────┘
                                            │
                                            ▼
          ┌──────────┐     系统空闲      ┌──────────┐   超过设定时间    ┌──────────┐
          │          │  ─────────────►  │          │  ────────────►  │         │
          │          │                  │          │                 │   auto  │
          │  active  │                  │   IDLE   │                 │  light  │
          │          │                  │          │                 │   sleep │
          │          │  ◄─────────────  │          │                 │         │
          └──────────┘    系统非空闲      └──────────┘                 └────┬────┘
            ▲                                                              │
            │                         配置唤醒源唤醒                         │
            └──────────────────────────────────────────────────────────────┘

                          Auto Light-sleep 模式工作流程图

根据 Auto Light-sleep 模式的工作流程可得其理想电流图，关键节点均在图上标出。

.. code-block:: text

              电流  ▲     系统 IDLE DFS 降频
              大小  |        │    系统判断空闲时间超过设定值
                   |        ▼    / light sleep
                   |     ┌──┐   /       ┌──┐
                   |     │  └──┐        │  └──┐
                   |     |     │        |     │
                   |     │     │        │     │
                   |  ───┘     └────────┘     └────
                   |                     \
                   │                      唤醒源唤醒
                   └───────────────────────────────►
                                                 时间
                    Auto Light-sleep 模式理想电流图

.. note::

    - 为更加清晰地展现出 Auto Light-sleep 模式的主要变化，上图中省略了 DFS 降频过程。

    - Auto Light-sleep 模式适用于不需要实时响应外界需求的场景。

    - Auto Light-sleep 功能基于定时器唤醒源实现，所以用户应用程序中不可以再主动配置定时器唤醒源。


.. _Deep-sleep 模式:

Deep-sleep 模式
-----------------

Deep-sleep 模式是为了追求更好的功耗表现所设计，休眠时仅保留 RTC/LP 相关内存及外设，其余模块全部关闭。与 Light-sleep 模式类似，Deep-sleep 模式同样通过 API 进入，且需要配置唤醒源进行唤醒，用户可以通过调用 :cpp:func:`esp_deep_sleep_start` 接口主动切换至 Deep-sleep 模式。

Deep-sleep 模式需配置唤醒源，{IDF_TARGET_NAME} 支持多种唤醒源，完整唤醒源列表详见 :doc:`睡眠模式 <../../api-reference/system/sleep_modes>`。这些唤醒源也可以组合在一起，此时任何一个唤醒源都可以触发唤醒。若不配置唤醒源进入 Deep-sleep 模式，芯片将一直处在睡眠状态，直到外部复位。与 Light-sleep 模式不同，Deep-sleep 模式唤醒后会丢失睡眠前的 CPU 运行上下文，因此，唤醒后需要重新运行引导加载程序才可进入用户程序。

Deep-sleep 模式工作流程如下图所示：

.. code-block:: text

                  ┌───────┐  调用 API   ┌───────┐
                  │       ├───────────►│ deep  │
                  │active │            │ sleep │
                  │       │            │       │
                  └───────┘            └───┬───┘
                      ▲                    │
                      └────────────────────┘
                          配置的唤醒源唤醒
                    Deep-sleep 模式工作流程图

Deep-sleep 模式主要应用场景决定了系统很长时间才会苏醒一次，完成工作后又会继续进入 deep sleep 状态，所以其理想电流图如下。

.. code-block:: text

      电流 ▲
      大小 |
          |               调用 API
          |                 │
          |                 ▼
          |              ┌────┐
          |              │    │
          |   唤醒源唤醒   |    |
          |           \  |    |
          |            \ │    │
          │    ──────────┘    └────────────────
          │
          └──────────────────────────────────────────►
                                                  时间
                  Deep-sleep 模式理想电流图

Deep-sleep 模式可以用于低功耗的传感器应用，或是大部分时间都不需要进行数据传输的情况，也就是通常所说的待机模式。


.. only:: ESP_ROM_SUPPORT_DEEP_SLEEP_WAKEUP_STUB

  设备也可以每隔一段时间从 deep sleep 状态醒来测量数据并上传，之后重新进入 deep sleep；也可以将多个数据存储于 RTC 内存，然后在下次唤醒时一次性发送。此特性可借助 deep-sleep-stub 功能实现，详见 :doc:`Deep-sleep 唤醒存根 <../../api-guides/deep-sleep-stub>`。

如何配置纯系统下低功耗模式
------------------------------

介绍完纯系统下的低功耗模式后，本节将介绍公共配置选项、每种模式的推荐配置以及配置方法。

公共配置选项
------------

.. note::

      下文为公共配置选项的简单介绍，点击相应链接获取详细内容。

.. only:: esp32 or esp32s3

  - 单双核工作模式 (:ref:`CONFIG_FREERTOS_UNICORE`)
      对于多核心芯片，可以选择单核工作模式。


  - RTOS Tick rate (Hz) (:ref:`CONFIG_FREERTOS_HZ`)
      该参数表示系统周期任务调度的频率。


DFS 配置
------------

DFS 有如下可配置选项：

- ``max_freq_mhz``
    该参数表示最大 CPU 频率 (MHz)，即 CPU 最高性能工作时候的频率，一般设置为芯片参数的最大值。

- ``min_freq_mhz``
    该参数表示最小 CPU 频率 (MHz)，即系统处在空闲状态时 CPU 的工作频率。该字段可设置为晶振 (XTAL) 频率值，或者 XTAL 频率值除以整数。

- ``light_sleep_enable``
    使能该选项，系统将在空闲状态下自动进入 light sleep 状态，即 Auto Light-sleep 模式使能，上文已经具体介绍。

具体配置方法如下：

1. 使能 :ref:`CONFIG_PM_ENABLE`
2. 配置 ``max_freq_mhz`` 和 ``min_freq_mhz``，方式如下：

  ::

    esp_pm_config_t pm_config = {
            .max_freq_mhz = CONFIG_EXAMPLE_MAX_CPU_FREQ_MHZ,
            .min_freq_mhz = CONFIG_EXAMPLE_MIN_CPU_FREQ_MHZ,
            .light_sleep_enable = false
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));

推荐配置
++++++++++++++

.. list-table::
  :header-rows: 1
  :widths: 40 20

  * - 配置名称
    - 设置情况

  * - 启用电源管理组件 (:ref:`CONFIG_PM_ENABLE`)
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
    上表中不涉及的配置均是默认。


Light-sleep 模式配置
---------------------

本节介绍 Auto Light-sleep 模式的推荐配置和配置步骤。

.. note::

      下文为 Auto Light-sleep 模式配置选项的简单介绍，点击相应链接获取详细内容。

- 进入睡眠模式前最小 IDLE Tick 数 (:ref:`CONFIG_FREERTOS_IDLE_TIME_BEFORE_SLEEP`)
- 将 light sleep 相关代码放置在片内 RAM 中 (:ref:`CONFIG_PM_SLP_IRAM_OPT`)
- 将 RTOS IDLE 相关代码放置在片内 RAM 中 (:ref:`CONFIG_PM_RTOS_IDLE_OPT`)
- RTC 慢速时钟源 (:ref:`CONFIG_RTC_CLK_SRC`)

  .. list-table::
    :header-rows: 1
    :widths: 40 40 20

    * - 时钟源
      - 定时精度
      - 频偏

    * - RTC_CLK_SRC_INT_RC
      - 高
      - 大

    * - RTC_CLK_SRC_EXT_CRYS
      - 低
      - 小

- 芯片休眠时禁用所有 GPIO (:ref:`CONFIG_PM_SLP_DISABLE_GPIO`)

.. only:: SOC_PM_SUPPORT_MAC_BB_PD

    - 关闭 MAC 和基带 (:ref:`CONFIG_ESP_PHY_MAC_BB_PD`)


.. only:: SOC_PM_SUPPORT_CPU_PD

    - 关闭 CPU (:ref:`CONFIG_PM_POWER_DOWN_CPU_IN_LIGHT_SLEEP`)


.. only:: SOC_PM_SUPPORT_TAGMEM_PD

    - 关闭指令和数据缓存中的标签存储器 (I/D-cache tag memory) (:ref:`CONFIG_PM_RESTORE_CACHE_TAGMEM_AFTER_LIGHT_SLEEP`)


.. only:: SOC_PM_SUPPORT_VDDSDIO_PD

    - 在 light sleep 状态关闭 flash 供电 (:ref:`CONFIG_ESP_SLEEP_POWER_DOWN_FLASH`)

        .. only:: SOC_SPIRAM_SUPPORTED

          由于 flash 和 PSRAM 共用供电管脚，PSRAM 关闭供电将会导致数据丢失，因此，为保证 light sleep 不破坏程序运行状态，启用该选项的前提是系统没有使用 PSRAM。

.. only:: esp32c2

    - 减少 Power Managemet 模块片内 RAM 内存资源使用量 (:ref:`CONFIG_PM_SLEEP_FUNC_IN_IRAM`)，当 :ref:`CONFIG_PM_SLEEP_FUNC_IN_IRAM` 禁止时，下面表格中选项分别用于控制 Power Management 模块各部分组件启用或禁止片内 RAM 优化

    .. list-table::
      :header-rows: 1
      :widths: 60 30

      * - 配置项
        - 描述

      * - :ref:`CONFIG_PM_SLP_IRAM_OPT`
        - 使能该选项后执行 light / deep sleep 软件流程上下文被链接到片内 RAM 空间，这意味着系统进出睡眠时间变短，消耗更多的片内 RAM 资源；禁止该选项，light / deep sleep 软件流程上下文将被编译到 Flash 空间，这意味着系统进出睡眠时间变长，但会节约片内 RAM 资源。

      * - :ref:`CONFIG_PM_RTOS_IDLE_OPT`
        - auto light sleep 模式下，使能该选项后，FreeRTOS 系统滴答，IDLE 任务回调函数及 FreeRTOS Tickless idle 软件上下文被链接到片内 RAM 空间，反之将被链接到 Flash 空间。

      * - :ref:`CONFIG_ESP_PERIPH_CTRL_FUNC_IN_IRAM`
        - 使能该选项后外设时钟及复位控制相关的软件实现将被链接到片内 RAM 空间，反之将被链接到 Flash 空间。

      * - :ref:`CONFIG_ESP_REGI2C_CTRL_FUNC_IN_IRAM`
        - 使能该选项后 Analog I2C 读写访问相关的软件实现将被链接到片内 RAM 空间，反之被链接到 Flash 空间。

配置方法：

1. 配置唤醒源（详见 :doc:`睡眠模式 <../../api-reference/system/sleep_modes>`）
2. 使能 :ref:`CONFIG_PM_ENABLE`
3. 使能 :ref:`CONFIG_FREERTOS_USE_TICKLESS_IDLE`
4. 配置 DFS 参数
5. ``light_sleep_enable`` = true，具体如下：

  ::

    esp_pm_config_t pm_config = {
      .max_freq_mhz = CONFIG_EXAMPLE_MAX_CPU_FREQ_MHZ,
      .min_freq_mhz = CONFIG_EXAMPLE_MIN_CPU_FREQ_MHZ,
      #if CONFIG_FREERTOS_USE_TICKLESS_IDLE
      .light_sleep_enable = true
      #endif
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));

6. 配置介绍的其余相关参数

推荐配置
+++++++++

.. only:: esp32c3 or esp32s3

  .. list-table::
   :header-rows: 1
   :widths: 30 15

   * - 配置名称
     - 设置情况

   * - 启用电源管理组件 (:ref:`CONFIG_PM_ENABLE`)
     - ON

   * - 启用 Tickless IDLE 模式 (:ref:`CONFIG_FREERTOS_USE_TICKLESS_IDLE`)
     - ON

   * - RTOS Tick rate (Hz) (:ref:`CONFIG_FREERTOS_HZ`)
     - 1000

   * - 进入睡眠模式前最小 IDLE Tick 数 (:ref:`CONFIG_FREERTOS_IDLE_TIME_BEFORE_SLEEP`)
     - 3

   * - 将 light sleep 相关代码放置在片内 RAM 中 (:ref:`CONFIG_PM_SLP_IRAM_OPT`)
     - OFF

   * - 将 RTOS IDLE 相关代码放置在片内 RAM 中 (:ref:`CONFIG_PM_RTOS_IDLE_OPT`)
     - OFF

   * - RTC 慢速时钟源 (:ref:`CONFIG_RTC_CLK_SRC`)
     - 内部 150 kHz 振荡器

   * - 芯片休眠时禁用所有 GPIO (:ref:`CONFIG_PM_SLP_DISABLE_GPIO`)
     - ON

   * - 关闭 MAC 和基带 (:ref:`CONFIG_ESP_PHY_MAC_BB_PD`)
     - ON

   * - 关闭 CPU (:ref:`CONFIG_PM_POWER_DOWN_CPU_IN_LIGHT_SLEEP`)
     - ON

.. only:: SOC_PM_SUPPORT_TAGMEM_PD

   * - 关闭指令和数据缓存中的标签存储器 (I/D-cache tag memory)  (:ref:`CONFIG_PM_RESTORE_CACHE_TAGMEM_AFTER_LIGHT_SLEEP`)
     - ON


   * - 在 light sleep 状态关闭 flash 供电 (:ref:`CONFIG_ESP_SLEEP_POWER_DOWN_FLASH`)
     - OFF

   * - ``max_freq_mhz``
     - 160

   * - ``min_freq_mhz``
     - 40

   * - ``light_sleep_enable``
     - true

  .. note::
      上表中不涉及的配置均是默认。

.. only:: esp32 or esp32s2

  .. list-table::
    :header-rows: 1
    :widths: 30 15

    * - 配置名称
      - 设置情况

    * - 启用电源管理组件 (:ref:`CONFIG_PM_ENABLE`)
      - ON

    * - 启用 Tickless IDLE 模式 (:ref:`CONFIG_FREERTOS_USE_TICKLESS_IDLE`)
      - ON

    * - RTOS Tick rate (Hz) (:ref:`CONFIG_FREERTOS_HZ`)
      - 1000

    * - 进入睡眠模式前最小 IDLE Tick 数 (:ref:`CONFIG_FREERTOS_IDLE_TIME_BEFORE_SLEEP`)
      - 3

    * - 将 light sleep 相关代码放置在片内 RAM 中 (:ref:`CONFIG_PM_SLP_IRAM_OPT`)
      - OFF

    * - 将 RTOS IDLE 相关代码放置在片内 RAM 中 (:ref:`CONFIG_PM_RTOS_IDLE_OPT`)
      - OFF

    * - RTC 慢速时钟源 (:ref:`CONFIG_RTC_CLK_SRC`)
      - 内部 150 kHz 振荡器

    * - 芯片休眠时禁用所有 GPIO (:ref:`CONFIG_PM_SLP_DISABLE_GPIO`)
      - ON

    * - ``max_freq_mhz``
      - 160

    * - ``min_freq_mhz``
      - 40

    * - ``light_sleep_enable``
      - true

  .. note::
      上表中不涉及的配置均是默认。

.. only:: esp32c2

  .. list-table::
   :header-rows: 1
   :widths: 30 15

   * - 配置名称
     - 设置情况

   * - 启用电源管理组件 (:ref:`CONFIG_PM_ENABLE`)
     - ON

   * - 启用 Tickless IDLE 模式 (:ref:`CONFIG_FREERTOS_USE_TICKLESS_IDLE`)
     - ON

   * - RTOS Tick rate (Hz) (:ref:`CONFIG_FREERTOS_HZ`)
     - 1000

   * - 进入睡眠模式前最小 IDLE Tick 数 (:ref:`CONFIG_FREERTOS_IDLE_TIME_BEFORE_SLEEP`)
     - 3

   * - 将 light sleep 相关代码放置在片内 RAM 中 (:ref:`CONFIG_PM_SLP_IRAM_OPT`)
     - OFF

   * - 将 RTOS IDLE 相关代码放置在片内 RAM 中 (:ref:`CONFIG_PM_RTOS_IDLE_OPT`)
     - OFF

   * - RTC 慢速时钟源 (:ref:`CONFIG_RTC_CLK_SRC`)
     - 内部 150 kHz 振荡器

   * - 芯片休眠时禁用所有 GPIO (:ref:`CONFIG_PM_SLP_DISABLE_GPIO`)
     - ON

   * - 在 light sleep 状态关闭 flash 供电 (:ref:`CONFIG_ESP_SLEEP_POWER_DOWN_FLASH`)
     - OFF

   * - ``max_freq_mhz``
     - 120

   * - ``min_freq_mhz``
     - 40

   * - ``light_sleep_enable``
     - true

  .. note::
      上表中不涉及的配置均是默认。

Deep-sleep 模式配置
---------------------

对 Deep-sleep 模式来说，除了唤醒源相关配置，其余配置意义已经不大。

配置方法：

1. 配置唤醒源（详见 :doc:`睡眠模式 <../../api-reference/system/sleep_modes>`）
2. 调用 API，具体如下

.. code-block:: none

   /* Enter deep sleep */
   esp_deep_sleep_start();

用户可以通过下列配置选项，让一些特定模块在休眠时保持开启状态：

- 打开外部 40 MHz 晶振 (XTAL)
    在一些特殊应用中，部分模块对休眠时的时钟精度及稳定度有很高要求（例如蓝牙模块）。这种情况下，可以考虑在休眠过程中打开 外部 40 MHz 晶振。
    打开和关闭代码如下::

      ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_ON));
      ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_OFF));

- 打开内部 8 MHz 振荡器 (OSC)
    在一些特殊应用中，部分模块（例如 LEDC 模块）将 内部 8 MHz 振荡器作为时钟源，并且希望在 light sleep 状态也可以正常使用。这种情况下，可以考虑在休眠过程中打开内部 8 MHz 振荡器。
    打开和关闭代码如下::

      ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_RTC8M, ESP_PD_OPTION_ON));
      ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_RTC8M, ESP_PD_OPTION_OFF));
