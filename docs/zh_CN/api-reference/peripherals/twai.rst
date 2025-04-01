双线汽车接口 (TWAI)
===================

:link_to_translation:`en:[English]`

本编程指南包含以下部分：

.. contents::
  :local:
  :depth: 1

.. -------------------------------- Overview -----------------------------------

概述
--------

双线汽车接口 (TWAI) 是一种适用于汽车和工业应用的实时串行通信协议。它兼容 ISO11898-1 经典帧，因此可以支持标准帧格式（11 位 ID）和扩展帧格式（29 位 ID）。{IDF_TARGET_NAME} 包含 {IDF_TARGET_CONFIG_SOC_TWAI_CONTROLLER_NUM} 个 TWAI 控制器，经配置可以在 TWAI 总线上使用外部收发器通信。

.. warning::

    TWAI 控制器不兼容 ISO11898-1 FD 格式帧，并会将这些帧解析为错误。

.. --------------------------- Basic TWAI Concepts -----------------------------

TWAI 协议概述
-------------

TWAI 是一种多主机、多播、异步、串行通信协议，该协议还支持错误检测和通报，并具有内置报文优先级。

**多主机：** 总线上的任何节点都可以发起报文传输。

**多播：** 节点传输报文时，总线上的所有节点都会接收该报文（即广播），确保所有节点数据一致。但通过接收过滤，某些节点可以选择性地接收报文（多播）。

**异步：** 总线不包含时钟信号。总线上的所有节点以相同的位速率运行，并使用在总线上传输位的边沿进行同步。

**错误检测和通报：** 每个节点不断监视总线。节点检测到错误时，通过传输错误帧通报检测到的错误。其他节点会接收错误帧，并传输自己的错误帧作为回应，这样一来检测的错误即可传播到总线上的所有节点。

**报文优先级：** 每个报文包含唯一的 ID 字段。如果两个或多个节点尝试同时传输，ID 小的节点将获得总线的控制权，而其他节点将自动转为接收器，确保无论何时最多只有一个发射器。

TWAI 报文
^^^^^^^^^

TWAI 报文分为数据帧和远程帧。数据帧用于向其他节点传递数据载荷，远程帧用于请求其他节点的数据帧，其他节点可以选择性地用数据帧响应。数据帧和远程帧有两种帧格式，称为 **扩展帧** 和 **标准帧**，分别包含了 29 位 ID 和 11 位 ID。TWAI 报文包括以下字段：

    - 29 位或 11 位的 ID：确定报文优先级，值越小优先级越高。
    - 0 到 8 之间的数据长度代码 (DLC)：以字节为单位，表示数据帧的数据载荷大小，或者远程帧请求的数据量。
    - 数据帧数据，最多为 8 个字节，应与 DLC 匹配。

错误状态和计数器
^^^^^^^^^^^^^^^^

TWAI 协议具备“故障隔离”功能，它可以使持续存在错误的节点最终自行断开总线。该功能通过要求每个节点维护两个内部错误计数器实现，这两个计数器分别称为 **发送错误计数器 (TEC)**和 **接收错误计数器 (REC)**。根据一组规则，这两个错误计数器在发生错误时递增，在报文发送/接收成功时递减。计数器值决定节点的 **错误状态**，即 **主动错误**、**被动错误** 和 **离线**。

**主动错误:** 当 **TEC 和 REC 都小于 128** 时，节点处于主动错误状态，表示节点正常运行。主动错误节点可以参与总线通信，并会自动通过总线发送 **主动错误标志**，主动报告检测到的错误。

**被动错误:** 当 **TEC 或 REC 中的一个大于或等于 128** 时，节点处于被动错误状态。被动错误的节点仍可以参与总线通信，但在检测到错误时，只能发送一次 **被动错误标志**。

**离线:** 当 **TEC 大于或等于 256** 时，节点进入离线状态。离线的节点无法对总线产生任何影响，相当于断开连接，进而从总线自行清除。节点将保持离线状态，直到触发离线恢复。

