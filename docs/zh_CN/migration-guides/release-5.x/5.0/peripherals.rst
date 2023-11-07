外设
============

:link_to_translation:`en:[English]`

外设时钟门控
------------------------------

与更新之前相同，外设的时钟仍由驱动处理，用户无需对外设模块的时钟门控进行设置。

但是，如果用户想基于组件 ``hal`` 和 ``soc`` 开发自己的驱动，请注意时钟门控的头文件引用路径已由 ``driver/periph_ctrl.h`` 更新为 ``esp_private/periph_ctrl.h``。

RTC 子系统控制
----------------------------------

RTC 控制 API 已经从 ``driver/rtc_cntl.h`` 移动到了 ``esp_private/rtc_ctrl.h``。

ADC
-----------------

ADC 单次模式及连续模式驱动
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ADC 单次模式的驱动已更新。

- 新的驱动位于组件 ``esp_adc`` 中，头文件引用路径为 ``esp_adc/adc_oneshot.h``。
- 旧版驱动仍然可用，其头文件引用路径为 ``driver/adc.h``。

对于 ADC 连续模式驱动，其位置已由组件 ``driver`` 更新为 ``esp_adc``。

- 头文件引用路径由 ``driver/adc.h`` 更新为 ``esp_adc/adc_continuous.h``。

但是，引用两种模式的旧版路径 ``driver/adc.h`` 会默认触发如下编译警告，可通过配置 Kconfig 选项 :ref:`CONFIG_ADC_SUPPRESS_DEPRECATE_WARN` 关闭该警告。

.. code-block:: text

    legacy adc driver is deprecated, please migrate to use esp_adc/adc_oneshot.h and esp_adc/adc_continuous.h for oneshot mode and continuous mode drivers respectively

ADC 校准驱动
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ADC 校准驱动已更新。

- 新的驱动位于组件 ``esp_adc`` 中，头文件引用路径为 ``esp_adc/adc_cali.h`` 和 ``esp_adc/adc_cali_scheme.h``。

旧版驱动仍然可用，其头文件引用路径为 ``esp_adc_cal.h``。如果用户要使用旧版路径，需要将组件 ``esp_adc`` 添加到文件 CMakeLists.txt 的组件需求表中。

默认情况下，引用路径 ``esp_adc_cal.h`` 会默认触发如下编译警告，可通过配置 Kconfig 选项 :ref:`CONFIG_ADC_CALI_SUPPRESS_DEPRECATE_WARN` 关闭该警告。

.. code-block:: text

    legacy adc calibration driver is deprecated, please migrate to use esp_adc/adc_cali.h and esp_adc/adc_cali_scheme.h

API 更新
^^^^^^^^^^^^^^^^^^^^^

- ADC 电源管理 API ``adc_power_acquire`` 和 ``adc_power_release`` 已被移至 ``esp_private/adc_share_hw_ctrl.h``，用于内部功能。

    - 更新前，由于硬件勘误表的工作原理，这两个 API 可以被用户调用。
    - 更新后，ADC 电源管理完全由驱动在内部实现。
    - 如果用户仍需调用这个 API，可以通过引用路径 ``esp_private/adc_share_hw_ctrl.h`` 来调用它。

- 更新后， ``driver/adc2_wifi_private.h`` 已被移至 ``esp_private/adc_share_hw_ctrl.h``。
- ``adc_unit_t`` 中的枚举 ``ADC_UNIT_BOTH``， ``ADC_UNIT_ALTER`` 及 ``ADC_UNIT_MAX`` 已被删除。
- 由于只有部分芯片支持下列枚举的某些取值，因此将下列枚举删除。如果用户使用了不支持的取值，会造成驱动运行错误。

    - 枚举 ``ADC_CHANNEL_MAX``
    - 枚举 ``ADC_ATTEN_MAX``
    - 枚举 ``ADC_CONV_UNIT_MAX``

- ESP32 中的 API ``hall_sensor_read`` 已被删除，因此 ESP32 不再支持霍尔传感器。
- API ``adc_set_i2s_data_source`` 和 ``adc_i2s_mode_init`` 已被弃用，相关的枚举 ``adc_i2s_source_t`` 也已被弃用，请使用 ``esp_adc/adc_continuous.h`` 进行迁移。
- API ``adc_digi_filter_reset`` ， ``adc_digi_filter_set_config`` ， ``adc_digi_filter_get_config`` 和 ``adc_digi_filter_enable`` 已被移除. 这些接口的行为不被保证。 枚举 ``adc_digi_filter_idx_t`` ， ``adc_digi_filter_mode_t`` 和结构体 ``adc_digi_iir_filter_t`` 已被移除。
- API ``esp_adc_cal_characterize`` 已被弃用， 请迁移到 ``adc_cali_create_scheme_curve_fitting`` 或 ``adc_cali_create_scheme_line_fitting``.
- API ``esp_adc_cal_raw_to_voltage`` 已被弃用， 请迁移到 ``adc_cali_raw_to_voltage``.
- API ``esp_adc_cal_get_voltage`` 已被弃用， 请迁移到 ``adc_oneshot_get_calibrated_result``.

GPIO
----------

- 之前的 Kconfig 选项 `RTCIO_SUPPORT_RTC_GPIO_DESC` 已被删除，因此数组 ``rtc_gpio_desc`` 已不可用，请使用替代数组 ``rtc_io_desc``。

- 更新后，用户回调函数无法再通过读取 GPIO 中断的状态寄存器来获取用于触发中断的 GPIO 管脚的编号。但是，用户可以通过使用回调函数变量来确定该管脚编号。

    - 更新前，GPIO 中断发生时，GPIO 中断状态寄存器调用用户回调函数之后，会被清空。因此，用户可以在回调函数中读取 GPIO 中断状态寄存器，以便确定触发中断的 GPIO 管脚。
    - 但是，在调用回调函数后清空中断状态寄存器可能会导致边沿触发的中断丢失。例如，在调用用户回调函数时，如果某个边沿触发的中断 (re) 被触发，该中断会被清除，并且其注册的用户回调函数还未被处理。
    - 更新后，GPIO 的中断状态寄存器在调用用户回调函数 **之前** 被清空。因此，用户无法读取 GPIO 中断状态寄存器来确定哪个管脚触发了中断。但是，用户可以通过回调函数变量来传递被触发的管脚编号。

