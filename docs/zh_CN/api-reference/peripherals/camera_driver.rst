摄像头控制器驱动程序
====================

:link_to_translation:`en:[English]`

简介
----

{IDF_TARGET_NAME} 具有以下硬件，用于与外部摄像头传感器通信：

.. list::

    :SOC_MIPI_CSI_SUPPORTED: - MIPI 摄像头串行接口 (MIPI CSI)
    :SOC_ISP_DVP_SUPPORTED: - ISP的DVP端口 (ISP DVP)
    :SOC_LCDCAM_CAM_SUPPORTED: - LCD_CAM的DVP端口 (LCD_CAM DVP)

摄像头控制器驱动程序是为上述硬件外设而设计的。

功能概述
------------

.. list::

    - :ref:`cam-resource-allocation` - 涵盖如何通过恰当的配置来分配摄像头控制器实例，以及如何回收资源。
    - :ref:`cam-enable-disable` - 涵盖如何启用和禁用摄像头控制器。
    - :ref:`cam-start-stop` - 涵盖如何启动和停止摄像头控制器。
    - :ref:`cam-receive` - 涵盖如何从传感器或其他设备接收摄像头信号。
    - :ref:`cam-callback` - 涵盖如何将用户特定代码挂接到摄像头控制器驱动程序事件回调函数。
    - :ref:`cam-thread-safety` - 列出了驱动程序中线程安全的 API。
    - :ref:`cam-kconfig-options` - 列出了支持的 Kconfig 选项，这些选项可以对驱动程序产生不同的影响。
    - :ref:`cam-iram-safe` - 描述了当 cache 被禁用时，如何使 CSI 中断和控制功能更好地工作。

.. _cam-resource-allocation:

资源分配
^^^^^^^^

安装摄像头控制器驱动程序
~~~~~~~~~~~~~~~~~~~~~~~~

摄像头控制器驱动程序可以通过以下方式之一安装：

.. list::

    :SOC_MIPI_CSI_SUPPORTED: - :cpp:func:`esp_cam_new_csi_ctlr`
    :SOC_ISP_DVP_SUPPORTED: - :cpp:func:`esp_cam_new_isp_dvp_ctlr`
    :SOC_LCDCAM_CAM_SUPPORTED: - :cpp:func:`esp_cam_new_lcd_cam_ctlr`

.. only:: SOC_MIPI_CSI_SUPPORTED

    摄像头控制器驱动程序可以通过 CSI 外设实现，需要应用 :cpp:type:`esp_cam_ctlr_csi_config_t` 指定的配置。

    如果指定了 :cpp:type:`esp_cam_ctlr_csi_config_t` 中的配置，就可以调用 :cpp:func:`esp_cam_new_csi_ctlr` 来分配和初始化 CSI 摄像头控制器句柄。如果函数运行正确，将返回一个 CSI 摄像头控制器句柄。请参考以下代码。

    .. code:: c

        #include "esp_cam_ctlr.h"
        #include "esp_cam_ctlr_types.h"
        #include "esp_cam_ctlr_csi.h"

        void app_main(void)
        {
            esp_cam_ctlr_csi_config_t csi_config = {
                .ctlr_id = 0,
                .h_res = MIPI_CSI_DISP_HSIZE,
                .v_res = MIPI_CSI_DISP_VSIZE_640P,
                .lane_bit_rate_mbps = MIPI_CSI_LANE_BITRATE_MBPS,
                .input_data_color_type = CAM_CTLR_COLOR_RAW8,
                .output_data_color_type = CAM_CTLR_COLOR_RGB565,
                .data_lane_num = 2,
                .byte_swap_en = false,
                .queue_items = 1,
            };

            esp_cam_ctlr_handle_t handle = NULL;
            ESP_ERROR_CHECK(esp_cam_new_csi_ctlr(&csi_config, &handle));
        }

    {IDF_TARGET_NAME} 中的 CSI 控制器需要稳定的 2.5 V 电源供电，请查阅原理图，确保在使用 MIPI CSI 驱动之前，已将其供电管脚连接至 2.5 V 电源。

    .. only:: SOC_GP_LDO_SUPPORTED

        在 {IDF_TARGET_NAME} 中，CSI 控制器可以使用内部的可调 LDO 供电。请将 LDO 通道的输出管脚连接至 CSI 控制器的供电管脚。然后在初始化 CSI 驱动之前，使用 :doc:`/api-reference/peripherals/ldo_regulator` 中提供的 API 配置 LDO 输出 2.5 V 电压。