.. ---------------------- Signal Lines and Transceiver -------------------------

信号线和收发器
--------------

TWAI 控制器不含集成收发器。因此，要将 TWAI 控制器连接到 TWAI 总线，**需要外部收发器**。所使用的外部收发器类型应根据应用的物理层规范而定。例如，使用 SN65HVD23x 收发器以兼容 ISO 11898-2。

TWAI 控制器的接口由四条信号线组成，分别为 **TX、RX、BUS-OFF 和 CLKOUT**。这四条信号线可以通过 GPIO 矩阵连接到 {IDF_TARGET_NAME} 的 GPIO 管脚上。

.. blockdiag:: ../../../_static/diagrams/twai/controller_signals.diag
    :caption: TWAI控制器的信号线
    :align: center

**TX 和 RX:** TX 和 RX 信号线用于与外部收发器通信。这两条信号线将显性位表示/解析为低逻辑电平 (0 V)，将隐性位表示/解析为高逻辑电平 (3.3 V)。

**BUS-OFF:**  BUS-OFF 信号线是 **可选** 的，在 TWAI 控制器进入离线状态时为低逻辑电平 (0 V)。否则，BUS-OFF 信号线将设置为高逻辑电平 (3.3 V)。

**CLKOUT:** CLKOUT 信号线是 **可选** 的，会输出控制器源时钟的分频时钟。

.. note::

    外部收发器 **必须在内部连接 TX 与 RX**，以便观察 TX 信号线上的逻辑电平变化。如果没有内部回环，TWAI 控制器将会将两个信号线上的逻辑电平差异解析为仲裁丢失或位错误。


.. ------------------------------ Configuration --------------------------------

API 命名规范
------------

.. note::

  TWAI 驱动程序提供了两套 API。其中一套是无句柄的，广泛适用于 IDF v5.2 之前的版本，但仅支持单个 TWAI 硬件控制器；另一套是带句柄的，其函数名称通常以 "v2" 为后缀，并支持任意数量的 TWAI 控制器。这两套 API 可以同时使用，但建议在新项目中使用 "v2" 版本的 API。

配置 TWAI 驱动
--------------

本节描述了如何配置 TWAI 驱动。

操作模式
^^^^^^^^^^^^^^^

TWAI 驱动支持以下操作模式：

**正常模式：** 正常模式支持 TWAI 控制器参与总线活动，如传输和接收报文/错误帧。发送报文时需要来自另一个节点的应答。

**无应答模式：** 无应答模式与正常模式类似，但不需要接收方发送应答信号，即使没有应答信号也会视为成功传输。这种模式在 TWAI 控制器（如传输回环）自测时非常有用。

**只听模式：** 此模式防止 TWAI 控制器干扰总线，因此会禁用报文/应答信号/错误帧的传输。但 TWAI 控制器仍然能够接收报文，只是不会应答。这种模式适用于总线监视应用。

报警
^^^^

TWAI 驱动程序包含报警功能，可以对应用层发起特定 TWAI 控制器或 TWAI 总线事件通知。在安装 TWAI 驱动程序时，可以选择启用报警，也可以在运行时通过调用 :cpp:func:`twai_reconfigure_alerts` 重新配置。随后，应用程序可以通过调用 :cpp:func:`twai_read_alerts` 等待任何已启用的报警发生。TWAI 驱动程序支持以下报警：