.. only:: SOC_SDM_SUPPORTED

    Sigma-Delta 调制器
    ---------------------------------

    Sigma-Delta 调制器的驱动现已更新为 :doc:`SDM <../../../api-reference/peripherals/sdm>`。

    - 新驱动中实现了工厂模式，SDM 通道都位于内部通道池中，因此用户无需手动将 SDM 通道配置到 GPIO 管脚。
    - SDM 通道会被自动分配。

    尽管我们推荐用户使用新的驱动 API，旧版驱动仍然可用，位于头文件引用路径 ``driver/sigmadelta.h`` 中。但是，引用 ``driver/sigmadelta.h`` 会默认触发如下编译警告，可通过配置 Kconfig 选项 :ref:`CONFIG_SDM_SUPPRESS_DEPRECATE_WARN` 关闭该警告。

    .. code-block:: text

        The legacy sigma-delta driver is deprecated, please use driver/sdm.h

    概念与使用方法上的主要更新如下所示：

    主要概念更新
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - SDM 通道名称已由 ``sigmadelta_channel_t`` 更新为 :cpp:type:`sdm_channel_handle_t`，后者为一个不透明指针。
    - SDM 通道配置原来存放于 ``sigmadelta_config_t``，现存放于 :cpp:type:`sdm_config_t`。
    - 旧版驱动中，用户无需为 SDM 通道设置时钟源。但是在新驱动中，用户需要在 :cpp:member:`sdm_config_t::clk_src` 为 SDM 通道设置合适的时钟源，:cpp:type:`soc_periph_sdm_clk_src_t` 中列出了可用的时钟源。
    - 旧版驱动中，用户需要为通道设置 ``prescale``，该参数会影响调制器输出脉冲的频率。在新的驱动中，用户需要使用 :cpp:member:`sdm_config_t::sample_rate_hz` 实现该功能。
    - 旧版驱动中，用户通过设置 ``duty`` 来改变输出的模拟量，现在换成了一个更贴切的名字 ``density``

    主要使用方法更新
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - 更新前，通道配置由通道分配在 :cpp:func:`sdm_new_channel` 完成。在新驱动中，只有 ``density`` 可在运行时由 :cpp:func:`sdm_channel_set_pulse_density` 更新。其他参数如 ``gpio number``、 ``prescale`` 只能在通道分配时进行设置。
    - 在进行下一步通道操作前，用户应通过调用 :cpp:func:`sdm_channel_enable` 提前 **使能** 该通道。该函数有助于管理一些系统级服务，如 **电源管理**。

定时器组驱动
-----------------------------------------

为统一和简化通用定时器的使用，定时器组驱动已更新为 :doc:`GPTimer <../../../api-reference/peripherals/gptimer>`。

尽管我们推荐使用新的驱动 API， 旧版驱动仍然可用，其头文件引用路径为 ``driver/timer.h``。但是，引用 ``driver/timer.h`` 会默认触发如下编译警告，可通过配置 Kconfig 选项 :ref:`CONFIG_GPTIMER_SUPPRESS_DEPRECATE_WARN` 关闭该警告。

.. code-block:: text

    legacy timer group driver is deprecated, please migrate to driver/gptimer.h

概念和使用方法上的主要更新如下所示：

主要概念更新
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

-  用于识别定时器的 ``timer_group_t`` 和 ``timer_idx_t`` 已被删除。在新驱动中，定时器用参数 :cpp:type:`gptimer_handle_t` 表示。
-  更新后，定时器的时钟源由 :cpp:type:`gptimer_clock_source_t` 定义，之前的时钟源参数 ``timer_src_clk_t`` 不再使用。
-  更新后，定时器计数方向由 :cpp:type:`gptimer_count_direction_t` 定义，之前的计数方向参数 ``timer_count_dir_t`` 不再使用。
-  更新后，仅支持电平触发的中断， ``timer_intr_t`` 和 ``timer_intr_mode_t`` 不再使用。
-  更新后，通过设置标志位 :cpp:member:`gptimer_alarm_config_t::auto_reload_on_alarm`， 可以使能自动加载。 ``timer_autoreload_t`` 不再使用。

主要使用方法更新
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

-  更新后，通过从 :cpp:func:`gptimer_new_timer` 创建定时器示例可以初始化定时器。用户可以在 :cpp:type:`gptimer_config_t` 进行一些基本设置，如时钟源，分辨率和计数方向。请注意，无需在驱动安装阶段进行报警事件的特殊设置。
-  更新后，报警事件在 :cpp:func:`gptimer_set_alarm_action` 中进行设置，参数在 :cpp:type:`gptimer_alarm_config_t` 中进行设置。
-  更新后，通过 :cpp:func:`gptimer_get_raw_count` 设置计数数值，通过 :cpp:func:`gptimer_set_raw_count` 获取计数数值。驱动不会自动将原始数据同步到 UTC 时间戳。由于定时器的分辨率已知，用户可以自行转换数据。
-  更新后，如果 :cpp:member:`gptimer_event_callbacks_t::on_alarm` 被设置为有效的回调函数，驱动程序也会安装中断服务。在回调函数中，用户无需配置底层寄存器，如用于“清除中断状态”，“重新使能事件”的寄存器等。因此， ``timer_group_get_intr_status_in_isr`` 与 ``timer_group_get_auto_reload_in_isr`` 这些函数不再使用。
-  更新后，当报警事件发生时，为更新报警配置，用户可以在中断回调中调用 :cpp:func:`gptimer_set_alarm_action`，这样报警事件会被重新使能。
-  更新后，如果用户将 :cpp:member:`gptimer_alarm_config_t::auto_reload_on_alarm` 设置为 true，报警事件将会一直被驱动程序使能。

UART
------------

.. list-table::
    :width: 700 px
    :header-rows: 1

    * - 删除/弃用项目
      - 替代
      - 备注
    * - ``uart_isr_register()``
      - 无
      - 更新后，UART 中断由驱动处理。
    * - ``uart_isr_free()``
      - 无
      - 更新后，UART 中断由驱动处理。
    * - :cpp:type:`uart_config_t` 中的 ``use_ref_tick``
      - :cpp:member:`uart_config_t::source_clk`
      - 选择时钟源。
    * - ``uart_enable_pattern_det_intr()``
      - :cpp:func:`uart_enable_pattern_det_baud_intr`
      - 使能模式检测中断。

I2C
-------

