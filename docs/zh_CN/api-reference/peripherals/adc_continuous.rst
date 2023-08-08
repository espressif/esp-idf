模数转换器 (ADC) 连续转换模式驱动
========================================================

:link_to_translation:`en:[English]`

{IDF_TARGET_ADC_NUM:default="两", esp32c2="一", esp32c6="一", esp32h2="一"}

简介
------------

{IDF_TARGET_NAME} 芯片集成了模数转换器 (ADC)，支持测量特定模拟 IO 管脚的模拟信号。此外，ADC 还支持直接内存访问 (DMA) 功能，高效获取 ADC 转换结果。

{IDF_TARGET_NAME} 具有 {IDF_TARGET_ADC_NUM} 个 ADC 单元，可应用于以下场景：

- 生成单次 ADC 转换结果
- 生成连续 ADC 转换结果

本指南介绍了 ADC 连续转换模式。

ADC 连续转换模式驱动概念
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ADC 连续转换模式驱动由多个转换帧组成。

- 转换帧：一个转换帧包含多个转换结果。转换帧大小以字节为单位，在 :cpp:func:`adc_continuous_new_handle` 中配置。
- 转换结果：一个转换结果包含多个字节，即 :c:macro:`SOC_ADC_DIGI_RESULT_BYTES`。转换结果的数据结构由 :cpp:type:`adc_digi_output_data_t` 定义，包括 ADC 单元、ADC 通道以及原始数据。

.. image:: /../_static/diagrams/adc/adc_conversion_frame.png
    :scale: 100 %
    :align: center

功能概述
-------------------

下文将分节概述安装 ADC 连续转换模式驱动、并从一组 ADC 通道连续读取 ADC 转换结果的基本步骤：

- :ref:`adc-continuous-resource-allocation`：介绍初始化 ADC 连续转换模式驱动所需设置的参数，以及如何将驱动去初始化。
- :ref:`adc-continuous-adc-configurations`：介绍如何将 ADC 配置为在连续转换模式下工作。
- :ref:`adc-continuous-adc-control`：介绍 ADC 控制函数。
- :ref:`adc-continuous-register-event-callbacks`：介绍如何将特定用户代码链接到 ADC 连续转换模式事件回调函数。
- :ref:`adc-continuous-read-conversion-result`：介绍如何获取 ADC 转换结果。
- :ref:`adc-continuous-hardware-limitations`：介绍与 ADC 相关的硬件限制。
- :ref:`adc-continuous-power-management`：介绍电源管理的相关内容。
- :ref:`adc-continuous-iram-safe`：介绍与 IRAM 安全相关的函数。
- :ref:`adc-continuous-thread-safety`：介绍由驱动程序认证为线程安全的 API。


.. _adc-continuous-resource-allocation:

资源分配
^^^^^^^^^^^^^^^^^^^

ADC 连续转换模式驱动基于 {IDF_TARGET_NAME} SAR ADC 模块实现，不同的 ESP 目标芯片可能拥有不同数量的独立 ADC。

请按照以下步骤设置配置结构体 :cpp:type:`adc_continuous_handle_cfg_t`，创建 ADC 连续转换模式驱动的句柄：

- :cpp:member:`adc_continuous_handle_cfg_t::max_store_buf_size`：以字节为单位设置最大缓冲池的大小，驱动程序将 ADC 转换结果保存到该缓冲池中。缓冲池已满时，新的转换将丢失。
- :cpp:member:`adc_continuous_handle_cfg_t::conv_frame_size`：以字节为单位设置 ADC 转换帧大小。
- :cpp:member:`adc_continuous_handle_cfg_t::flags`：设置可以改变驱动程序行为的标志。

  - ``flush_pool``：缓冲池满时自动清空缓冲池。


完成以上 ADC 配置后，使用已设置的配置结构体 :cpp:type:`adc_continuous_handle_cfg_t` 调用 :cpp:func:`adc_continuous_new_handle`。该函数可能将在特定情况下返回错误值，如无效参数、内存不足等。

.. only:: esp32

    函数返回 :c:macro:`ESP_ERR_NOT_FOUND` 时，表明 I2S0 外设正在使用中，详情请参阅 :ref:`adc-continuous-hardware-limitations`。

.. only:: esp32s2

    函数返回 :c:macro:`ESP_ERR_NOT_FOUND` 时，表明 SPI3 外设正在使用中，详情请参阅 :ref:`adc-continuous-hardware-limitations`。