.. list-table:: TWAI Driver Alerts
    :widths: 40 60
    :header-rows: 1

    * - 报警标志
      - 描述
    * - ``TWAI_ALERT_TX_IDLE``
      - 队列中无待传输报文
    * - ``TWAI_ALERT_TX_SUCCESS``
      - 上一次传输成功
    * - ``TWAI_ALERT_RX_DATA``
      - 收到一帧数据并添加到 RX 队列
    * - ``TWAI_ALERT_BELOW_ERR_WARN``
      - 两个错误计数器都低于错误报警限制
    * - ``TWAI_ALERT_ERR_ACTIVE``
      - TWAI 控制器已进入主动错误状态
    * - ``TWAI_ALERT_RECOVERY_IN_PROGRESS``
      - TWAI 控制器正在进行离线恢复
    * - ``TWAI_ALERT_BUS_RECOVERED``
      - TWAI 控制器已成功完成离线恢复
    * - ``TWAI_ALERT_ARB_LOST``
      - 上一次传输丢失仲裁
    * - ``TWAI_ALERT_ABOVE_ERR_WARN``
      - 有错误计数器超过了错误报警限制
    * - ``TWAI_ALERT_BUS_ERROR``
      - 总线上发生了（位、填充、CRC、格式、ACK）错误
    * - ``TWAI_ALERT_TX_FAILED``
      - 上一次传输失败
    * - ``TWAI_ALERT_RX_QUEUE_FULL``
      - RX 队列已满，接收到的帧丢失
    * - ``TWAI_ALERT_ERR_PASS``
      - TWAI 控制器已进入被动错误状态
    * - ``TWAI_ALERT_BUS_OFF``
      - 离线条件已触发，TWAI 控制器无法干扰总线

.. note::

    TWAI 控制器的 **错误报警限制** 用于在被动错误状态之前，预先提醒应用程序发生了总线错误。TWAI 驱动程序将 **错误报警限制** 默认设置为 **96**。当 TEC 或 REC 大于或等于错误报警限制时，将引发报警 ``TWAI_ALERT_ABOVE_ERR_WARN``。当 TEC 和 REC 都返回到小于 **96** 的值时，将引发报警 ``TWAI_ALERT_BELOW_ERR_WARN``。

.. note::

    启用错误报警时，可以使用 ``TWAI_ALERT_AND_LOG`` 标志，让 TWAI 驱动程序把所有报警都记录到 UART。但是，如果启用了 :ref:`CONFIG_TWAI_ISR_IN_IRAM` 选项，则会禁用报警记录和 ``TWAI_ALERT_AND_LOG``。请参阅 :ref:`placing-isr-into-iram`。

.. note::

    ``TWAI_ALERT_ALL`` 和 ``TWAI_ALERT_NONE`` 宏也可在配置或重新配置期间，启用或禁用所有报警。

位时序
^^^^^^

可以使用结构体 :cpp:type:`twai_timing_config_t` 配置 TWAI 驱动程序运行的位速率，每个位的周期由多个 **时间定额** 组成，TWAI 控制器源时钟的预分频时钟确定 **时间定额** 的周期。单个位按顺序包含以下部分：

    1. **同步段** 由一个时间定额组成
    2. **时序段 1** 在采样点之前由 1 到 16 个时间定额组成
    3. **时序段 2** 在采样点之后由 1 到 8 个时间定额组成

{IDF_TARGET_MAX_BRP:default="32768", esp32="128", esp32s3="16384", esp32c3="16384"}

**波特率分频器 (BRP)** 通过对 TWAI 控制器的源时钟分频，确定每个时间定额的周期。在 {IDF_TARGET_NAME} 上，``brp`` 可以是 **从 2 到 {IDF_TARGET_MAX_BRP} 的任何偶数**。也可以将 :cpp:member:`twai_timing_config_t::quanta_resolution_hz` 设置为非零值，决定各时间定额的分辨率。此时，驱动程序即可计算底层 ``brp`` 值。此方法适用于需要设置不同的时钟源，但希望位速率保持不变的情况。

TWAI 控制器支持的时钟源请参阅 :cpp:type:`twai_clock_source_t`，可以在 :cpp:member:`twai_timing_config_t::clk_src` 中指定时钟源。

.. only:: esp32

    对于 v2.0 及更高芯片版本的 ESP32，``brp`` **还支持 132 到 256 之间的任何 4 的倍数**，可以将 :ref:`CONFIG_ESP32_REV_MIN` 设置为 v2.0 或更高版本以启用。

.. packetdiag:: ../../../_static/diagrams/twai/bit_timing.diag
    :caption: BRP = 8、时钟源频率为 80 MHz 时，位时序配置为 500 Kbit/s
    :align: center

