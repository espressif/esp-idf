Image Signal Processor
======================

Introduction
------------

{IDF_TARGET_NAME} includes an image signal processor (ISP), which is a feature pipeline that consists of many image processing algorithms. ISP receives image data from the DVP camera or MIPI-CSI camera, or system memory, and writes the processed image data to the system memory through DMA. ISP shall work with other modules to read and write data, it can not work alone.

Terminology
-----------

.. list::
  - MIPI-CSI: Camera serial interface, a high-speed serial interface for cameras compliant with MIPI specifications
  - DVP: Digital video parallel interface, generally composed of vsync, hsync, de, and data signals
  - RAW: Unprocessed data directly output from an image sensor, typically divided into R, Gr, Gb, and B four channels classified into RAW8, RAW10, RAW12, etc., based on bit width
  - RGB: Colored image format composed of red, green, and blue colors classified into RGB888, RGB565, etc., based on the bit width of each color
  - YUV: Colored image format composed of luminance and chrominance classified into YUV444, YUV422, YUV420, etc., based on the data arrangement
  - AF:  Auto-focus

Functional Overview
-------------------

The ISP driver offers following services:

-  `Resource Allocation <#isp-resource-allocation>`__ - covers how to allocate ISP resources with properly set of configurations. It also covers how to recycle the resources when they finished working.
-  `Enable and disable ISP processor <#isp-enable-disable>`__ - covers how to enable and disable an ISP processor.
-  `Get AF oneshot result <#isp-af-get-oneshot-result>`__ - covers how to get AF oneshot result.
-  `Register callback <#isp-callback>`__ - covers how to hook user specific code to ISP driver event callback function.
-  `Thread Safety <#isp-thread-safety>`__ - lists which APIs are guaranteed to be thread safe by the driver.
-  `Kconfig Options <#isp-kconfig-options>`__ - lists the supported Kconfig options that can bring different effects to the driver.
-  `IRAM SAFE <#isp-iram-safe>`__ - describes tips on how to make the ISP interrupt and control functions work better along with a disabled cache.

.. _isp-resource-allocation:

Resource Allocation
^^^^^^^^^^^^^^^^^^^

Install Image Signal Processor (ISP) Driver
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ISP driver requires the configuration that specified by :cpp:type:`esp_isp_processor_cfg_t`.

If the configurations in :cpp:type:`esp_isp_processor_cfg_t` is specified, users can call :cpp:func:`esp_isp_new_processor` to allocate and initialize an ISP processor. This function will return an ISP processor handle if it runs correctly. You can take following code as reference.

.. code:: c

    esp_isp_processor_cfg_t isp_config = {
        .clk_src = ISP_CLK_SRC_DEFAULT,
        .clk_hz = 80 * 1000 * 1000,
        .input_data_source = ISP_INPUT_DATA_SOURCE_CSI,
        .input_data_color_type = ISP_COLOR_RAW8,
        .output_data_color_type = ISP_COLOR_RGB565,
    };

    isp_proc_handle_t isp_proc = NULL;
    ESP_ERROR_CHECK(esp_isp_new_processor(&isp_config, &isp_proc));

You can use the created handle to do driver enable / disable the ISP driver and do other ISP module installation.


Install Image Signal Processor (ISP) Auto-Focus (AF) Driver
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ISP auto-focus (AF) driver requires the configuration that specified by :cpp:type:`esp_isp_af_config_t`.

If the configurations in :cpp:type:`esp_isp_af_config_t` is specified, users can call :cpp:func:`esp_isp_new_af_controller` to allocate and initialize an ISP AF processor. This function will return an ISP AF processor handle if it runs correctly. You can take following code as reference.

.. code:: c

    esp_isp_af_config_t af_config = {
        .edge_thresh = 128,
    };
    isp_af_ctrlr_t af_ctrlr = NULL;
    ESP_ERROR_CHECK(esp_isp_new_af_controller(isp_proc, &af_config, &af_ctrlr));

You can use the created handle to do driver enable / disable the ISP AF driver and ISP AF Env module installation.

Uninstall Image Signal Processor (ISP) Driver
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If a previously installed ISP processor is no longer needed, it's recommended to recycle the resource by calling :cpp:func:`esp_isp_del_processor`, so that to release the underlying hardware.

UnInstall Image Signal Processor (ISP) Auto-Focus (AF) Driver
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If a previously installed ISP AF processor is no longer needed, it's recommended to recycle the resource by calling :cpp:func:`esp_isp_del_af_controller`, so that to release the underlying hardware.


.. _isp-enable-disable:

Enable and Disable Image Signal Processor (ISP)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Image Signal Processor (ISP)
----------------------------

Before doing ISP pipeline, you need to enable the ISP processor first, by calling :cpp:func:`esp_isp_enable`. This function:

* Switches the driver state from **init** to **enable**.

