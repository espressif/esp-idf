图像信号处理器 (ISP)
====================

:link_to_translation:`en:[English]`

简介
----

{IDF_TARGET_NAME} 内含图像信号处理器 (ISP)，是由众多图像处理算法组成的流水线。ISP 从 DVP 摄像头、MIPI-CSI 摄像头或系统存储处接收图像数据，并通过 DMA 将处理后的图像数据写入系统存储。ISP 需要与其他摄像头控制器模块协同工作，无法独立工作。

术语表
------

.. list::

    - MIPI-CSI：符合 MIPI 规范的高速串行摄像头接口
    - DVP：数字视频并行接口，通常由 vsync、hsync、de 和 data 信号组成
    - RAW：直接从图像传感器输出的未处理数据，通常分为 R、Gr、Gb 和 B 四个通道，按位宽分为 RAW8、RAW10、RAW12 等不同格式
    - RGB：由红、绿、蓝三种颜色组成的彩色图像格式，按每种颜色的位宽分为 RGB888、RGB565 等格式
    - YUV：由亮度和色度组成的彩色图像格式，按数据排列方式分为 YUV444、YUV422、YUV420 等格式
    - AF：自动对焦
    - AWB：自动白平衡
    - AE：自动曝光
    - HIST：直方图
    - BF：拜耳域降噪
    - LSC：镜头阴影校正
    - CCM：色彩校正矩阵

ISP 流水线
----------

.. blockdiag::
    :scale: 100%
    :caption: ISP 流水线
    :align: center

    blockdiag isp_pipeline {
        orientation = portrait;
        node_height = 30;
        node_width = 120;
        span_width = 100;
        default_fontsize = 16;

        isp_header [label = "ISP Header"];
        isp_tail [label = "ISP Tail"];
        isp_chs [label = "对比度 &\n 色调 & 饱和度", width = 150, height = 70];
        isp_yuv [label = "YUV 限制\n YUB2RGB", width = 120, height = 70];

        isp_header -> BF -> LSC -> 去马赛克 -> CCM -> gamma 校正 -> RGB 转 YUV -> 锐化 -> isp_chs -> isp_yuv -> isp_tail;

        LSC -> HIST
        去马赛克 -> AWB
        去马赛克 -> AE
        去马赛克 -> HIST
        CCM -> AWB
        gamma 校正 -> AE
        RGB 转 YUV -> HIST
        RGB 转 YUV -> AF
    }

功能概述
--------

ISP 驱动程序提供以下服务：

- :ref:`isp-resource-allocation` - 涵盖如何通过正确的配置来分配 ISP 资源，以及完成工作后如何回收资源。
- :ref:`isp-enable-disable` - 涵盖如何启用和禁用 ISP 处理器。
- :ref:`isp-af-statistics` - 涵盖如何单次或连续获取 AF 统计信息。
- :ref:`isp-awb-statistics` - 涵盖如何单次或连续获取 AWB 白块统计信息。
- :ref:`isp-ae-statistics` - 涵盖如何单次或连续获取 AE 统计信息。
- :ref:`isp-hist-statistics` - 涵盖如何单次或连续获取直方图统计信息。
- :ref:`isp-bf` - 涵盖如何启用和配置 BF 功能。
- :ref:`isp-lsc` - 涵盖如何启用和配置 LSC 功能。
- :ref:`isp-ccm-config` - 涵盖如何配置 CCM。
- :ref:`isp-demosaic` - 涵盖如何配置去马赛克功能。
- :ref:`isp-gamma-correction` - 涵盖如何启用和配置 gamma 校正。
- :ref:`isp-sharpen` - 涵盖如何配置锐化功能。
- :ref:`isp-callback` - 涵盖如何将用户特定代码挂接到 ISP 驱动事件回调。
- :ref:`isp-thread-safety` - 列出了驱动程序中线程安全的 API。
- :ref:`isp-kconfig-options` - 列出了支持的 Kconfig 选项，这些选项可以对驱动程序产生不同影响。
- :ref:`isp-iram-safe` - 描述了当 cache 被禁用时，如何使 ISP 中断和控制功能正常工作。

.. _isp-resource-allocation:

资源分配
^^^^^^^^

安装 ISP 驱动程序
~~~~~~~~~~~~~~~~~

ISP 驱动程序需要由 :cpp:type:`esp_isp_processor_cfg_t` 指定配置。

指定 :cpp:type:`esp_isp_processor_cfg_t` 中的配置后，可以调用 :cpp:func:`esp_isp_new_processor` 来分配和初始化 ISP 处理器。如果函数运行正常，将返回一个 ISP 处理器句柄。请参考以下代码：

.. code-block:: c

    esp_isp_processor_cfg_t isp_config = {
        .clk_src = ISP_CLK_SRC_DEFAULT,
        ...
    };

    isp_proc_handle_t isp_proc = NULL;
    ESP_ERROR_CHECK(esp_isp_new_processor(&isp_config, &isp_proc));

使用上述句柄，可以启用/禁用 ISP 驱动程序，也可以安装其他 ISP 模块。

.. note::

    如果将 MIPI CSI 或 ISP_DVP 用作摄像头控制器，则必须使用 ISP 外设。因此即便无需使用 ISP 功能，也要调用 :cpp:func:`esp_isp_new_processor` 函数安装 ISP 驱动程序。

    如果无需使用 ISP 功能，也可以设置 :cpp:member:`esp_isp_processor_cfg_t::bypass_isp`，使 ISP 驱动程序绕过 ISP 流水线，仅启用必要的功能。

安装 ISP 自动对焦 (AF) 驱动程序
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ISP 自动对焦 (AF) 驱动程序需要由 :cpp:type:`esp_isp_af_config_t` 指定配置。