数据位的采样点位于时序段 1 和时序段 2 的交汇处，启用 **三重采样** 会导致每个位采样 3 个时间定额，而不是 1 个，额外的采样点位于时序段 1 尾部。

**同步跳变宽度 (SJW)** 用于确定单个位时间可以为了同步而延长/缩短的最大时间定额数，``sjw`` 可以在 1 到 4 之间。

.. note::

    ``brp``、``tseg_1``、``tseg_2`` 和 ``sjw`` 的不同组合可以实现相同位速率。用户应考虑 **传播延迟、节点信息处理时间和相位误差** 等因素，根据总线的物理特性进行调整。

常用的位速率时序可以使用 **初始化宏**。以下是 TWAI 驱动程序提供的一些初始化宏。

.. list::

    - :c:macro:`TWAI_TIMING_CONFIG_1MBITS`
    - :c:macro:`TWAI_TIMING_CONFIG_800KBITS`
    - :c:macro:`TWAI_TIMING_CONFIG_500KBITS`
    - :c:macro:`TWAI_TIMING_CONFIG_250KBITS`
    - :c:macro:`TWAI_TIMING_CONFIG_125KBITS`
    - :c:macro:`TWAI_TIMING_CONFIG_100KBITS`
    - :c:macro:`TWAI_TIMING_CONFIG_50KBITS`
    - :c:macro:`TWAI_TIMING_CONFIG_25KBITS`
    :not esp32: - :c:macro:`TWAI_TIMING_CONFIG_20KBITS`
    :not esp32: - :c:macro:`TWAI_TIMING_CONFIG_16KBITS`
    :not esp32: - :c:macro:`TWAI_TIMING_CONFIG_12_5KBITS`
    :not esp32: - :c:macro:`TWAI_TIMING_CONFIG_10KBITS`
    :not esp32: - :c:macro:`TWAI_TIMING_CONFIG_5KBITS`
    :not esp32: - :c:macro:`TWAI_TIMING_CONFIG_1KBITS`

.. only:: esp32

    v2.0 或更高芯片版本的 ESP32 还支持以下位速率：

    - :c:macro:`TWAI_TIMING_CONFIG_20KBITS`
    - :c:macro:`TWAI_TIMING_CONFIG_16KBITS`
    - :c:macro:`TWAI_TIMING_CONFIG_12_5KBITS`

接收过滤器
^^^^^^^^^^

TWAI 控制器内置硬件接收过滤器，可以过滤特定 ID 的报文。过滤掉报文的节点 **不会接收到该报文，但仍会应答**。接收过滤器通过过滤掉总线上与节点无关的报文，使节点更加高效。接收过滤器使用在 :cpp:type:`twai_filter_config_t` 中的两个 32 位值配置，分别称为 **接收码** 和 **接收掩码**。

**接收码** 指定报文的 ID、RTR 和数据字节必须匹配的位序列，使报文可以由 TWAI 控制器接收。**接收掩码** 是一个位序列，指定接受码中可以忽略的位，从而实现用单个接收码接受不同 ID 的报文。

接收过滤器可以在 **单过滤器模式或双过滤器模式** 下使用。单过滤器模式使用接收代码和掩码定义一个过滤器，支持筛选标准帧的前两个数据字节，或扩展帧的 29 位 ID 的全部内容。以下图示说明了在单过滤器模式下解析 32 位接收代码和掩码的方式。注意：黄色和蓝色字段分别表示标准和扩展帧格式。

.. packetdiag:: ../../../_static/diagrams/twai/acceptance_filter_single.diag
    :caption: 单过滤器模式的位布局（右侧为最高有效位）
    :align: center

**双过滤器模式** 使用接收代码和掩码定义两个单独的过滤器，支持接收更多 ID，但不支持筛选扩展 ID 的全部 29 位。以下图示说明了在 **双过滤器模式** 下解析 32 位接收代码和掩码的方式。注意：黄色和蓝色字段分别表示标准和扩展帧格式。

