模数转换器 (ADC) 单次转换模式驱动
===================================

:link_to_translation:`en:[English]`

{IDF_TARGET_ADC_NUM:default="两", esp32c2="一", esp32c6="一", esp32h2="一", esp32c5="一"}

简介
----

模数转换器集成于芯片，支持测量特定模拟 IO 管脚的模拟信号。

{IDF_TARGET_NAME} 有 {IDF_TARGET_ADC_NUM} 个 ADC 单元，可以在以下场景使用：

- 生成 ADC 单次转换结果

.. only:: SOC_ADC_DMA_SUPPORTED

    - 生成连续 ADC 转换结果

本指南介绍了 ADC 单次转换模式。


功能概述
--------

下文将分节概述安装和运行 ADC 的基本步骤：

- :ref:`adc-oneshot-resource-allocation` - 介绍获取 ADC 句柄所需设置的参数，以及如何在 ADC 完成工作后回收资源。
- :ref:`adc-oneshot-unit-configuration` - 介绍配置 ADC 单元所需设置的参数，用于获取 ADC 转换的原始结果。
- :ref:`adc-oneshot-read-conversion-result` - 介绍如何获取 ADC 转换的原始结果。
- :ref:`hardware_limitations_adc_oneshot` - 介绍与 ADC 相关的硬件限制。
- :ref:`adc-oneshot-power-management` - 介绍电源管理的相关内容。
- :ref:`adc-oneshot-iram-safe` - 介绍在禁用 cache 时，如何读取 ADC 转换的原始结果。
- :ref:`adc-oneshot-thread-safety` - 介绍由驱动程序认证为线程安全的 API。
- :ref:`adc-oneshot-kconfig-options` - 介绍支持的 Kconfig 选项，不同选项对驱动程序的操作会产生不同影响。

.. _adc-oneshot-resource-allocation:

资源分配
^^^^^^^^

ADC 单次转换模式驱动基于 {IDF_TARGET_NAME} SAR ADC 模块实现，不同的 ESP 芯片可能拥有不同数量的独立 ADC。对于单次转换模式驱动而言，ADC 实例以 :cpp:type:`adc_oneshot_unit_handle_t` 表示。

请设置所需的初始配置结构体 :cpp:type:`adc_oneshot_unit_init_cfg_t` 安装 ADC 实例，具体如下：

- :cpp:member:`adc_oneshot_unit_init_cfg_t::unit_id` 选择 ADC。请参阅 `技术规格书 <{IDF_TARGET_TRM_CN_URL}>`__，了解对应 ADC 的专用模拟 IO 管脚。
- :cpp:member:`adc_oneshot_unit_init_cfg_t::clk_src` 选择 ADC 的时钟源。设置为 0 时，驱动程序将使用默认时钟源，详情请参阅 :cpp:type:`adc_oneshot_clk_src_t`。
- :cpp:member:`adc_oneshot_unit_init_cfg_t::ulp_mode` 设置是否支持 ADC 在 ULP 模式下工作。

.. todo::

   Add ULP ADC-related docs here.

完成 ADC 初始配置后，使用已设置的初始配置结构体 :cpp:type:`adc_oneshot_unit_init_cfg_t` 调用 :cpp:func:`adc_oneshot_new_unit`。如果分配成功，该函数将返回 ADC 单元实例句柄。

该函数可能因参数无效、内存不足等原因返回错误代码。比如，当要分配的 ADC 实例已经注册时，该函数会返回 :c:macro:`ESP_ERR_NOT_FOUND` 错误。可用 ADC 数量可通过 :c:macro:`SOC_ADC_PERIPH_NUM` 查看。

如果不再需要先前创建的 ADC 单元实例，请调用 :cpp:func:`adc_oneshot_del_unit` 回收该实例，相关的硬件和软件资源也会回收。

在普通单次转换模式下创建 ADC 单元实例句柄
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: c

    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));


回收 ADC 单元实例
~~~~~~~~~~~~~~~~~~~~

