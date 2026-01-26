Wi-Fi 场景下低功耗模式介绍
==========================

:link_to_translation:`en:[English]`

本节将结合纯系统下的功耗模式来介绍 Wi-Fi 场景下的低功耗模式。因为 Wi-Fi 场景的复杂性，本节会首先介绍 Wi-Fi 省电的基本原理，然后再介绍具体的低功耗模式。本节主要针对 station 模式。

.. todo - add sleep-current/esp32c61_summary.inc

.. only:: not esp32c61

  Wi-Fi 场景如何选择低功耗模式
  --------------------------------------

  在介绍具体内容前先给出 Wi-Fi 场景下低功耗模式总结表，以方便用户根据需求快速选择想要了解的内容，选择合适的低功耗模式。

  .. include:: ../sleep-current/{IDF_TARGET_PATH_NAME}_summary.inc

  .. note::

      上表中所有电流均为平均电流，所测数据均在屏蔽箱中测试得出。表中术语在下文均有介绍，用户可根据需求进行查看。

.. _Basic Principles of Wi-Fi Power Saving:

Wi-Fi 省电的基本原理
---------------------

首先，在 station 的工作过程中，为在接收发送过程中避免冲突，需要长时间监听信道，能耗较大的 RF 模块会一直处于工作中，浪费电量。为此，Wi-Fi 协议引入省电模式。

省电模式的基本原理是通过减少不必要的监听时间来降低耗能。AP 会缓存进入省电模式的 station 的包，同时周期发送包含 TIM 信息的 beacon 帧，TIM 会指示 AP 缓存的单播包。TIM 中，DTIM 较为特殊，其会缓存广播包，并以 n 个（由 AP 决定）TIM 为周期发送。对 station 来说，TIM 非必听，而 DTIM 为必听。因此，station 可以选择只在每一个 DTIM 帧前醒来打开 Wi-Fi 相关模块（RF 模块），而不必时刻处于监听状态，这样就能有效降低功耗。

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

                        DTIM4 省电模式示意图

其次，station 从打开到再次关闭 Wi-Fi 相关模块的时间也会影响功耗。除必要的数据传输处理时间外，主要有四项配置会影响时间长短：

  - 时钟准确性导致的 time offset，主要原因是时钟或多或少都会与理想的时间存在偏移，同时偏移的正负不定。
  - 处理 beacon 漏听后的时间，如漏听后持续监听时间、允许最多丢失 beacon 数目等，这段时间存不存在以及存在多久都不定，但是可以配置范围。
  - 为了确保能够接收突发数据包而添加的 active 时间，可由配置决定。
  - IDLE 时间是具体某些功耗模式进入条件要求。因此在满足通信需求的情况下，降低工作时间可以改善功耗表现。

.. code-block:: text

      ┌────────────┬────────────┬────────────┬────────────┬────────────┐
      │            │            │            │            │            │
      │time offset │ 必要处理时间 │   beacon   │   active   │   IDLE     │
      │            │            │ 漏听处理时间 │    时间     │   时间     │
      │            │            │            │            │            │
      └────────────┴────────────┴────────────┴────────────┴────────────┘
                          芯片工作时间组成图

此外，在 station 没有处于 Wi-Fi 接收或发送状态时，影响功耗的因素变成了芯片的其他模块。不同的功耗模式会配置不同的时钟源，或者动态调整一些模块的工作频率如 CPU，同时还会关闭不同数量的功能模块，这将有效降低芯片的功耗。用户可根据需求自己选择合适的配置。

如果以时间为横轴，电流大小为纵轴建立坐标轴，那么处在低功耗模式下芯片的理想工作电流图可以简化成下图：

.. code-block:: text

         ▲
    电流  |
    大小  |
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
                                                    时间
                理想情况下 Wi-Fi 场景低功耗模式电流图