指定 :cpp:type:`esp_isp_af_config_t` 中的配置后，可以调用 :cpp:func:`esp_isp_new_af_controller` 来分配和初始化 ISP AF 控制器。如果函数运行正常，将返回一个 ISP AF 控制器句柄。请参考以下代码：

.. code-block:: c

    esp_isp_af_config_t af_config = {
        .edge_thresh = 128,
    };
    isp_af_ctlr_t af_ctrlr = NULL;
    ESP_ERROR_CHECK(esp_isp_new_af_controller(isp_proc, &af_config, &af_ctrlr));

使用上述句柄，可以启用/禁用 ISP AF 驱动程序，也可以安装 ISP AF 环境检测模块。

安装 ISP 自动白平衡 (AWB) 驱动程序
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ISP 自动白平衡 (AWB) 驱动程序需要由 :cpp:type:`esp_isp_awb_config_t` 指定配置。

指定 :cpp:type:`esp_isp_awb_config_t` 中的配置后，可以调用 :cpp:func:`esp_isp_new_awb_controller` 来分配和初始化 ISP AWB 控制器。如果函数运行正常，将返回一个 ISP AWB 控制器句柄。请参考以下代码：

.. code-block:: c

    isp_awb_ctlr_t awb_ctlr = NULL;
    uint32_t image_width = 800;
    uint32_t image_height = 600;
    /* AWB 配置，请参考 API 注释来调整参数 */
    esp_isp_awb_config_t awb_config = {
        .sample_point = ISP_AWB_SAMPLE_POINT_AFTER_CCM,
        ...
    };
    ESP_ERROR_CHECK(esp_isp_new_awb_controller(isp_proc, &awb_config, &awb_ctlr));

其他 AWB API 和 AWB 方案也需要此步骤中创建的 AWB 句柄。

安装 ISP 自动曝光 (AE) 驱动程序
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ISP 自动曝光 (AE) 驱动程序需要由 :cpp:type:`esp_isp_ae_config_t` 指定配置。

指定 :cpp:type:`esp_isp_ae_config_t` 中的配置后，可以调用 :cpp:func:`esp_isp_new_ae_controller` 来分配和初始化 ISP AE 控制器。如果函数运行正常，将返回一个 ISP AE 控制器句柄。请参考以下代码：

.. code-block:: c

    esp_isp_ae_config_t ae_config = {
        .sample_point = ISP_AE_SAMPLE_POINT_AFTER_DEMOSAIC,
        ...
    };
    isp_ae_ctlr_t ae_ctlr = NULL;
    ESP_ERROR_CHECK(esp_isp_new_ae_controller(isp_proc, &ae_config, &ae_ctlr));

使用上述句柄，可以启用/禁用 ISP AE 驱动程序，也可以设置 ISP AE 环境检测器。

安装 ISP 直方图 (HIST) 驱动程序
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ISP 直方图 (HIST) 驱动程序需要由 :cpp:type:`esp_isp_hist_config_t` 指定配置。

指定 :cpp:type:`esp_isp_hist_config_t` 中的配置后，可以调用 :cpp:func:`esp_isp_new_hist_controller` 来分配和初始化 ISP 直方图控制器。如果此函数运行正常，将返回一个 ISP HIST 控制器句柄。请参考以下代码。

.. list::

    - 所有子窗口权重的十进制值之和应为 256，否则统计数据将较小，并且整数值应为 0。
    - 所有 RGB 系数的十进制值之和应为 256，否则统计数据将较小，并且整数值应为 0。
    - segment_threshold 必须在 0~255 之间且按顺序排列。

.. code:: c

    esp_isp_hist_config_t hist_cfg = {
        .segment_threshold = {16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240},
        .hist_mode = ISP_HIST_SAMPLING_RGB,
        .rgb_coefficient.coeff_r = {
            .integer = 0,
            .decimal = 86,
        },
        .rgb_coefficient.coeff_g = {
            .integer = 0,
            .decimal = 85,
        },
        .rgb_coefficient.coeff_b = {
            .integer = 0,
            .decimal = 85,
        },
        .window_weight = {
            {{16, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}},
            {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}},
            {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}},
            {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}},
            {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}},
        },
    };
    isp_hist_ctlr_t hist_ctlr_ctlr = NULL;
    ESP_ERROR_CHECK(esp_isp_new_hist_controller(isp_proc, &hist_config, &hist_ctlr));

使用上述句柄，可以启用/禁用 ISP HIST 驱动程序的设置。

卸载 ISP 驱动程序
~~~~~~~~~~~~~~~~~

如果不再需要先前安装的 ISP 驱动程序，建议通过调用 API 来回收资源，并释放底层硬件：

.. list::

    - :cpp:func:`esp_isp_del_processor`，用于 ISP 核心处理器。
    - :cpp:func:`esp_isp_del_af_controller`，用于 ISP AF 控制器。
    - :cpp:func:`esp_isp_del_awb_controller`，用于 ISP AWB 控制器。
    - :cpp:func:`esp_isp_del_ae_controller`，用于 ISP AE 控制器。
    - :cpp:func:`esp_isp_del_hist_controller`，用于 ISP 直方图控制器。

.. _isp-enable-disable:

启用和禁用 ISP
^^^^^^^^^^^^^^

ISP
~~~

在进行 ISP 流水线操作之前，需要先调用 :cpp:func:`esp_isp_enable` 函数来启用 ISP 处理器。此函数：

* 将驱动程序状态从 **init** 切换到 **enable**。

调用 :cpp:func:`esp_isp_disable` 函数会执行相反的操作，即将驱动程序恢复到 **init** 状态。