.. code:: c

    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc1_handle));


.. _adc-oneshot-unit-configuration:

配置 ADC 单元实例
^^^^^^^^^^^^^^^^^^^^^^

创建 ADC 单元实例后，请设置 :cpp:type:`adc_oneshot_chan_cfg_t` 配置 ADC IO 以测量模拟信号，具体如下：

- :cpp:member:`adc_oneshot_chan_cfg_t::atten`，ADC 衰减。请参阅 `技术规格书 <{IDF_TARGET_DATASHEET_CN_URL}>`__ > ``ADC 特性``。
- :cpp:member:`adc_oneshot_chan_cfg_t::bitwidth`，原始转换结果的位宽。

.. note::

    ADC IO 及其对应的 ADC 通道编号，请参阅 `技术规格书 <{IDF_TARGET_TRM_CN_URL}>`__。

    此外，可以使用 :cpp:func:`adc_continuous_io_to_channel` 和 :cpp:func:`adc_continuous_channel_to_io` 了解 ADC 通道和 ADC IO。

为使以上设置生效，请使用上述配置结构体调用 :cpp:func:`adc_oneshot_config_channel`，并指定要配置的 ADC 通道。函数 :cpp:func:`adc_oneshot_config_channel` 支持多次调用，以配置不同的 ADC 通道。驱动程序将在内部保存每个通道的配置。


配置两个 ADC 通道
~~~~~~~~~~~~~~~~~

.. code:: c

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, EXAMPLE_ADC1_CHAN0, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, EXAMPLE_ADC1_CHAN1, &config));


.. _adc-oneshot-read-conversion-result:

读取转换结果
^^^^^^^^^^^^^^^^^^^^^^

完成上述配置后，ADC 即可测量来自配置好的 ADC 通道的模拟信号。调用 :cpp:func:`adc_oneshot_read` 可以获取 ADC 通道的原始转换结果。

- :cpp:func:`adc_oneshot_read` 可安全使用。ADC 由其他驱动程序/外设共享，请参阅 :ref:`hardware_limitations_adc_oneshot`。函数 :cpp:func:`adc_oneshot_read` 使用互斥锁，避免与其他函数同时使用硬件，因此该函数不应在 ISR 上下文中使用。当 ADC 由其他驱动程序/外设占用时，该函数可能出错，并返回 :c:macro:`ESP_ERR_TIMEOUT` 错误。此时，ADC 原始结果无效。

该函数可能因参数无效而调用失败。

通过该函数获取的 ADC 转换结果为原始数据。可以使用以下公式，根据 ADC 原始结果计算电压：

.. parsed-literal::

    Vout = Dout * Vmax / Dmax       (1)

其中：

.. list-table::
    :header-rows: 1
    :widths: 20 80
    :align: center

    * - Vout
      - 数字输出结果，代表电压。
    * - Dout
      - ADC 原始数字读取结果。
    * - Vmax
      - 可测量的最大模拟输入电压，与 ADC 衰减相关，请参考 `技术参考手册 <{IDF_TARGET_TRM_CN_URL}>`__ > ``片上传感器与模拟信号处理``。
    * - Dmax
      - 输出 ADC 原始数字读取结果的最大值，即 2^位宽，位宽即之前配置的 :cpp:member:`adc_oneshot_chan_cfg_t::bitwidth`。

若需进一步校准，将 ADC 原始结果转换为以 mV 为单位的电压数据，请参考校准文档 :doc:`adc_calibration`。


读取原始结果
~~~~~~~~~~~~~~~

.. code:: c

    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, EXAMPLE_ADC1_CHAN0, &adc_raw[0][0]));
    ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, EXAMPLE_ADC1_CHAN0, adc_raw[0][0]);

    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, EXAMPLE_ADC1_CHAN1, &adc_raw[0][1]));
    ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, EXAMPLE_ADC1_CHAN1, adc_raw[0][1]);


.. _hardware_limitations_adc_oneshot:

硬件限制
^^^^^^^^^^^^^^^^^^^^

- 随机数生成器 (RNG) 以 ADC 为输入源。使用 ADC 单次转换模式驱动从 RNG 生成随机数时，随机性会减弱。

.. only:: SOC_ADC_DMA_SUPPORTED

    - 一个 ADC 单元每次只能在一种操作模式下运行，可以是连续模式或单次模式。:cpp:func:`adc_oneshot_start` 提供了保护措施。

.. only:: esp32 or esp32s2 or esp32s3

    - Wi-Fi 也使用 ADC2，:cpp:func:`adc_oneshot_read` 提供了 Wi-Fi 驱动与 ADC 单次转换模式驱动间的保护。

.. only:: esp32c3

    - 由于硬件限制，现已不再支持使用 ADC2 DMA 功能获取 ADC 转换结果。使用 ADC2 单次转换的结果可能不稳定，具体可参考 `ESP32-C3 系列芯片勘误表 <https://www.espressif.com/sites/default/files/documentation/esp32-c3_errata_cn.pdf>`__。出于兼容性考虑，可以启用 :ref:`CONFIG_ADC_ONESHOT_FORCE_USE_ADC2_ON_C3`，强制使用 ADC2。

.. only:: esp32

    - ESP32-DevKitC：GPIO0 已用于自动烧录功能，不能用于 ADC 单次转换模式。

    - ESP-WROVER-KIT：GPIO0、GPIO2、GPIO4 和 GPIO15 已有其他用途，不能用于 ADC 单次转换模式。

    .. _adc-oneshot-power-management:

.. only:: not esp32

    .. _adc-oneshot-power-management:

电源管理
^^^^^^^^

启用电源管理，即启用 :ref:`CONFIG_PM_ENABLE` 时，系统在空闲状态下可能会调整系统时钟频率。然而，ADC 单次转换模式驱动以轮询例程运行，:cpp:func:`adc_oneshot_read` 会不断检查 CPU 是否完成读取，直到函数返回。在此期间，ADC 单次转换模式驱动程序所在的任务不会受阻塞。因此，在读取时时钟频率保持稳定。


.. _adc-oneshot-iram-safe:

IRAM 安全
^^^^^^^^^

flash 写入/擦除、OTA 等原因都可能导致 cache 禁用，此时，默认不应运行任何 ADC 单次转换模式驱动 API。如果在禁用 cache 时执行了 ADC 单次转换模式驱动 API，可能会出现类似 ``Illegal Instruction`` 或 ``Load/Store Prohibited`` 的错误。


.. _adc-oneshot-thread-safety:

线程安全
^^^^^^^^

- :cpp:func:`adc_oneshot_new_unit`
- :cpp:func:`adc_oneshot_config_channel`
- :cpp:func:`adc_oneshot_read`

上述函数均为线程安全，使用时，可以直接从不同的 RTOS 任务中调用以上函数，无需额外锁保护。

- :cpp:func:`adc_oneshot_del_unit` 非线程安全。此外，与上文中线程安全的函数一起调用该函数时，可能导致线程安全函数的调用出错。


.. _adc-oneshot-kconfig-options:

Kconfig 选项
^^^^^^^^^^^^

- :ref:`CONFIG_ADC_ONESHOT_CTRL_FUNC_IN_IRAM` 决定了放置 ADC 快速读取函数的位置，即 IRAM 或 flash 中，详情请参阅 :ref:`adc-oneshot-iram-safe`。


应用示例
--------------------

* :example:`peripherals/adc/oneshot_read` 演示了如何使用 ADC 单次模式驱动程序从 GPIO 管脚获取单次 ADC 数值，并展示了如何使用 ADC 校准功能在 {IDF_TARGET_NAME} 上获得校准后的结果，单位为毫伏。


API 参考
-------------

.. include-build-file:: inc/adc_types.inc
.. include-build-file:: inc/adc_oneshot.inc
