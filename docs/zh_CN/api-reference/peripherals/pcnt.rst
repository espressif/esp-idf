脉冲计数器 (PCNT)
=================

概述
-----

PCNT 用于统计输入信号的上升沿和/或下降沿的数量。{IDF_TARGET_NAME} 集成了多个脉冲计数单元，[1]_ 每个单元都是包含多个通道的独立计数器。通道可独立配置为统计上升沿或下降沿数量的递增计数器或递减计数器。

PCNT 通道可检测 **边沿** 信号及 **电平** 信号。对于比较简单的应用，检测边沿信号就足够了。PCNT 通道可检测上升沿信号、下降沿信号，同时也能设置为递增计数，递减计数，或停止计数。电平信号就是所谓的 **控制信号**，可用来控制边沿信号的计数模式。通过设置电平信号与边沿信号的检测模式，PCNT 单元可用作 **正交解码器**。

每个 PCNT 单元还包含一个滤波器，用于滤除线路毛刺。

PCNT 模块通常用于：

-  对一段时间内的脉冲计数，进而计算得到周期信号的频率；
-  对正交信号进行解码，进而获得速度和方向信息。

功能描述
--------

PCNT 的功能从以下几个方面进行说明：

.. list::

    - :ref:`pcnt-resource-allocation` - 说明如何通过配置分配 PCNT 单元和通道，以及在相应操作完成之后，如何回收单元和通道。
    - :ref:`pcnt-setup-channel-actions` - 说明如何设置通道针对不同信号沿和电平进行操作。
    - :ref:`pcnt-watch-points` - 说明如何配置观察点，即当计数达到某个数值时，命令 PCNT 单元触发某个事件。
    :SOC_PCNT_SUPPORT_STEP_NOTIFY: - :ref:`pcnt-step-notify` - 说明如何配置观察步进，即当计数增量达到某个数值时，命令 PCNT 单元触发某个事件。
    - :ref:`pcnt-register-event-callbacks` - 说明如何将您的代码挂载到观察点事件的回调函数上。
    - :ref:`pcnt-set-glitch-filter` - 说明如何使能毛刺滤波器并设置其时序参数。
    :SOC_PCNT_SUPPORT_CLEAR_SIGNAL: - :ref:`pcnt-set-clear-signal` - 说明如何使能外部清零信号并设置其参数。
    - :ref:`pcnt-enable-disable-unit` - 说明如何使能和关闭 PCNT 单元。
    - :ref:`pcnt-unit-io-control` - 说明 PCNT 单元的 IO 控制功能，例如使能毛刺滤波器，开启和停用 PCNT 单元，获取和清除计数。
    - :ref:`pcnt-power-management` - 说明哪些功能会阻止芯片进入低功耗模式。
    - :ref:`pcnt-iram-safe` - 说明在缓存禁用的情况下，如何执行 PCNT 中断和 IO 控制功能。
    - :ref:`pcnt-thread-safe` - 列出线程安全的 API。
    - :ref:`pcnt-kconfig-options` - 列出了支持的 Kconfig 选项，这些选项可实现不同的驱动效果。

.. _pcnt-resource-allocation:

分配资源
^^^^^^^^^^^^^

PCNT 单元和通道分别用 :cpp:type:`pcnt_unit_handle_t` 与 :cpp:type:`pcnt_channel_handle_t` 表示。所有的可用单元和通道都由驱动在资源池中进行维护，无需了解底层实例 ID。

安装 PCNT 单元
~~~~~~~~~~~~~~~~~~

安装 PCNT 单元时，需要先完成配置 :cpp:type:`pcnt_unit_config_t`：

.. list::

    -  :cpp:member:`pcnt_unit_config_t::low_limit` 与 :cpp:member:`pcnt_unit_config_t::high_limit` 用于指定内部计数器的最小值和最大值。当计数器超过任一限值时，计数器将归零。
    -  :cpp:member:`pcnt_unit_config_t::accum_count` 用于设置是否需要软件在硬件计数值溢出的时候进行累加保存，这有助于“拓宽”计数器的实际位宽。默认情况下，计数器的位宽最高只有 16 比特。请参考 :ref:`pcnt-compensate-overflow-loss` 了解如何利用此功能来补偿硬件计数器的溢出损失。
    :SOC_PCNT_SUPPORT_STEP_NOTIFY: -  :cpp:member:`pcnt_unit_config_t::en_step_notify_up` 配置是否使能观察正方向步进。
    :SOC_PCNT_SUPPORT_STEP_NOTIFY: -  :cpp:member:`pcnt_unit_config_t::en_step_notify_down` 配置是否使能观察负方向步进。
    -  :cpp:member:`pcnt_unit_config_t::intr_priority` 设置中断的优先级。如果设置为 ``0``，则会分配一个默认优先级的中断，否则会使用指定的优先级。