ISP AF 控制器
~~~~~~~~~~~~~

在进行 ISP AF 操作之前，需要先调用 :cpp:func:`esp_isp_af_controller_enable` 函数来启用 ISP AF 控制器。此函数：

* 将驱动程序状态从 **init** 切换到 **enable**。

调用 :cpp:func:`esp_isp_af_controller_disable` 函数会执行相反的操作，即将驱动程序恢复到 **init** 状态。

.. _isp-af-statistics:

单次与连续 AF 数据统计
^^^^^^^^^^^^^^^^^^^^^^

调用 :cpp:func:`esp_isp_af_controller_get_oneshot_statistics` 可获取单次 AF 统计结果，请参考以下代码。

除此之外，ISP AF 驱动程序还可以连续获取 AF 统计信息。调用 :cpp:func:`esp_isp_af_controller_start_continuous_statistics` 可启动连续统计，调用 :cpp:func:`esp_isp_af_controller_stop_continuous_statistics` 可停止统计。

若想启用连续统计，需要先注册回调函数 :cpp:member:`esp_isp_af_env_detector_evt_cbs_t::on_env_statistics_done` 或 :cpp:member:`esp_isp_af_env_detector_evt_cbs_t::on_env_change` 以获取统计数据。有关如何注册回调函数，请参见 :ref:`isp-callback`。

.. note::

    使用连续统计时，AF 环境检测器将失效。

.. code-block:: c

    esp_isp_af_config_t af_config = {
        .edge_thresh = 128,
    };
    isp_af_ctlr_t af_ctrlr = NULL;
    ESP_ERROR_CHECK(esp_isp_new_af_controller(isp_proc, &af_config, &af_ctrlr));
    ESP_ERROR_CHECK(esp_isp_af_controller_enable(af_ctrlr));
    isp_af_result_t result = {};
    /* 触发单次 AF 统计并获取结果，超时时长为 2000 ms */
    ESP_ERROR_CHECK(esp_isp_af_controller_get_oneshot_statistics(af_ctrlr, 2000, &result));

    /* 启动连续 AF 数据统计 */
    ESP_ERROR_CHECK(esp_isp_af_controller_start_continuous_statistics(af_ctrlr));
    // 可在此进行其他操作，统计结果可从回调函数中获取
    // ......
    // vTaskDelay(pdMS_TO_TICKS(1000));
    /* 停止连续 AF 数据统计  */
    ESP_ERROR_CHECK(esp_isp_af_controller_stop_continuous_statistics(af_ctrlr));

    /* 禁用 AF 控制器 */
    ESP_ERROR_CHECK(esp_isp_af_controller_disable(af_ctrlr));
    /* 删除 AF 控制器并释放资源 */
    ESP_ERROR_CHECK(esp_isp_del_af_controller(af_ctrlr));

设置 AF 环境检测器
^^^^^^^^^^^^^^^^^^

调用 :cpp:func:`esp_isp_af_controller_set_env_detector` 来设置 ISP AF 环境检测器，请参考以下代码：

.. code-block:: c

    esp_isp_af_env_config_t env_config = {
        .interval = 10,
    };
    isp_af_ctlr_t af_ctrlr = NULL;
    ESP_ERROR_CHECK(esp_isp_new_af_controller(isp_proc, &af_config, &af_ctrlr));
    ESP_ERROR_CHECK(esp_isp_af_controller_set_env_detector(af_ctrlr, &env_config));

设置 AF 环境检测器阈值
^^^^^^^^^^^^^^^^^^^^^^

调用 :cpp:func:`esp_isp_af_controller_set_env_detector_threshold` 来设置 ISP AF 环境检测器的阈值。

.. code-block:: c

    int definition_thresh = 0;
    int luminance_thresh = 0;
    ESP_ERROR_CHECK(esp_isp_af_env_detector_set_threshold(env_detector, definition_thresh, luminance_thresh));

ISP AWB 控制器
~~~~~~~~~~~~~~

在进行 ISP AWB 操作之前，需要先调用 :cpp:func:`esp_isp_awb_controller_enable` 以启用 ISP AWB 控制器。此函数：

* 将驱动程序状态从 **init** 切换到 **enable**。

调用 :cpp:func:`esp_isp_awb_controller_disable` 函数会执行相反的操作，即将驱动程序恢复到 **init** 状态。

.. _isp-awb-statistics:

单次与连续 AWB 数据统计
^^^^^^^^^^^^^^^^^^^^^^^

调用 :cpp:func:`esp_isp_awb_controller_get_oneshot_statistics` 可获取单次 AWB 白块统计结果，请参考以下代码。

除此之外，ISP AWB 驱动程序还可以连续获取 AWB 统计信息。调用 :cpp:func:`esp_isp_awb_controller_start_continuous_statistics` 可启动连续统计，调用 :cpp:func:`esp_isp_awb_controller_stop_continuous_statistics` 可停止统计。

若想启用连续统计，需要先注册回调函数 :cpp:member:`esp_isp_awb_cbs_t::on_statistics_done` 以获取统计结果。有关如何注册回调函数，请参见 :ref:`isp-callback`。

