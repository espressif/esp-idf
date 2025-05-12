Image Signal Processor (ISP)
============================

:link_to_translation:`zh_CN:[中文]`

Introduction
------------

{IDF_TARGET_NAME} includes an Image Signal Processor (ISP), which is a feature pipeline that consists of many image processing algorithms. ISP receives image data from the DVP camera or MIPI-CSI camera, or system memory, and writes the processed image data to the system memory through DMA. The ISP is designed to work with other camera controller modules and can not operate independently.

Terminology
-----------

.. list::

    - MIPI-CSI: Camera serial interface, a high-speed serial interface for cameras compliant with MIPI specifications
    - DVP: Digital video parallel interface, generally composed of vsync, hsync, de, and data signals
    - RAW: Unprocessed data directly output from an image sensor, typically divided into R, Gr, Gb, and B four channels classified into RAW8, RAW10, RAW12, etc., based on bit width
    - RGB: Colored image format composed of red, green, and blue colors classified into RGB888, RGB565, etc., based on the bit width of each color
    - YUV: Colored image format composed of luminance and chrominance classified into YUV444, YUV422, YUV420, etc., based on the data arrangement
    - AF: Auto focus
    - AWB: Auto white balance
    - AE: Auto exposure
    - HIST: Histogram
    - BF: Bayer noise filter
    - LSC: Lens Shading Correction
    - CCM: Color correction matrix

ISP Pipeline
------------

.. blockdiag::
    :scale: 100%
    :caption: ISP Pipeline
    :align: center

    blockdiag isp_pipeline {
        orientation = portrait;
        node_height = 30;
        node_width = 120;
        span_width = 100;
        default_fontsize = 16;

        isp_header [label = "ISP Header"];
        isp_tail [label = "ISP Tail"];
        isp_chs [label = "Contrast &\n Hue & Saturation", width = 150, height = 70];
        isp_yuv [label = "YUV Limit\n YUB2RGB", width = 120, height = 70];

        isp_header -> BF -> LSC -> Demosaic -> CCM -> Gamma -> RGB2YUV -> SHARP -> isp_chs -> isp_yuv -> isp_tail;

        LSC -> HIST
        Demosaic -> AWB
        Demosaic -> AE
        Demosaic -> HIST
        CCM -> AWB
        Gamma -> AE
        RGB2YUV -> HIST
        RGB2YUV -> AF
    }

Functional Overview
-------------------

The ISP driver offers following services:

- :ref:`isp-resource-allocation` - covers how to allocate ISP resources with properly set of configurations. It also covers how to recycle the resources when they finished working.
- :ref:`isp-enable-disable` - covers how to enable and disable an ISP processor.
- :ref:`isp-af-statistics` - covers how to get AF statistics one-shot or continuously.
- :ref:`isp-awb-statistics` - covers how to get AWB white patches statistics one-shot or continuously.
- :ref:`isp-ae-statistics` - covers how to get AE statistics one-shot or continuously.
- :ref:`isp-hist-statistics` - covers how to get histogram statistics one-shot or continuously.
- :ref:`isp-bf` - covers how to enable and configure BF function.
- :ref:`isp-lsc` - covers how to enable and configure LSC function.
- :ref:`isp-ccm-config` - covers how to configure the CCM.
- :ref:`isp-demosaic` - covers how to configure the Demosaic function.
- :ref:`isp-gamma-correction` - covers how to enable and configure gamma correction.
- :ref:`isp-sharpen` - covers how to configure the sharpening function.
- :ref:`isp-callback` - covers how to hook user specific code to ISP driver event callback function.
- :ref:`isp-thread-safety` - lists which APIs are guaranteed to be thread safe by the driver.
- :ref:`isp-kconfig-options` - lists the supported Kconfig options that can bring different effects to the driver.
- :ref:`isp-iram-safe` - describes tips on how to make the ISP interrupt and control functions work better along with a disabled cache.

.. _isp-resource-allocation:

Resource Allocation
^^^^^^^^^^^^^^^^^^^

Install ISP Driver
~~~~~~~~~~~~~~~~~~

ISP driver requires the configuration that specified by :cpp:type:`esp_isp_processor_cfg_t`.

If the configurations in :cpp:type:`esp_isp_processor_cfg_t` is specified, users can call :cpp:func:`esp_isp_new_processor` to allocate and initialize an ISP processor. This function will return an ISP processor handle if it runs correctly. You can take following code as reference:

.. code-block:: c

    esp_isp_processor_cfg_t isp_config = {
        .clk_src = ISP_CLK_SRC_DEFAULT,
        ...
    };

    isp_proc_handle_t isp_proc = NULL;
    ESP_ERROR_CHECK(esp_isp_new_processor(&isp_config, &isp_proc));