.. list-table::
    :width: 700 px
    :header-rows: 1

    * - 删除/弃用项目
      - 替代
      - 备注
    * - ``i2c_isr_register()``
      - 无
      - 更新后，I2C 中断由驱动处理。
    * - ``i2c_isr_register()``
      - 无
      - 更新后，I2C 中断由驱动处理。
    * - ``i2c_opmode_t``
      - 无
      - 更新后，该项不再在 esp-idf 中使用。

SPI
---

.. list-table::
    :width: 700 px
    :header-rows: 1

    * - 删除/弃用项目
      - 替代
      - 备注
    * - ``spi_cal_clock()``
      - :cpp:func:`spi_get_actual_clock`
      - 获取 SPI 真实的工作频率。

- 内部头文件 ``spi_common_internal.h`` 已被移至 ``esp_private/spi_common_internal.h``。

.. only:: SOC_SDMMC_HOST_SUPPORTED

    SDMMC
    -----

    .. list-table::
        :width: 700 px
        :header-rows: 1

        * - 删除/弃用项目
          - 替代
          - 备注
        * - ``sdmmc_host_pullup_en()``
          - 在 :cpp:member:`sdmmc_slot_config_t::flags` 设置标志位 ``SDMMC_SLOT_FLAG_INTERNAL_PULLUP``
          - 使能内部上拉。

LEDC
-----

.. list-table::
    :width: 700 px
    :header-rows: 1

    * - 删除/弃用项目
      - 替代
      - 备注
    * - :cpp:type:`ledc_timer_config_t` 中的 ``bit_num``
      - :cpp:member:`ledc_timer_config_t::duty_resolution`
      - 设置占空比分辨率。

.. only:: SOC_PCNT_SUPPORTED

    脉冲计数器 (PCNT) 驱动
    ----------------------------------

    为统一和简化 PCNT 外设，PCNT 驱动已更新，详见 :doc:`PCNT <../../../api-reference/peripherals/pcnt>`。

    尽管我们推荐使用新的驱动 API，旧版驱动仍然可用，保留在头文件引用路径 ``driver/pcnt.h`` 中。但是，引用路径 ``driver/pcnt.h`` 会默认触发如下编译警告，可通过配置 Kconfig 选项 :ref:`CONFIG_PCNT_SUPPRESS_DEPRECATE_WARN` 来关闭该警告。

    .. code-block:: text

        legacy pcnt driver is deprecated, please migrate to use driver/pulse_cnt.h

    主要概念和使用方法上的更新如下所示：

    主要概念更新
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - 更新后， ``pcnt_port_t``、 ``pcnt_unit_t`` 和 ``pcnt_channel_t`` 这些用于识别 PCNT 单元和通道的参数已被删除。在新的驱动中，PCNT 单元由参数 :cpp:type:`pcnt_unit_handle_t` 表示，PCNT 通道由参数 :cpp:type:`pcnt_channel_handle_t` 表示，这两个参数都是不透明指针。
    - 更新后，不再使用 ``pcnt_evt_type_t``，它们由统一的 **观察点事件** 表示。在事件回调函数 :cpp:type:`pcnt_watch_cb_t` 中，通过 :cpp:type:`pcnt_watch_event_data_t` 可以分辨不同观察点。
    - ``pcnt_count_mode_t`` 更新为:cpp:type:`pcnt_channel_edge_action_t`， ``pcnt_ctrl_mode_t`` 更新为 :cpp:type:`pcnt_channel_level_action_t`。

    主要使用方法更新
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - 更新前，PCNT 的单元配置和通道配置都通过函数 ``pcnt_unit_config`` 实现。更新后，PCNT 的单元配置通过工厂 API :cpp:func:`pcnt_new_unit` 完成，通道配置通过工厂 API :cpp:func:`pcnt_new_channel` 完成。

        - 只需配置计数范围即可初始化一个 PCNT 单元。更新后，GPIO 管脚分配通过 :cpp:func:`pcnt_new_channel` 完成。
        - 高/低电平控制模式和上升沿/下降沿计数模式分别通过函数 :cpp:func:`pcnt_channel_set_edge_action` 和 :cpp:func:`pcnt_channel_set_level_action` 进行设置。

    - ``pcnt_get_counter_value`` 更新为 :cpp:func:`pcnt_unit_get_count`。
    - ``pcnt_counter_pause`` 更新为 :cpp:func:`pcnt_unit_stop`。
    - ``pcnt_counter_resume`` 更新为 :cpp:func:`pcnt_unit_start`。
    - ``pcnt_counter_clear`` 更新为 :cpp:func:`pcnt_unit_clear_count`。
    - 更新后， ``pcnt_intr_enable`` 与 ``pcnt_intr_disable`` 已被删除。新的驱动中，通过注册时间回调函数 :cpp:func:`pcnt_unit_register_event_callbacks` 来使能中断。
    - 更新后， ``pcnt_event_enable`` 与 ``pcnt_event_disable`` 已被删除。新的驱动中，可通过 :cpp:func:`pcnt_unit_add_watch_point` 和 :cpp:func:`pcnt_unit_remove_watch_point` 来增加/删除观察点，以使能/停用 PCNT 事件。
    - 更新后， ``pcnt_set_event_value`` 已被删除。新的驱动中，通过 :cpp:func:`pcnt_unit_add_watch_point` 增加观察点时，也同时设置了事件的数值。
    - 更新后， ``pcnt_get_event_value`` 与 ``pcnt_get_event_status`` 已被删除。在新的驱动中，这些信息存储在 :cpp:type:`pcnt_watch_event_data_t` 的回调函数 :cpp:type:`pcnt_watch_cb_t` 中。
    - 更新后， ``pcnt_isr_register`` 与 ``pcnt_isr_unregister`` 已被删除，不允许注册 ISR 句柄。 用户可以通过调用:cpp:func:`pcnt_unit_register_event_callbacks` 来注册事件回调函数。
    - 更新后， ``pcnt_set_pin`` 已被删除，新的驱动不再允许在运行时切换 GPIO 管脚。如果用户想切换为其他 GPIO 管脚，可通过:cpp:func:`pcnt_del_channel` 删除当前的 PCNT 通道，然后通过:cpp:func:`pcnt_new_channel` 安装新的 GPIO 管脚。
    - ``pcnt_filter_enable``， ``pcnt_filter_disable`` 与 ``pcnt_set_filter_value`` 更新为 :cpp:func:`pcnt_unit_set_glitch_filter`。同时， ``pcnt_get_filter_value`` 已被删除。
    - ``pcnt_set_mode`` 更新为 :cpp:func:`pcnt_channel_set_edge_action` 与 :cpp:func:`pcnt_channel_set_level_action`。
    - ``pcnt_isr_service_install``， ``pcnt_isr_service_uninstall``， ``pcnt_isr_handler_add`` 与 ``pcnt_isr_handler_remove`` 更新为 :cpp:func:`pcnt_unit_register_event_callbacks`。默认的 ISR 句柄已安装在新的驱动中。

