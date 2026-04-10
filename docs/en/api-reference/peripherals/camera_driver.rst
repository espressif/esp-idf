Camera Controller Driver
========================

:link_to_translation:`zh_CN:[中文]`

Introduction
------------

{IDF_TARGET_NAME} has the following hardware designed for communication with external camera sensors:

.. list::

    :SOC_MIPI_CSI_SUPPORTED: - MIPI Camera Serial Interface (MIPI CSI)
    :SOC_ISP_DVP_SUPPORTED: - Digital Video Port via ISP module (ISP DVP)
    :SOC_LCDCAM_CAM_SUPPORTED: - Digital Video Port via LCD_CAM module (LCD_CAM DVP)

The camera controller driver provides an interface for these hardware peripherals.

Functional Overview
-------------------

.. list::

    - :ref:`cam-resource-allocation` – Describes how to allocate and configure camera controller instances, and how to release resources when no longer needed.
    - :ref:`cam-enable-disable` – Explains how to enable and disable a camera controller.
    - :ref:`cam-start-stop` – Details how to start and stop a camera controller.
    - :ref:`cam-receive` – Explains how to receive signals from a camera sensor.
    - :ref:`cam-callback` – Describes how to register user-defined event callback functions.
    - :ref:`cam-thread-safety` – Lists APIs that are thread-safe.
    - :ref:`cam-kconfig-options` – Lists supported Kconfig options that affect driver behavior.
    - :ref:`cam-iram-safe` – Provides guidance on making CSI interrupt and control functions work reliably when the cache is disabled.

.. _cam-resource-allocation:

Resource Allocation
^^^^^^^^^^^^^^^^^^^

Install the Camera Controller Driver
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You can implement the camera controller driver using one of the following methods:

.. list::

    :SOC_MIPI_CSI_SUPPORTED: - :cpp:func:`esp_cam_new_csi_ctlr`
    :SOC_ISP_DVP_SUPPORTED: - :cpp:func:`esp_cam_new_isp_dvp_ctlr`
    :SOC_LCDCAM_CAM_SUPPORTED: - :cpp:func:`esp_cam_new_lcd_cam_ctlr`

.. only:: SOC_MIPI_CSI_SUPPORTED

    You can implement a camera controller driver using the CSI peripheral. This requires configuration via :cpp:type:`esp_cam_ctlr_csi_config_t`.

    After specifying :cpp:type:`esp_cam_ctlr_csi_config_t`, call :cpp:func:`esp_cam_new_csi_ctlr` to allocate and initialize a CSI camera controller handle. If successful, this function returns a CSI camera controller handle. See the example below.

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

    The CSI peripheral in {IDF_TARGET_NAME} requires a stable 2.5 V power supply. Refer to the schematic diagram to ensure the power supply pins are connected to 2.5 V before using the MIPI CSI driver.

    .. only:: SOC_GP_LDO_SUPPORTED

        In {IDF_TARGET_NAME}, you can power the CSI using the internal adjustable LDO. Connect the LDO channel output pin to the CSI power supply pin. Before initializing the CSI driver, use the API in :doc:`/api-reference/peripherals/ldo_regulator` to configure the LDO to output 2.5 V.

.. only:: SOC_ISP_DVP_SUPPORTED

    You can implement a camera controller driver using the ISP DVP peripheral. This requires configuration via :cpp:type:`esp_cam_ctlr_isp_dvp_cfg_t`.

    After specifying :cpp:type:`esp_cam_ctlr_isp_dvp_cfg_t`, call :cpp:func:`esp_cam_new_isp_dvp_ctlr` to allocate and initialize an ISP DVP camera controller handle. If successful, this function returns an ISP DVP camera controller handle. See the example below.

    Before calling :cpp:func:`esp_cam_new_isp_dvp_ctlr`, create an ISP handle using :cpp:func:`esp_isp_new_processor`.

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

    You can implement a camera controller driver using the DVP port of LCD_CAM. This requires configuration via :cpp:type:`esp_cam_ctlr_dvp_config_t`.

    :cpp:member:`esp_cam_ctlr_dvp_config_t::exexternal_xtal`: Set this to use an externally generated xclk. Otherwise, the camera driver generates it internally.

    After specifying :cpp:type:`esp_cam_ctlr_lcd_cam_cfg_t`, call :cpp:func:`esp_cam_new_lcd_cam_ctlr` to allocate and initialize a DVP camera controller handle. If successful, this function returns a DVP camera controller handle. See the example below.

    After calling :cpp:func:`esp_cam_new_dvp_ctlr`, allocate a camera buffer that meets alignment constraints, or call :cpp:func:`esp_cam_ctlr_alloc_buffer` to allocate automatically.

    To configure format conversion, call :cpp:func:`esp_cam_ctlr_format_conversion`. The driver supports the following conversion types:

    * YUV to RGB
    * RGB to YUV
    * YUV to YUV

    Supported color ranges:
    * Full range: 0-255 for both RGB and YUV
    * Limited range: RGB 16-240, YUV Y:16-240, U-V:16-235

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

Uninstall the Camera Controller Driver
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To release resources for a camera controller driver that is no longer needed, call :cpp:func:`esp_cam_ctlr_del`. This releases the underlying hardware.