.. only:: SOC_GDMA_SUPPORTED

    函数返回 :c:macro:`ESP_ERR_NOT_FOUND` 时，表明 GDMA 空闲通道不足。

如果不再使用 ADC 连续转换模式驱动，请调用 :cpp:func:`adc_continuous_deinit` 将驱动去初始化。


.. only:: SOC_ADC_DIG_IIR_FILTER_SUPPORTED

    IIR 滤波器
    ~~~~~~~~~~

    ADC 连续转换模式下支持使用两个 IIR 滤波器。请设置 :cpp:type:`adc_continuous_iir_filter_config_t` 结构体并调用 :cpp:func:`adc_new_continuous_iir_filter`，以创建 ADC IIR 滤波器。

    - :cpp:member:`adc_digi_filter_config_t::unit`：ADC 单元。
    - :cpp:member:`adc_digi_filter_config_t::channel`：将进行滤波的 ADC 通道。
    - :cpp:member:`adc_digi_filter_config_t::coeff`：滤波器系数。

    .. only:: SOC_ADC_DIG_IIR_FILTER_UNIT_BINDED

            在 {IDF_TARGET_NAME} 上，滤波器按 ADC 单元设置。一旦启用了滤波器，将对当前 ADC 单元中所有启用的 ADC 通道进行滤波。每个通道的滤波结果取决于前一次的滤波结果，因此为避免混淆滤波结果，建议在使用滤波器功能时，每个 ADC 单元只启用一条 ADC 通道，请勿同时启用多条 ADC 通道。

    调用 :cpp:func:`adc_del_continuous_iir_filter` 可以回收滤波器。

    .. only:: not SOC_ADC_DIG_IIR_FILTER_UNIT_BINDED

        .. note::

            在一个 ADC 通道上同时使用两个滤波器时，只有第一个滤波器会生效。

.. only:: SOC_ADC_MONITOR_SUPPORTED

    监视器
    ~~~~~~~

    当 ADC 在连续转换模式下运行时，支持使用 {IDF_TARGET_SOC_ADC_DIGI_MONITOR_NUM} 个监视器。你可以在运行中的 ADC 通道上设置一到两个监视器阈值，一旦转换结果超出阈值，监视器将在每个采样循环中触发中断。请设置 :cpp:type:`adc_monitor_config_t`，并调用 :cpp:func:`adc_new_continuous_monitor` 以创建 ADC 监视器。

    - :cpp:member:`adc_monitor_config_t::adc_unit`：配置要监视的 ADC 通道所属的 ADC 单元。
    - :cpp:member:`adc_monitor_config_t::channel`：要监视的 ADC 通道。
    - :cpp:member:`adc_monitor_config_t::h_threshold`：高阈值，转换结果大于此值将触发中断，如果不使用此阈值，则将其设置为 -1。
    - :cpp:member:`adc_monitor_config_t::l_threshold`：低阈值，转换结果小于此值将触发中断，如果不使用此阈值，则将其设置为 -1。

    创建监视器后，可以使用以下 API 操作监视器，构建你的应用程序。

    - :cpp:func:`adc_continuous_monitor_enable`：启用监视器。
    - :cpp:func:`adc_continuous_monitor_disable`：禁用监视器.
    - :cpp:func:`adc_monitor_register_callbacks`：注册用户回调函数，在 ADC 转换结果超出阈值时，执行相应操作。
    - :cpp:func:`adc_del_continuous_monitor`：删除监视器，释放资源。

    .. only:: esp32s2

        .. NOTE::

            {IDF_TARGET_NAME} 上存在以下硬件限制：
            1. 每个监视器仅支持一个阈值。
            2. 每个 ADC 单元仅支持一个监视器。
            3. ADC 连续转换模式驱动中，如果启用了监视器，无需使用参数 :cpp:member:`adc_monitor_config_t::channel` 指定，某个 ADC 单元中所有已启用的通道都会受监视。

初始化 ADC 连续转换模式驱动
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: c

    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = 1024,
        .conv_frame_size = 100,
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config));


回收 ADC 单元
~~~~~~~~~~~~~~~~~~~~

.. code:: c

    ESP_ERROR_CHECK(adc_continuous_deinit());


.. _adc-continuous-adc-configurations:

配置 ADC
^^^^^^^^^^^^^^^^^^