.. code-block:: c

    bool example_isp_awb_on_statistics_done_cb(isp_awb_ctlr_t awb_ctlr, const esp_isp_awb_evt_data_t *edata, void *user_data);
    // ...
    isp_awb_ctlr_t awb_ctlr = NULL;
    uint32_t image_width = 800;
    uint32_t image_height = 600;
    /* AWB 配置，请参考 API 注释来调整参数 */
    esp_isp_awb_config_t awb_config = {
        .sample_point = ISP_AWB_SAMPLE_POINT_AFTER_CCM,
        ...
    };
    isp_awb_stat_result_t stat_res = {};
    /* 创建 AWB 控制器 */
    ESP_ERROR_CHECK(esp_isp_new_awb_controller(isp_proc, &awb_config, &awb_ctlr));
    /* 注册 AWB 回调函数 */
    esp_isp_awb_cbs_t awb_cb = {
        .on_statistics_done = example_isp_awb_on_statistics_done_cb,
    };
    ESP_ERROR_CHECK(esp_isp_awb_register_event_callbacks(awb_ctlr, &awb_cb, NULL));
    /* 启用 AWB 控制器 */
    ESP_ERROR_CHECK(esp_isp_awb_controller_enable(awb_ctlr));

    /* 获取单次 AWB 统计结果 */
    ESP_ERROR_CHECK(esp_isp_awb_controller_get_oneshot_statistics(awb_ctlr, -1, &stat_res));

    /* 启动连续 AWB 数据统计，注意在此之前需要先注册 `on_statistics_done` 回调函数 */
    ESP_ERROR_CHECK(esp_isp_awb_controller_start_continuous_statistics(awb_ctlr));
    // 可在此进行其他操作，统计结果可从回调函数中获取
    // ......
    // vTaskDelay(pdMS_TO_TICKS(1000));
    /* 停止连续 AWB 数据统计 */
    ESP_ERROR_CHECK(esp_isp_awb_controller_stop_continuous_statistics(awb_ctlr));

    /* 禁用 AWB 控制器 */
    ESP_ERROR_CHECK(esp_isp_awb_controller_disable(awb_ctlr));
    /* 删除 AWB 控制器并释放资源 */
    ESP_ERROR_CHECK(esp_isp_del_awb_controller(awb_ctlr));

ISP AE 控制器
~~~~~~~~~~~~~

在进行 ISP AE 操作之前，需要先调用 :cpp:func:`esp_isp_ae_controller_enable` 来启用 ISP AE 控制器。此函数：

* 将驱动程序状态从 **init** 切换到 **enable**。

调用 :cpp:func:`esp_isp_ae_controller_disable` 函数会执行相反的操作，即将驱动程序恢复到 **init** 状态。

.. _isp-ae-statistics:

单次与连续 AE 数据统计
^^^^^^^^^^^^^^^^^^^^^^

调用 :cpp:func:`esp_isp_ae_controller_get_oneshot_statistics` 可获取单次 AE 统计结果，请参考以下代码。

使用单次 AE 数据统计时，需要禁用连续 AE 模式，否则结果可能会被环境检测器覆盖。完成单次操作后，请重新启动连续模式。

除了上述单次统计 API 外，ISP AE 驱动程序还可以连续获取 AE 统计信息。调用 :cpp:member:`esp_isp_ae_env_detector_evt_cbs_t::on_env_statistics_done` 可启动连续统计，调用 :cpp:member:`esp_isp_ae_env_detector_evt_cbs_t::on_env_change` 可停止统计。

若想启用连续统计，需要先注册回调函数 :cpp:member:`esp_isp_ae_env_detector_evt_cbs_t::on_env_statistics_done` 或 :cpp:member:`esp_isp_ae_env_detector_evt_cbs_t::on_env_change` 以获取统计数据。有关如何注册回调函数，请参见 :ref:`isp-callback`。

.. note::

    使用单次统计时，AE 环境检测器将暂时失效，并在完成单次操作后自动恢复。

.. code-block:: c

     esp_isp_ae_config_t ae_config = {
        .sample_point = ISP_AE_SAMPLE_POINT_AFTER_DEMOSAIC,
    };
    isp_ae_ctlr_t ae_ctlr = NULL;
    ESP_ERROR_CHECK(esp_isp_new_ae_controller(isp_proc, &ae_config, &ae_ctlr));
    ESP_ERROR_CHECK(esp_isp_ae_controller_enable(ae_ctlr));
    isp_ae_result_t result = {};
    /* 触发单次 AE 统计并获取结果，超时时长为 2000 ms */
    ESP_ERROR_CHECK(esp_isp_ae_controller_get_oneshot_statistics(ae_ctlr, 2000, &result));

    /* 启动连续 AE 数据统计 */
    ESP_ERROR_CHECK(esp_isp_ae_controller_start_continuous_statistics(ae_ctlr));
    // 可在此进行其他操作，统计结果可从回调函数中获取
    // ......
    // vTaskDelay(pdMS_TO_TICKS(1000));
    /* 停止连续 AE 数据统计 */
    ESP_ERROR_CHECK(esp_isp_ae_controller_stop_continuous_statistics(ae_ctlr));

    /* 禁用 AE 控制器 */
    ESP_ERROR_CHECK(esp_isp_ae_controller_disable(ae_ctlr));
    /* 删除 AE 控制器并释放资源 */
    ESP_ERROR_CHECK(esp_isp_del_ae_controller(ae_ctlr));

设置 AE 环境检测器
^^^^^^^^^^^^^^^^^^

调用 :cpp:func:`esp_isp_ae_controller_set_env_detector` 来设置 ISP AE 环境检测器，请参考以下代码：

.. code:: c

    esp_isp_ae_env_config_t env_config = {
        .interval = 10,
    };
    ESP_ERROR_CHECK(esp_isp_ae_controller_set_env_detector(ae_ctlr, &env_config));

设置 AE 环境检测器阈值
^^^^^^^^^^^^^^^^^^^^^^

调用 :cpp:func:`esp_isp_ae_controller_set_env_detector_threshold` 来设置 ISP AE 环境检测器的阈值 (1-255)。