其中 station 要进行 Wi-Fi 通信时，Wi-Fi 相关模块 (PHY) 开启，电流会显著上升，在工作完成前，电流会一直维持在一个较高的水平。工作完成后，芯片会关闭 Wi-Fi 相关模块，这时电流又会降低到一个较低水平。

可以看出影响功耗表现的主要有三点：interval、period 和 base current。

  - **interval** 是 station Wi-Fi 相关模块工作的间隔，既可以由低功耗模式自定义，也可根据 Wi-Fi 协议省电机制（详细内容请见 :ref:`Basic Principles of Wi-Fi Power Saving`），由 DTIM 周期决定。可以看出在同等情下，interval 越大，功耗表现会更好，但是响应会更慢，影响通信的及时性。

  - **period** 可以看作每次 station Wi-Fi 工作的时间，这段时间的长度也会影响功耗的表现。period 不是一个固定的时长（详细内容请见 :ref:`Basic Principles of Wi-Fi Power Saving`），在保证 Wi-Fi 通信正常的情况下，period 持续时间越短，功耗表现越好。但是减少 period 时间，必然会影响通信的可靠性。

  - **base current** 是 Wi-Fi 相关模块不工作时芯片的电流，影响其大小的因素很多，不同的功耗模式下休眠策略不同。所以，在满足功能的情况下，优化配置降低该电流大小可以提高功耗表现，但同时关闭其余模块会影响相关功能和芯片的唤醒时间。

知道了影响功耗的三点因素之后，要想降低功耗应从这三点入手，接下来介绍两种低功耗模式，Modem-sleep 模式、Auto Light-sleep 模式。两种模式主要区别就是对三点因素的优化不同。


Modem-sleep 模式
------------------

Modem-sleep 模式主要工作原理基于 DTIM 机制，周期性的醒来处理 Wi-Fi 相关工作，又在周期间隔之间进入休眠，关闭 PHY（RF 模块）来降低功耗。同时通过 DTIM 机制，station 可以与 AP 保持 Wi-Fi 连接，数据传输。

Modem-sleep 模式会在 Wi-Fi 任务结束后自动进入休眠无需调用 API，休眠时仅会关闭 Wi-Fi 相关模块 (PHY)，其余模块均处在正常上电状态。

Modem-sleep 模式默认会根据 DTIM 周期或 listen interval（于 :ref:`Modem-sleep Mode Configuration` 中介绍）醒来，相当于系统自动设置了一个 Wi-Fi 唤醒源，因此用户无需再配置唤醒源，同时系统主动发包时也可以唤醒。

Modem-sleep 模式是一个开关型的模式，调用 API 开启后一直自动运行，其工作流程十分简单，具体如下图。

.. code-block:: text

          ┌───────────┐      Wi-Fi 任务结束       ┌───────────┐
          │           ├────────────────────────►│   modem   │
          │   active  │                         │   sleep   │
          │           │◄────────────────────────┤           │
          └───────────┘        DTIM 周期到来      └───────────┘
                                  / 主动发包

                          Modem-sleep 模式工作流程图

根据上文的基本电流图，结合 Modem-sleep 模式的工作原理，以 Min Modem 模式（于 :ref:`Modem-sleep Mode Configuration` 中介绍）为例可得理想情况下电流变化图。

.. code-block:: text

       电流  ▲
       大小  |
            |                 DTIM 周期
            |             │ ◄──────────► │
            |          ┌─────┐        ┌─────┐        ┌─────┐
            |          │     │        │     │        │     │
            |   DTIM   |     |        |     |        |     |
            |   到来前  |     |        |     |        |     |  Wi-Fi 任务结束
            |       \  |     |        |     |        |     | /
            |        \ │     │        │     │        │     │/
            |  ────────┘     └────────┘     └────────┘     └──────
            |
            └─────────────────────────────────────────────────────►
                                                              时间
                          Min Modem-sleep 模式理想电流图