You can use the created handle to enable/disable the ISP driver and do other ISP module installation.

.. note::

    ISP peripheral is necessary if MIPI CSI or ISP_DVP is used as camera controller. This means that even if ISP functions are not needed, you still need to install the ISP driver by calling :cpp:func:`esp_isp_new_processor`.

    If ISP functions are not needed, ISP driver supports bypassing ISP pipelines and enabling only the necessary functions. This can be achieved by setting :cpp:member:`esp_isp_processor_cfg_t::bypass_isp`.

Install ISP Auto Focus (AF) Driver
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ISP auto focus (AF) driver requires the configuration that specified by :cpp:type:`esp_isp_af_config_t`.

If the configurations in :cpp:type:`esp_isp_af_config_t` is specified, users can call :cpp:func:`esp_isp_new_af_controller` to allocate and initialize an ISP AF controller. This function will return an ISP AF controller handle if it runs correctly. You can take following code as reference:

.. code-block:: c

    esp_isp_af_config_t af_config = {
        .edge_thresh = 128,
    };
    isp_af_ctlr_t af_ctrlr = NULL;
    ESP_ERROR_CHECK(esp_isp_new_af_controller(isp_proc, &af_config, &af_ctrlr));

You can use the created handle to enable/disable the ISP AF driver and install ISP AF environment detector module.

Install ISP Auto White Balance (AWB) Driver
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ISP auto white balance (AWB) driver requires the configuration specified by :cpp:type:`esp_isp_awb_config_t`.

If an :cpp:type:`esp_isp_awb_config_t` configuration is specified, you can call :cpp:func:`esp_isp_new_awb_controller` to allocate and initialize an ISP AWB controller. This function will return an ISP AWB controller handle on success. You can take following code as reference:

.. code-block:: c

    isp_awb_ctlr_t awb_ctlr = NULL;
    uint32_t image_width = 800;
    uint32_t image_height = 600;
    /* The AWB configuration, please refer to the API comment for how to tune these parameters */
    esp_isp_awb_config_t awb_config = {
        .sample_point = ISP_AWB_SAMPLE_POINT_AFTER_CCM,
        ...
    };
    ESP_ERROR_CHECK(esp_isp_new_awb_controller(isp_proc, &awb_config, &awb_ctlr));

The AWB handle created in this step is required by other AWB APIs and AWB scheme.

Install ISP Auto Exposure (AE) Driver
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ISP auto exposure (AE) driver requires the configuration that specified by :cpp:type:`esp_isp_ae_config_t`.

If the configurations in :cpp:type:`esp_isp_ae_config_t` is specified, call :cpp:func:`esp_isp_new_ae_controller` to allocate and initialize an ISP AE controller. This function will return an ISP AE controller handle if it runs correctly. You can take following code as reference.

.. code-block:: c

    esp_isp_ae_config_t ae_config = {
        .sample_point = ISP_AE_SAMPLE_POINT_AFTER_DEMOSAIC,
        ...
    };
    isp_ae_ctlr_t ae_ctlr = NULL;
    ESP_ERROR_CHECK(esp_isp_new_ae_controller(isp_proc, &ae_config, &ae_ctlr));

You can use the created handle to enable/disable the ISP AE driver and do ISP AE environment detector setup.

Install ISP Histogram (HIST) Driver
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ISP histogram (HIST) driver requires the configuration that specified by :cpp:type:`esp_isp_hist_config_t`.

If the configurations in :cpp:type:`esp_isp_hist_config_t` is specified, users can call :cpp:func:`esp_isp_new_hist_controller` to allocate and initialize an ISP Histogram controller. This function will return an ISP HIST controller handle if it runs correctly. You can take following code as reference.

.. list::

    - The sum of all subwindow weights' decimal values should be 256; otherwise, the statistics will be small. The integer value should be 0.
    - The sum of all RGB coefficients' decimal values should be 256; otherwise, the statistics will be small. The integer value should be 0.
    - The segment_threshold must be 0–255 and in order.

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

You can use the created handle to enable/disable the ISP HIST driver setup.

Uninstall ISP Drivers
~~~~~~~~~~~~~~~~~~~~~~~

If previously installed ISP drivers are no longer needed, it's recommended to recycle the resource by following APIs to release the underlying hardware:

.. list::

    - :cpp:func:`esp_isp_del_processor`, for ISP processor.
    - :cpp:func:`esp_isp_del_af_controller`, for ISP AF controller.
    - :cpp:func:`esp_isp_del_awb_controller`, for ISP AWB controller.
    - :cpp:func:`esp_isp_del_ae_controller`, for ISP AE controller.
    - :cpp:func:`esp_isp_del_hist_controller`, for ISP Histogram controller.

.. _isp-enable-disable:

Enable and Disable ISP
^^^^^^^^^^^^^^^^^^^^^^

ISP
~~~

Before doing ISP pipeline, you need to enable the ISP processor first, by calling :cpp:func:`esp_isp_enable`. This function:

* Switches the driver state from **init** to **enable**.

Calling :cpp:func:`esp_isp_disable` does the opposite, that is, put the driver back to the **init** state.

ISP AF Controller
~~~~~~~~~~~~~~~~~

Before doing ISP AF, you need to enable the ISP AF controller first, by calling :cpp:func:`esp_isp_af_controller_enable`. This function:

* Switches the driver state from **init** to **enable**.

Calling :cpp:func:`esp_isp_af_controller_disable` does the opposite, that is, put the driver back to the **init** state.

.. _isp-af-statistics:

AF One-shot and Continuous Statistics
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Calling :cpp:func:`esp_isp_af_controller_get_oneshot_statistics` to get one-shot AF statistics result. You can take following code as reference.

Aside from the above one-shot API, the ISP AF driver also provides a way to start AF statistics continuously. Calling :cpp:func:`esp_isp_af_controller_start_continuous_statistics` to start the continuous statistics and :cpp:func:`esp_isp_af_controller_stop_continuous_statistics` to stop it.

Note that if you want to use the continuous statistics, you need to register the :cpp:member:`esp_isp_af_env_detector_evt_cbs_t::on_env_statistics_done` or :cpp:member:`esp_isp_af_env_detector_evt_cbs_t::on_env_change` callbacks to get the statistics result. See how to register in :ref:`isp-callback`.

.. note::

    When you use the continuous statistics, AF Environment Detector will be invalid.

.. code:: c

    esp_isp_af_config_t af_config = {
        .edge_thresh = 128,
    };
    isp_af_ctlr_t af_ctrlr = NULL;
    ESP_ERROR_CHECK(esp_isp_new_af_controller(isp_proc, &af_config, &af_ctrlr));
    ESP_ERROR_CHECK(esp_isp_af_controller_enable(af_ctrlr));
    isp_af_result_t result = {};
    /* Trigger the AF statistics and get its result for one time with timeout value 2000 ms */
    ESP_ERROR_CHECK(esp_isp_af_controller_get_oneshot_statistics(af_ctrlr, 2000, &result));

    /* Start continuous AF statistics */
    ESP_ERROR_CHECK(esp_isp_af_controller_start_continuous_statistics(af_ctrlr));
    // You can do other stuffs here, the statistics result can be obtained in the callback
    // ......
    // vTaskDelay(pdMS_TO_TICKS(1000));
    /* Stop continuous AF statistics */
    ESP_ERROR_CHECK(esp_isp_af_controller_stop_continuous_statistics(af_ctrlr));

    /* Disable the AF controller */
    ESP_ERROR_CHECK(esp_isp_af_controller_disable(af_ctrlr));
    /* Delete the AF controller and free the resources */
    ESP_ERROR_CHECK(esp_isp_del_af_controller(af_ctrlr));

Set AF Environment Detector
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Calling :cpp:func:`esp_isp_af_controller_set_env_detector` to set an ISP AF environment detector. You can take following code as reference:

.. code-block:: c

    esp_isp_af_env_config_t env_config = {
        .interval = 10,
    };
    isp_af_ctlr_t af_ctrlr = NULL;
    ESP_ERROR_CHECK(esp_isp_new_af_controller(isp_proc, &af_config, &af_ctrlr));
    ESP_ERROR_CHECK(esp_isp_af_controller_set_env_detector(af_ctrlr, &env_config));

Set AF Environment Detector Threshold
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Calling :cpp:func:`esp_isp_af_controller_set_env_detector_threshold` to set the threshold of an ISP AF environment detector.

.. code-block:: c

    int definition_thresh = 0;
    int luminance_thresh = 0;
    ESP_ERROR_CHECK(esp_isp_af_env_detector_set_threshold(env_detector, definition_thresh, luminance_thresh));

ISP AWB Controller
~~~~~~~~~~~~~~~~~~

Before doing ISP AWB, you need to enable the ISP AWB controller first, by calling :cpp:func:`esp_isp_awb_controller_enable`. This function:

* Switches the driver state from **init** to **enable**.