.. code:: c

    esp_isp_ae_env_thresh_t env_thresh = {
        .low_thresh = 110,
        .high_thresh = 130,
    };
    ESP_ERROR_CHECK(esp_isp_ae_controller_set_env_detector_threshold(ae_ctlr, env_thresh));

.. _isp-hist:

ISP 直方图控制器
~~~~~~~~~~~~~~~~

在进行 ISP 直方图统计之前，需要先调用 :cpp:func:`esp_isp_hist_controller_enable` 以启用 ISP 直方图控制器。此函数：

* 将驱动程序状态从 **init** 切换到 **enable**。

调用 :cpp:func:`esp_isp_hist_controller_disable` 函数会执行相反的操作，即将驱动程序恢复到 **init** 状态。

.. _isp-hist-statistics:

单次与连续直方图数据统计
^^^^^^^^^^^^^^^^^^^^^^^^

调用 :cpp:func:`esp_isp_hist_controller_get_oneshot_statistics` 可获取单次直方图统计结果，请参考以下代码。

除此之外，ISP 直方图驱动程序还可以连续获取直方图统计信息。调用 :cpp:func:`esp_isp_hist_controller_start_continuous_statistics` 可启动连续统计，调用 :cpp:func:`esp_isp_hist_controller_stop_continuous_statistics` 可停止连续统计。

若想启用连续统计，需要先注册回调函数 :cpp:member:`esp_isp_hist_cbs_t::on_statistics_done` 以获取统计结果。有关如何注册回调函数，请参见 :ref:`isp-callback`。

.. code:: c

    static bool s_hist_scheme_on_statistics_done_callback(isp_hist_ctlr_t awb_ctrlr, const esp_isp_hist_evt_data_t *edata, void *user_data)
    {
        for(int i = 0; i < 16; i++) {
            esp_rom_printf(DRAM_STR("val %d is %x\n"), i, edata->hist_result.hist_value[i]); // 获取直方图统计值
        }
        return true;
    }

    esp_isp_hist_cbs_t hist_cbs = {
        .on_statistics_done = s_hist_scheme_on_statistics_done_callback,
    };

    esp_isp_hist_register_event_callbacks(hist_ctlr, &hist_cbs, hist_ctlr);
    esp_isp_hist_controller_enable(hist_ctlr);


.. _isp-bf:

ISP BF 控制器
~~~~~~~~~~~~~

此流水线用于在拜耳模式下进行图像输入降噪。

可调用 :cpp:func:`esp_isp_bf_configure` 函数配置 BF 功能，请参考以下代码：

.. code-block:: c

    esp_isp_bf_config_t bf_config = {
        .denoising_level = 5,
        .bf_template = {
            {1, 2, 1},
            {2, 4, 2},
            {1, 2, 1},
        },
        ...
    };
    ESP_ERROR_CHECK(esp_isp_bf_configure(isp_proc, &bf_config));
    ESP_ERROR_CHECK(esp_isp_bf_enable(isp_proc));

:cpp:member:`esp_isp_bf_config_t::bf_template` 用于拜耳域降噪。可以通过高斯滤波器模板或均值滤波器模板来设置 :cpp:member:`esp_isp_bf_config_t::bf_template`。

调用 :cpp:func:`esp_isp_bf_configure` 后，需要通过调用 :cpp:func:`esp_isp_bf_enable` 来启用 ISP BF 控制器。此函数：

* 将驱动程序状态从 **init** 切换到 **enable**。

调用 :cpp:func:`esp_isp_bf_disable` 函数会执行相反的操作，即将驱动程序恢复到 **init** 状态。


.. _isp-lsc:

ISP LSC 控制器
~~~~~~~~~~~~~~

镜头阴影校正 (LSC) 旨在解决因相机镜头中光线折射不均而引起的问题。

可调用 :cpp:func:`esp_isp_lsc_configure` 函数配置 LSC 模块以进行校正。硬件进行校正相关计算时需要用到 :cpp:type:`esp_isp_lsc_gain_array_t` 类型的数据结构。:cpp:func:`esp_isp_lsc_allocate_gain_array` 是一个辅助函数，为增益值分配大小合适的系统存储。

.. code-block:: c

    esp_isp_lsc_gain_array_t gain_array = {};
    size_t gain_size = 0;
    ESP_ERROR_CHECK(esp_isp_lsc_allocate_gain_array(isp_proc, &gain_array, &gain_size));

    esp_isp_lsc_config_t lsc_config = {
        .gain_array = &gain_array,
    };
    isp_lsc_gain_t gain_val = {
        .decimal = 204,
        .integer = 0,
    };
    for (int i = 0; i < gain_size; i++) {
        gain_array.gain_r[i].val = gain_val.val;
        gain_array.gain_gr[i].val = gain_val.val;
        gain_array.gain_gb[i].val = gain_val.val;
        gain_array.gain_b[i].val = gain_val.val;
    }
    ESP_ERROR_CHECK(esp_isp_lsc_configure(isp_proc, &lsc_config));

调用 :cpp:func:`esp_isp_lsc_configure` 后，需要通过调用 :cpp:func:`esp_isp_lsc_enable` 来启用 ISP LSC 控制器。可以通过调用 :cpp:func:`esp_isp_lsc_disable` 来禁用 LSC。此外，即使未启用 LSC 控制器，也可以调用 :cpp:func:`esp_isp_lsc_configure`，但 LSC 功能仅在启用后才会生效。


.. _isp-color:

ISP 色彩控制器
~~~~~~~~~~~~~~

该流水线用于调整图像的对比度、饱和度、色调和亮度。

可调用 :cpp:func:`esp_isp_color_configure` 函数配置色彩功能，请参考以下代码。