.. note::

    由于所有 PCNT 单元共享一个中断源，安装多个 PCNT 单元时请确保每个单元的中断优先级 :cpp:member:`pcnt_unit_config_t::intr_priority` 一致。

调用函数 :cpp:func:`pcnt_new_unit` 并将 :cpp:type:`pcnt_unit_config_t` 作为其输入值，可对 PCNT 单元进行分配和初始化。该函数正常运行时，会返回一个 PCNT 单元句柄。没有可用的 PCNT 单元时（即 PCNT 单元全部被占用），该函数会返回错误 :c:macro:`ESP_ERR_NOT_FOUND`。可用的 PCNT 单元总数记录在 :c:macro:`SOC_PCNT_UNITS_PER_GROUP` 中，以供参考。

如果不再需要之前创建的某个 PCNT 单元，建议通过调用 :cpp:func:`pcnt_del_unit` 来回收该单元，从而该单元可用于其他用途。删除某个 PCNT 单元之前，需要满足以下条件：

- 该单元处于初始状态，即该单元要么已经被 :cpp:func:`pcnt_unit_disable` 禁用，要么尚未使能。
- 附属于该单元的通道已全部被 :cpp:func:`pcnt_del_channel` 删除。

.. code:: c

    #define EXAMPLE_PCNT_HIGH_LIMIT 100
    #define EXAMPLE_PCNT_LOW_LIMIT  -100

    pcnt_unit_config_t unit_config = {
        .high_limit = EXAMPLE_PCNT_HIGH_LIMIT,
        .low_limit = EXAMPLE_PCNT_LOW_LIMIT,
    };
    pcnt_unit_handle_t pcnt_unit = NULL;
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));

安装 PCNT 通道
~~~~~~~~~~~~~~~~~~~

安装 PCNT 通道时，需要先初始化 :cpp:type:`pcnt_chan_config_t`，然后调用 :cpp:func:`pcnt_new_channel`。对 :cpp:type:`pcnt_chan_config_t` 配置如下所示：

-  :cpp:member:`pcnt_chan_config_t::edge_gpio_num` 与 :cpp:member:`pcnt_chan_config_t::level_gpio_num` 用于指定 **边沿** 信号和 **电平** 信号对应的 GPIO 编号。请注意，这两个参数未被使用时，可以设置为 `-1`，即成为 **虚拟 IO** 。对于一些简单的脉冲计数应用，电平信号或边沿信号是固定的（即不会发生改变），可将其设置为虚拟 IO，然后该信号会被连接到一个固定的高/低逻辑电平，这样就可以在通道分配时回收一个 GPIO，节省一个 GPIO 管脚资源。
-  :cpp:member:`pcnt_chan_config_t::virt_edge_io_level` 与 :cpp:member:`pcnt_chan_config_t::virt_level_io_level` 用于指定 **边沿** 信号和 **电平** 信号的虚拟 IO 电平，以保证这些控制信号处于确定状态。请注意，只有在 :cpp:member:`pcnt_chan_config_t::edge_gpio_num` 或 :cpp:member:`pcnt_chan_config_t::level_gpio_num` 设置为 `-1` 时，这两个参数才有效。
-  :cpp:member:`pcnt_chan_config_t::invert_edge_input` 与 :cpp:member:`pcnt_chan_config_t::invert_level_input` 用于确定信号在输入 PCNT 之前是否需要被翻转，信号翻转由 GPIO 矩阵（不是 PCNT 单元）执行。
-  :cpp:member:`pcnt_chan_config_t::io_loop_back` 仅用于调试，它可以使能 GPIO 的输入和输出路径。这样，就可以通过调用位于同一 GPIO 上的函数 :cpp:func:`gpio_set_level` 来模拟脉冲信号。

