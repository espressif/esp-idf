模数转换器 (ADC) 校准驱动程序
=================================

:link_to_translation:`en:[English]`

简介
----

在 {IDF_TARGET_NAME} 中，模数转换器 (ADC) 比较输入的模拟电压和参考电压，以确定每一位数字输出结果。{IDF_TARGET_NAME} 设计的 ADC 参考电压为 1100 mV。然而，不同芯片的真实参考电压可能会略有变化，范围在 1000 mV 到 1200 mV 之间。本文介绍了 ADC 校准驱动程序，可以降低参考电压不同带来的影响，获取更准确的输出结果。


功能概述
--------

下文将分节概述安装和使用 ADC 校准驱动程序的基本步骤：

.. list::

 - :ref:`adc-calibration-scheme-creation` - 介绍如何创建和删除校准方案句柄。
 - :ref:`adc-result-conversion` - 介绍如何将原始 ADC 结果转换为校准后的结果。
 - :ref:`adc-thread-safety` - 列出由驱动程序认证为线程安全的 API。
 - :ref:`减少噪声 <adc-minimize-noise>` - 介绍一种常见的降低噪声的方法。
 :esp32: - :ref:`adc-kconfig-options` - 列出了驱动程序支持的 Kconfig 选项，这些选项会对驱动程序的行为产生不同影响。


.. _adc-calibration-scheme-creation:

创建校准方案
^^^^^^^^^^^^^^^^^^^^^^^^^^^

ADC 校准驱动程序会提供 ADC 校准方案。对于驱动程序来说，每个 ADC 校准方案对应一个 ADC 校准句柄 :cpp:type:`adc_cali_handle_t`。

使用 :cpp:func:`adc_cali_check_scheme` 可以查看芯片支持的校准方案。若已了解芯片支持的校准方案，可以跳过该步骤，直接调用对应函数创建校准方案句柄。

使用自定义 ADC 校准方案时，可以选择调整函数 :cpp:func:`adc_cali_check_scheme`，或直接跳过该步骤，调用自定义函数创建校准方案句柄。