.. only:: SOC_ISP_DVP_SUPPORTED

    摄像头控制器驱动程序可以通过 ISP 外设实现，需要应用 :cpp:type:`esp_cam_ctlr_isp_dvp_cfg_t` 指定的配置。

    如果指定了 :cpp:type:`esp_cam_ctlr_isp_dvp_cfg_t` 中的配置，就可以调用 :cpp:func:`esp_cam_new_isp_dvp_ctlr` 来分配和初始化 ISP DVP 摄像头控制器句柄。如果函数运行正确，将返回一个 ISP DVP 摄像头控制器句柄。请参考以下代码。

    在调用 :cpp:func:`esp_cam_new_isp_dvp_ctlr` 之前，还应调用 :cpp:func:`esp_isp_new_processor` 来创建 ISP 句柄。

    .. code:: c

        #include "esp_err.h"
        #include "esp_cam_ctlr.h"
        #include "esp_cam_ctlr_isp_dvp.h"
        #include "driver/isp.h"

        #define MIPI_CSI_DISP_HSIZE   800   // example value, replace with actual resolution
        #define MIPI_CSI_DISP_VSIZE   600   // example value, replace with actual resolution

        void app_main(void)
        {
            isp_proc_handle_t isp_proc = NULL;
            esp_isp_processor_cfg_t isp_config = {
                .clk_hz = 120 * 1000 * 1000,
                .input_data_source = ISP_INPUT_DATA_SOURCE_DVP,
                .input_data_color_type = ISP_COLOR_RAW8,
                .output_data_color_type = ISP_COLOR_RGB565,
                .has_line_start_packet = false,
                .has_line_end_packet = false,
                .h_res = MIPI_CSI_DISP_HSIZE,
                .v_res = MIPI_CSI_DISP_VSIZE,
            };
            ESP_ERROR_CHECK(esp_isp_new_processor(&isp_config, &isp_proc));

            esp_cam_ctlr_handle_t cam_handle = NULL;
            esp_cam_ctlr_isp_dvp_cfg_t dvp_ctlr_config = {
                .data_width = 8,
                .data_io = {53, 54, 52, 0, 1, 45, 46, 47, -1, -1, -1, -1, -1, -1, -1, -1},
                .pclk_io = 21,
                .hsync_io = 5,
                .vsync_io = 23,
                .de_io = 22,
                .io_flags.vsync_invert = 1,
                .queue_items = 10,
            };
            ESP_ERROR_CHECK(esp_cam_new_isp_dvp_ctlr(isp_proc, &dvp_ctlr_config, &cam_handle));
        }

.. only:: SOC_LCDCAM_CAM_SUPPORTED

    摄像头控制器驱动程序可以通过 LCD_CAM外设实现，需要应用 :cpp:type:`esp_cam_ctlr_dvp_config_t` 和 :cpp:type:`esp_cam_ctlr_dvp_pin_config_t` 指定的配置。

    :cpp:member:`esp_cam_ctlr_dvp_config_t::exexternal_xtal`：使用外部生成的 xclk，或者使用驱动内部内部生成的 xclk。

    如果指定了 :cpp:type:`esp_cam_ctlr_dvp_config_t` 中的配置，就可以调用 :cpp:func:`esp_cam_new_dvp_ctlr` 来分配和初始化 DVP 摄像头控制器句柄。如果函数运行正确，将返回一个 DVP 摄像头控制器句柄。请参考以下代码。

    在调用 :cpp:func:`esp_cam_new_dvp_ctlr` 之后，需要分配符合对齐约束的摄像头缓冲区，或调用 :cpp:func:`esp_cam_ctlr_alloc_buffer` 来自动分配。

    可以调用 :cpp:func:`esp_cam_ctlr_format_conversion` 来配置格式转换。驱动程序支持以下转换类型：

    * YUV 到 RGB 转换
    * RGB 到 YUV 转换
    * YUV 到 YUV 转换

    色彩空间范围支持：
    * 全色彩空间：RGB 和 YUV 的取值范围为 0-255
    * 有限色彩空间：RGB 取值范围为 16-240，YUV Y 分量取值范围为 16-240，U-V 分量取值范围为 16-235

    .. code:: c

        #include "esp_err.h"
        #include "esp_cam_ctlr.h"
        #include "esp_cam_ctlr_types.h"
        #include "esp_cam_ctlr_isp_dvp.h"

        void app_main(void)
        {
            esp_cam_ctlr_handle_t cam_handle = NULL;

            esp_cam_ctlr_dvp_pin_config_t pin_cfg = {
                .data_width = EXAMPLE_DVP_CAM_DATA_WIDTH,
                .data_io = {
                    EXAMPLE_DVP_CAM_D0_IO,
                    EXAMPLE_DVP_CAM_D1_IO,
                    EXAMPLE_DVP_CAM_D2_IO,
                    EXAMPLE_DVP_CAM_D3_IO,
                    EXAMPLE_DVP_CAM_D4_IO,
                    EXAMPLE_DVP_CAM_D5_IO,
                    EXAMPLE_DVP_CAM_D6_IO,
                    EXAMPLE_DVP_CAM_D7_IO,
                },
                .vsync_io = EXAMPLE_DVP_CAM_VSYNC_IO,
                .de_io = EXAMPLE_DVP_CAM_DE_IO,
                .pclk_io = EXAMPLE_DVP_CAM_PCLK_IO,
                .xclk_io = EXAMPLE_DVP_CAM_XCLK_IO, // Set XCLK pin to generate XCLK signal
            };

            esp_cam_ctlr_dvp_config_t dvp_config = {
                .ctlr_id = 0,
                .clk_src = CAM_CLK_SRC_DEFAULT,
                .h_res = CONFIG_EXAMPLE_CAM_HRES,
                .v_res = CONFIG_EXAMPLE_CAM_VRES,
                .input_data_color_type = CAM_CTLR_COLOR_RGB565,
                .output_data_color_type = CAM_CTLR_COLOR_RGB565,
                .dma_burst_size = 128,
                .pin = &pin_cfg,
                .bk_buffer_dis = 1,
                .xclk_freq = EXAMPLE_DVP_CAM_XCLK_FREQ_HZ,
            };

            ESP_ERROR_CHECK(esp_cam_new_dvp_ctlr(&dvp_config, &cam_handle));
        }