调用函数 :cpp:func:`pcnt_new_channel`，将 :cpp:type:`pcnt_chan_config_t` 作为输入值并调用 :cpp:func:`pcnt_new_unit` 返回的 PCNT 单元句柄，可对 PCNT 通道进行分配和初始化。如果该函数正常运行，会返回一个 PCNT 通道句柄。如果没有可用的 PCNT 通道（PCNT 通道资源全部被占用），该函数会返回错误 :c:macro:`ESP_ERR_NOT_FOUND`。可用的 PCNT 通道总数记录在 :c:macro:`SOC_PCNT_CHANNELS_PER_UNIT`，以供参考。注意，为某个单元安装 PCNT 通道时，应确保该单元处于初始状态，否则函数 :cpp:func:`pcnt_new_channel` 会返回错误 :c:macro:`ESP_ERR_INVALID_STATE`。

如果不再需要之前创建的某个 PCNT 通道，建议通过调用 :cpp:func:`pcnt_del_channel` 回收该通道，从而该通道可用于其他用途。

.. code:: c

    #define EXAMPLE_CHAN_GPIO_A 0
    #define EXAMPLE_CHAN_GPIO_B 2

    pcnt_chan_config_t chan_config = {
        .edge_gpio_num = EXAMPLE_CHAN_GPIO_A,
        .level_gpio_num = EXAMPLE_CHAN_GPIO_B,
    };
    pcnt_channel_handle_t pcnt_chan = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_config, &pcnt_chan));

.. _pcnt-setup-channel-actions:

设置通道操作
^^^^^^^^^^^^^^

当输入脉冲信号切换时，PCNT 通道会增加，减少或停止计数。边沿信号及电平信号可设置为不同的计数器操作。

-  :cpp:func:`pcnt_channel_set_edge_action` 为输入到 :cpp:member:`pcnt_chan_config_t::edge_gpio_num` 的信号上升沿和下降沿设置操作，:cpp:type:`pcnt_channel_edge_action_t` 中列出了支持的操作。
-  :cpp:func:`pcnt_channel_set_level_action` 为输入到 :cpp:member:`pcnt_chan_config_t::level_gpio_num` 的信号高电平和低电平设置操作，:cpp:type:`pcnt_channel_level_action_t` 中列出了支持的操作。使用 :cpp:func:`pcnt_new_channel` 分配 PCNT 通道时，如果 :cpp:member:`pcnt_chan_config_t::level_gpio_num` 被设置为 `-1`，就无需对该函数进行设置了。

.. code:: c

    // decrease the counter on rising edge, increase the counter on falling edge
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    // keep the counting mode when the control signal is high level, and reverse the counting mode when the control signal is low level
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

.. _pcnt-watch-points:

PCNT 观察点
^^^^^^^^^^^

PCNT 单元可被设置为观察几个特定的数值，这些被观察的数值被称为 **观察点**。观察点不能超过 :cpp:type:`pcnt_unit_config_t` 设置的范围，最小值和最大值分别为 :cpp:member:`pcnt_unit_config_t::low_limit` 和 :cpp:member:`pcnt_unit_config_t::high_limit`。当计数器到达任一观察点时，会触发一个观察事件，如果在 :cpp:func:`pcnt_unit_register_event_callbacks` 注册过事件回调函数，该事件就会通过中断发送通知。关于如何注册事件回调函数，请参考 :ref:`pcnt-register-event-callbacks`。

观察点分别可以通过 :cpp:func:`pcnt_unit_add_watch_point` 和 :cpp:func:`pcnt_unit_remove_watch_point` 进行添加和删除。常用的观察点包括 **过零**、**最大/最小计数** 以及其他的阈值。可用的观察点是有限的，如果 :cpp:func:`pcnt_unit_add_watch_point` 无法获得空闲硬件资源来存储观察点，会返回错误 :c:macro:`ESP_ERR_NOT_FOUND`。不能多次添加同一个观察点，否则将返回错误 :c:macro:`ESP_ERR_INVALID_STATE`。

建议通过 :cpp:func:`pcnt_unit_remove_watch_point` 删除未使用的观察点来回收资源。