.. only:: SOC_TEMP_SENSOR_SUPPORTED

    温度传感器驱动
    ------------------------------------------------------------

    温度传感器的驱动已更新，推荐用户使用新驱动。旧版驱动仍然可用，但是无法与新驱动同时使用。

    新驱动的头文件引用路径为 ``driver/temperature_sensor.h``。旧版驱动仍然可用，保留在引用路径 ``driver/temp_sensor.h`` 中。但是，引用路径 ``driver/temp_sensor.h`` 会默认触发如下编译警告，可通过设置 Kconfig 选项 :ref:`CONFIG_TEMP_SENSOR_SUPPRESS_DEPRECATE_WARN` 来关闭该警告。

    .. code-block:: text

        legacy temperature sensor driver is deprecated, please migrate to driver/temperature_sensor.h

    配置内容已更新。更新前，用户需要设置 ``clk_div`` 与 ``dac_offset``。更新后，用户仅需设置 ``tsens_range``。

    温度传感器的使用过程也已更新。更新前，用户可通过 ``config->start->read_celsius`` 获取数据。更新后，用户需要通过 ``temperature_sensor_install`` 先安装温度传感器的驱动，测量完成后需卸载驱动，详情请参考 :doc:`Temperature Sensor <../../../api-reference/peripherals/temp_sensor>`。

.. only:: SOC_RMT_SUPPORTED

    RMT 驱动
    ----------------------

    为统一和扩展 RMT 外设的使用，RMT 驱动已更新，详见 :doc:`RMT transceiver <../../../api-reference/peripherals/rmt>`。

    尽管我们建议使用新的驱动 API，旧版驱动仍然可用，保留在头文件引用路径 ``driver/rmt.h``中。但是，引用路径 ``driver/rmt.h`` 会默认触发如下编译警告，可通过配置 Kconfig 选项 :ref:`CONFIG_RMT_SUPPRESS_DEPRECATE_WARN` 来关闭该警告。

    .. code-block:: text

        The legacy RMT driver is deprecated, please use driver/rmt_tx.h and/or driver/rmt_rx.h

    主要概念和使用方法更新如下所示：

    主要概念更新
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    - 更新后，用于识别硬件通道的 ``rmt_channel_t`` 已删除。在新的驱动中，RMT 通道用参数 :cpp:type:`rmt_channel_handle_t` 表示，该通道由驱动程序动态分配，而不是由用户指定。
    - ``rmt_item32_t`` 更新为 :cpp:type:`rmt_symbol_word_t`，以避免在结构体中出现嵌套的共用体。
    - 更新后， ``rmt_mem_t`` 已被删除，因为我们不允许用户直接访问 RMT 内存块（即 RMTMEM）。直接访问 RMTMEM 没有意义，反而会引发错误，特别是当 RMT 通道与 DMA 通道相连时。
    - 更新后，由于 ``rmt_mem_owner_t`` 由驱动控制，而不是用户，因此 ``rmt_mem_owner_t`` 已被删除。
    - ``rmt_source_clk_t`` 更新为 :cpp:type:`rmt_clock_source_t`，后者不支持二进制兼容。
    - 更新后， ``rmt_data_mode_t`` 已被删除，RMT 内存访问模式配置为始终使用 Non-FIFO 和 DMA 模式。
    - 更新后，由于驱动有独立的发送和接收通道安装函数，因此 ``rmt_mode_t`` 已被删除。
    - 更新后， ``rmt_idle_level_t`` 已被删除，在 :cpp:member:`rmt_transmit_config_t::eot_level` 中可为发送通道设置空闲状态电平。
    - 更新后， ``rmt_carrier_level_t`` 已被删除，可在 :cpp:member:`rmt_carrier_config_t::polarity_active_low` 设置载流子极性。
    - 更新后， ``rmt_channel_status_t`` 与 ``rmt_channel_status_result_t`` 已被删除，不再使用。
    - 通过 RMT 通道发送并不需要用户提供 RMT 符号，但是用户需要提供一个 RMT 编码器用来告诉驱动如何将用户数据转换成 RMT 符号。

    主要使用方法更新
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - 更新后，分别通过 :cpp:func:`rmt_new_tx_channel` 和 :cpp:func:`rmt_new_rx_channel` 安装发送通道和接收通道。
    - 更新后， ``rmt_set_clk_div`` 和 ``rmt_get_clk_div`` 已被删除。通道时钟配置只能在通道安装时完成。
    - 更新后， ``rmt_set_rx_idle_thresh`` 和 ``rmt_get_rx_idle_thresh`` 已被删除。新驱动中，接收通道的空闲状态阈值定义为 :cpp:member:`rmt_receive_config_t::signal_range_max_ns`。
    - 更新后， ``rmt_set_mem_block_num`` 和 ``rmt_get_mem_block_num`` 已被删除。新驱动中，内存块的数量由 :cpp:member:`rmt_tx_channel_config_t::mem_block_symbols` 与 :cpp:member:`rmt_rx_channel_config_t::mem_block_symbols` 决定。
    - 更新后， ``rmt_set_tx_carrier`` 已被删除。新驱动使用 :cpp:func:`rmt_apply_carrier` 来设置载波动作。
    - 更新后， ``rmt_set_mem_pd`` 和 ``rmt_get_mem_pd`` 已被删除，驱动程序自动调整内存的功率。
    - 更新后， ``rmt_memory_rw_rst``， ``rmt_tx_memory_reset`` 和 ``rmt_rx_memory_reset`` 已被删除，驱动程序自动进行内存重置。
    - 更新后， ``rmt_tx_start`` 和 ``rmt_rx_start`` 被合并为函数 :cpp:func:`rmt_enable`，该函数同时适用于发射通道和接收通道。
    - 更新后， ``rmt_tx_stop`` 和 ``rmt_rx_stop`` 被合并为函数 :cpp:func:`rmt_disable`，该函数同时适用于发射通道和接收通道。
    - 更新后， ``rmt_set_memory_owner`` 和 ``rmt_get_memory_owner`` 已被删除，驱动程序自动添加 RMT 内存保护。
    - 更新后， ``rmt_set_tx_loop_mode`` 和 ``rmt_get_tx_loop_mode`` 已被删除。新驱动中，在 :cpp:member:`rmt_transmit_config_t::loop_count` 中设置循环模式。
    - 更新后， ``rmt_set_source_clk`` 和 ``rmt_get_source_clk`` 已被删除。仅能在通道安装时通过 :cpp:member:`rmt_tx_channel_config_t::clk_src` 和 :cpp:member:`rmt_rx_channel_config_t::clk_src` 设置时钟源。
    - 更新后， ``rmt_set_rx_filter`` 已被删除。新驱动中，过滤阈值定义为 :cpp:member:`rmt_receive_config_t::signal_range_min_ns`。
    - 更新后， ``rmt_set_idle_level`` 和 ``rmt_get_idle_level`` 已被删除，可在 :cpp:member:`rmt_transmit_config_t::eot_level` 中设置发射通道的空闲状态电平。
    - 更新后， ``rmt_set_rx_intr_en``， ``rmt_set_err_intr_en``， ``rmt_set_tx_intr_en``， ``rmt_set_tx_thr_intr_en`` 和 ``rmt_set_rx_thr_intr_en`` 已被删除。新驱动不允许用户在用户端开启/关闭中断，而是提供了回调函数。
    - 更新后， ``rmt_set_gpio`` 和 ``rmt_set_pin`` 已被删除。新驱动不支持运行时动态切换 GPIO 管脚。
    - 更新后， ``rmt_config`` 已被删除。新驱动中，基础配置在通道安装阶段完成。
    - 更新后， ``rmt_isr_register`` 和 ``rmt_isr_deregister`` 已被删除，驱动程序负责分配中断。
    - ``rmt_driver_install`` 更新为 :cpp:func:`rmt_new_tx_channel` 与 :cpp:func:`rmt_new_rx_channel`。
    - ``rmt_driver_uninstall`` 更新为 :cpp:func:`rmt_del_channel`。
    - 更新后， ``rmt_fill_tx_items``， ``rmt_write_items`` 和 ``rmt_write_sample`` 已被删除。新驱动中，用户需要提供一个编码器用来将用户数据“翻译”为 RMT 符号。
    - 更新后，由于用户可以通过 :cpp:member:`rmt_tx_channel_config_t::resolution_hz` 配置通道的时钟分辨率， ``rmt_get_counter_clock`` 已被删除。
    - ``rmt_wait_tx_done`` 更新为 :cpp:func:`rmt_tx_wait_all_done`。
    - 更新后， ``rmt_translator_init``， ``rmt_translator_set_context`` 和 ``rmt_translator_get_context`` 已被删除。新驱动中，翻译器更新为 RMT 译码器。
    - 更新后， ``rmt_get_ringbuf_handle`` 已被删除。新驱动程序不再使用 Ringbuffer 来保存 RMT 符号。输入数据会直接保存到用户提供的缓冲区中，这些缓冲区甚至可以直接被挂载到 DMA 链接内部。
    - ``rmt_register_tx_end_callback`` 更新为 :cpp:func:`rmt_tx_register_event_callbacks`，用户可以在这个参数里面注册事件回调函数 :cpp:member:`rmt_tx_event_callbacks_t::on_trans_done`。
    - 更新后， ``rmt_set_intr_enable_mask`` 和 ``rmt_clr_intr_enable_mask`` 已被删除。由于驱动程序负责处理中断，因此用户无需进行处理。
    - ``rmt_add_channel_to_group`` 和 ``rmt_remove_channel_from_group`` 更新为 RMT 同步管理器，详见 :cpp:func:`rmt_new_sync_manager`。
    - 更新后， ``rmt_set_tx_loop_count`` 已被删除。新驱动中，循环计数在 :cpp:member:`rmt_transmit_config_t::loop_count` 进行配置。
    - 更新后， ``rmt_enable_tx_loop_autostop`` 已被删除。新驱动中，发射循环自动终止一直使能，用户无法进行配置。