Calling :cpp:func:`esp_isp_awb_controller_disable` does the opposite, that is, put the driver back to the **init** state.

.. _isp-awb-statistics:

AWB One-shot and Continuous Statistics
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Calling :cpp:func:`esp_isp_awb_controller_get_oneshot_statistics` to get oneshot AWB statistics result of white patches. You can take following code as reference.

Aside from the above one-shot API, the ISP AWB driver also provides a way to start AWB statistics continuously. Calling :cpp:func:`esp_isp_awb_controller_start_continuous_statistics` starts the continuous statistics and :cpp:func:`esp_isp_awb_controller_stop_continuous_statistics` stops it.

Note that if you want to use the continuous statistics, you need to register the :cpp:member:`esp_isp_awb_cbs_t::on_statistics_done` callback to get the statistics result. See how to register it in :ref:`isp-callback`.

.. code-block:: c

    bool example_isp_awb_on_statistics_done_cb(isp_awb_ctlr_t awb_ctlr, const esp_isp_awb_evt_data_t *edata, void *user_data);
    // ...
    isp_awb_ctlr_t awb_ctlr = NULL;
    uint32_t image_width = 800;
    uint32_t image_height = 600;
    /* The AWB configuration, please refer to the API comment for how to tune these parameters */
    esp_isp_awb_config_t awb_config = {
        .sample_point = ISP_AWB_SAMPLE_POINT_AFTER_CCM,
        ...
    };
    isp_awb_stat_result_t stat_res = {};
    /* Create the AWB controller */
    ESP_ERROR_CHECK(esp_isp_new_awb_controller(isp_proc, &awb_config, &awb_ctlr));
    /* Register the AWB callback */
    esp_isp_awb_cbs_t awb_cb = {
        .on_statistics_done = example_isp_awb_on_statistics_done_cb,
    };
    ESP_ERROR_CHECK(esp_isp_awb_register_event_callbacks(awb_ctlr, &awb_cb, NULL));
    /* Enable the AWB controller */
    ESP_ERROR_CHECK(esp_isp_awb_controller_enable(awb_ctlr));

    /* Get one-shot AWB statistics result */
    ESP_ERROR_CHECK(esp_isp_awb_controller_get_oneshot_statistics(awb_ctlr, -1, &stat_res));

    /* Start continuous AWB statistics, note that continuous statistics requires `on_statistics_done` callback */
    ESP_ERROR_CHECK(esp_isp_awb_controller_start_continuous_statistics(awb_ctlr));
    // You can do other stuffs here, the statistics result can be obtained in the callback
    // ......
    // vTaskDelay(pdMS_TO_TICKS(1000));
    /* Stop continuous AWB statistics */
    ESP_ERROR_CHECK(esp_isp_awb_controller_stop_continuous_statistics(awb_ctlr));

    /* Disable the AWB controller */
    ESP_ERROR_CHECK(esp_isp_awb_controller_disable(awb_ctlr));
    /* Delete the AWB controller and free the resources */
    ESP_ERROR_CHECK(esp_isp_del_awb_controller(awb_ctlr));

ISP AE Controller
~~~~~~~~~~~~~~~~~

Before doing ISP AE, you need to enable the ISP AE controller first, by calling :cpp:func:`esp_isp_ae_controller_enable`. This function:

* Switches the driver state from **init** to **enable**.

Calling :cpp:func:`esp_isp_ae_controller_disable` does the opposite, that is, put the driver back to the **init** state.

.. _isp-ae-statistics:

AE One-shot and Continuous Statistics
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Calling :cpp:func:`esp_isp_ae_controller_get_oneshot_statistics` to get oneshot AE statistics result. You can take following code as reference.

When using AE oneshot statistics, the AE continuous mode need to be disabled otherwise the result may be overwritten by the environment detector. After oneshot operation finishes, you need to restart continuous mode again.

Aside from the above oneshot API, the ISP AE driver also provides a way to start AE statistics continuously. Calling :cpp:func:`esp_isp_ae_controller_start_continuous_statistics` to start the continuous statistics and :cpp:func:`esp_isp_ae_controller_stop_continuous_statistics` to stop it.

Note that if you want to use the continuous statistics, you need to register the :cpp:member:`esp_isp_ae_env_detector_evt_cbs_t::on_env_statistics_done` or :cpp:member:`esp_isp_ae_env_detector_evt_cbs_t::on_env_change` callback to get the statistics result. See how to register in :ref:`isp-callback`.

.. note::

    When using oneshot statistics, the AE environment detector will be temporarily disabled and will automatically recover once the oneshot is completed.