.. packetdiag:: ../../../_static/diagrams/twai/acceptance_filter_dual.diag
    :caption: 双过滤器模式的位布局（右侧为最高有效位）
    :align: center

禁用 TX 队列
^^^^^^^^^^^^

可以将 :cpp:type:`twai_general_config_t` 结构体的 ``tx_queue_len`` 成员设置为 ``0``，在配置期间禁用 TX 队列。使用 TWAI 驱动程序时，禁用 TX 队列可以为不需要报文传输的应用程序节省一小部分内存。

.. _placing-isr-into-iram:

将 ISR 存入 IRAM
^^^^^^^^^^^^^^^^

TWAI 驱动程序的 ISR（中断服务程序）可以存入 IRAM，这样可以在禁用 cache 时运行 ISR。长时间禁用 cache 时（例如 SPI flash 写操作、OTA 更新等），可能需要将 ISR 存入 IRAM，才能确保 TWAI 驱动程序的功能。禁用 cache 时，ISR 继续执行以下操作：

- 从 RX buffer 读取接收到的报文，并将它们存入驱动程序的 RX 队列。
- 从驱动程序的 TX 队列中加载待传输的报文，并将它们写入 TX buffer。

将 TWAI 驱动程序的 ISR 存入 IRAM，必须执行以下操作：

- 使用 ``idf.py menuconfig`` 启用 :ref:`CONFIG_TWAI_ISR_IN_IRAM` 选项。
- 调用 :cpp:func:`twai_driver_install` 时，:cpp:type:`twai_general_config_t` 的成员 ``intr_flags`` 应设置为 :c:macro:`ESP_INTR_FLAG_IRAM`。

.. note::

    启用 :ref:`CONFIG_TWAI_ISR_IN_IRAM` 选项时，TWAI 驱动程序将不再记录报警，即 ``TWAI_ALERT_AND_LOG`` 标志失效。

.. only:: esp32

    ESP32 芯片错误变通方案
    ^^^^^^^^^^^^^^^^^^^^^^

    ESP32 的 TWAI 控制器有多个硬件错误，详情请参阅 `ESP32 系列芯片勘误表 <https://www.espressif.com/sites/default/files/documentation/eco_and_workarounds_for_bugs_in_esp32_cn.pdf>`_。其中一些错误至关重要，在特定情况下，可能会将 TWAI 控制器置于不可逆转的状态，即控制器在 CPU 重置它前一直卡住。

    TWAI 驱动程序为这些关键错误提供了变通方案，尽管可能降低性能，但可以使 ESP32 TWAI 驱动程序正常运行。性能下降可能造成以下影响，具体取决于遇到的错误：

    - TWAI 驱动程序间歇丢弃收到的报文。
    - TWAI 驱动程序可能在短时间内无响应，即不会在 11 位时间或更长时间内传输或发送应答信号。
    - 如果启用了 :ref:`CONFIG_TWAI_ISR_IN_IRAM`，该变通方案可能增加约 1 KB 的 IRAM 使用量。

    此软件变通方案默认启用，建议保持其启用状态。

.. ------------------------------- TWAI Driver ---------------------------------

驱动程序操作
------------

TWAI 驱动程序经设计，具有明确定义的状态和严格的规则，规定了触发状态转换的函数或条件。下图展示了各种状态及其转换。

.. blockdiag:: ../../../_static/diagrams/twai/state_transition.diag
    :caption: TWAI 驱动程序状态转换图（请参阅下表）
    :align: center

.. list-table::
    :widths: 20 40 40
    :header-rows: 1

    * - 标签
      - 转换
      - 行为/条件
    * - A
      - 未安装 -> 已停止
      - :cpp:func:`twai_driver_install`
    * - B
      - 已停止 -> 未安装
      - :cpp:func:`twai_driver_uninstall`
    * - C
      - 已停止 -> 运行中
      - :cpp:func:`twai_start`
    * - D
      - 运行中 -> 已停止
      - :cpp:func:`twai_stop`
    * - E
      - 运行中 -> 离线
      - 传输错误计数 >= 256
    * - F
      - 离线 -> 未安装
      - :cpp:func:`twai_driver_uninstall`
    * - G
      - 离线 -> 恢复中
      - :cpp:func:`twai_initiate_recovery`
    * - H
      - 恢复中 -> 已停止
      - 11 个连续的隐性位出现了 128 次