LCD
-----

- LCD 面板的初始化流程也有一些更新。更新后，:cpp:func:`esp_lcd_panel_init` 不再会自动打开显示器。用户需要调用 :cpp:func:`esp_lcd_panel_disp_on_off` 来手动打开显示器。请注意，打开显示器与打开背光是不同的。更新后，打开屏幕前，用户可以烧录一个预定义的图案，这可以避免开机复位后屏幕上的随机噪音。
- 更新后， :cpp:func:`esp_lcd_panel_disp_off` 已被弃用，请使用 :cpp:func:`esp_lcd_panel_disp_on_off` 作为替代。
- 更新后， ``dc_as_cmd_phase`` 已被删除，SPI LCD 驱动不再支持 9-bit 的 SPI LCD。请使用专用的 GPIO 管脚来控制 LCD D/C 线。
- 更新后，用于注册 RGB 面板的事件回调函数已从 :cpp:type:`esp_lcd_rgb_panel_config_t` 更新为单独的 API :cpp:func:`esp_lcd_rgb_panel_register_event_callbacks`。但是，事件回调签名仍保持不变。
- 更新后， :cpp:type:`esp_lcd_rgb_panel_config_t` 中的标志位 ``relax_on_idle`` 被重命名为 :cpp:member:`esp_lcd_rgb_panel_config_t::refresh_on_demand`，后者虽表达了同样的含义，但是其命名更有意义。
- 更新后，如果创建 RGB LCD 时，标志位 ``refresh_on_demand`` 使能，驱动不会在 :cpp:func:`esp_lcd_panel_draw_bitmap` 中进行刷新，用户需要调用 :cpp:func:`esp_lcd_rgb_panel_refresh` 来刷新屏幕。
- 更新后，:cpp:type:`esp_lcd_color_space_t` 已被弃用，请使用 :cpp:type:`lcd_color_space_t` 来描述色彩空间，使用 :cpp:type:`lcd_rgb_element_order_t` 来描述 RGB 颜色的排列顺序。