初始化 ADC 连续转换模式驱动后，设置 :cpp:type:`adc_continuous_config_t` 配置 ADC IO，测量模拟信号：

- :cpp:member:`adc_continuous_config_t::pattern_num`：要使用的 ADC 通道数量。
- :cpp:member:`adc_continuous_config_t::adc_pattern`：每个要使用的 ADC 通道的配置列表，请参阅下文描述。
- :cpp:member:`adc_continuous_config_t::sample_freq_hz`：期望的 ADC 采样频率，单位为 Hz。
- :cpp:member:`adc_continuous_config_t::conv_mode`：连续转换模式。
- :cpp:member:`adc_continuous_config_t::format`：转换模式结果的输出格式。

按照以下步骤设置 :cpp:type:`adc_digi_pattern_config_t`：

- :cpp:member:`adc_digi_pattern_config_t::atten`：ADC 衰减。请参阅 `技术参考手册 <{IDF_TARGET_TRM_CN_URL}#sensor>`__ 中的片上传感器与模拟信号处理章节。
- :cpp:member:`adc_digi_pattern_config_t::channel`：IO 对应的 ADC 通道号，请参阅下文注意事项。
- :cpp:member:`adc_digi_pattern_config_t::unit`：IO 所属的 ADC 单元。
- :cpp:member:`adc_digi_pattern_config_t::bit_width`：原始转换结果的位宽。

.. note::

    对于 IO 对应的 ADC 通道号，请参阅 `技术参考手册 <{IDF_TARGET_TRM_CN_URL}#sensor>`__ 获取 ADC IO 管脚的详细信息。另外，可以使用 :cpp:func:`adc_continuous_io_to_channel` 和 :cpp:func:`adc_continuous_channel_to_io` 获取 ADC 通道和 ADC IO 的对应关系。

为使这些设置生效，请使用上述配置结构体，调用 :cpp:func:`adc_continuous_config`。此 API 可能由于 :c:macro:`ESP_ERR_INVALID_ARG` 等原因返回错误。当它返回 :c:macro:`ESP_ERR_INVALID_STATE` 时，意味着 ADC 连续转换模式驱动已经启动，此时不应调用此 API。

请参考 ADC 连续转换模式示例 :example:`peripherals/adc/continuous_read`，查看相应配置代码。


.. only:: SOC_ADC_DIG_IIR_FILTER_SUPPORTED

    请调用 :cpp:func:`adc_continuous_iir_filter_enable` 或 :cpp:func:`adc_continuous_iir_filter_disable`，以启用或禁用 ADC IIR 滤波器。

.. only:: SOC_ADC_MONITOR_SUPPORTED

    请调用 :cpp:func:`adc_continuous_monitor_enable` 或 :cpp:func:`adc_continuous_monitor_disable`，以启用或禁用 ADC 监视器。

.. _adc-continuous-adc-control:

ADC 控制
^^^^^^^^^^^

启动和停止
~~~~~~~~~~~~~~

调用 :cpp:func:`adc_continuous_start`，将使 ADC 开始从配置好的 ADC 通道测量模拟信号，并生成转换结果。

相反，调用 :cpp:func:`adc_continuous_stop` 则会停止 ADC 转换。

.. code::c

    ESP_ERROR_CHECK(adc_continuous_start());

.. code:: c

    ESP_ERROR_CHECK(adc_continuous_stop());


.. _adc-continuous-register-event-callbacks:

注册事件回调
^^^^^^^^^^^^^^^^^^^^^^^^

调用 :cpp:func:`adc_continuous_register_event_callbacks`，可以将自己的函数链接到驱动程序的 ISR 中。通过 :cpp:type:`adc_continuous_evt_cbs_t` 可查看所有支持的事件回调。

- :cpp:member:`adc_continuous_evt_cbs_t::on_conv_done`：当一个转换帧完成时，触发此事件。
- :cpp:member:`adc_continuous_evt_cbs_t::on_pool_ovf`：当内部缓冲池已满时，触发此事件，新的转换结果将丢失。

由于上述回调函数在 ISR 中调用，请确保回调函数适合在 ISR 上下文中运行，且这些回调不应涉及阻塞逻辑。回调函数的原型在 :cpp:type:`adc_continuous_callback_t` 中声明。

在调用 :cpp:func:`adc_continuous_register_event_callbacks` 时，还可以通过参数 ``user_data`` 注册自己的上下文，该用户数据将直接传递给回调函数。