卸载摄像头控制器驱动程序
~~~~~~~~~~~~~~~~~~~~~~~~

如果不再需要先前安装的摄像头控制器驱动程序，建议通过调用 :cpp:func:`esp_cam_ctlr_del` 来回收资源，从而释放底层硬件。

.. _cam-enable-disable:

启用和禁用摄像头控制器驱动程序
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

在开始摄像头控制器操作之前，首先要调用 :cpp:func:`esp_cam_ctlr_enable` 以启用摄像头控制器驱动程序。此函数将驱动程序状态从 **init** 切换到 **enable**。

.. code:: c

        #include "esp_cam_ctlr.h"
        #include "esp_cam_ctlr_types.h"
        #include "esp_err.h"

        void app_main(void)
        {
            esp_cam_ctlr_handle_t handle;
            ESP_ERROR_CHECK(esp_cam_ctlr_enable(handle));
        }

调用 :cpp:func:`esp_cam_ctlr_disable` 则会执行与上述过程相反的操作，即将驱动程序切回到 **init** 状态。

.. code:: c

        #include "esp_err.h"
        #include "esp_cam_ctlr.h"
        #include "esp_cam_ctlr_types.h"

        void app_main(void)
        {
            esp_cam_ctlr_handle_t handle;
            ESP_ERROR_CHECK(esp_cam_ctlr_disable(handle));
        }

.. _cam-start-stop:

启动和停止摄像头控制器驱动程序
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

从摄像头传感器接收信号之前，首先要调用 :cpp:func:`esp_cam_ctlr_start` 以启动摄像头控制器驱动程序。此函数将驱动程序状态从 **enable** 切换到 **start**。

.. code:: c

        #include "esp_err.h"
        #include "esp_log.h"
        #include "esp_cam_ctlr.h"
        #include "esp_cam_ctlr_types.h"

        void app_main(void)
        {
            esp_cam_ctlr_handle_t handle = NULL;
            ESP_ERROR_CHECK(esp_cam_ctlr_start(handle));
            ESP_LOGI("CAM", "Camera controller started successfully");
        }

调用 :cpp:func:`esp_cam_ctlr_stop` 则会执行与上述过程相反的操作，即将驱动程序切回到 **enable** 状态。

.. code:: c

        #include "esp_err.h"
        #include "esp_cam_ctlr.h"
        #include "esp_cam_ctlr_types.h"

        void app_main(void)
        {
            esp_cam_ctlr_handle_t handle = NULL;
            ESP_ERROR_CHECK(esp_cam_ctlr_stop(handle));
        }

.. _cam-receive:

从摄像头传感器处接收信号
^^^^^^^^^^^^^^^^^^^^^^^^

调用 :cpp:func:`esp_cam_ctlr_receive`，可以接收来自摄像头传感器或其他设备的信号。