.. code:: c

    // add zero across watch point
    ESP_ERROR_CHECK(pcnt_unit_add_watch_point(pcnt_unit, 0));
    // add high limit watch point
    ESP_ERROR_CHECK(pcnt_unit_add_watch_point(pcnt_unit, EXAMPLE_PCNT_HIGH_LIMIT));

.. only:: not SOC_PCNT_SUPPORT_RUNTIME_THRES_UPDATE

    .. note::

        由于硬件上的限制，在添加一个新的观察点后，你需要调用 :cpp:func:`pcnt_unit_clear_count` 函数来使之生效。

.. only:: SOC_PCNT_SUPPORT_STEP_NOTIFY

    .. _pcnt-step-notify:

    PCNT 观察步进
    ^^^^^^^^^^^^^^^^

    PCNT 单元可被设置为观察一个特定的数值增量（可以是正方向或负方向），这个观察数值增量的功能被称为 **观察步进**。启用观察步进需要使能 :cpp:member:`pcnt_unit_config_t::en_step_notify_up` 或 :cpp:member:`pcnt_unit_config_t::en_step_notify_down` 选项。 步进间隔不能超过 :cpp:type:`pcnt_unit_config_t` 设置的范围，最小值和最大值分别为 :cpp:member:`pcnt_unit_config_t::low_limit` 和 :cpp:member:`pcnt_unit_config_t::high_limit`。当计数器增量到达步进间隔时，会触发一个观察事件，如果在 :cpp:func:`pcnt_unit_register_event_callbacks` 注册过事件回调函数，该事件就会通过中断发送通知。关于如何注册事件回调函数，请参考 :ref:`pcnt-register-event-callbacks`。

    观察步进分别可以通过 :cpp:func:`pcnt_unit_add_watch_step` 和 :cpp:func:`pcnt_unit_remove_watch_step` 进行添加和删除。不能同时添加多个观察步进，否则将返回错误 :c:macro:`ESP_ERR_INVALID_STATE`。

    建议通过 :cpp:func:`pcnt_unit_remove_watch_step` 删除未使用的观察步进来回收资源。

    .. note::

        当观察步进和观察点同时被触发时，回调函数只会被调用一次。

        当计数达到上/下限值时，步进增量会被重置为 0，请勿过分依赖于精确的步进间隔。

    .. code:: c

        // add positive direction step notify with 100 step intervals
        ESP_ERROR_CHECK(pcnt_unit_add_watch_step(pcnt_unit, 100));

    .. _pcnt-register-event-callbacks:

.. only:: not SOC_PCNT_SUPPORT_STEP_NOTIFY

    .. _pcnt-register-event-callbacks:



注册事件回调函数
^^^^^^^^^^^^^^^^^^^^

当 PCNT 单元的数值达到任一使能的观察点的数值时，会触发相应的事件并通过中断通知 CPU。如果要在事件触发时执行相关函数，可通过调用 :cpp:func:`pcnt_unit_register_event_callbacks` 将函数挂载到中断服务程序 (ISR) 上。:cpp:type:`pcnt_event_callbacks_t` 列出了所有支持的事件回调函数：

-  :cpp:member:`pcnt_event_callbacks_t::on_reach` 用于为观察点事件设置回调函数。由于该回调函数是在 ISR 的上下文中被调用的，必须确保该函数不会阻塞调用的任务，（例如，可确保只有以 ``ISR`` 为后缀的 FreeRTOS API 才能在函数中调用）。:cpp:type:`pcnt_watch_cb_t` 中声明了该回调函数的原型。

可通过 ``user_ctx`` 将函数上下文保存到 :cpp:func:`pcnt_unit_register_event_callbacks` 中，这些数据会直接传递给回调函数。

驱动程序会将特定事件的数据写入回调函数中，例如，观察点事件或观察步进事件数据被声明为 :cpp:type:`pcnt_watch_event_data_t`：

-  :cpp:member:`pcnt_watch_event_data_t::watch_point_value` 用于保存触发事件时计数器的数值。
-  :cpp:member:`pcnt_watch_event_data_t::zero_cross_mode` 用于保存上一次 PCNT 单元的过零模式，:cpp:type:`pcnt_unit_zero_cross_mode_t` 中列出了所有可能的过零模式。通常，不同的过零模式意味着不同的 **计数方向** 和 **计数步长**。