.. only:: esp32 or esp32s2 or esp32c2

    ADC 校准线性拟合方案
    ```````````````````````````````````

    {IDF_TARGET_NAME} 支持 :c:macro:`ADC_CALI_SCHEME_VER_LINE_FITTING` 方案。要创建此方案，请先根据以下配置选项，设置 :cpp:type:`adc_cali_line_fitting_config_t`。

    - :cpp:member:`adc_cali_line_fitting_config_t::unit_id`，表示 ADC 原始结果来自哪个 ADC 单元。
    - :cpp:member:`adc_cali_line_fitting_config_t::atten`，表示 ADC 原始结果的衰减程度。
    - :cpp:member:`adc_cali_line_fitting_config_t::bitwidth`，表示 ADC 原始结果的位宽。

    .. only:: esp32

        :cpp:member:`adc_cali_line_fitting_config_t::default_vref` 通常默认设置为 0，此值不影响线性拟合方案的校准过程。然而，如果线性拟合方案所需使用的 eFuse 位没有烧录到板上，则驱动程序将根据 :cpp:member:`adc_cali_line_fitting_config_t::default_vref` 进行校准。

        通过 :cpp:func:`adc_cali_scheme_line_fitting_check_efuse` 可以检查 eFuse 位。一般情况下，线性拟合方案的 eFuse 值为 :c:macro:`ADC_CALI_LINE_FITTING_EFUSE_VAL_EFUSE_TP` 或 :c:macro:`ADC_CALI_LINE_FITTING_EFUSE_VAL_EFUSE_VREF`。这意味着线性拟合方案在校准时，使用的是烧录在 eFuse 中的校准参数。

        当线性拟合方案的 eFuse 值为 :c:macro:`ADC_CALI_LINE_FITTING_EFUSE_VAL_DEFAULT_VREF` 时，请设置 :cpp:member:`esp_adc_cali_line_fitting_init::default_vref`。默认 vref 为 ADC 参考电压的估计值，在校准时作为参数提供。

    设置完上述配置结构体后，请调用 :cpp:func:`adc_cali_create_scheme_line_fitting` 创建线性拟合校准方案句柄。

    .. only:: esp32s2

        由于 :c:macro:`ESP_ERR_INVALID_ARG` 或 :c:macro:`ESP_ERR_NO_MEM` 等原因，该函数调用可能失败。函数返回 :c:macro:`ESP_ERR_NOT_SUPPORTED` 时，说明你的开发板缺少烧录所需的 eFuse 位。

    .. code:: c

        ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&cali_config, &handle));


    ADC 校准使用完毕后，请调用 :cpp:func:`adc_cali_delete_scheme_line_fitting`，删除线性拟合校准方案句柄。


    删除线性拟合校准方案句柄
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    .. code:: c

        ESP_LOGI(TAG, "delete %s calibration scheme", "Line Fitting");
        ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(handle));


.. only:: esp32c3 or esp32s3 or esp32c6 or esp32h2

    ADC 校准曲线拟合方案
    ````````````````````````````````````

    {IDF_TARGET_NAME} 支持 :c:macro:`ADC_CALI_SCHEME_VER_CURVE_FITTING` 方案。要创建此方案，请先根据以下配置选项，设置 :cpp:type:`adc_cali_curve_fitting_config_t`。


    .. only:: esp32c3 or esp32s3

        -  :cpp:member:`adc_cali_curve_fitting_config_t::unit_id`，表示 ADC 原始结果来自哪个 ADC 单元。
        -  :cpp:member:`adc_cali_curve_fitting_config_t::chan`，此选项保留以供扩展。校准方案仅因衰减程度而异，与通道选择无关。
        -  :cpp:member:`adc_cali_curve_fitting_config_t::atten`，表示 ADC 原始结果的衰减程度。
        -  :cpp:member:`adc_cali_curve_fitting_config_t::bitwidth`，表示 ADC 原始结果的位宽。

    .. only:: esp32c6 or esp32h2

        -  :cpp:member:`adc_cali_curve_fitting_config_t::unit_id`，表示 ADC 原始结果来自哪个 ADC 单元。
        -  :cpp:member:`adc_cali_curve_fitting_config_t::chan`，表示获取 ADC 原始结果的 ADC 通道。校准方案不仅因衰减程度而异，还与通道选择有关。
        -  :cpp:member:`adc_cali_curve_fitting_config_t::atten`，表示 ADC 原始结果的衰减程度。
        -  :cpp:member:`adc_cali_curve_fitting_config_t::bitwidth`，表示 ADC 原始结果的位宽。

    设置完上述配置结构体后，请调用 :cpp:func:`adc_cali_create_scheme_curve_fitting` 创建曲线拟合方案句柄。 由于 :c:macro:`ESP_ERR_INVALID_ARG` 或 :c:macro:`ESP_ERR_NO_MEM` 等原因，该函数调用可能失败。函数返回 :c:macro:`ESP_ERR_NOT_SUPPORTED` 时，说明你的开发板没有烧录校准方案所需的 eFuse 位。

    与 eFuse 相关的 ADC 校准故障
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    函数 :cpp:func:`adc_cali_create_scheme_curve_fitting` 返回 :c:macro:`ESP_ERR_NOT_SUPPORTED` 时，代表开发板上校准方案所需的 eFuse 位不正确。

    ESP-IDF 提供的 ADC 校准方案基于芯片上某些与 ADC 校准相关的 eFuse 位的值。乐鑫模组已在出厂时完成烧录，无需用户额外烧录。

    如果遇到此类错误，请前往 `技术咨询 <https://www.espressif.com/en/contact-us/technical-inquiries>`__ 进行反馈。

    创建曲线拟合方案句柄
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    .. code:: c

        ESP_LOGI(TAG, "calibration scheme version is %s", "Curve Fitting");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cali_config, &handle));


    ADC 校准使用完毕后，请调用 :cpp:func:`adc_cali_delete_scheme_curve_fitting`，删除校准方案句柄。


    删除曲线拟合方案句柄
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~

    .. code:: c

        ESP_LOGI(TAG, "delete %s calibration scheme", "Curve Fitting");
        ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(handle));