此回调函数可能由于 :c:macro:`ESP_ERR_INVALID_ARG` 等原因返回错误。启用 :ref:`CONFIG_ADC_CONTINUOUS_ISR_IRAM_SAFE` 时，如果回调函数失败并报错，可能是因为回调函数不在内部 RAM 中，请查看错误日志了解详情。此外，如果回调函数出现 :c:macro:`ESP_ERR_INVALID_STATE` 错误，表明 ADC 连续转换模式驱动已经启动，此时不应添加回调。


转换完成事件
~~~~~~~~~~~~~~~~~~~~~

当驱动程序完成一次转换后，会触发 :cpp:member:`adc_continuous_evt_cbs_t::on_conv_done` 事件，并填充事件数据。事件数据包含一个指向转换帧缓冲区的指针，以及转换帧缓冲区大小。要了解事件数据结构，请参阅 :cpp:type:`adc_continuous_evt_data_t`。

.. note::

    注意，数据缓冲区 :cpp:member:`adc_continuous_evt_data_t::conv_frame_buffer` 由驱动程序本身维护，请勿释放此内存。

.. note::

    启用 Kconfig 选项 :ref:`CONFIG_ADC_CONTINUOUS_ISR_IRAM_SAFE` 时，注册的回调函数以及回调函数中调用的函数应放置在 IRAM 中，涉及的变量也应放置在内部 RAM 中。

缓冲池溢出事件
~~~~~~~~~~~~~~~~~~~

ADC 连续转换模式驱动使用内部缓冲池保存转换结果，缓冲池满时将发生缓冲池溢出事件。此时，驱动程序不会继续填充事件数据。缓冲池溢出通常是因为调用 :cpp:func:`adc_continuous_read` 从池中读取数据的速度远低于 ADC 转换的速度。


.. _adc-continuous-read-conversion-result:

读取转换结果
^^^^^^^^^^^^^^^^^^^^^^

调用 :cpp:func:`adc_continuous_start` 启动 ADC 连续转换，调用 :cpp:func:`adc_continuous_read` 可以获取 ADC 通道的转换结果。注意提供缓冲区，获取原始结果。

函数 :cpp:func:`adc_continuous_read` 每次都会尝试以期望长度读取转换结果。

- 调用 :cpp:func:`adc_continuous_read` 可以请求读取指定长度的转换结果。但有时实际可用的转换结果可能少于请求长度，此时，函数仍会将数据从内部池移动到你提供的缓冲区中。因此，请查看 ``out_length`` 的值，了解实际移动到缓冲区中的转换结果数量。
- 如果内部池中没有生成转换结果，函数将会阻塞一段时间，即 ``timeout_ms``，直到转换结果生成。如果始终没有转换结果生成，函数将返回 :c:macro:`ESP_ERR_TIMEOUT`。
- 如果 ADC 连续转换生成的结果填满了内部池，新产生的结果将丢失。下次调用 :cpp:func:`adc_continuous_read` 时，将返回 :c:macro:`ESP_ERR_INVALID_STATE`，提示此情况发生。

此 API 提供了一个读取所有 ADC 连续转换结果的机会。

从上述函数读取的 ADC 转换结果为原始数据。要根据 ADC 原始结果计算电压，可以使用以下公式：

.. parsed-literal::

    Vout = Dout * Vmax / Dmax       (1)

其中：

.. list-table::
    :header-rows: 1
    :widths: 20 80
    :align: center

    * - Vout
      - 数据输出结果，代表电压。
    * - Dout
      - ADC 原始数据读取结果。
    * - Vmax
      - 可测量的最大模拟输入电压，与 ADC 衰减相关，请参考 `技术参考手册 <{IDF_TARGET_TRM_CN_URL}#sensor>`__ 中的片上传感器与模拟信号处理章节。
    * - Dmax
      - 输出 ADC 原始数据读取结果的最大值，即 2^位宽，位宽即之前配置的 :cpp:member:`adc_digi_pattern_config_t::bit_width`。

若需进一步校准，将 ADC 原始结果转换为以 mV 为单位的电压数据，请参考 :doc:`adc_calibration`。

.. _adc-continuous-hardware-limitations:

.. _hardware_limitations_adc_continuous:

硬件限制
^^^^^^^^^^^^^^^^^^^^