.. code-block:: c

    esp_isp_ae_config_t ae_config = {
        .sample_point = ISP_AE_SAMPLE_POINT_AFTER_DEMOSAIC,
    };
    isp_ae_ctlr_t ae_ctlr = NULL;
    ESP_ERROR_CHECK(esp_isp_new_ae_controller(isp_proc, &ae_config, &ae_ctlr));
    ESP_ERROR_CHECK(esp_isp_ae_controller_enable(ae_ctlr));
    isp_ae_result_t result = {};
    /* Trigger the AE statistics and get its result for one time with timeout value 2000 ms. */
    ESP_ERROR_CHECK(esp_isp_ae_controller_get_oneshot_statistics(ae_ctlr, 2000, &result));

    /* Start continuous AE statistics */
    ESP_ERROR_CHECK(esp_isp_ae_controller_start_continuous_statistics(ae_ctlr));
    // You can do other stuffs here, the statistics result can be obtained in the callback
    // ......
    // vTaskDelay(pdMS_TO_TICKS(1000));
    /* Stop continuous AE statistics */
    ESP_ERROR_CHECK(esp_isp_ae_controller_stop_continuous_statistics(ae_ctlr));

    /* Disable the AE controller */
    ESP_ERROR_CHECK(esp_isp_ae_controller_disable(ae_ctlr));
    /* Delete the AE controller and free the resources */
    ESP_ERROR_CHECK(esp_isp_del_ae_controller(ae_ctlr));

Set AE Environment Detector
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Calling :cpp:func:`esp_isp_ae_controller_set_env_detector` to set an ISP AE environment detector. You can take following code as reference.

.. code:: c

    esp_isp_ae_env_config_t env_config = {
        .interval = 10,
    };
    ESP_ERROR_CHECK(esp_isp_ae_controller_set_env_detector(ae_ctlr, &env_config));

Set AE Environment Detector Threshold
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Calling :cpp:func:`esp_isp_ae_controller_set_env_detector_threshold` to set the thresholds (1-255) of an ISP AE environment detector.

.. code:: c

    esp_isp_ae_env_thresh_t env_thresh = {
        .low_thresh = 110,
        .high_thresh = 130,
    };
    ESP_ERROR_CHECK(esp_isp_ae_controller_set_env_detector_threshold(ae_ctlr, env_thresh));

.. _isp-hist:

ISP Histogram Controller
~~~~~~~~~~~~~~~~~~~~~~~~

Before doing ISP histogram statistics, you need to enable the ISP histogram controller first, by calling :cpp:func:`esp_isp_hist_controller_enable`. This function:

* Switches the driver state from **init** to **enable**.

Calling :cpp:func:`esp_isp_hist_controller_disable` does the opposite, that is, put the driver back to the **init** state.

.. _isp-hist-statistics:

Histogram One-shot and Continuous Statistics
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Calling :cpp:func:`esp_isp_hist_controller_get_oneshot_statistics` to get oneshot histogram statistics result. You can take following code as reference.

Aside from the above oneshot API, the ISP histogram driver also provides a way to start histogram statistics continuously. Calling :cpp:func:`esp_isp_hist_controller_start_continuous_statistics` starts the continuous statistics and :cpp:func:`esp_isp_hist_controller_stop_continuous_statistics` stops it.

Note that if you want to use the continuous statistics, you need to register the :cpp:member:`esp_isp_hist_cbs_t::on_statistics_done` callback to get the statistics result. See how to register it in :ref:`isp-callback`.

.. code:: c

    static bool s_hist_scheme_on_statistics_done_callback(isp_hist_ctlr_t awb_ctrlr, const esp_isp_hist_evt_data_t *edata, void *user_data)
    {
        for(int i = 0; i < 16; i++) {
            esp_rom_printf(DRAM_STR("val %d is %x\n"), i, edata->hist_result.hist_value[i]); // get the histogram statistic value
        }
        return true;
    }

    esp_isp_hist_cbs_t hist_cbs = {
        .on_statistics_done = s_hist_scheme_on_statistics_done_callback,
    };

    esp_isp_hist_register_event_callbacks(hist_ctlr, &hist_cbs, hist_ctlr);
    esp_isp_hist_controller_enable(hist_ctlr);


.. _isp-bf:

ISP BF Controller
~~~~~~~~~~~~~~~~~

This pipeline is used for doing image input denoising under bayer mode.

Calling :cpp:func:`esp_isp_bf_configure` to configure BF function, you can take following code as reference.

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

:cpp:member:`esp_isp_bf_config_t::bf_template` is used for bayer denoise. You can set the :cpp:member:`esp_isp_bf_config_t::bf_template` with a Gaussian filter template or an average filter template.