Modem-sleep 模式一般用于 CPU 持续处于工作状态并需要保持 Wi-Fi 连接的应用场景，例如，使用 {IDF_TARGET_NAME} 本地语音唤醒功能，CPU 需要持续采集和处理音频数据。

DFS + Modem-sleep 模式
-------------------------

Modem-sleep 模式休眠状态中 CPU 仍处在工作状态，而 DFS 机制主要作用于 CPU 和 APB 工作频率来降低功耗，因此 DFS + Modem sleep 模式可以进一步优化功耗表现，又因为 Wi-Fi 任务会申请 :cpp:enumerator:`ESP_PM_CPU_FREQ_MAX` 电源锁来保证 Wi-Fi 任务快速运行，所以 DFS + Modem-sleep 模式产生调频只会发生在 base current 阶段，即 Wi-Fi 任务结束后。

在 Wi-Fi 场景下，为了让用户抓住主要的变化，DFS 可以进行一定的状态简化。具体来说，虽然 DFS 主要根据 CPU 和 APB 两把锁的最高需求来调频，但是 Wi-Fi 场景都需要 CPU 的频率最大化来工作，同时 Wi-Fi 任务结束后，也可以理想化的认为，没有其余的工作要完成，这样就可以简单认为经过一段时间会释放两把锁进入空闲状态（IDLE 状态），也同时忽略这段时间锁的变化导致的电流变化，简化状态。

在 Wi-Fi 场景下，DFS 最终简化为如下流程：

.. code-block:: text

                                    ┌────────┐
                                    │        │
                                    │  DFS   │
                                    │        │
                                    └───┬────┘
                                        │
                                        ▼
    ┌──────────┐     系统空闲      ┌──────────┐
    │          │  ─────────────►  │          │
    │          │                  │          │
    │  active  │                  │   IDLE   │
    │          │                  │          │
    │          │  ◄─────────────  │          │
    └──────────┘    系统非空闲      └──────────┘

              Wi-Fi 场景 DFS 简化流程图

在 Wi-Fi 工作的 active 状态与系统空闲的 IDLE 状态转换，Wi-Fi 任务结束后，系统经过一段时间释放了所有锁进入 IDLE 状态，此时 DFS 机制降低频率到设定最低值，忽略了转换状态期间的调频动作，方便理解。

简化过后的 DFS + Modem sleep 模式理想状态下的电流大小如下图所示：

.. code-block:: text

    电流 ▲                  Wi-Fi 任务结束
    大小 |                  / modem sleep
        |                 /
        |          ┌─────┐                 ┌─────┐
        |          │     │                 │     │
        |          |     |                 |     |   锁释放完
        |          |     |                 |     |   DFS 降频
        |          |     |                 |     |   IDLE
        |          |     |                 |     |    /
        |          |     |                 |     |   /
        |          |     │                 |     │  /
        |          │     └─┐               │     └─┐
        |  ────────┘       └───────────────┘       └─────────
        |
        └─────────────────────────────────────────────────────►
                                                          时间
                DFS + Modem sleep 模式理想电流图

Auto Light-sleep 模式 + Wi-Fi 场景
------------------------------------

Auto Light-sleep 模式在 Wi-Fi 场景下是 ESP-IDF 电源管理机制、DTIM 机制和 Light-sleep 模式的结合。开启电源管理是其前置条件，auto 体现在系统进入 IDLE 状态超过设定值后自动进入 light sleep。同时 Auto Light-sleep 模式同样遵循 DTIM 机制，会自动苏醒，可以与 AP 保持 Wi-Fi 连接。

Auto Light-sleep 模式在 Wi-Fi 场景下休眠机制与纯系统下一样，仍然依赖于电源管理机制，进入休眠的条件为系统处于 IDLE 状态的时间超过设定时间，并且系统会提前判断空闲时间是否满足条件，若满足直接休眠。该过程为自动进行。休眠时会自动关闭 RF、8 MHz 振荡器、40 MHz 高速晶振、PLL，门控数字内核时钟，暂停 CPU 工作。