注册回调函数会导致中断服务延迟安装，因此回调函数只能在 PCNT 单元被 :cpp:func:`pcnt_unit_enable` 使能之前调用。否则，回调函数会返回错误 :c:macro:`ESP_ERR_INVALID_STATE`。

.. code:: c

    static bool example_pcnt_on_reach(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx)
    {
        BaseType_t high_task_wakeup;
        QueueHandle_t queue = (QueueHandle_t)user_ctx;
        // send watch point to queue, from this interrupt callback
        xQueueSendFromISR(queue, &(edata->watch_point_value), &high_task_wakeup);
        // return whether a high priority task has been waken up by this function
        return (high_task_wakeup == pdTRUE);
    }

    pcnt_event_callbacks_t cbs = {
        .on_reach = example_pcnt_on_reach,
    };
    QueueHandle_t queue = xQueueCreate(10, sizeof(int));
    ESP_ERROR_CHECK(pcnt_unit_register_event_callbacks(pcnt_unit, &cbs, queue));

.. _pcnt-set-glitch-filter:

设置毛刺滤波器
^^^^^^^^^^^^^^^^^

PCNT 单元的滤波器可滤除信号中的短时毛刺，:cpp:type:`pcnt_glitch_filter_config_t` 中列出了毛刺滤波器的配置参数：

-  :cpp:member:`pcnt_glitch_filter_config_t::max_glitch_ns` 设置了最大的毛刺宽度，单位为纳秒。如果一个信号脉冲的宽度小于该数值，则该信号会被认定为噪声而不会触发计数器操作。

可通过调用 :cpp:func:`pcnt_unit_set_glitch_filter` 来使能毛刺滤波器，并对上述参数进行配置。之后，还可通过调用 :cpp:func:`pcnt_unit_set_glitch_filter` 来关闭毛刺滤波器，并将上述参数设置为 `NULL`。

调用该函数时，PCNT 单元应处于初始状态。否则，函数将返回错误 :c:macro:`ESP_ERR_INVALID_STATE`。

.. note::

    毛刺滤波器的时钟信息来自 APB。为确保 PCNT 单元不会滤除脉冲信号，最大毛刺宽度应大于一个 APB_CLK 周期（如果 APB 的频率为 80 MHz，则最大毛刺宽度为 12.5 ns）。使能动态频率缩放 (DFS) 后，APB 的频率会发生变化，从而最大毛刺宽度也会发生变化，这会导致计数器无法正常工作。因此，第一次使能毛刺滤波器时，驱动会为 PCNT 单元安装 PM 锁。关于 PCNT 驱动的电源管理的更多信息，请参考 :ref:`pcnt-power-management`。

.. code:: c

    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 1000,
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

.. only:: SOC_PCNT_SUPPORT_CLEAR_SIGNAL

    .. _pcnt-set-clear-signal:

    使用外部清零信号
    ^^^^^^^^^^^^^^^^

    PCNT 单元的可以接收来自 GPIO 的清零信号，:cpp:type:`pcnt_clear_signal_config_t` 中列出了清零信号的配置参数：

        -  :cpp:member:`pcnt_clear_signal_config_t::clear_signal_gpio_num` 用于指定 **清零** 信号对应的 GPIO 编号。默认有效电平为高，使能下拉输入。
        -  :cpp:member:`pcnt_clear_signal_config_t::invert_clear_signal` 用于确定信号在输入 PCNT 之前是否需要被翻转，信号翻转由 GPIO 矩阵 (不是 PCNT 单元) 执行。驱动会使能上拉输入，以确保信号在未连接时保持高电平。
        -  :cpp:member:`pcnt_clear_signal_config_t::io_loop_back` 仅用于调试，它可以使能 GPIO 的输入和输出路径。这样，就可以通过 :cpp:func:`gpio_set_level` 函数来模拟外部输入的清零信号。

    该输入信号的作用与调用 :cpp:func:`pcnt_unit_clear_count` 函数相同，但它不受软件延迟的限制，更适用于需要低延迟的场合。请注意，该信号的翻转频率不能太高。

    .. code:: c

        pcnt_clear_signal_config_t clear_signal_config = {
            .clear_signal_gpio_num = PCNT_CLEAR_SIGNAL_GPIO,
        };
        ESP_ERROR_CHECK(pcnt_unit_set_clear_signal(pcnt_unit, &clear_signal_config));

    .. _pcnt-enable-disable-unit:

.. only:: not SOC_PCNT_SUPPORT_CLEAR_SIGNAL

    .. _pcnt-enable-disable-unit:

使能和禁用单元
^^^^^^^^^^^^^^^^^

在对 PCNT 单元进行 IO 控制之前，需要通过调用函数 :cpp:func:`pcnt_unit_enable` 来使能该 PCNT 单元。该函数将完成以下操作：

* 将 PCNT 单元的驱动状态从 **初始** 切换到 **使能** 。
* 如果中断服务已经在 :cpp:func:`pcnt_unit_register_event_callbacks` 延迟安装，使能中断服务。
* 如果电源管理锁已经在 :cpp:func:`pcnt_unit_set_glitch_filter` 延迟安装，获取该电源管理锁。请参考 :ref:`pcnt-power-management` 获取更多信息。

调用函数 :cpp:func:`pcnt_unit_disable` 会进行相反的操作，即将 PCNT 单元的驱动状态切换回 **初始** 状态，禁用中断服务并释放电源管理锁。

.. code::c

    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));

.. _pcnt-unit-io-control:

控制单元 IO 操作
^^^^^^^^^^^^^^^^

启用/停用及清零
^^^^^^^^^^^^^^^^^^

通过调用 :cpp:func:`pcnt_unit_start` 可启用 PCNT 单元，根据不同脉冲信号进行递增或递减计数；通过调用 :cpp:func:`pcnt_unit_stop` 可停用 PCNT 单元，当前的计数值会保留；通过调用 :cpp:func:`pcnt_unit_clear_count` 可将计数器清零。

注意 :cpp:func:`pcnt_unit_start` 和 :cpp:func:`pcnt_unit_stop` 应该在 PCNT 单元被 :cpp:func:`pcnt_unit_enable` 使能后调用，否则将返回错误 :c:macro:`ESP_ERR_INVALID_STATE`。

.. code::c

    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));

获取计数器数值
^^^^^^^^^^^^^^^^^^^

调用 :cpp:func:`pcnt_unit_get_count` 可随时获取当前计数器的数值。返回的计数值是一个 **带符号** 的整型数，其符号反映了计数的方向。

 .. code:: c

    int pulse_count = 0;
    ESP_ERROR_CHECK(pcnt_unit_get_count(pcnt_unit, &pulse_count));

.. _pcnt-compensate-overflow-loss:

计数溢出补偿
~~~~~~~~~~~~

PCNT 内部的硬件计数器会在计数达到高/低门限的时候自动清零。如果你想补偿该计数值的溢出损失，以期进一步拓宽计数器的实际位宽，你可以：

.. list::

    1. 在安装 PCNT 计数单元的时候使能 :cpp:member:`pcnt_unit_config_t::accum_count` 选项。
    :SOC_PCNT_SUPPORT_STEP_NOTIFY: 2. 将高/低计数门限设置为 :ref:`pcnt-watch-points` 或添加观察步进 :ref:`pcnt-step-notify`
    :not SOC_PCNT_SUPPORT_STEP_NOTIFY: 2. 将高/低计数门限设置为 :ref:`pcnt-watch-points`。
    3. 现在，:cpp:func:`pcnt_unit_get_count` 函数返回的计数值就会包含硬件计数器当前的计数值，累加上计数器溢出造成的损失。

.. note::

    :cpp:func:`pcnt_unit_clear_count` 会复位该软件累加器。

.. _pcnt-power-management:

电源管理
^^^^^^^^^^

使能电源管理（即 :ref:`CONFIG_PM_ENABLE` 开启）后，在进入 Light-sleep 模式之前，系统会调整 APB 的频率。这会改变 PCNT 毛刺滤波器的参数，从而可能导致有效信号被滤除。