.. code:: c

    #include "esp_err.h"
    #include "esp_cam_ctlr.h"
    #include "esp_cam_ctlr_types.h"

    ESP_ERROR_CHECK(esp_cam_ctlr_receive(handle, &my_trans, ESP_CAM_CTLR_MAX_DELAY));

.. _cam-callback:

注册事件回调函数
^^^^^^^^^^^^^^^^

摄像头控制器驱动程序开始接收信号时，会动态生成特定事件。如果在事件发生时需要调用一些函数，请通过调用 :cpp:func:`esp_cam_ctlr_register_event_callbacks` 将这些函数挂接到中断服务程序。所有支持的事件回调函数参见 :cpp:type:`esp_cam_ctlr_evt_cbs_t`：

- :cpp:member:`esp_cam_ctlr_evt_cbs_t::on_get_new_trans` 可设置回调函数，当摄像头控制器驱动程序完成传输并尝试获取新的事务描述符时，该回调函数会被调用。在 :cpp:func:`s_ctlr_csi_start` 中也会调用此回调函数。如果此回调函数未能获取新的事务描述符，但设置了 ``bk_buffer_dis`` 标志，则摄像头控制器驱动程序将使用内部备份 buffer。

- :cpp:member:`esp_cam_ctlr_evt_cbs_t::on_trans_finished` 可设置回调函数，当摄像头控制器驱动程序完成传输时，该回调函数会被调用。此函数在 ISR 上下文中被调用，因此必须确保该函数不会尝试阻塞（例如，确保只从该函数中调用带有 ``ISR`` 后缀的 FreeRTOS API）。

.. _cam-thread-safety:

线程安全
^^^^^^^^

以下工厂函数由驱动程序保证线程安全。使用时，可以直接从不同的 RTOS 任务中调用此类函数，无需额外锁保护。

.. list::

    :SOC_MIPI_CSI_SUPPORTED: - :cpp:func:`esp_cam_new_csi_ctlr`
    :SOC_ISP_DVP_SUPPORTED: - :cpp:func:`esp_cam_new_isp_dvp_ctlr`
    - :cpp:func:`esp_cam_ctlr_del`

.. _cam-kconfig-options:

Kconfig 选项
^^^^^^^^^^^^

当 cache 被禁用时，以下 Kconfig 选项会影响中断处理程序的行为：

.. list::

    :SOC_MIPI_CSI_SUPPORTED: - :ref:`CONFIG_CAM_CTLR_MIPI_CSI_ISR_CACHE_SAFE`，详情请参阅 :ref:`cam-thread-safety`。
    :SOC_ISP_DVP_SUPPORTED: - :ref:`CONFIG_CAM_CTLR_ISP_DVP_ISR_CACHE_SAFE`，详情请参阅 :ref:`cam-thread-safety`。

.. _cam-iram-safe:

IRAM 安全
^^^^^^^^^

默认情况下，当 cache 因写入或擦除 flash 等原因而被禁用时，CSI 中断将被推迟。这些中断会在 cache 重新启用后再被处理。

以下 Kconfig 选项支持：

.. list::

    :SOC_MIPI_CSI_SUPPORTED: - :ref:`CONFIG_CAM_CTLR_MIPI_CSI_ISR_CACHE_SAFE`
    :SOC_ISP_DVP_SUPPORTED: - :ref:`CONFIG_CAM_CTLR_ISP_DVP_ISR_CACHE_SAFE`

- 即使 cache 被禁用也能启用中断服务
- 将 ISR 使用的所有函数放入 IRAM
- 将驱动程序对象放入 DRAM（以防意外映射到 PSRAM）

启用上述 Kconfig 选项，保证 cache 被禁用时中断可以正常运行，但这会增加 IRAM 使用量。因此，当 cache 被禁用时，用户回调函数需要注意（回调函数的）代码和数据应该是 IRAM 安全或 DRAM 安全的。

应用示例
--------

* :example:`peripherals/camera/mipi_isp_dsi` 演示了如何使用 ``esp_driver_cam`` 组件从 MIPI CSI 摄像头传感器捕获信号，传入 ISP 模块，并通过 DSI 接口将其显示在 LCD 屏幕上。
* :example:`peripherals/camera/dvp_isp_dsi` 演示了如何使用 ``esp_driver_cam`` 组件从 DVP 摄像头传感器捕获信号，传入 ISP 模块，并通过 DSI 接口将其显示在 LCD 屏幕上。

API 参考
--------

.. include-build-file:: inc/esp_cam_ctlr.inc
.. include-build-file:: inc/esp_cam_ctlr_types.inc
.. include-build-file:: inc/esp_cam_ctlr_csi.inc
.. include-build-file:: inc/esp_cam_ctlr_isp_dvp.inc
