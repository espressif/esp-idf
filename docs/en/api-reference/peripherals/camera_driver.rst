Camera Controller Driver
========================

Introduction
------------

{IDF_TARGET_NAME} has the following hardware that is intended for communication with external camera sensor:

.. list::

    : SOC_MIPI_CSI_SUPPORTED : - MIPI Camera Serial Interface (CSI)

The Camera Controller Driver is designed for this hardware peripheral.


Functional Overview
-------------------

.. list::

    -  `Resource Allocation <#cam-resource-allocation>`__ - covers how to allocate camera controller instances with properly set of configurations. It also covers how to recycle the resources when they are no longer needed.
    -  `Enable and disable a camera controller <#cam-enable-disable>`__ - covers how to enable and disable a camera controller.
    -  `Start and stop a camera controller <#cam-start-stop>`__ - covers how to start and stop a camera controller.
    -  `Receive from a camera sensor or something else <#cam-receive>`__ - covers how to receive camera signal from a sensor or something else.
    -  `Register callback <#cam-callback>`__ - covers how to hook user specific code to camera controller driver event callback function.
    -  `Thread Safety <#cam-thread-safety>`__ - lists which APIs are guaranteed to be thread safe by the driver.
    -  `Kconfig Options <#cam-kconfig-options>`__ - lists the supported Kconfig options that can bring different effects to the driver.
    -  `IRAM SAFE <#cam-iram-safe>`__ - describes tips on how to make the CSI interrupt and control functions work better along with a disabled cache.

.. _cam-resource-allocation:

Resource Allocation
^^^^^^^^^^^^^^^^^^^

.. only:: SOC_MIPI_CSI_SUPPORTED

    Install Camera Controller Driver
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    A Camera Controller Driver can be implemented by the CSI peripheral, which requires the configuration that specified by :cpp:type:`esp_cam_ctlr_csi_config_t`.

    If the configurations in :cpp:type:`esp_cam_ctlr_csi_config_t` is specified, users can call :cpp:func:`esp_cam_new_csi_ctlr` to allocate and initialize a CSI camera controller handle. This function will return an CSI camera controller handle if it runs correctly. You can take following code as reference.

    .. code:: c

        esp_cam_ctlr_csi_config_t csi_config = {
            .ctlr_id = 0,
            .h_res = MIPI_CSI_DISP_HSIZE,
            .v_res = MIPI_CSI_DISP_VSIZE_640P,
            .lane_bit_rate_mbps = MIPI_CSI_LANE_BITRATE_MBPS,
            .input_data_color_type = MIPI_CSI_COLOR_RAW8,
            .output_data_color_type = MIPI_CSI_COLOR_RGB565,
            .data_lane_num = 2,
            .byte_swap_en = false,
            .queue_items = 1,
        };
        esp_cam_ctlr_handle_t handle = NULL;
        ESP_ERROR_CHECK(esp_cam_new_csi_ctlr(&csi_config, &handle));

Uninstall Camera Controller Driver
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If a previously installed Camera Controller Driver is no longer needed, it's recommended to recycle the resource by calling :cpp:func:`esp_cam_del_ctlr`, so that to release the underlying hardware.

.. _cam-enable-disable:

Enable and Disable Camera Controller Driver
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Before starting camera controller operation, you need to enable the camera controller controller first, by calling :cpp:func:`esp_cam_ctlr_enable`. This function:

* Switches the driver state from **init** to **enable**.

.. code:: c

    ESP_ERROR_CHECK(esp_cam_ctlr_enable(handle));

Calling :cpp:func:`esp_cam_ctlr_disable` does the opposite, that is, put the driver back to the **init** state.

.. code:: c

    ESP_ERROR_CHECK(esp_cam_ctlr_disable(handle));

.. _cam-start-stop:

Start and Stop Camera Controller Driver
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Before receiving camera signal from camera sensor, you need to start the Camera Controller Driver first, by calling :cpp:func:`esp_cam_ctlr_start`. This function:

* Switches the driver stat from **enable** to **start**

.. code:: c

    ESP_ERROR_CHECK(esp_cam_ctlr_start(handle));

Calling :cpp:func:`esp_cam_ctlr_stop` does the opposite, that is, put the driver back to the **enable** state.

.. code:: c

    ESP_ERROR_CHECK(esp_cam_ctlr_stop(handle));

.. _cam-receive:

Receive from A Camera Sensor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Now you can call :cpp:func:`esp_cam_ctlr_receive` to receive from a camera sensor or something else.

.. code:: c

    ESP_ERROR_CHECK(esp_cam_ctlr_receive(handle, &my_trans, ESP_CAM_CTLR_MAX_DELAY));

.. _cam-callback:

Register Event Callbacks
^^^^^^^^^^^^^^^^^^^^^^^^

After the Camera Controller Driver starts receiving, it can generate a specific event dynamically. If you have some functions that should be called when the event happens, please hook your function to the interrupt service routine by calling :cpp:func:`esp_cam_ctlr_register_event_callbacks`. All supported event callbacks are listed in :cpp:type:`esp_cam_ctlr_evt_cbs_t`:

-  :cpp:member:`esp_cam_ctlr_evt_cbs_t::on_get_new_trans` sets a callback function when the Camera Controller Driver gets a new transaction which is passed from :cpp:func:`esp_cam_ctlr_receive`. As this function is called within the ISR context, you must ensure that the function does not attempt to block (e.g., by making sure that only FreeRTOS APIs with ``ISR`` suffix are called from within the function).

-  :cpp:member:`esp_cam_ctlr_evt_cbs_t::on_trans_finished` sets a callback function when the Camera Controller Driver finishes a transaction. As this function is called within the ISR context, you must ensure that the function does not attempt to block (e.g., by making sure that only FreeRTOS APIs with ``ISR`` suffix are called from within the function).

.. _cam-thread-safety:

Thread Safety
^^^^^^^^^^^^^

The factory function :cpp:func:`esp_cam_new_csi_ctlr` and :cpp:func:`esp_cam_del_ctlr` are guaranteed to be thread safe by the driver, which means, user can call them from different RTOS tasks without protection by extra locks.

.. _cam-kconfig-options:

Kconfig Options
^^^^^^^^^^^^^^^

- :ref:`CONFIG_MIPI_CSI_ISR_IRAM_SAFE` controls whether the default ISR handler should be masked when the cache is disabled


.. _cam-iram-safe:

IRAM Safe
^^^^^^^^^

By default, the CSI interrupt will be deferred when the cache is disabled because of writing or erasing the flash.

There is a Kconfig option :ref:`CONFIG_MIPI_CSI_ISR_IRAM_SAFE` that:

-  Enables the interrupt being serviced even when the cache is disabled
-  Places all functions that used by the ISR into IRAM
-  Places driver object into DRAM (in case it is mapped to PSRAM by accident)

This allows the interrupt to run while the cache is disabled, but comes at the cost of increased IRAM consumption. So user callbacks need to notice that the code and data inside (the callback) should be IRAM-safe or DRAM-safe, when cache is disabled.

API Reference
-------------

.. include-build-file:: inc/esp_cam_ctlr.inc
.. include-build-file:: inc/esp_cam_ctlr_types.inc
.. include-build-file:: inc/esp_cam_ctlr_csi.inc