.. _cam-enable-disable:

Enable and Disable the Camera Controller Driver
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Before operating the camera controller, enable the driver by calling :cpp:func:`esp_cam_ctlr_enable`. This switches the driver state from **init** to **enable**.

.. code:: c

        #include "esp_cam_ctlr.h"
        #include "esp_cam_ctlr_types.h"
        #include "esp_err.h"

        void app_main(void)
        {
            esp_cam_ctlr_handle_t handle;
            ESP_ERROR_CHECK(esp_cam_ctlr_enable(handle));
        }

To disable the driver and return to the **init** state, call :cpp:func:`esp_cam_ctlr_disable`.

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

Start and Stop the Camera Controller Driver
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Before receiving signals from a camera sensor, start the driver by calling :cpp:func:`esp_cam_ctlr_start`. This switches the driver state from **enable** to **start**.

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

To stop the driver and return to the **enable** state, call :cpp:func:`esp_cam_ctlr_stop`.

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

Receive Data from a Camera Sensor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To receive data from a camera sensor, call :cpp:func:`esp_cam_ctlr_receive`.

.. code:: c

    #include "esp_err.h"
    #include "esp_cam_ctlr.h"
    #include "esp_cam_ctlr_types.h"

    ESP_ERROR_CHECK(esp_cam_ctlr_receive(handle, &my_trans, ESP_CAM_CTLR_MAX_DELAY));

.. _cam-callback:

Register Event Callbacks
^^^^^^^^^^^^^^^^^^^^^^^^

After the camera controller driver starts receiving data, it can generate events dynamically. To execute user-defined functions when events occur, register your callback function using :cpp:func:`esp_cam_ctlr_register_event_callbacks`. Supported event callbacks are listed in :cpp:type:`esp_cam_ctlr_evt_cbs_t`:

- :cpp:member:`esp_cam_ctlr_evt_cbs_t::on_get_new_trans` – Called after the driver finishes a transaction and attempts to get a new transaction descriptor. Also called in :cpp:func:`s_ctlr_csi_start`. If this callback does not provide a new transaction descriptor, the driver uses the internal backup buffer if the ``bk_buffer_dis`` flag is set.

- :cpp:member:`esp_cam_ctlr_evt_cbs_t::on_trans_finished` – Called when the driver finishes a transaction. This function runs in the ISR context. Ensure that it does not block (for example, only use FreeRTOS APIs with the ``ISR`` suffix).

.. _cam-thread-safety:

Thread Safety
^^^^^^^^^^^^^

The following factory functions are thread-safe and can be called from different RTOS tasks without additional locking:

.. list::

    :SOC_MIPI_CSI_SUPPORTED: - :cpp:func:`esp_cam_new_csi_ctlr`
    :SOC_ISP_DVP_SUPPORTED: - :cpp:func:`esp_cam_new_isp_dvp_ctlr`
    - :cpp:func:`esp_cam_ctlr_del`

.. _cam-kconfig-options:

Kconfig Options
^^^^^^^^^^^^^^^

The following Kconfig options affect interrupt handler behavior when the cache is disabled:

.. list::

    :SOC_MIPI_CSI_SUPPORTED: - :ref:`CONFIG_CAM_CTLR_MIPI_CSI_ISR_CACHE_SAFE`, see :ref:`cam-thread-safety` for details.
    :SOC_ISP_DVP_SUPPORTED: - :ref:`CONFIG_CAM_CTLR_ISP_DVP_ISR_CACHE_SAFE`, see :ref:`cam-thread-safety` for details.

.. _cam-iram-safe:

IRAM Safety
^^^^^^^^^^^^^^

By default, CSI interrupts are delayed when the cache is disabled during flash write or erase operations. They are handled after the cache is enabled again.

The following Kconfig options:

.. list::

    :SOC_MIPI_CSI_SUPPORTED: - :ref:`CONFIG_CAM_CTLR_MIPI_CSI_ISR_CACHE_SAFE`
    :SOC_ISP_DVP_SUPPORTED: - :ref:`CONFIG_CAM_CTLR_ISP_DVP_ISR_CACHE_SAFE`

-  Enable the interrupt being serviced even when the cache is disabled.
-  Place all functions used by the ISR into IRAM.
-  Place driver object into DRAM (to avoid mapping to PSRAM).

This allows interrupts to run while the cache is disabled, but increases IRAM usage. Ensure that user callbacks and related code/data are IRAM-safe or DRAM-safe when the cache is disabled.

Application Examples
--------------------

* :example:`peripherals/camera/mipi_isp_dsi` – Demonstrates how to use the ``esp_driver_cam`` component to capture signals from a MIPI CSI camera sensor via the ISP module and display them on an LCD screen via a DSI interface.
* :example:`peripherals/camera/dvp_isp_dsi` – Demonstrates how to use the ``esp_driver_cam`` component to capture signals from a DVP camera sensor via the ISP module and display them on an LCD screen via a DSI interface.

API Reference
-------------

.. include-build-file:: inc/esp_cam_ctlr.inc
.. include-build-file:: inc/esp_cam_ctlr_types.inc
.. include-build-file:: inc/esp_cam_ctlr_csi.inc
.. include-build-file:: inc/esp_cam_ctlr_isp_dvp.inc
