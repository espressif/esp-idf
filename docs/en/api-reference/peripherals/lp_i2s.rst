Low Power Inter-IC Sound (LP I2S)
=================================


Introduction
------------

LP I2S (Low Power Inter-IC Sound) is a synchronous protocol which can be used for audio data transmission. It also provides a data reception communication interface for Voice Activity Detection (VAD) and some digital audio applications in low power mode. For more details about VAD, see :doc:`Voice Activity Detection <./vad>`.

.. only:: SOC_I2S_SUPPORTED

    .. note::

        For I2S documentation, see :doc:`Inter-IC Sound <./i2s>`.

The I2S standard bus defines three signals,
- BCK: bit clock
- WS: word select
- SD: serial data

A basic I2S data bus has one master and one slave. The roles remain unchanged throughout the communication.

.. only:: esp32p4

    LP I2S on {IDF_TARGET_NAME} only supports working as an I2S Slave.

The LP I2S module on {IDF_TARGET_NAME} provides an independent RX unit, which supports receiving data when the chip is running under sleep modes. Compared to HP I2S, LP I2S does not support DMA access. Instead, it uses a piece of separate internal memory to store data.


I2S Communication Mode
----------------------

Standard Mode
^^^^^^^^^^^^^

In standard mode, there are always two sound channels, i.e., the left and right channels, which are called "slots". These slots support 16-bit-width sample data. The communication format for the slots can be found in this :ref:`i2s-communication-mode` section.

PDM Mode (RX)
^^^^^^^^^^^^^

PDM (Pulse-density Modulation) mode for RX channel can receive PDM-format data. Only 16-bit-width sample data are supported. The communication format for the slots can be found in this :ref:`i2s-communication-mode` section.


Functional Overview
-------------------

Resource Allocation
^^^^^^^^^^^^^^^^^^^

To create a LP I2S channel handle, you should set up the LP I2S channel configuration structure :cpp:type:`lp_i2s_chan_config_t`, and call :cpp:func:`lp_i2s_new_channel` with the prepared configuration structure.

If the LP I2S channel is no longer used, you should recycle the allocated resource by calling :cpp:func:`lp_i2s_del_channel`.

.. code:: c

    //initialization
    lp_i2s_chan_handle_t rx_handle = NULL;
    lp_i2s_chan_config_t config = {
        .id = 0,
        .role = I2S_ROLE_SLAVE,
        .threshold = 512,
    };
    ESP_ERROR_CHECK(lp_i2s_new_channel(&config, NULL, &rx_handle));

    //deinitialization
    ESP_ERROR_CHECK(lp_i2s_del_channel(rx_chan));


Register Event Callbacks
^^^^^^^^^^^^^^^^^^^^^^^^

By calling :cpp:func:`lp_i2s_register_event_callbacks`, you can hook your own function to the driver ISR. Supported event callbacks are listed in :cpp:type:`lp_i2s_evt_cbs_t`.

As the above callbacks are called in an ISR context, you should always ensure the callback function is suitable for an ISR context. Blocking logic should not appear in these callbacks. The callback function prototype is declared in :cpp:type:`lp_i2s_callback_t`.

You can also register your own context when calling :cpp:func:`lp_i2s_register_event_callbacks` by the parameter ``user_data``. This user data will be passed to the callback functions directly.

This function may fail due to reasons like :c:macro:`ESP_ERR_INVALID_ARG`, especially, this error may indicate that the callback functions are not in the internal RAM. Callbacks should be placed in IRAM since the default ISR handler is allocated with the `ESP_INTR_FLAG_IRAM` flag.

Please check the error log for more details. If it fails due to :c:macro:`ESP_ERR_INVALID_STATE`, it indicates that the LP I2S channel is enabled, and you cannot add a callback at this moment.

.. code:: c

    lp_i2s_evt_cbs_t cbs = {
            .on_thresh_met = s_lp_i2s_on_thresh_met,
            .on_request_new_trans = s_lp_i2s_on_request_new_trans,
    };
    ESP_ERROR_CHECK(lp_i2s_register_event_callbacks(rx_chan, &cbs, &trans));

Enable and Disable LP I2S
^^^^^^^^^^^^^^^^^^^^^^^^^

Before using LP I2S to receive data, you need to enable the LP I2S channel by calling :cpp:func:`lp_i2s_channel_enable`, this function switches the driver state from **init** to **enable**. Calling :cpp:func:`lp_i2s_channel_disable` does the opposite, that is, puts the driver back to the **init** state.

Communication Mode
^^^^^^^^^^^^^^^^^^

.. list::

    - Calling :cpp:func:`lp_i2s_channel_init_std_mode` can help you initialize the LP I2S channel to STD mode. Some initialization helpers are listed below:
      - :c:macro:`LP_I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG`
      - :c:macro:`LP_I2S_STD_MSB_SLOT_DEFAULT_CONFIG`
      - :c:macro:`LP_I2S_STD_PCM_SHORT_SLOT_DEFAULT_CONFIG`

    - Calling :cpp:func:`lp_i2s_channel_init_pdm_rx_mode` can help you initialize the LP I2S channel to PDM mode. :c:macro:`LP_I2S_PDM_RX_SLOT_DEFAULT_CONFIG` is an initialization helper.

Read Data via LP I2S
^^^^^^^^^^^^^^^^^^^^

After the LP I2S channel is enabled, :cpp:func:`lp_i2s_channel_read` and :cpp:func:`lp_i2s_channel_read_until_bytes` will be available.

.. list::

    - For :cpp:func:`lp_i2s_channel_read`, if there are new data received by the LP I2S channel, this API will move the received data to the ``buffer`` you specified in :cpp:type:`lp_i2s_trans_t`. The API will try to receive the data as the ``buflen`` you specified. Check the ``received_size`` to know how many bytes you received, in case there are no enough received data. If no new received data, the API will block until ``timeout_ms``.

    - For :cpp:func:`lp_i2s_channel_read_until_bytes`, this API is a wrapper of the :cpp:func:`lp_i2s_channel_read`. The difference is, the :cpp:func:`lp_i2s_channel_read_until_bytes` will block until ``buflen`` bytes are received.

    - For both of the two APIs, if :cpp:member:`lp_i2s_evt_cbs_t::on_request_new_trans` is set, the driver will each time requesting a new LP I2S transaction descriptor (:cpp:type:`lp_i2s_trans_t`) from the callback event data structure (:cpp:type:`lp_i2s_evt_data_t`). This also means, the ``buffer`` in the (:cpp:type:`lp_i2s_trans_t`) needs to be ready for receiving data.


Thread Safety
^^^^^^^^^^^^^

All the APIs are guaranteed to be thread safe by the driver, which means, you can call them from different RTOS tasks without protection by extra locks.

All the APIs are not allowed to be used in ISR context.


API Reference
-------------

.. include-build-file:: inc/lp_i2s.inc
.. include-build-file:: inc/lp_i2s_std.inc
.. include-build-file:: inc/lp_i2s_pdm.inc