驱动程序状态
^^^^^^^^^^^^

**未安装**：在此状态下，不会为驱动程序分配任何内存，且 TWAI 控制器处于掉电状态。

**已停止**：在此状态下，TWAI 控制器已上电，且 TWAI 驱动程序已安装。但 TWAI 控制器无法参与任何总线活动，如传输、接收或确认报文。

**运行中**：在此状态下，TWAI 控制器能够参与总线活动，因此可以传输/接收/应答报文。此外，TWAI 控制器能够在检测到总线上的错误时传输错误帧。

**离线**：TWAI 控制器的传输错误计数器计数大于或等于 256 时，将自动进入离线状态。离线状态表示总线或 TWAI 控制器上发生严重错误。在离线状态下，TWAI 控制器无法参与任何总线活动。退出离线状态前，TWAI 控制器必须进行离线恢复。

**恢复中**：TWAI 控制器进行离线恢复时，将进入恢复中状态。在此状态下，TWAI 控制器/TWAI 驱动程序将保持状态，直到在总线上检测到 128 次连续 11 个隐性位。

报文字段和标志
^^^^^^^^^^^^^^

TWAI 驱动程序通过 :cpp:type:`twai_message_t` 结构体的不同位字段成员区分不同类型的报文。这些位字段成员决定了报文是标准格式还是扩展格式、是否是远程帧以及在传输时要使用的传输类型。

这些位字段成员还可以使用 :cpp:type:`twai_message_t` 的 ``flags`` 成员以及以下报文标志切换：

.. list-table::
    :widths: 30 70
    :header-rows: 1

    * - 报文标志
      - 描述
    * - ``TWAI_MSG_FLAG_EXTD``
      - 报文采用扩展帧格式（29 位 ID）
    * - ``TWAI_MSG_FLAG_RTR``
      - 报文为远程帧（远程传输请求）
    * - ``TWAI_MSG_FLAG_SS``
      - 使用单次发送传输报文，即报文不会在出现错误或仲裁丢失时重新传输（不用于接收报文）
    * - ``TWAI_MSG_FLAG_SELF``
      - 使用自接收请求传输报文，即传输的报文将由同一节点接收（不用于接收报文）
    * - ``TWAI_MSG_FLAG_DLC_NON_COMP``
      - 报文的数据长度代码大于 8，不符合 TWAI 的规定
    * - ``TWAI_MSG_FLAG_NONE``
      - 清除所有位字段，等同于标准帧格式（11 位 ID）数据帧

.. -------------------------------- Examples -----------------------------------

示例
----

配置及安装
^^^^^^^^^^

以下代码片段展示了如何使用各种配置结构体、初始化宏、:cpp:func:`twai_driver_install` 函数和 :cpp:func:`twai_start` 函数，来配置、安装和启动 TWAI 驱动程序。

.. code-block:: c

    #include "driver/gpio.h"
    #include "driver/twai.h"

    void app_main()
    {
        // 使用初始化宏初始化配置结构体
        twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_21, GPIO_NUM_22, TWAI_MODE_NORMAL);
        twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
        twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

        // 安装 TWAI 驱动程序
        if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
            printf("Driver installed\n");
        } else {
            printf("Failed to install driver\n");
            return;
        }

        // 启动 TWAI 驱动程序
        if (twai_start() == ESP_OK) {
            printf("Driver started\n");
        } else {
            printf("Failed to start driver\n");
            return;
        }

        ...

    }

初始化宏并非强制的，每个配置结构体都可以手动完成。

安装多个 TWAI 实例
^^^^^^^^^^^^^^^^^^