.. only:: SOC_MCPWM_SUPPORTED

    MCPWM
    -----

    MCPWM 驱动已更新（详见 :doc:`MCPWM <../../../api-reference/peripherals/mcpwm>`）。同时，旧版驱动已被弃用。

    新驱动中，每个 MCPWM 子模块相互独立，用户可以自由进行资源连接。

    尽管我们推荐使用新的驱动 API，旧版驱动仍然可用，其引用路径为 ``driver/mcpwm.h``。但是，使用旧版驱动会默认触发如下编译警告，可以通过配置 Kconfig 选项 :ref:`CONFIG_MCPWM_SUPPRESS_DEPRECATE_WARN` 来关闭该警告。

    .. code-block:: text

        legacy MCPWM driver is deprecated, please migrate to the new driver (include driver/mcpwm_prelude.h)

    主要概念和使用方法上的更新如下所示：

    主要概念更新
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    更新后，MCPWM 驱动是面向对象的，大多数 MCPWM 子模块都有一个与之相关的驱动对象。驱动对象是由工厂函数创建的，如 :cpp:func:`mcpwm_new_timer`。IO 控制函数总是需要对象句柄。

    旧版驱动有一个不恰当的假设，即 MCPWM 运算器应连接到不同的 MCPWM 定时器上。但是，硬件上并没有这样的限制。新驱动中，同一个 MCPWM 定时器可以连接多个运算器，这样运算器可以获得最佳的同步性能。

    更新前，驱动将生成 PWM 波形的方法预设为所谓的 ``mcpwm_duty_type_t``，但是，列出的占空比模式远远不够。类似的，旧版驱动有一些预设的 ``mcpwm_deadtime_type_t`` 也没有包含所有的使用场景。更重要的是，用户通常会被占空比模式和死区时间模式的名称所迷惑。更新后，驱动没有这些限制，但是用户必须从头开始构建发生器的行为。

    在旧版驱动中，通过 GPIO 管脚，软件和其他定时器模块同步 MCPWM 定时器的方法并不统一。这增加了用户的学习成本，因此新驱动统一了同步 API。

    旧版驱动混淆了“故障检测器”和“故障处理器”的概念。这让用户对 API 感到非常困惑。新驱动中，故障对象只代表一个故障源，而且我们引入了一个新概念， **制动器**，来表示故障处理器。而且，新驱动支持软件故障。

    旧版驱动只为获取子模块提供了回调函数，而新驱动为 MCPWM 子模块提供多种回调函数，如停止定时器，比较匹配，故障进入，紧急停止等。

    - 更新后，不再使用 ``mcpwm_io_signals_t`` 和 ``mcpwm_pin_config_t``， GPIO 管脚配置被移至子模块的配置结构中。
    - 更新后，不再使用 ``mcpwm_timer_t`` 和 ``mcpwm_generator_t``，定时器和发生器分别用 :cpp:type:`mcpwm_timer_handle_t` 和:cpp:type:`mcpwm_gen_handle_t` 表示。
    - 更新后，不再使用 ``mcpwm_fault_signal_t`` 和 ``mcpwm_sync_signal_t``，故障和同步源分别用 :cpp:type:`mcpwm_fault_handle_t` 和:cpp:type:`mcpwm_sync_handle_t` 表示。
    - 更新后，不再使用 ``mcpwm_capture_signal_t``，获取通道用 :cpp:type:`mcpwm_cap_channel_handle_t` 表示。

    主要使用方法更新
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - ``mcpwm_gpio_init`` 和 ``mcpwm_set_pin``：GPIO 管脚配置在子模块配置中完成，例如在 :cpp:member:`mcpwm_generator_config_t::gen_gpio_num` 中设置 PWM GPIO 管脚。
    - ``mcpwm_init``：为得到预期的 PWM 波形，用户需要至少分配一个 MCPWM 定时器和 MCPWM 运算器，然后通过调用 :cpp:func:`mcpwm_operator_connect_timer` 将二者连接起来。然后，用户需要调用如:cpp:func:`mcpwm_generator_set_actions_on_timer_event`， :cpp:func:`mcpwm_generator_set_actions_on_compare_event` 来设置发生器对不同事件的动作。
    - ``mcpwm_group_set_resolution``：新驱动中，群组分辨率固定为最大值，通常为 80 MHz。
    - ``mcpwm_timer_set_resolution``：MCPWM 定时器的分辨率在 :cpp:member:`mcpwm_timer_config_t::resolution_hz` 中进行设置。
    - ``mcpwm_set_frequency``：PWM 频率由 :cpp:member:`mcpwm_timer_config_t::resolution_hz` ，:cpp:member:`mcpwm_timer_config_t::count_mode` 和:cpp:member:`mcpwm_timer_config_t::period_ticks` 决定。
    - ``mcpwm_set_duty``：为设置 PWM 占空比，用户应调用 :cpp:func:`mcpwm_comparator_set_compare_value` 来改变比较器的阈值。
    - ``mcpwm_set_duty_type``：新驱动中没有预设的占空比模式，通过设置不同的发生器行为，如 :cpp:func:`mcpwm_generator_set_actions_on_timer_event`，来配置占空比模式。
    - ``mcpwm_set_signal_high`` 和 ``mcpwm_set_signal_low`` 更新为 :cpp:func:`mcpwm_generator_set_force_level`。新驱动中，这是通过为发生器设置力作用来实现的，而不是在后台将占空比改为 0% 或 100%。
    - ``mcpwm_start`` 和 ``mcpwm_stop`` 更新为 :cpp:func:`mcpwm_timer_start_stop`。用户可以用更多的模式来启动和停止 MCPWM 定时器，详见 :cpp:type:`mcpwm_timer_start_stop_cmd_t`。
    - ``mcpwm_carrier_init`` 更新为 :cpp:func:`mcpwm_operator_apply_carrier`。
    - ``mcpwm_carrier_enable`` 与 ``mcpwm_carrier_disable``：通过检查载波设置结构 :cpp:type:`mcpwm_carrier_config_t` 是否为空集来自动使能和停用载波子模块。
    - ``mcpwm_carrier_set_period`` 更新为 :cpp:member:`mcpwm_carrier_config_t::frequency_hz`。
    - ``mcpwm_carrier_set_duty_cycle`` 更新为 :cpp:member:`mcpwm_carrier_config_t::duty_cycle`。
    - ``mcpwm_carrier_oneshot_mode_enable`` 更新为 :cpp:member:`mcpwm_carrier_config_t::first_pulse_duration_us`。
    - 更新后， ``mcpwm_carrier_oneshot_mode_disable`` 被删除。硬件不支持停用第一个载波脉冲（即一次性脉冲）。
    - ``mcpwm_carrier_output_invert`` 更新为 :cpp:member:`mcpwm_carrier_config_t::invert_before_modulate` 和 :cpp:member:`mcpwm_carrier_config_t::invert_after_modulate`。
    - ``mcpwm_deadtime_enable`` 与 ``mcpwm_deadtime_disable`` 更新为 :cpp:func:`mcpwm_generator_set_dead_time`。
    - ``mcpwm_fault_init`` 更新为 :cpp:func:`mcpwm_new_gpio_fault`。
    - ``mcpwm_fault_set_oneshot_mode`` 与 ``mcpwm_fault_set_cyc_mode`` 更新为 :cpp:func:`mcpwm_operator_set_brake_on_fault` 与 :cpp:func:`mcpwm_generator_set_actions_on_brake_event`。
    - 由于 ``mcpwm_capture_enable`` 与 :cpp:func:`mcpwm_capture_enable_channel` 重复，因此在更新后被删除。
    - 由于 ``mcpwm_capture_disable`` 与 :cpp:func:`mcpwm_capture_capture_disable_channel` 重复，因此在更新后被删除。
    - ``mcpwm_capture_enable_channel`` 与 ``mcpwm_capture_disable_channel`` 更新为 :cpp:func:`mcpwm_capture_channel_enable` 与 :cpp:func:`mcpwm_capture_channel_disable`。
    - ``mcpwm_capture_signal_get_value`` 与 ``mcpwm_capture_signal_get_edge``：通过 :cpp:type:`mcpwm_capture_event_data_t`，获取事件回调函数中提供了计时器的数值和边缘电平。只有获取事件发生时，获取数据才有意义，提供单一的 API 来获取捕获数据是没有意义的。
    - 由于 ``mcpwm_sync_enable`` 与 :cpp:func:`mcpwm_sync_configure` 重复，因此更新后被删除。
    - ``mcpwm_sync_configure`` 更新为 :cpp:func:`mcpwm_timer_set_phase_on_sync`。
    - ``mcpwm_sync_disable`` 相当于将 :cpp:member:`mcpwm_timer_sync_phase_config_t::sync_src` 设置为 ``NULL``。
    - ``mcpwm_set_timer_sync_output`` 更新为 :cpp:func:`mcpwm_new_timer_sync_src`。
    - ``mcpwm_timer_trigger_soft_sync`` 更新为 :cpp:func:`mcpwm_soft_sync_activate`。
    - ``mcpwm_sync_invert_gpio_synchro`` 与设置 :cpp:member:`mcpwm_gpio_sync_src_config_t::active_neg` 功能相同。
    - 更新后， ``mcpwm_isr_register`` 已被删除。用户可以注册不同的事件回调函数来替代其功能，例如，可以使用 :cpp:func:`mcpwm_capture_channel_register_event_callbacks` 注册获取事件注册函数。