{IDF_TARGET_SOC_ISP_COLOR_CONTRAST_MAX:default="1.0", esp32p4="1.0"}
{IDF_TARGET_SOC_ISP_COLOR_CONTRAST_DEFAULT:default="1.0", esp32p4="1.0"}

{IDF_TARGET_SOC_ISP_COLOR_SATURATION_MAX:default="1.0", esp32p4="1.0"}
{IDF_TARGET_SOC_ISP_COLOR_SATURATION_DEFAULT:default="1.0", esp32p4="1.0"}

{IDF_TARGET_SOC_ISP_COLOR_HUE_MAX:default="360", esp32p4="360"}
{IDF_TARGET_SOC_ISP_COLOR_HUE_DEFAULT:default="0", esp32p4="0"}

{IDF_TARGET_SOC_ISP_COLOR_BRIGHTNESS_MIN:default="-127", esp32p4="-127"}
{IDF_TARGET_SOC_ISP_COLOR_BRIGHTNESS_MAX:default="128", esp32p4="128"}
{IDF_TARGET_SOC_ISP_COLOR_BRIGHTNESS_DEFAULT:default="0", esp32p4="0"}

.. list::

    - 对比度应为 0 ~ {IDF_TARGET_SOC_ISP_COLOR_CONTRAST_MAX}，默认值为 {IDF_TARGET_SOC_ISP_COLOR_CONTRAST_DEFAULT}
    - 饱和度应为 0 ~ {IDF_TARGET_SOC_ISP_COLOR_SATURATION_MAX}，默认值为 {IDF_TARGET_SOC_ISP_COLOR_SATURATION_DEFAULT}
    - 色调应为 0 ~ {IDF_TARGET_SOC_ISP_COLOR_HUE_MAX}，默认值为 {IDF_TARGET_SOC_ISP_COLOR_HUE_DEFAULT}
    - 亮度应为 {IDF_TARGET_SOC_ISP_COLOR_BRIGHTNESS_MIN} ~ {IDF_TARGET_SOC_ISP_COLOR_BRIGHTNESS_MAX}，默认值为 {IDF_TARGET_SOC_ISP_COLOR_BRIGHTNESS_DEFAULT}

.. code:: c

    esp_isp_color_config_t color_config = {
        .color_contrast = {
            .integer = 1,
            .decimal = 0,
        },
        .color_saturation = {
            .integer = 1,
            .decimal = 0,
        },
        .color_hue = 0,
        .color_brightness = 0,
    };
    ESP_ERROR_CHECK(esp_isp_color_configure(isp_proc, &color_config));
    ESP_ERROR_CHECK(esp_isp_color_enable(isp_proc));

调用 :cpp:func:`esp_isp_color_configure` 后，需要通过调用 :cpp:func:`esp_isp_color_enable` 来启用 ISP 色彩控制器。此函数：

* 将驱动程序状态从 **init** 切换为 **enable**。

调用 :cpp:func:`esp_isp_color_disable` 函数会执行相反的操作，即将驱动程序恢复到 **init** 状态。

.. note::

    当 ISP DVP 外设在使用且输出颜色格式设置为 RGB 色彩空间时，摄像头驱动程序会自动启用 :ref:`isp-color` 以确保数据输出正确。在这种情况下，禁止调用 :cpp:func:`esp_isp_color_disable` 函数，否则可能导致摄像头数据混乱。

.. _isp-ccm-config:

配置 CCM
^^^^^^^^

色彩校正矩阵可以调整 RGB888 像素格式的颜色比例，可用于通过算法调整图像颜色（例如，使用 AWB 计算结果进行白平衡），或者通过滤波算法用作过滤器。

调整色彩校正矩阵的公式如下：