**注意：** {IDF_TARGET_NAME} 包含 {IDF_TARGET_CONFIG_SOC_TWAI_CONTROLLER_NUM} 个 TWAI 控制器，因此可创建 {IDF_TARGET_CONFIG_SOC_TWAI_CONTROLLER_NUM} 个 TWAI 实例。

以下代码片段演示了如何使用 :cpp:func:`twai_driver_install_v2` 函数来安装多个 TWAI 实例。

.. code-block:: c

    #include "driver/gpio.h"
    #include "driver/twai.h"

    void app_main()
    {
        twai_handle_t twai_bus_0;
        twai_handle_t twai_bus_1;
        // 使用宏初始化器初始化配置结构体
        twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_0, GPIO_NUM_1, TWAI_MODE_NORMAL);
        twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
        twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

        // 安装 TWAI 总线 0 的驱动程序
        g_config.controller_id = 0;
        if (twai_driver_install_v2(&g_config, &t_config, &f_config, &twai_bus_0) == ESP_OK) {
            printf("Driver installed\n");
        } else {
            printf("Failed to install driver\n");
            return;
        }
        // 启动 TWAI 驱动程序
        if (twai_start_v2(twai_bus_0) == ESP_OK) {
            printf("Driver started\n");
        } else {
            printf("Failed to start driver\n");
            return;
        }

        // 安装 TWAI 总线 1 的驱动程序
        g_config.controller_id = 1;
        g_config.tx_io = GPIO_NUM_2;
        g_config.rx_io = GPIO_NUM_3;
        if (twai_driver_install_v2(&g_config, &t_config, &f_config, &twai_bus_1) == ESP_OK) {
            printf("Driver installed\n");
        } else {
            printf("Failed to install driver\n");
            return;
        }
        // 启动 TWAI 驱动程序
        if (twai_start_v2(twai_bus_1) == ESP_OK) {
            printf("Driver started\n");
        } else {
            printf("Failed to start driver\n");
            return;
        }

        // 其他驱动程序操作也必须使用 _v2 版本的 API
        ...

    }

报文传输
^^^^^^^^

以下代码片段展示了如何使用 :cpp:type:`twai_message_t` 类型和 :cpp:func:`twai_transmit` 函数传输报文。