.. only:: SOC_DEDICATED_GPIO_SUPPORTED

    专用的 GPIO 驱动
    -------------------------------------

    - 更新后，所有与专用 GPIO 管脚相关的底层 (LL) 函数从 ``cpu_ll.h`` 中被移至 ``dedic_gpio_cpu_ll.h``，并重新命名。

.. only:: SOC_I2S_SUPPORTED

    I2S 驱动
    -----------------------

    旧版 I2S 驱动在支持 ESP32-C3 和 ESP32-S3 新功能时暴露了很多缺点，为解决这些缺点，I2S 驱动已更新（请参考:doc:`I2S Driver <../../../api-reference/peripherals/i2s>`）。用户可以通过引用不同 I2S 模式对应的头文件来使用新版驱动的 API，如 :component_file:`esp_driver_i2s/include/driver/i2s_std.h`， :component_file:`esp_driver_i2s/include/driver/i2s_pdm.h` 以及 :component_file:`esp_driver_i2s/include/driver/i2s_tdm.h`。

    为保证前向兼容，旧版驱动的 API 仍然在 :component_file:`driver/deprecated/driver/i2s.h` 中可用。但使用旧版 API 会触发编译警告，该警告可通过配置 Kconfig 选项 :ref:`CONFIG_I2S_SUPPRESS_DEPRECATE_WARN` 来关闭。

    以下是更新后的 I2S 文件概况。

    .. figure:: ../../../../_static/diagrams/i2s/i2s_file_structure.png
        :align: center
        :alt: I2S File Structure

    主要概念更新
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    独立的发送通道和接收通道
    """"""""""""""""""""""""""""""""""""""""""""""""""""""""

    更新后，I2S 驱动的最小控制单元是发送/接收通道，而不是整个 I2S 控制器（控制器包括多个通道）。

    - 用户可以分别控制同一个 I2S 控制器的发送通道和接收通道，即可以通过配置实现分别开启和关闭发送通道和接收通道。
    - :cpp:type:`i2s_chan_handle_t` 句柄类型用于唯一地识别 I2S 通道。所有的 API 都需要该通道句柄，用户需要对这些通道句柄进行维护。
    - 对于 ESP32-C3 和 ESP32-S3，同一个控制器中的发送通道和接收通道可以配置为不同的时钟或不同的模式。
    - 但是对于 ESP32 和 ESP32-S2， 同一个控制器中的发送通道和接收通道共享某些硬件资源。因此，配置可能会造成一个通道影响同一个控制器中的另一个通道。
    - 通过将 :cpp:enumerator:`i2s_port_t::I2S_NUM_AUTO` 设置为 I2S 端口 ID，驱动会搜索可用的发送/接收通道，之后通道会被自动注册到可用的 I2S 控制器上。但是，驱动仍然支持将通道注册到一个特定的端口上。
    - 为区分发送/接收通道和声音通道，在更新后的驱动中，“通道 (channel)”一词仅代表发送/接收通道，用“声道 (slot)”来表示声音通道。

    I2S 模式分类
    """"""""""""""""""""""""""""""""

    I2S 通信模式包括以下三种模式，请注意：

    - **标准模式**：标准模式通常包括两个声道，支持 Philips，MSB 和 PCM（短帧同步）格式，详见 :component_file:`esp_driver_i2s/include/driver/i2s_std.h`。
    - **PDM模式**：PDM 模式仅支持两个声道，16 bit 数据位宽，但是 PDM TX 和 PDM RX 的配置略有不同。对于 PDM TX，采样率可通过 :cpp:member:`i2s_pdm_tx_clk_config_t::sample_rate` 进行设置，其时钟频率取决于上采样的配置。对于 PDM RX，采样率可通过 :cpp:member:`i2s_pdm_rx_clk_config_t::sample_rate` 进行设置，其时钟频率取决于下采样的配置，详见 :component_file:`esp_driver_i2s/include/driver/i2s_pdm.h`。
    - **TDM 模式**：TDM 模式可支持高达 16 声道，该模式可工作在 Philips，MSB，PCM（短帧同步）和PCM（长帧同步）格式下，详见 :component_file:`esp_driver_i2s/include/driver/i2s_tdm.h`。

    在某个模式下分配新通道时，必须通过相应的函数初始化这个通道。我们强烈建议使用辅助宏来生成默认配置，以避免默认值被改动。

    独立的声道配置和时钟配置
    """"""""""""""""""""""""""""""""""""""""""""""""""""

    可以单独进行声道配置和时钟配置。

    - 通过调用 :cpp:func:`i2s_channel_init_std_mode`， :cpp:func:`i2s_channel_init_pdm_rx_mode`， :cpp:func:`i2s_channel_init_pdm_tx_mode` 或:cpp:func:`i2s_channel_init_tdm_mode` 初始化声道/时钟/GPIO 管脚配置。
    - 通过调用 :cpp:func:`i2s_channel_reconfig_std_slot`， :cpp:func:`i2s_channel_reconfig_pdm_rx_slot`， :cpp:func:`i2s_channel_reconfig_pdm_tx_slot` 或 :cpp:func:`i2s_channel_reconfig_tdm_slot` 可以在初始化之后改变声道配置。
    - 通过调用 :cpp:func:`i2s_channel_reconfig_std_clock`， :cpp:func:`i2s_channel_reconfig_pdm_rx_clock`， :cpp:func:`i2s_channel_reconfig_pdm_tx_clock` 或 :cpp:func:`i2s_channel_reconfig_tdm_clock` 可以在初始化之后改变时钟配置。
    - 通过调用 :cpp:func:`i2s_channel_reconfig_std_gpio`， :cpp:func:`i2s_channel_reconfig_pdm_rx_gpio`， :cpp:func:`i2s_channel_reconfig_pdm_tx_gpio` 或 :cpp:func:`i2s_channel_reconfig_tdm_gpio` 可以在初始化之后改变 GPIO 管脚配置。

    Misc
    """"

    - 更新后，I2S 驱动利用状态和状态机避免在错误状态下调用 API。
    - 更新后，ADC 和 DAC 模式已被删除，只有它们各自专用的驱动及 I2S 旧版驱动还支持这两种模式。

    主要使用方法更新
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    请参考以下步骤使用更新后的 I2S 驱动：

    1. 通过调用 :cpp:func:`i2s_new_channel` 来获取通道句柄。我们应该在此步骤中指定外设为主机还是从机以及 I2S 端口。此外，驱动负责生成发送通道或接收通道的句柄。不需要同时输入发送通道和接收通道句柄，但需要输入至少一个句柄。输入两个句柄时，驱动会工作在双工模式。在同一端口上，发送通道和接收通道同时可用，并且共享 MCLK，BCLK 和 WS 信号。如果只输入了发送通道句柄或接收通道句柄，该通道只能工作在单工模式。
    2. 通过调用 :func:`i2s_channel_init_std_mode`， :func:`i2s_channel_init_pdm_rx_mode`， :func:`i2s_channel_init_pdm_tx_mode` 或 :func:`i2s_channel_init_tdm_mode` 将通道初始化为指定模式。进行相应的声道、时钟和 GPIO 管脚配置。
    3. （可选）通过调用 :cpp:func:`i2s_channel_register_event_callback` 注册 ISR 事件回调函数。I2S 事件由回调函数同步接收，而不是从事件队列中异步接收。
    4. 通过调用 :cpp:func:`i2s_channel_enable` 来开启 I2S 通道的硬件资源。在更新后的驱动中，I2S 在安装后不会再自动开启，用户需要确定通道是否已经开启。
    5. 分别通过 :cpp:func:`i2s_channel_read` 和 :cpp:func:`i2s_channel_write` 来读取和写入数据。当然，在 :cpp:func:`i2s_channel_read` 中只能输入接收通道句柄，在 :cpp:func:`i2s_channel_write` 中只能输入发送通道句柄。
    6. （可选）通过相应的 'reconfig' 函数可以更改声道、时钟和 GPIO 管脚配置，但是更改配置前必须调用 :cpp:func:`i2s_channel_disable`。
    7. 通过调用 :cpp:func:`i2s_channel_disable` 可以停止使用 I2S 通道的硬件资源。
    8. 不再使用某通道时，通过调用 :cpp:func:`i2s_del_channel` 可以删除和释放该通道资源，但是删除之前必须先停用该通道。