驱动通过获取 :cpp:enumerator:`ESP_PM_APB_FREQ_MAX` 类型的电源管理锁来防止系统修改 APB 频率。每当通过 :cpp:func:`pcnt_unit_set_glitch_filter` 使能毛刺滤波器时，驱动可以保证系统在 :cpp:func:`pcnt_unit_enable` 使能 PCNT 单元后获取电源管理锁。而系统调用 :cpp:func:`pcnt_unit_disable` 之后，驱动会释放电源管理锁。

.. _pcnt-iram-safe:

支持 IRAM 安全中断
^^^^^^^^^^^^^^^^^^^^^^

当缓存由于写入/擦除 flash 等原因被禁用时，PCNT 中断会默认被延迟。这会导致报警中断无法及时执行，从而无法满足实时性应用的要求。

Konfig 选项 :ref:`CONFIG_PCNT_ISR_IRAM_SAFE` 可以实现以下功能：

1. 即使缓存被禁用也可以使能中断服务
2. 将 ISR 使用的所有函数都放入 IRAM 中 [2]_
3. 将驱动对象放入 DRAM（防止驱动对象被意外映射到 PSRAM 中）

这样，在缓存被禁用时，中断也可运行，但是这也会增加 IRAM 的消耗。

另外一个 Konfig 选项 :ref:`CONFIG_PCNT_CTRL_FUNC_IN_IRAM` 也可以把常用的 IO 控制函数放在 IRAM 中。这样，当缓存禁用时，这些函数仍然可以执行。这些 IO 控制函数如下所示：

- :cpp:func:`pcnt_unit_start`
- :cpp:func:`pcnt_unit_stop`
- :cpp:func:`pcnt_unit_clear_count`
- :cpp:func:`pcnt_unit_get_count`

.. _pcnt-thread-safe:

支持线程安全
^^^^^^^^^^^^^

驱动保证工厂函数 :cpp:func:`pcnt_new_unit` 与 :cpp:func:`pcnt_new_channel` 是线程安全的，因此可以从 RTOS 任务中调用这些函数，而无需使用额外的电源管理锁。

以下函数可以在 ISR 上下文中运行，驱动可以防止这些函数在任务和 ISR 中同时被调用。

- :cpp:func:`pcnt_unit_start`
- :cpp:func:`pcnt_unit_stop`
- :cpp:func:`pcnt_unit_clear_count`
- :cpp:func:`pcnt_unit_get_count`

其他以 :cpp:type:`pcnt_unit_handle_t` 和 :cpp:type:`pcnt_channel_handle_t` 作为第一个参数的函数被视为线程不安全函数，在多任务场景下应避免调用这些函数。

.. _pcnt-kconfig-options:

支持的 Kconfig 选项
^^^^^^^^^^^^^^^^^^^^^^

- :ref:`CONFIG_PCNT_CTRL_FUNC_IN_IRAM` 用于确定 PCNT 控制函数的位置（放在 IRAM 还是 flash 中），请参考 :ref:`pcnt-iram-safe` 获取更多信息。
- :ref:`CONFIG_PCNT_ISR_IRAM_SAFE` 用于控制当缓存禁用时，默认的 ISR 句柄是否可以工作，请参考 :ref:`pcnt-iram-safe` 获取更多信息。
- :ref:`CONFIG_PCNT_ENABLE_DEBUG_LOG` 用于使能调试日志输出，而这会增大固件二进制文件。

应用示例
------------

* :example:`peripherals/pcnt/rotary_encoder` 演示了如何使用 PCNT 外设来解码由常见的旋转编码器 EC11 生成的差分信号，以及如何配置旋转编码器将系统从 light-sleep 状态唤醒。


API 参考
--------------

.. include-build-file:: inc/pulse_cnt.inc
.. include-build-file:: inc/pcnt_types.inc

.. [1]
   在不同的 ESP 芯片系列中，PCNT 单元和通道的数量可能会有差异，具体信息请参考 [`TRM <{IDF_TARGET_TRM_CN_URL}#pcnt>`__]。驱动程序对通道申请数量不做限制，但当硬件资源用尽时，驱动程序将返回错误。因此分配资源时，应注意检查返回值，如 :cpp:func:`pcnt_new_unit`。

.. [2]
   :cpp:member:`pcnt_event_callbacks_t::on_reach` 回调函数和其调用的函数也应该放在 IRAM 中。