.. note::

    要使用自定义校准方案，可以通过提供创建函数，创建自己的校准方案句柄。请参阅 ``components/esp_adc/interface/adc_cali_interface.h`` 中的函数表 ``adc_cali_scheme_t``，了解 ESP ADC 校准接口。


.. _adc-result-conversion:

结果转换
^^^^^^^^^^^^^^^^^

对驱动程序进行完上述配置和初始化工作后，可以调用 :cpp:func:`adc_cali_raw_to_voltage`，将原始 ADC 结果转换为校准结果，校准结果以 mV 为单位。该函数可能因参数无效而调用失败。如果函数返回 :c:macro:`ESP_ERR_INVALID_STATE`，说明校准方案尚未创建。因此你需要创建一个校准方案句柄，通过 :cpp:func:`adc_cali_check_scheme` 可以了解当前芯片支持的校准方案；你也可以提供自定义校准方案，创建对应的校准方案句柄。

.. only:: esp32c2

    .. note::

        ADC 校准仅在 :c:macro:`ADC_ATTEN_DB_0` 和 :c:macro:`ADC_ATTEN_DB_12` 时支持。在 :c:macro:`ADC_ATTEN_DB_0` 时，ADC 的衰减程度设置为 0 dB，仅支持低于 950 mV 的输入电压；在 :c:macro:`ADC_ATTEN_DB_12` 时，ADC 的衰减程度设置为 12 dB，仅支持低于 2800 mV 的输入电压。

获取电压
~~~~~~~~~~~

.. code:: c

    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc_cali_handle, adc_raw[0][0], &voltage[0][0]));
    ESP_LOGI(TAG, "ADC%d Channel[%d] Cali Voltage: %d mV", ADC_UNIT_1 + 1, EXAMPLE_ADC1_CHAN0, voltage[0][0]);


.. _adc-thread-safety:

线程安全
^^^^^^^^^^^^^

驱动程序会确保工厂函数 :cpp:func:`esp_adc_cali_new_scheme` 的线程安全，使用时，可以直接从不同的 RTOS 任务中调用此类函数，无需额外锁保护。

其他以 :cpp:type:`adc_cali_handle_t` 作为第一个位置参数的函数均非线程安全，在没有设置互斥锁保护的任务中，应避免从多个任务中调用这类函数。


.. only:: esp32

    .. _adc-kconfig-options:

    Kconfig 选项
    ^^^^^^^^^^^^^^^

    - :ref:`CONFIG_ADC_CAL_EFUSE_TP_ENABLE` - 如果校准相关的 eFuse 值没有配置为 :cpp:type:`ADC_CALI_LINE_FITTING_EFUSE_VAL_EFUSE_TP`，则可以禁用该选项，减小代码大小。
    - :ref:`CONFIG_ADC_CAL_EFUSE_VREF_ENABLE` - 如果校准相关的 eFuse 值没有配置为 :cpp:type:`ADC_CALI_LINE_FITTING_EFUSE_VAL_EFUSE_VREF`，则可以禁用该选项，减小代码大小。
    - :ref:`CONFIG_ADC_CAL_LUT_ENABLE` - 如果校准 ADC 原始结果时，衰减没有设置成 :c:macro:`ADC_ATTEN_DB_12`，则可以禁用该选项，减小代码大小。


.. _adc-minimize-noise:

减少噪声
^^^^^^^^^^^^^^

{IDF_TARGET_NAME} ADC 对噪声敏感，可能导致 ADC 读数出现较大偏差。根据不同使用场景，要减少噪声影响，你可能需要将旁路电容（如 100 nF 陶瓷电容）连接到 ADC 使用的输入管脚。此外，也可以通过多次采样，进一步减轻噪声的影响。

.. only:: esp32

    .. figure:: ../../../_static/diagrams/adc/adc-noise-graph.jpg
        :align: center
        :alt: ADC 噪声抑制

        图中展示了连接电容以及 64 次采样对噪声的抑制效果。


API 参考
--------


.. include-build-file:: inc/adc_cali.inc
.. include-build-file:: inc/adc_cali_scheme.inc