.. code-block:: none

    [ R' ]     [ RR  RG  RB  ]   [ R ]
    [ G' ] =   [ GR  GG  GB  ] * [ G ]
    [ B' ]     [ BR  BG  BB  ]   [ B ]

可以参考以下代码进行配置：

.. code-block:: c

    // ...
    // 配置 CCM
    esp_isp_ccm_config_t ccm_cfg = {
        .matrix = {
            1.0, 0.0, 0.0,
            0.0, 1.0, 0.0,
            0.0, 0.0, 1.0
        },
        .saturation = false,
        ...
    };
    ESP_ERROR_CHECK(esp_isp_ccm_configure(isp_proc, &ccm_cfg));
    // 启用 CCM 模块后，配置好的 CCM 将应用到图像上
    ESP_ERROR_CHECK(esp_isp_ccm_enable(isp_proc));
    // CCM 也可以在启用后进行配置
    ccm_cfg.matrix[0][0] = 2.0;
    ESP_ERROR_CHECK(esp_isp_ccm_configure(isp_proc, &ccm_cfg));
    // 如果不再需要 CCM，则禁用它
    ESP_ERROR_CHECK(esp_isp_ccm_disable(isp_proc));

.. _isp-demosaic:

ISP 去马赛克控制器
~~~~~~~~~~~~~~~~~~~~~~

此流水线用于执行图像去马赛克算法，将 RAW 图像转换为 RGB 模式。

可调用 :cpp:func:`esp_isp_demosaic_configure` 来配置去马赛克功能，请参考以下代码：

.. code:: c

    esp_isp_demosaic_config_t demosaic_config = {
        .grad_ratio = {
            .integer = 2,
            .decimal = 5,
        },
        ...
    };

    ESP_ERROR_CHECK(esp_isp_demosaic_configure(isp_proc, &sharpen_config));
    ESP_ERROR_CHECK(esp_isp_demosaic_enable(isp_proc));

调用 :cpp:func:`esp_isp_demosaic_configure` 后，需要通过调用 :cpp:func:`esp_isp_demosaic_enable` 来启用 ISP 去马赛克控制器。此函数：

* 将驱动程序状态从 **init** 切换到 **enable**。

调用 :cpp:func:`esp_isp_demosaic_disable` 会执行相反的操作，即将驱动程序恢复到 **init** 状态。

即使驱动程序处于 **init** 状态，也可以调用 :cpp:func:`esp_isp_demosaic_configure`，但去马赛克配置只有在 **enable** 状态下才会生效。

.. _isp-gamma-correction:

启用 gamma 校正
^^^^^^^^^^^^^^^

人眼的视觉系统对物理亮度的感知是非线性的。将 gamma 校正添加到 ISP 流水线中，可以将 RGB 坐标转换为坐标与主观亮度成正比的空间。

驱动程序提供了帮助函数 :cpp:func:`esp_isp_gamma_fill_curve_points`，用于填充 :cpp:type:`isp_gamma_curve_points_t`，这是描述 gamma 校正曲线的点集合。也可以通过手动声明点来获得期望的 gamma 校正曲线。每个 R/G/B 分量有自己的 gamma 校正曲线，可以通过调用 :cpp:func:`esp_isp_gamma_configure` 来配置。

以下是一个典型的代码示例：

.. code:: c

    #include <math.h>

    // 设置相机 gamma 为 0.7，gamma 校正曲线为 y = 256 * (x / 256) ^ 0.7
    static uint32_t s_gamma_curve(uint32_t x)
    {
        return pow((double)x / 256, 0.7) * 256;
    }

    isp_gamma_curve_points_t pts = {};
    ESP_ERROR_CHECK(esp_isp_gamma_fill_curve_points(s_gamma_curve, &pts));
    ESP_ERROR_CHECK(esp_isp_gamma_configure(isp_proc, COLOR_COMPONENT_R, &pts));
    ESP_ERROR_CHECK(esp_isp_gamma_configure(isp_proc, COLOR_COMPONENT_G, &pts));
    ESP_ERROR_CHECK(esp_isp_gamma_configure(isp_proc, COLOR_COMPONENT_B, &pts));

    // 配置完曲线参数后启用 gamma 模块
    ESP_ERROR_CHECK(esp_isp_gamma_enable(isp_proc));

    // 如果不再需要，则禁用 gamma
    ESP_ERROR_CHECK(esp_isp_gamma_disable(isp_proc));

.. _isp-sharpen:

ISP 锐化控制器
~~~~~~~~~~~~~~

此流水线用于在 YUV 模式下锐化输入图像。

调用 :cpp:func:`esp_isp_sharpen_configure` 来配置锐化功能，请参考以下代码。

.. code:: c

    esp_isp_sharpen_config_t sharpen_config = {
        .h_thresh = 255,
        .sharpen_template = {
            {1, 2, 1},
            {2, 4, 2},
            {1, 2, 1},
        },
        ...
    };
    ESP_ERROR_CHECK(esp_isp_sharpen_configure(isp_proc, &sharpen_config));
    ESP_ERROR_CHECK(esp_isp_sharpen_enable(isp_proc));

调用 :cpp:member:`esp_isp_sharpen_config_t::sharpen_template` 进行锐化。可以通过高斯滤波器模板或均值滤波器模板来设置  :cpp:member:`esp_isp_sharpen_config_t::sharpen_template`。

调用 :cpp:func:`esp_isp_sharpen_configure` 后，需要通过调用 :cpp:func:`esp_isp_sharpen_enable` 以启用 ISP 锐化控制器。此函数：

* 将驱动程序状态从 **init** 切换到 **enable**。

调用 :cpp:func:`esp_isp_sharpen_disable` 函数会执行相反的操作，即将驱动程序恢复到 **init** 状态。

即使驱动程序处于 **init** 状态，也可以调用 :cpp:func:`esp_isp_sharpen_configure`，但锐化配置只有在 **enable** 状态下才会生效。


.. _isp-callback:

注册事件回调函数
^^^^^^^^^^^^^^^^

ISP 模块启动后，会动态生成特定事件。

你也可以通过参数 ``user_data`` 将自己的上下文保存到回调函数中，用户数据将直接传递给回调函数。

.. note::

    下文中提到的回调函数在 ISR 上下文中被调用，必须确保这些函数不会尝试阻塞（例如，确保只从函数中调用带有 ``ISR`` 后缀的 FreeRTOS API）。

注册 ISP 处理器事件回调函数
~~~~~~~~~~~~~~~~~~~~~~~~~~~

启用 ISP 处理器后，会动态生成多个 ISP 子模块的事件。可以通过调用 :cpp:func:`esp_isp_register_event_callbacks` 将函数挂接到中断服务例程。所有支持的事件回调函数可参见 :cpp:type:`esp_isp_evt_cbs_t`：

- :cpp:member:`esp_isp_evt_cbs_t::on_sharpen_frame_done` 在完成锐化帧后设置回调函数。ISP 锐化子模块完成一帧的操作后会调用此函数。函数原型在 :cpp:type:`esp_isp_sharpen_callback_t` 中声明。

注册 ISP AF 环境检测器事件回调函数
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ISP AF 环境检测器启动后，将动态生成特定事件。若想在事件发生时调用某些函数，请通过调用 :cpp:func:`esp_isp_af_env_detector_register_event_callbacks` 将目标函数挂接到中断服务程序中。所有支持的事件回调函数可参见 :cpp:type:`esp_isp_af_env_detector_evt_cbs_t`：

- :cpp:member:`esp_isp_af_env_detector_evt_cbs_t::on_env_statistics_done` 为环境统计完成事件设置回调函数。该函数原型在 :cpp:type:`esp_isp_af_env_detector_callback_t` 中声明。
- :cpp:member:`esp_isp_af_env_detector_evt_cbs_t::on_env_change` 为环境变化事件设置回调函数。该函数原型在 :cpp:type:`esp_isp_af_env_detector_callback_t` 中声明。

注册 ISP AWB 统计完成事件回调函数
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ISP AWB 控制器完成白块数据统计后，将动态生成特定事件。若想在统计完成时收到通知，请通过调用 :cpp:func:`esp_isp_awb_register_event_callbacks` 将目标函数挂接到中断服务程序中。所有支持的事件回调函数可参见 :cpp:type:`esp_isp_awb_cbs_t`：

- :cpp:member:`esp_isp_awb_cbs_t::on_statistics_done` 在白块数据统计完成后设置回调函数。该函数原型在 :cpp:type:`esp_isp_awb_callback_t` 中声明。


注册 ISP AE 环境检测器事件回调函数
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ISP AE 环境检测器启动后，将动态生成特定事件。若想在事件发生时调用某些函数，请通过调用 :cpp:func:`esp_isp_ae_env_detector_register_event_callbacks` 将目标函数挂接到中断服务程序中。所有支持的事件回调函数可参见 :cpp:type:`esp_isp_ae_env_detector_evt_cbs_t`：

- :cpp:member:`esp_isp_ae_env_detector_evt_cbs_t::on_env_statistics_done` 为环境统计完成事件设置回调函数。该函数原型在 :cpp:type:`esp_isp_ae_env_detector_callback_t` 中声明。
- :cpp:member:`esp_isp_ae_env_detector_evt_cbs_t::on_env_change` 为环境变化事件设置回调函数。该函数原型在 :cpp:type:`esp_isp_ae_env_detector_callback_t` 中声明。


注册 ISP HIST 统计完成事件回调函数
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ISP HIST 控制器完成亮度统计后，将动态生成特定事件。若想在统计完成时收到通知，请通过调用 :cpp:func:`esp_isp_hist_register_event_callbacks` 将目标函数挂挂接到中断服务程序。所有支持的事件回调函数可参见 :cpp:type:`esp_isp_hist_cbs_t`：

- :cpp:member:`esp_isp_hist_cbs_t::on_statistics_done` 在完成亮度统计时设置回调函数。该函数原型在 :cpp:type:`esp_isp_hist_callback_t` 中声明。

.. _isp-thread-safety:

线程安全
^^^^^^^^

驱动程序会确保以下工厂函数的线程安全：

.. list::

    - :cpp:func:`esp_isp_new_processor`
    - :cpp:func:`esp_isp_del_processor`
    - :cpp:func:`esp_isp_new_af_controller`
    - :cpp:func:`esp_isp_del_af_controller`
    - :cpp:func:`esp_isp_new_awb_controller`
    - :cpp:func:`esp_isp_del_awb_controller`
    - :cpp:func:`esp_isp_new_ae_controller`
    - :cpp:func:`esp_isp_del_ae_controller`
    - :cpp:func:`esp_isp_new_hist_controller`
    - :cpp:func:`esp_isp_del_hist_controller`

使用时，可以直接从不同的 RTOS 任务中调用此类函数，无需额外锁保护。其他 API 无法确保线程安全。

.. _isp-kconfig-options:

Kconfig 选项
^^^^^^^^^^^^

- :ref:`CONFIG_ISP_ISR_IRAM_SAFE` 控制默认的 ISR 句柄在 cache 被禁用时是否可以正常工作。

.. _isp-iram-safe:

IRAM 安全
^^^^^^^^^

默认情况下，当 cache 因写入或擦除 flash 等原因而被禁用时，ISP 的中断将会延迟。

Kconfig 选项 :ref:`CONFIG_ISP_ISR_IRAM_SAFE` 支持：

- 即使 cache 被禁用也能启用中断
- 将 ISR 使用的所有函数放入 IRAM
- 将驱动程序对象放入 DRAM（以防意外映射到 PSRAM）

启用上述 Kconfig 选项，保证 cache 被禁用时中断可以正常运行，但这会增加 IRAM 使用量。启用此选项后，当 cache 被禁用时，ISR 回调函数将继续运行。因此，必须确保回调函数及其上下文也是 IRAM 安全的。

Kconfig 选项 :ref:`CONFIG_ISP_CTRL_FUNC_IN_IRAM` 支持：

- 将一些 ISP 控制函数放入 IRAM，函数列表请参见：

    .. list::

        - :cpp:func:`esp_isp_sharpen_configure`
        - :cpp:func:`esp_isp_demosaic_configure`

应用示例
--------

* :example:`peripherals/isp/multi_pipelines` 演示了如何使用 ISP 流水线处理来自摄像头传感器的图像信号，并通过 DSI 外设在 LCD 屏幕上显示视频。

API 参考
--------

.. include-build-file:: inc/isp.inc
.. include-build-file:: inc/isp_af.inc
.. include-build-file:: inc/isp_ae.inc
.. include-build-file:: inc/isp_awb.inc
.. include-build-file:: inc/isp_bf.inc
.. include-build-file:: inc/isp_lsc.inc
.. include-build-file:: inc/isp_ccm.inc
.. include-build-file:: inc/isp_demosaic.inc
.. include-build-file:: inc/isp_sharpen.inc
.. include-build-file:: inc/isp_gamma.inc
.. include-build-file:: inc/isp_hist.inc
.. include-build-file:: inc/isp_color.inc
.. include-build-file:: inc/isp_core.inc
.. include-build-file:: inc/components/esp_driver_isp/include/driver/isp_types.inc
.. include-build-file:: inc/components/hal/include/hal/isp_types.inc