.. only:: SOC_TWAI_SUPPORTED

    TWAI 驱动程序
    --------------

    ``CAN`` 外设驱动程序已弃用并被删除，请使用 ``TWAI`` 驱动程序代替（即在应用程序中包括 ``driver/twai.h``）。

用于访问寄存器的宏
---------------------------------------------

更新前，所有用于访问寄存器的宏都可以作为表达式来使用，所以以下命令是允许的::

    uint32_t val = REG_SET_BITS(reg, bits, mask);

在 ESP-IDF v5.0 中，用于写入或读取-修改-写入寄存器的宏不能再作为表达式使用，而只能作为语句使用，这适用于以下宏： ``REG_WRITE``， ``REG_SET_BIT``， ``REG_CLR_BIT``， ``REG_SET_BITS``， ``REG_SET_FIELD``， ``WRITE_PERI_REG``， ``CLEAR_PERI_REG_MASK``， ``SET_PERI_REG_MASK``， ``SET_PERI_REG_BITS``。

为存储要写入寄存器的值，请按以下步骤完成操作::

    uint32_t new_val = REG_READ(reg) | mask;
    REG_WRITE(reg, new_val);

要获得修改后的寄存器的值（该值可能与写入的值不同），要增加一个显示的读取命令::

    REG_SET_BITS(reg, bits, mask);
    uint32_t new_val = REG_READ(reg);