Calling :cpp:func:`esp_isp_disable` does the opposite, that is, put the driver back to the **init** state.

Image Signal Processor (ISP) Auto-Focus (AF) Processor
------------------------------------------------------

Before doing ISP AF, you need to enable the ISP AF processor first, by calling :cpp:func:`esp_isp_af_controller_enable`. This function:

* Switches the driver state from **init** to **enable**.

Calling :cpp:func:`esp_isp_af_controller_disable` does the opposite, that is, put the driver back to the **init** state.

.. _isp-af-get-oneshot-result:

Get Auto-Focus (AF) Oneshot Result
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Calling :cpp:func:`esp_isp_af_controller_get_oneshot_result` to get oneshot AF result. You can take following code as reference.

.. code:: c

    esp_isp_af_config_t af_config = {
        .edge_thresh = 128,
    };
    isp_af_ctrlr_t af_ctrlr = NULL;
    ESP_ERROR_CHECK(esp_isp_new_af_controller(isp_proc, &af_config, &af_ctrlr));
    isp_af_result_t result = {};
    ESP_ERROR_CHECK(esp_isp_af_controller_get_oneshot_result(ctx->af_ctlr, &result));

Set Auto-Focus (AF) Environment Detector
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Calling :cpp:func:`esp_isp_af_controller_set_env_detector` to set an ISP AF environment detector. You can take following code as reference.

.. code:: c

    esp_isp_af_env_config_t env_config = {
        .interval = 10,
    };
    isp_af_ctrlr_t af_ctrlr = NULL;
    ESP_ERROR_CHECK(esp_isp_new_af_controller(isp_proc, &af_config, &af_ctrlr));
    ESP_ERROR_CHECK(esp_isp_af_controller_set_env_detector(af_ctrlr, &env_config));

Set Auto-Focus (AF) Environment Detector Threshold
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Calling :cpp:func:`esp_isp_af_env_detector_set_threshold` to set the threshold of an ISP AF environment detector.

.. code:: c

    int definition_thresh = 0;
    int luminance_thresh = 0;
    ESP_ERROR_CHECK(esp_isp_af_env_detector_set_threshold(env_detector, definition_thresh, luminance_thresh));

.. _isp-callback:

Register Event Callbacks
^^^^^^^^^^^^^^^^^^^^^^^^

Register Image Signal Processor (ISP) Auto-Focus (AF) Environment Detector Event Callbacks
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After the ISP AF environment detector starts up, it can generate a specific event dynamically. If you have some functions that should be called when the event happens, please hook your function to the interrupt service routine by calling :cpp:func:`esp_isp_af_env_detector_register_event_callbacks`. All supported event callbacks are listed in :cpp:type:`esp_isp_af_env_detector_evt_cbs_t`:

-  :cpp:member:`esp_isp_af_env_detector_evt_cbs_t::on_env_change` sets a callback function for environment change. As this function is called within the ISR context, you must ensure that the function does not attempt to block (e.g., by making sure that only FreeRTOS APIs with ``ISR`` suffix are called from within the function). The function prototype is declared in :cpp:type:`esp_isp_af_env_detector_callback_t`.

You can save your own context to :cpp:func:`esp_isp_af_env_detector_register_event_callbacks` as well, via the parameter ``user_data``. The user data will be directly passed to the callback function.

.. _isp-thread-safety:

Thread Safety
^^^^^^^^^^^^^

The factory function :cpp:func:`esp_isp_new_processor`, :cpp:func:`esp_isp_del_processor`, :cpp:func:`esp_isp_new_af_controller`, :cpp:func:`esp_isp_del_af_controller`, :cpp:func:`esp_isp_new_af_env_detector`, and :cpp:func:`esp_isp_del_af_env_detector` are guaranteed to be thread safe by the driver, which means, user can call them from different RTOS tasks without protection by extra locks.

.. _isp-kconfig-options:

Kconfig Options
^^^^^^^^^^^^^^^

- :ref:`CONFIG_ISP_ISR_IRAM_SAFE` controls whether the default ISR handler should be masked when the cache is disabled

.. _isp-iram-safe:

IRAM Safe
^^^^^^^^^

By default, the ISP interrupt will be deferred when the cache is disabled because of writing or erasing the flash.

There is a Kconfig option :ref:`CONFIG_ISP_ISR_IRAM_SAFE` that:

-  Enables the interrupt being serviced even when the cache is disabled
-  Places all functions that used by the ISR into IRAM
-  Places driver object into DRAM (in case it is mapped to PSRAM by accident)

This allows the interrupt to run while the cache is disabled, but comes at the cost of increased IRAM consumption.

API Reference
-------------

.. include-build-file:: inc/isp.inc
.. include-build-file:: inc/isp_types.inc
.. include-build-file:: inc/isp_af.inc