- 一个 ADC 单元一次只能运行一种操作模式，即连续模式或单次模式。:cpp:func:`adc_continuous_start` 提供了保护措施。

- 随机数生成器 (RNG) 以 ADC 为输入源。使用 ADC 连续转换模式驱动从 RNG 生成随机数时，随机性会减弱。

.. only:: esp32 or esp32s2

    - Wi-Fi 也使用 ADC2，:cpp:func:`adc_continuous_start` 提供了 Wi-Fi 驱动和 ADC 连续转换模式驱动之间的保护。

.. only:: esp32

    - ADC 连续转换模式驱动使用 I2S0 外设作为硬件 DMA FIFO。因此，如果 I2S0 已在使用中，:cpp:func:`adc_continuous_new_handle` 将返回 :c:macro:`ESP_ERR_NOT_FOUND`。

    - ESP32 DevKitC：由于存在外部自动烧录电路，GPIO 0 不能用于 ADC 连续转换模式。

    - ESP-WROVER-KIT：由于部分 GPIO 管脚可能已经用于其他目的，GPIO 0、2、4 和 15 不能用于 ADC 连续转换模式。

.. only:: esp32s2

    - ADC 连续转换模式驱动使用 SPI3 外设作为硬件 DMA FIFO。因此，如果 SPI3 已在使用中，:cpp:func:`adc_continuous_new_handle` 将返回 :c:macro:`ESP_ERR_NOT_FOUND`。

.. only:: esp32c3

    - 由于硬件限制，现已不再支持使用 ADC2 DMA 功能获取 ADC 转换结果。使用 ADC2 连续转换的结果可能不稳定，具体可参考 `ESP32-C3 系列芯片勘误表 <https://www.espressif.com/sites/default/files/documentation/esp32-c3_errata_cn.pdf>`__。出于兼容性考虑，可以启用 :ref:`CONFIG_ADC_CONTINUOUS_FORCE_USE_ADC2_ON_C3_S3`，强制使用 ADC2。

.. only:: esp32s3

    - 由于硬件限制，现已不再支持使用 ADC2 DMA 功能获取 ADC 转换结果。使用 ADC2 连续转换的结果可能不稳定，具体可参考 `ESP32-S3 系列芯片勘误表 <https://www.espressif.com/sites/default/files/documentation/esp32-s3_errata_cn.pdf>`__。出于兼容性考虑，可以启用 :ref:`CONFIG_ADC_CONTINUOUS_FORCE_USE_ADC2_ON_C3_S3`，强制使用 ADC2。

    .. _adc-continuous-power-management:

.. only:: not esp32s3

    .. _adc-continuous-power-management:

电源管理
^^^^^^^^^^^^^^^^

启用电源管理，即启用 :ref:`CONFIG_PM_ENABLE` 时，系统在空闲状态下，可能会调整 APB 时钟频率，这可能会改变 ADC 连续转换的行为。

然而，通过获取类型为 :cpp:enumerator:`ESP_PM_APB_FREQ_MAX` 的电源管理锁，ADC 连续转换模式驱动可以阻止这种改变。调用 :cpp:func:`adc_continuous_start` 启动连续转换后即可获取该锁。同样，调用 :cpp:func:`adc_continuous_stop` 停止转换后将释放该锁。因此，必须确保 :cpp:func:`adc_continuous_start` 和 :cpp:func:`adc_continuous_stop` 成对出现，否则电源管理将失效。


.. _adc-continuous-iram-safe:

IRAM 安全
^^^^^^^^^

ADC 连续转换模式驱动的所有 API 均非 IRAM 安全。禁用 cache 时，不应运行这类 API。启用 Kconfig 选项 :ref:`CONFIG_ADC_CONTINUOUS_ISR_IRAM_SAFE` 可确保驱动的内部 ISR 处理程序为 IRAM 安全，此时即使禁用 cache，驱动仍然会将转换结果保存到其内部缓冲池中。


.. _adc-continuous-thread-safety:

线程安全
^^^^^^^^^^^^^

ADC 连续转换模式驱动的 API 不一定线程安全，但驱动程序提供了共享硬件互斥，详情请参阅 :ref:`adc-continuous-hardware-limitations`。


应用示例
--------------------

* ADC 连续转换模式示例：:example:`peripherals/adc/continuous_read`。


API 参考
-------------

.. include-build-file:: inc/adc_continuous.inc