After calling :cpp:func:`esp_isp_bf_configure`, you need to enable the ISP BF controller, by calling :cpp:func:`esp_isp_bf_enable`. This function:

* Switches the driver state from **init** to **enable**.

Calling :cpp:func:`esp_isp_bf_disable` does the opposite, that is, put the driver back to the **init** state.


.. _isp-lsc:

ISP LSC Controller
~~~~~~~~~~~~~~~~~~

Lens Shading Correction (LSC) aims for the issues caused by the uneven refraction of light through the camera lens.

Calling :cpp:func:`esp_isp_lsc_configure` to configure the LSC module to do the correction. The :cpp:type:`esp_isp_lsc_gain_array_t` is necessary for the hardware to do the correction related calculation. :cpp:func:`esp_isp_lsc_allocate_gain_array` is a helper function to help allocate proper size of memory for the gains.

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

After calling :cpp:func:`esp_isp_lsc_configure`, you need to enable the ISP LSC controller by calling :cpp:func:`esp_isp_lsc_enable`. The LSC can be disabled by calling :cpp:func:`esp_isp_lsc_disable`. It is allowed to call :cpp:func:`esp_isp_lsc_configure` when the LSC is not enabled, but the LSC function will only take effect when it is enabled.


.. _isp-color:

ISP Color Controller
~~~~~~~~~~~~~~~~~~~~

This pipeline is used to adjust the image contrast, saturation, hue and brightness.

Calling :cpp:func:`esp_isp_color_configure` to configure color function, you can take following code as reference.

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

    - Contrast value should be 0 ~ {IDF_TARGET_SOC_ISP_COLOR_CONTRAST_MAX}, default {IDF_TARGET_SOC_ISP_COLOR_CONTRAST_DEFAULT}
    - Saturation value should be 0 ~ {IDF_TARGET_SOC_ISP_COLOR_SATURATION_MAX}, default {IDF_TARGET_SOC_ISP_COLOR_SATURATION_DEFAULT}
    - Hue value should be 0 ~ {IDF_TARGET_SOC_ISP_COLOR_HUE_MAX}, default {IDF_TARGET_SOC_ISP_COLOR_HUE_DEFAULT}
    - Brightness value should be {IDF_TARGET_SOC_ISP_COLOR_BRIGHTNESS_MIN} ~ {IDF_TARGET_SOC_ISP_COLOR_BRIGHTNESS_MAX}, default {IDF_TARGET_SOC_ISP_COLOR_BRIGHTNESS_DEFAULT}

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

After calling :cpp:func:`esp_isp_color_configure`, you need to enable the ISP color controller, by calling :cpp:func:`esp_isp_color_enable`. This function:

* Switches the driver state from **init** to **enable**.

Calling :cpp:func:`esp_isp_color_disable` does the opposite, that is, put the driver back to the **init** state.

.. note::

    When the ISP DVP peripheral is used with the output color format set to the RGB color space, :ref:`isp-color` is automatically enabled in the camera driver to ensure correct data output. The function :cpp:func:`esp_isp_color_disable` should never be called in this case, otherwise it may result in disarrayed camera data.

.. _isp-ccm-config:

Configure CCM
^^^^^^^^^^^^^

Color correction matrix can scale the color ratio of RGB888 pixels. It can be used for adjusting the image color via some algorithms, for example, used for white balance by inputting the AWB computed result, or used as a filter with some filter algorithms.

To adjust the color correction matrix, here is the formula:

.. code-block:: none

    [ R' ]     [ RR  RG  RB  ]   [ R ]
    [ G' ] =   [ GR  GG  GB  ] * [ G ]
    [ B' ]     [ BR  BG  BB  ]   [ B ]

, and you can refer to the following code:

.. code-block:: c

    // ...
    // Configure CCM
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
    // The configured CCM will be applied to the image once the CCM module is enabled
    ESP_ERROR_CHECK(esp_isp_ccm_enable(isp_proc));
    // CCM can also be configured after it is enabled
    ccm_cfg.matrix[0][0] = 2.0;
    ESP_ERROR_CHECK(esp_isp_ccm_configure(isp_proc, &ccm_cfg));
    // Disable CCM if no longer needed
    ESP_ERROR_CHECK(esp_isp_ccm_disable(isp_proc));

.. _isp-demosaic:

ISP Demosaic Controller
~~~~~~~~~~~~~~~~~~~~~~~

This pipeline is used for doing image demosaic algorithm to convert RAW image to RGB mode.

Calling :cpp:func:`esp_isp_demosaic_configure` to configure Demosaic function, you can take following code as reference.

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

After calling :cpp:func:`esp_isp_demosaic_configure`, you need to enable the ISP Demosaic controller, by calling :cpp:func:`esp_isp_demosaic_enable`. This function:

* Switches the driver state from **init** to **enable**.

Calling :cpp:func:`esp_isp_demosaic_disable` does the opposite, that is, put the driver back to the **init** state.

:cpp:func:`esp_isp_demosaic_configure` is allowed to be called even if the driver is in **init** state, but the demosaic configurations will only be taken into effect when in **enable** state.

.. _isp-gamma-correction:

Enable Gamma Correction
^^^^^^^^^^^^^^^^^^^^^^^

The human visual system is non-linearly sensitive to the physical luminance. Adding gamma correction to the ISP pipeline to transform RGB coordinates into a space in which coordinates are proportional to subjective brightness.

The driver provides a helper API :cpp:func:`esp_isp_gamma_fill_curve_points` to fill :cpp:type:`isp_gamma_curve_points_t`, which is a group of points used to describe the gamma correction curve. Or you can manually declare the points as your desired gamma correction curve. Each R/G/B component can have its own gamma correction curve, you can set the configuration by calling :cpp:func:`esp_isp_gamma_configure`.

A typical code example is:

.. code:: c

    #include <math.h>

    // Set the camera gamma to be 0.7, so the gamma correction curve is y = 256 * (x / 256) ^ 0.7
    static uint32_t s_gamma_curve(uint32_t x)
    {
        return pow((double)x / 256, 0.7) * 256;
    }

    isp_gamma_curve_points_t pts = {};
    ESP_ERROR_CHECK(esp_isp_gamma_fill_curve_points(s_gamma_curve, &pts));
    ESP_ERROR_CHECK(esp_isp_gamma_configure(isp_proc, COLOR_COMPONENT_R, &pts));
    ESP_ERROR_CHECK(esp_isp_gamma_configure(isp_proc, COLOR_COMPONENT_G, &pts));
    ESP_ERROR_CHECK(esp_isp_gamma_configure(isp_proc, COLOR_COMPONENT_B, &pts));

    // Enable gamma module after curve parameters configured
    ESP_ERROR_CHECK(esp_isp_gamma_enable(isp_proc));

    // Disable gamma if no longer needed
    ESP_ERROR_CHECK(esp_isp_gamma_disable(isp_proc));

.. _isp-sharpen:

ISP Sharpen Controller
~~~~~~~~~~~~~~~~~~~~~~

This pipeline is used for doing image input sharpening under YUV mode.

Calling :cpp:func:`esp_isp_sharpen_configure` to configure Sharpen function, you can take following code as reference.

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

:cpp:member:`esp_isp_sharpen_config_t::sharpen_template` is used for sharpening. You can set the :cpp:member:`esp_isp_sharpen_config_t::sharpen_template` with a Gaussian filter template or an average filter template.

After calling :cpp:func:`esp_isp_sharpen_configure`, you need to enable the ISP Sharpen controller, by calling :cpp:func:`esp_isp_sharpen_enable`. This function:

* Switches the driver state from **init** to **enable**.

Calling :cpp:func:`esp_isp_sharpen_disable` does the opposite, that is, put the driver back to the **init** state.

:cpp:func:`esp_isp_sharpen_configure` is allowed to be called even if the driver is in **init** state, but the sharpen configurations will only be taken into effect when in **enable** state.


.. _isp-callback:

Register Event Callbacks
^^^^^^^^^^^^^^^^^^^^^^^^

After an ISP module starts up, it can generate a specific event dynamically.

You can save your own context to callback function as well, via the parameter ``user_data``. The user data will be directly passed to the callback function.

.. note::

    The below-mentioned callback functions are called within an ISR context. You must ensure that the functions do not attempt to block (e.g., by making sure that only FreeRTOS APIs with ``ISR`` suffix are called from within the function).

Register ISP Processor Event Callbacks
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After the ISP processor is enabled, it can generate multiple events of multiple ISP submodules dynamically. You can hook your functions to the interrupt service routine by calling :cpp:func:`esp_isp_register_event_callbacks`. All supported event callbacks are listed in :cpp:type:`esp_isp_evt_cbs_t`:

- :cpp:member:`esp_isp_evt_cbs_t::on_sharpen_frame_done` sets a callback function for sharpen frame done. It will be called after the ISP sharpen submodule finishes its operation for one frame. The function prototype is declared in :cpp:type:`esp_isp_sharpen_callback_t`.

Register ISP AF Environment Detector Event Callbacks
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After the ISP AF environment detector starts up, it can generate a specific event dynamically. If you have some functions that should be called when the event happens, please hook your function to the interrupt service routine by calling :cpp:func:`esp_isp_af_env_detector_register_event_callbacks`. All supported event callbacks are listed in :cpp:type:`esp_isp_af_env_detector_evt_cbs_t`:

-  :cpp:member:`esp_isp_af_env_detector_evt_cbs_t::on_env_statistics_done` sets a callback function for environment statistics done. The function prototype is declared in :cpp:type:`esp_isp_af_env_detector_callback_t`.
-  :cpp:member:`esp_isp_af_env_detector_evt_cbs_t::on_env_change` sets a callback function for environment change. The function prototype is declared in :cpp:type:`esp_isp_af_env_detector_callback_t`.

Register ISP AWB Statistics Done Event Callbacks
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After the ISP AWB controller finished statistics of white patches, it can generate a specific event dynamically. If you want to be informed when the statistics done event takes place, please hook your function to the interrupt service routine by calling :cpp:func:`esp_isp_awb_register_event_callbacks`. All supported event callbacks are listed in :cpp:type:`esp_isp_awb_cbs_t`:

-  :cpp:member:`esp_isp_awb_cbs_t::on_statistics_done` sets a callback function when finishing statistics of the white patches. The function prototype is declared in :cpp:type:`esp_isp_awb_callback_t`.


Register ISP AE Environment Detector Event Callbacks
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After the ISP AE environment detector starts up, it can generate a specific event dynamically. If you have some functions that should be called when the event happens, please hook your function to the interrupt service routine by calling :cpp:func:`esp_isp_ae_env_detector_register_event_callbacks`. All supported event callbacks are listed in :cpp:type:`esp_isp_ae_env_detector_evt_cbs_t`:

- :cpp:member:`esp_isp_ae_env_detector_evt_cbs_t::on_env_statistics_done` sets a callback function for environment statistics done. The function prototype is declared in :cpp:type:`esp_isp_ae_env_detector_callback_t`.
- :cpp:member:`esp_isp_ae_env_detector_evt_cbs_t::on_env_change` sets a callback function for environment change. The function prototype is declared in :cpp:type:`esp_isp_ae_env_detector_callback_t`.


Register ISP HIST Statistics Done Event Callbacks
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After the ISP HIST controller finished statistics of brightness, it can generate a specific event dynamically. If you want to be informed when the statistics done event takes place, please hook your function to the interrupt service routine by calling :cpp:func:`esp_isp_hist_register_event_callbacks`. All supported event callbacks are listed in :cpp:type:`esp_isp_hist_cbs_t`:

- :cpp:member:`esp_isp_hist_cbs_t::on_statistics_done` sets a callback function when finishing statistics of the brightness. The function prototype is declared in :cpp:type:`esp_isp_hist_callback_t`.

.. _isp-thread-safety:

Thread Safety
^^^^^^^^^^^^^

The following factory function are guaranteed to be thread safe by the driver:

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

These functions can be called from different RTOS tasks without protection by extra locks. Other APIs are not guaranteed to be thread-safe.

.. _isp-kconfig-options:

Kconfig Options
^^^^^^^^^^^^^^^

- :ref:`CONFIG_ISP_ISR_IRAM_SAFE` controls whether the default ISR handler should be masked when the cache is disabled.

.. _isp-iram-safe:

IRAM Safe
^^^^^^^^^

By default, the ISP interrupt will be deferred when the cache is disabled because of writing or erasing the flash.

Kconfig option :ref:`CONFIG_ISP_ISR_IRAM_SAFE` will:

-  Enable the interrupt being serviced even when the cache is disabled
-  Place all functions that used by the ISR into IRAM
-  Place driver object into DRAM (in case it is mapped to PSRAM by accident)

This allows the interrupt to run while the cache is disabled, but comes at the cost of increased IRAM consumption. With this option enabled, the ISR callbacks will be running when cache is disabled. Therefore you should make sure the callbacks and its involved context are IRAM-safe as well.

Kconfig option :ref:`CONFIG_ISP_CTRL_FUNC_IN_IRAM` will:

- Place some of the ISP control functions into IRAM, including:

    .. list::

        - :cpp:func:`esp_isp_sharpen_configure`
        - :cpp:func:`esp_isp_demosaic_configure`

Application Examples
--------------------

* :example:`peripherals/isp/multi_pipelines` demonstrates how to use the ISP pipelines to process the image signals from camera sensors and display the video on LCD screen via DSI peripheral.

API Reference
-------------

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