.. code-block:: c

    #include "driver/twai.h"

    ...

    // 配置要传输的报文
    twai_message_t message = {
        // 设置报文类型及格式
        .extd = 1,              // 标准格式或是扩展格式
        .rtr = 0,               // 数据帧或是远程传输请求帧
        .ss = 0,                // 报文是否为单次发送（即，在报错时不重复发送）
        .self = 0,              // 报文是否为自收发（回环）
        .dlc_non_comp = 0,      // 数据长度代码小于 8
        // 报文 ID 及有效载荷
        .identifier = 0xAAAA,
        .data_length_code = 4,
        .data = {0, 1, 2, 3},
    };

    // 报文排队等待传输
    if (twai_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {
        printf("Message queued for transmission\n");
    } else {
        printf("Failed to queue message for transmission\n");
    }

报文接收
^^^^^^^^

以下代码片段展示了如何使用 :cpp:type:`twai_message_t` 类型和 :cpp:func:`twai_receive` 函数接收报文。

.. code-block:: c

    #include "driver/twai.h"

    ...

    // 等待报文接收
    twai_message_t message;
    if (twai_receive(&message, pdMS_TO_TICKS(10000)) == ESP_OK) {
        printf("Message received\n");
    } else {
        printf("Failed to receive message\n");
        return;
    }

    // 处理接收到的报文
    if (message.extd) {
        printf("Message is in Extended Format\n");
    } else {
        printf("Message is in Standard Format\n");
    }
    printf("ID is %d\n", message.identifier);
    if (!(message.rtr)) {
        for (int i = 0; i < message.data_length_code; i++) {
            printf("Data byte %d = %d\n", i, message.data[i]);
        }
    }

重新配置并读取报警
^^^^^^^^^^^^^^^^^^

以下代码片段展示了如何使用 :cpp:func:`twai_reconfigure_alerts` 和 :cpp:func:`twai_read_alerts` 函数重新配置和读取 TWAI 驱动程序的报警。

.. code-block:: c

    #include "driver/twai.h"

    ...

    // 重新配置报警，检测被动错误和离线状态
    uint32_t alerts_to_enable = TWAI_ALERT_ERR_PASS | TWAI_ALERT_BUS_OFF;
    if (twai_reconfigure_alerts(alerts_to_enable, NULL) == ESP_OK) {
        printf("Alerts reconfigured\n");
    } else {
        printf("Failed to reconfigure alerts");
    }

    // 阻塞直到发生报警
    uint32_t alerts_triggered;
    twai_read_alerts(&alerts_triggered, portMAX_DELAY);

停止和卸载
^^^^^^^^^^

以下代码片段展示了如何使用 :cpp:func:`twai_stop` 和 :cpp:func:`twai_driver_uninstall` 函数停止和卸载 TWAI 驱动程序。

.. code-block:: c

    #include "driver/twai.h"

    ...

    // 停止 TWAI 驱动程序
    if (twai_stop() == ESP_OK) {
        printf("Driver stopped\n");
    } else {
        printf("Failed to stop driver\n");
        return;
    }

    // 卸载 TWAI 驱动程序
    if (twai_driver_uninstall() == ESP_OK) {
        printf("Driver uninstalled\n");
    } else {
        printf("Failed to uninstall driver\n");
        return;
    }

配置多个 ID 过滤器
^^^^^^^^^^^^^^^^^^

:cpp:type:`twai_filter_config_t` 中的接收掩码可以配置，使单过滤器接收两个或多个 ID。为了使特定过滤器接受多个 ID，必须在接收掩码中设置不同 ID 的冲突位。接收代码可以设置为这些 ID 中的任何一个。

以下示例展示了如何计算多个 ID 的接收掩码：

.. code-block::

    ID1 =  11'b101 1010 0000
    ID2 =  11'b101 1010 0001
    ID3 =  11'b101 1010 0100
    ID4 =  11'b101 1010 1000
    // 接收掩码
    MASK = 11'b000 0000 1101

应用示例
^^^^^^^^

**网络示例：** :example:`peripherals/twai/twai_network` 演示了如何通过 TWAI 驱动程序 API 在两个 {IDF_TARGET_NAME} 之间进行通信。其中一个 TWAI 节点为网络主节点，负责启动和终止与另一个网络从节点之间的数据传输。

**报警和恢复示例：** :example:`peripherals/twai/twai_alert_and_recovery` 演示了如何在 {IDF_TARGET_NAME} 上使用 TWAI 驱动程序的报警和离线恢复 API。通过初始化驱动程序，该示例创建消息传输和警报处理任务，触发比特错误进入 Bus-Off 离线状态，报警检测离线状态，并触发 Bus-Off 离线恢复过程。

**自测示例：** :example:`peripherals/twai/twai_self_test` 演示了节点如何使用 TWAI 驱动程序的无应答模式和自接收请求，向自身传输 TWAI 消息。此示例可用于测试目标芯片与外部收发器之间的连接是否正常。

.. only:: SOC_TWAI_SUPPORT_SLEEP_RETENTION

    睡眠保留
    ^^^^^^^^

    {IDF_TARGET_NAME} 支持在进入 **Light Sleep** 之前保留 TWAI 寄存器中的内容，并在唤醒后恢复。即程序不需要在 **Light Sleep** 唤醒后重新配置 TWAI

    该特性可以通过置位配置中的 :cpp:member:`twai_general_config_t::sleep_allow_pd` 标志位启用。启用后驱动允许系统在 Light Sleep 时对 TWAI 掉电，同时保存寄存器配置。它可以帮助降低轻度睡眠时的功耗，但需要花费一些额外的存储来保存寄存器的配置。

.. ---------------------------- API Reference ----------------------------------

API 参考
-------------

.. include-build-file:: inc/twai_types.inc
.. include-build-file:: inc/twai.inc