Auto Light-sleep 模式在 Wi-Fi 场景下遵循 DTIM 机制，自动在 DTIM 帧到来前苏醒，相当于系统自动设置了一个 Wi-Fi 唤醒源，因此用户无需再配置唤醒源。同时系统主动发包时也可以唤醒。

Auto Light-sleep 模式在 Wi-Fi 场景下工作流程较为复杂，但全程都是自动进行，具体如下图所示：

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
      │                    DTIM 周期到来 / 主动发包                     │
      └──────────────────────────────────────────────────────────────┘

                        Auto Light-sleep 模式工作流程图

Auto Light-sleep 模式在 Wi-Fi 场景下经常与 Modem-sleep 模式同时开启，这里给出 Modem + Auto Light-sleep 模式的理想电流图，关键节点均在图上标出。

.. code-block:: text

                                DTIM 周期
    电流 ▲             │ ◄───────────────────► │
    大小 |          ┌─────┐                 ┌─────┐
        |          │     │ \               │     │
        |          |     |  \              |     |
        |          |     | Wi-Fi 任务结束   |     |
        |          |     |  modem sleep    |     |
        |          |     |                 |     |
        |          |     |                 |     |      系统判断空闲
        |  DTIM    |     |     系统 IDLE    |     |     时间超过设定值
        |  到来前   |     │     DFS 降频     |     │       light sleep
        |       \  |     └─┐    /          |     └─┐    /
        |        \ │       └─┐ /           │       └─┐ /
        |  ────────┘         └─────────────┘         └─────────
        |
        └─────────────────────────────────────────────────────►
                                                          时间
              Modem + Auto Light-sleep 模式理想电流图

Auto Light-sleep 模式在 Wi-Fi 场景下可用于需要保持 Wi-Fi 连接，可以实时响应 AP 发来数据的场景。并且在未接收到命令时，CPU 可以处于空闲状态。比如 Wi-Fi 开关的应用，大部分时间 CPU 都是空闲的，直到收到控制命令，CPU 才需要进行 GPIO 的操作。


Deep-sleep 模式 + Wi-Fi 场景
------------------------------

Deep-sleep 模式在 Wi-Fi 场景下与纯系统下基本相同，详情可以参考 :ref:`Deep-sleep 模式`，这里不再介绍。


如何配置 Wi-Fi 场景下低功耗模式
----------------------------------------

介绍完 Wi-Fi 场景下低功耗模式后，本节将介绍公共配置选项、每种模式独有的配置选项，以及相应低功耗模式 API 的使用说明，同时给出相应模式推荐的配置（包含纯系统下的低功耗推荐配置）以及该配置的具体表现。

.. note::

      下文为配置选项的简单介绍，点击相应链接获取详细内容。

公共配置选项
++++++++++++++

- 功耗类：

  - Max Wi-Fi TX power (dBm) (:ref:`CONFIG_ESP_PHY_MAX_WIFI_TX_POWER`)

- 速度优化类：

  - Wi-Fi IRAM 速度优化 (:ref:`CONFIG_ESP_WIFI_IRAM_OPT`)
  - Wi-Fi RX IRAM 速度优化 (:ref:`CONFIG_ESP_WIFI_RX_IRAM_OPT`)
  - Wi-Fi Sleep IRAM 速度优化 (:ref:`CONFIG_ESP_WIFI_SLP_IRAM_OPT`)


- Wi-Fi 协议类：

  - Minimum active time (:ref:`CONFIG_ESP_WIFI_SLP_DEFAULT_MIN_ACTIVE_TIME`)

  - Maximum keep alive time (:ref:`CONFIG_ESP_WIFI_SLP_DEFAULT_MAX_ACTIVE_TIME`)

  - 周期性发送无条件 ARP (:ref:`CONFIG_LWIP_ESP_GRATUITOUS_ARP`)

  - 丢失 beacon 时睡眠优化 (:ref:`CONFIG_ESP_WIFI_SLP_BEACON_LOST_OPT`)


.. _Modem-sleep Mode Configuration:

Modem-sleep 模式配置
+++++++++++++++++++++++

- 可配置选项

  - Min Modem
      该参数表示 station 按照 DTIM 周期工作，在每个 DTIM 前醒来接收 beacon，这样不会漏掉广播信息，但是 DTIM 周期由 AP 决定，如果 DTIM 周期较短，省电效果会降低。

  - Max Modem
      该参数表示 station 会自定义一个 listen interval，并以 listen interval 为周期醒来接受 beacon。这样在 listen interval 较大时会省电，但是容易漏听 DTIM，错过广播数据。


- 配置方法

  - 调用 API，选择模式参数

    .. code-block:: none

        typedef enum {
            WIFI_PS_NONE,
            WIFI_PS_MIN_MODEM,
            WIFI_PS_MAX_MODEM,
        } wifi_ps_type_t;
        esp_err_t esp_wifi_set_ps(wifi_ps_type_t type);

    若选择 WIFI_PS_MAX_MODEM，还需配置 listen interval，示例如下::

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

- 推荐配置

    这里给出的推荐配置是 Min Modem-sleep 模式 + DFS 开启的配置

    .. list-table::
        :header-rows: 1
        :widths: 20 15

        * - 配置名称
          - 设置情况

        * - WIFI_PS_MIN_MODEM
          - ON

        * - CONFIG_PM_ENABLE
          - ON

        * - RTOS Tick rate (Hz)
          - 1000

        * - ``max_freq_mhz``
          - {IDF_TARGET_NAME} 支持的最大 CPU 频率

        * - ``min_freq_mhz``
          - {CONFIG_XTAL_FREQ}

        * - ``light_sleep_enable``
          - false

    .. todo - add sleep-current/esp32c61_modem_sleep.inc

    .. only:: not esp32c61

      - 配置表现

          .. include:: ../sleep-current/{IDF_TARGET_PATH_NAME}_modem_sleep.inc


Auto Light-sleep 模式 + Wi-Fi 场景配置
+++++++++++++++++++++++++++++++++++++++++

Auto Light-sleep 在 Wi-Fi 场景下的配置比纯系统下少了唤醒源的配置要求，其余几乎与纯系统下配置一致，因此可配置选项、配置步骤、推荐配置的详细介绍可以参考上文 :ref:`Deep-sleep 模式`。同时 Wi-Fi 相关配置保持默认。

.. todo - add sleep-current/esp32c61_light_sleep.inc

.. only:: not esp32c61

  - 配置表现

      该配置表现为 Auto Light-sleep 模式纯系统推荐配置 + 默认的 Wi-Fi 相关配置在 Wi-Fi 场景的表现。

      .. include:: ../sleep-current/{IDF_TARGET_PATH_NAME}_light_sleep.inc

Deep-sleep 模式 + Wi-Fi 场景配置
+++++++++++++++++++++++++++++++++++

Deep-sleep 模式在 Wi-Fi 场景下的配置与纯系统下配置基本一致，因此可配置选项、配置步骤、推荐配置的详细介绍可以参考上文 :ref:`Deep-sleep 模式`。同时 Wi-Fi 相关配置保持默认。

- 配置表现

    该配置表现为 Deep-sleep 模式纯系统推荐配置 + 默认的 Wi-Fi 相关配置在 Wi-Fi 场景的表现。

    .. only:: esp32

      平均电流约 5.0 μA

    .. only:: esp32s2

      平均电流约 5.0 μA

    .. only:: esp32s3

      平均电流约 6.9 μA

    .. only:: esp32c3

      平均电流约 4.8 μA

    .. only:: esp32c2

      平均电流约 4.9 μA

    .. only:: esp32c6

      平均电流约 6.7 μA

    .. only:: esp32c5

      平均电流约 10.0 μA
