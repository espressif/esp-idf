Remote Control Transceiver (RMT)
================================

Introduction
------------

The RMT (Remote Control Transceiver) peripheral was designed to act as an infrared transceiver. However, due to the flexibility of its data format, the functionality of RMT can be extended to a versatile and general purpose transceiver. From the perspective of network layering, the RMT hardware contains both physical and data link layer. The physical layer defines the communication media and bit signal representation. The data link layer defines the format of an RMT frame. The minimal data unit in the frame is called **RMT symbol**, which is represented by :cpp:type:`rmt_symbol_word_t` in the driver.

{IDF_TARGET_NAME} contains multiple channels in the RMT peripheral. [1]_ Each channel can be configured as either transmitter or receiver, independently.

Typically, the RMT peripheral can be used in the following scenarios:

-  Transmit or receive infrared signals, with any IR protocols, e.g. NEC
-  General purpose sequence generator
-  Transmit signals in a hardware controlled loop, with finite or infinite number of times
-  Multi-channel simultaneous transmission
-  Modulate the carrier to the output signal or demodulate the carrier from the input signal

Layout of RMT Symbols
^^^^^^^^^^^^^^^^^^^^^

The RMT hardware defines data in its own pattern -- the **RMT symbol**. Each symbol consists of two pairs of two values. The first value in a pair describes the signal duration in RMT ticks and is 15 bits long. The second provides the signal level (high or low) and is contained in a single bit, as shown below:

.. packetdiag:: /../_static/diagrams/rmt/rmt_symbols.diag
    :caption: Structure of RMT symbols (L - signal level)
    :align: center

RMT Transmitter Overview
^^^^^^^^^^^^^^^^^^^^^^^^

The data path and control path of an RMT TX channel is illustrated in the figure below:

.. blockdiag:: /../_static/diagrams/rmt/rmt_tx.diag
    :caption: RMT Transmitter Overview
    :align: center

The driver will encode user's data into RMT data format, then the RMT transmitter can generate the waveforms according to the encoding artifacts. It is also possible to modulate a high frequency carrier signal before being routed to a GPIO pad.

RMT Receiver Overview
^^^^^^^^^^^^^^^^^^^^^

The data path and control path of an RMT RX channel is illustrated in the figure below:

.. blockdiag:: /../_static/diagrams/rmt/rmt_rx.diag
    :caption: RMT Receiver Overview
    :align: center

The RMT receiver can sample incoming signals into RMT data format, and store the data in memory. It's feasible to tell the receiver the basic characteristics of the incoming signal, so that the signal's stop condition can be recognized, and signal glitches and noise can be filtered out. The RMT peripheral also supports demodulating the high frequency carrier from the base signal.

Functional Overview
-------------------

Description of the RMT functionality is divided into the following sections:

-  `Resource Allocation <#resource-allocation>`__ - covers how to allocate RMT channels with properly set of configurations. It also covers how to recycle the resources when they finished working.
-  `Carrier Modulation and Demodulation <#carrier-modulation-and-demodulation>`__ - describes how to modulate carrier for TX channel and demodulate carrier for RX channel.
-  `Register Event Callbacks <#register-event-callbacks>`__ - covers how to hook user specific code to RMT channel specific events.
-  `Enable and Disable channel <#enable-and-disable-channel>`__ - shows how to enable and disable the RMT channel.
-  `Initiate TX Transaction <#initiate-tx-transaction>`__ - describes the steps to initiate a transaction for TX channel.
-  `Initiate RX Transaction <#initiate-rx-transaction>`__ - describes the steps to initiate a transaction for RX channel.
-  `Multiple Channels Simultaneous Transmission <#multiple-channels-simultaneous-transmission>`__ - describes how to collect multiple channels into a sync group and start transaction at the same time.
-  `RMT Encoder <#rmt-encoder>`__ - focuses on how to write a customized encoder in a combination way, with the help of the primitive encoders provided by the driver.
-  `Power Management <#power-management>`__ - describes how different source clock will affect power consumption.
-  `IRAM Safe <#iram-safe>`__ - describes tips on how to make the RMT interrupt work better along with a disabled cache.
-  `Thread Safety <#thread-safety>`__ - lists which APIs are guaranteed to be thread safe by the driver.
-  `Kconfig Options <#kconfig-options>`__ - lists the supported Kconfig options that can bring different effects to the driver.

Resource Allocation
^^^^^^^^^^^^^^^^^^^

Both RMT TX and RX channels are represented by :cpp:type:`rmt_channel_handle_t` in the driver. The available channels are managed in a resource pool, which will hand out a free channel on request.

Install RMT TX Channel
~~~~~~~~~~~~~~~~~~~~~~

To install an RMT TX channel, there's a configuration structure that needs to be given in advance: :cpp:type:`rmt_tx_channel_config_t`:

-  :cpp:member:`rmt_tx_channel_config_t::gpio_num` sets the GPIO number used by the transmitter.
-  :cpp:member:`rmt_tx_channel_config_t::clk_src` selects the source clock for the RMT channel. The available clocks are listed in :cpp:type:`rmt_clock_source_t`. Note that, the selected clock will also be used by other channels, which means user should ensure this configuration is same when allocating other channels, regardless of TX or RX. For the effect on power consumption of different clock source, please refer to `Power Management <#power-management>`__  section.
-  :cpp:member:`rmt_tx_channel_config_t::resolution_hz` sets the resolution of the internal tick counter. The timing parameter of RMT signal is calculated based on this **tick**.
-  :cpp:member:`rmt_tx_channel_config_t::mem_block_symbols` sets the size of the dedicated memory block or DMA buffer that is used to store RMT encoding artifacts.
-  :cpp:member:`rmt_tx_channel_config_t::trans_queue_depth` sets the depth of internal transaction queue, the deeper the queue, the more transactions can be prepared in the backlog.
-  :cpp:member:`rmt_tx_channel_config_t::invert_out` is used to decide whether to invert the RMT signal before sending it to the GPIO pad.
-  :cpp:member:`rmt_tx_channel_config_t::with_dma` is used to indicate if the channel needs a DMA backend. A channel with DMA attached can offload the CPU by a lot. However, DMA backend is not available on all ESP chips, please refer to [`TRM <{IDF_TARGET_TRM_EN_URL}#rmt>`__] before you enable this option. Or you might encounter :c:macro:`ESP_ERR_NOT_SUPPORTED` error.
-  :cpp:member:`rmt_tx_channel_config_t::io_loop_back` enables both the GPIO's input and output ability through the GPIO matrix peripheral. Meanwhile, if both TX and RX channels are bound to the same GPIO, then monitoring of the data transmission line can be realized.
-  :cpp:member:`rmt_tx_channel_config_t::io_od_mode` configures the GPIO as open-drain mode. It is useful for simulating bi-directional buses, sucn as 1-wire bus, combined with :cpp:member:`rmt_tx_channel_config_t::io_loop_back`.

Once the :cpp:type:`rmt_tx_channel_config_t` structure is populated with mandatory parameters, users can call :cpp:func:`rmt_new_tx_channel` to allocate and initialize a TX channel. This function will return an RMT channel handle if it runs correctly. Specifically, when there are no more free channels in the RMT resource pool, this function will return :c:macro:`ESP_ERR_NOT_FOUND` error. If some feature (e.g. DMA backend) is not supported by hardware, it will return :c:macro:`ESP_ERR_NOT_SUPPORTED` error.

.. code:: c

    rmt_channel_handle_t tx_chan = NULL;
    rmt_tx_channel_config_t tx_chan_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,       // select source clock
        .gpio_num = 0,                    // GPIO number
        .mem_block_symbols = 64,          // memory block size, 64 * 4 = 256Bytes
        .resolution_hz = 1 * 1000 * 1000, // 1MHz tick resolution, i.e. 1 tick = 1us
        .trans_queue_depth = 4,           // set the number of transactions that can pend in the background
        .flags.invert_out = false,        // don't invert output signal
        .flags.with_dma = false,          // don't need DMA backend
    };
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &tx_chan));

Install RMT RX Channel
~~~~~~~~~~~~~~~~~~~~~~

To install an RMT RX channel, there's a configuration structure that needs to be given in advance: :cpp:type:`rmt_rx_channel_config_t`:

-  :cpp:member:`rmt_rx_channel_config_t::gpio_num` sets the GPIO number used by the receiver.
-  :cpp:member:`rmt_rx_channel_config_t::clk_src` selects the source clock for the RMT channel. The available clocks are listed in :cpp:type:`rmt_clock_source_t`. Note that, the selected clock will also be used by other channels, which means user should ensure this configuration is same when allocating other channels, regardless of TX or RX. For the effect on power consumption of different clock source, please refer to `Power Management <#power-management>`__  section.
-  :cpp:member:`rmt_rx_channel_config_t::resolution_hz` sets the resolution of the internal tick counter. The timing parameter of RMT signal is calculated based on this **tick**.
-  :cpp:member:`rmt_rx_channel_config_t::mem_block_symbols` sets the size of the dedicated memory block or DMA buffer that used to store RMT encoding artifacts.
-  :cpp:member:`rmt_rx_channel_config_t::invert_in` is used to decide whether to invert the input signals before they going into RMT receiver. The inversion is done by GPIO matrix instead of by the RMT peripheral.
-  :cpp:member:`rmt_rx_channel_config_t::with_dma` is used to indicate if the channel needs a DMA backend. A channel with DMA attached can offload the CPU by a lot. However, DMA backend is not available on all ESP chips, please refer to [`TRM <{IDF_TARGET_TRM_EN_URL}#rmt>`__] before you enable this option. Or you might encounter :c:macro:`ESP_ERR_NOT_SUPPORTED` error.
-  :cpp:member:`rmt_rx_channel_config_t::io_loop_back` is for debugging purposes only. It enables both the GPIO's input and output ability through the GPIO matrix peripheral. Meanwhile, if both TX and RX channels are bound to the same GPIO, then monitoring of the data transmission line can be realized.

Once the :cpp:type:`rmt_rx_channel_config_t` structure is populated with mandatory parameters, users can call :cpp:func:`rmt_new_rx_channel` to allocate and initialize a RX channel. This function will return an RMT channel handle if it runs correctly. Specifically, when there are no more free channels in the RMT resource pool, this function will return :c:macro:`ESP_ERR_NOT_FOUND` error. If some feature (e.g. DMA backend) is not supported by hardware, it will return :c:macro:`ESP_ERR_NOT_SUPPORTED` error.

.. code:: c

    rmt_channel_handle_t rx_chan = NULL;
    rmt_rx_channel_config_t rx_chan_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,       // select source clock
        .resolution_hz = 1 * 1000 * 1000, // 1MHz tick resolution, i.e. 1 tick = 1us
        .mem_block_symbols = 64,          // memory block size, 64 * 4 = 256Bytes
        .gpio_num = 2,                    // GPIO number
        .flags.invert_in = false,         // don't invert input signal
        .flags.with_dma = false,          // don't need DMA backend
    };
    ESP_ERROR_CHECK(rmt_new_rx_channel(&rx_chan_config, &rx_chan));

Uninstall RMT Channel
~~~~~~~~~~~~~~~~~~~~~

If a previously installed RMT channel is no longer needed, it's recommended to recycle the resources by calling :cpp:func:`rmt_del_channel`, which in return allows the underlying hardware to be usable for other purposes.

Carrier Modulation and Demodulation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The RMT transmitter can generate a carrier wave and modulate it onto the base signal. Compared to the base signal, the carrier frequency is usually high. In addition, user can only set the frequency and duty cycle for the carrier. The RMT receiver can demodulate the carrier from the incoming signal. Note that, carrier modulation and demodulation is not supported on all ESP chips, please refer to [`TRM <{IDF_TARGET_TRM_EN_URL}#rmt>`__] before configuring the carrier, or you might encounter a :c:macro:`ESP_ERR_NOT_SUPPORTED` error.

Carrier related configurations lie in :cpp:type:`rmt_carrier_config_t`:

-  :cpp:member:`rmt_carrier_config_t::frequency_hz` sets the carrier frequency, in Hz.
-  :cpp:member:`rmt_carrier_config_t::duty_cycle` sets the carrier duty cycle.
-  :cpp:member:`rmt_carrier_config_t::polarity_active_low` sets the carrier polarity, i.e. on which level the carrier is applied.
-  :cpp:member:`rmt_carrier_config_t::always_on` sets whether to output the carrier even when the data transmission has finished. This configuration is only valid for TX channel.

.. note::

    For RX channel, we shouldn't set the carrier frequency exactly to the theoretical value. It's recommended to leave a tolerance for the carrier frequency. For example, in the snippet below, we set the frequency to 25KHz, instead of the 38KHz that configured on the TX side. The reason is that reflection and refraction will occur when a signal travels through the air, leading to the a distortion on the receiver side.

.. code:: c

    rmt_carrier_config_t tx_carrier_cfg = {
        .duty_cycle = 0.33,                 // duty cycle 33%
        .frequency_hz = 38000,              // 38KHz
        .flags.polarity_active_low = false, // carrier should modulated to high level
    };
    // modulate carrier to TX channel
    ESP_ERROR_CHECK(rmt_apply_carrier(tx_chan, &tx_carrier_cfg));

    rmt_carrier_config_t rx_carrier_cfg = {
        .duty_cycle = 0.33,                 // duty cycle 33%
        .frequency_hz = 25000,              // 25KHz carrier, should be smaller than transmitter's carrier frequency
        .flags.polarity_active_low = false, // the carrier is modulated to high level
    };
    // demodulate carrier from RX channel
    ESP_ERROR_CHECK(rmt_apply_carrier(rx_chan, &rx_carrier_cfg));

Register Event Callbacks
^^^^^^^^^^^^^^^^^^^^^^^^

When an RMT channel finishes transmitting or receiving, a specific event will be generated and notify the CPU by interrupt. If you have some function that needs to be called when those events occurred, you can hook your function to the ISR (Interrupt Service Routine) by calling :cpp:func:`rmt_tx_register_event_callbacks` and :cpp:func:`rmt_rx_register_event_callbacks` for TX and RX channel respectively. Since the registered callback functions are called in the interrupt context, user should ensure the callback function doesn't attempt to block (e.g. by making sure that only FreeRTOS APIs with ``ISR`` suffix are called from within the function). The callback function has a boolean return value, to tell the caller whether a high priority task is woke up by it.

TX channel supported event callbacks are listed in the :cpp:type:`rmt_tx_event_callbacks_t`:

-  :cpp:member:`rmt_tx_event_callbacks_t::on_trans_done` sets a callback function for trans done event. The function prototype is declared in :cpp:type:`rmt_tx_done_callback_t`.

RX channel supported event callbacks are listed in the :cpp:type:`rmt_rx_event_callbacks_t`:

-  :cpp:member:`rmt_rx_event_callbacks_t::on_recv_done` sets a callback function for receive complete event. The function prototype is declared in :cpp:type:`rmt_rx_done_callback_t`.

User can save own context in :cpp:func:`rmt_tx_register_event_callbacks` and :cpp:func:`rmt_rx_register_event_callbacks` as well, via the parameter ``user_data``. The user data will be directly passed to each callback function.

In the callback function, users can fetch the event specific data that is filled by the driver in the ``edata``. Note that the ``edata`` pointer is only valid for the duration of the callback.

The TX done event data is defined in :cpp:type:`rmt_tx_done_event_data_t`:

- :cpp:member:`rmt_tx_done_event_data_t::num_symbols` tells the number of transmitted RMT symbols. This also reflects the size of encoding artifacts.

The RX complete event data is defined in :cpp:type:`rmt_rx_done_event_data_t`:

-  :cpp:member:`rmt_rx_done_event_data_t::received_symbols` points to the received RMT symbols. These symbols are saved in the ``buffer`` parameter of :cpp:func:`rmt_receive` function. User shouldn't free this receive buffer before the callback returns.
-  :cpp:member:`rmt_rx_done_event_data_t::num_symbols` tells the number of received RMT symbols. This value won't be bigger than ``buffer_size`` parameter of :cpp:func:`rmt_receive` function. If the ``buffer_size`` is not sufficient to accommodate all the received RMT symbols, the driver will truncate it.

Enable and Disable channel
^^^^^^^^^^^^^^^^^^^^^^^^^^

:cpp:func:`rmt_enable` must be called in advanced before transmitting or receiving RMT symbols. For transmitters, enabling a channel will enable a specific interrupt and prepare the hardware to dispatch transactions. For RX channels, enabling a channel will enable an interrupt, but the receiver is not started during this time, as it has no idea about the characteristics of the incoming signals. The receiver will be started in :cpp:func:`rmt_receive`.

:cpp:func:`rmt_disable` does the opposite work by disabling the interrupt and clearing pending status. The transmitter and receiver will be disabled as well.

.. code:: c

    ESP_ERROR_CHECK(rmt_enable(tx_chan));
    ESP_ERROR_CHECK(rmt_enable(rx_chan));

Initiate TX Transaction
^^^^^^^^^^^^^^^^^^^^^^^

RMT is a special communication peripheral as it's unable to transmit raw byte streams like SPI and I2C. RMT can only send data in its own format :cpp:type:`rmt_symbol_word_t`. However, the hardware doesn't help to convert the user data into RMT symbols, this can only be done in software --- by the so-called **RMT Encoder**. The encoder is responsible for encoding user data into RMT symbols and then write to RMT memory block or DMA buffer. For how to create an RMT encoder, please refer to `RMT Encoder <#rmt-encoder>`__.

Once we got an encoder, we can initiate a TX transaction by calling :cpp:func:`rmt_transmit`. This function takes several positional parameters like channel handle, encoder handle, payload buffer. Besides that, we also need to provide a transmission specific configuration in :cpp:type:`rmt_transmit_config_t`:

-  :cpp:member:`rmt_transmit_config_t::loop_count` sets the number of transmission loop. After the transmitter finished one round of transmission, it can restart the same transmission again if this value is not set to zero. As the loop is controlled by hardware, the RMT channel can be used to generate many periodic sequences at the cost of a very little CPU intervention. Specially, setting :cpp:member:`rmt_transmit_config_t::loop_count` to `-1` means an infinite loop transmission. In this situation, the channel won't stop until manually call of :cpp:func:`rmt_disable`. And the trans done event won't be generated as well. If :cpp:member:`rmt_transmit_config_t::loop_count` is set to a positive number, the trans done event won't be generated until target number of loop transmission have finished. Note that, the **loop transmit** feature is not supported on all ESP chips, please refer to [`TRM <{IDF_TARGET_TRM_EN_URL}#rmt>`__] before you configure this option. Or you might encounter :c:macro:`ESP_ERR_NOT_SUPPORTED` error.
-  :cpp:member:`rmt_transmit_config_t::eot_level` sets the output level when the transmitter finishes working or stops working by calling :cpp:func:`rmt_disable`.

.. note::

    There's a limitation in the transmission size if the :cpp:member:`rmt_transmit_config_t::loop_count` is set to non-zero (i.e. to enable the loop feature). The encoded RMT symbols should not exceed the capacity of RMT hardware memory block size. Or you might see error message like ``encoding artifacts can't exceed hw memory block for loop transmission``. If you have to start a large transaction by loop, you can try either:

    - Increase the :cpp:member:`rmt_tx_channel_config_t::mem_block_symbols`. This approach doesn't work if the DMA backend is also enabled.
    - Customize an encoder and construct a forever loop in the encoding function. See also `RMT Encoder <#rmt-encoder>`__.

Internally, :cpp:func:`rmt_transmit` will construct a transaction descriptor and send to a job queue, which will be dispatched in the ISR. So it is possible that the transaction is not started yet when :cpp:func:`rmt_transmit` returns. To ensure all pending transaction to complete, user can use :cpp:func:`rmt_tx_wait_all_done`.

Multiple Channels Simultaneous Transmission
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In some real-time control applications, we don't want any time drift in between when startup multiple TX channels. For example, to make two robotic arms move simultaneously. The RMT driver can help to manage this by creating a so-called **Sync Manager**. The sync manager is represented by :cpp:type:`rmt_sync_manager_handle_t` in the driver. The procedure of RMT sync transmission is shown as follows:

.. figure:: /../_static/rmt_tx_sync.png
    :align: center
    :alt: RMT TX Sync

    RMT TX Sync

Install RMT Sync Manager
~~~~~~~~~~~~~~~~~~~~~~~~

To create a sync manager, user needs to tell which channels are going to be managed in the :cpp:type:`rmt_sync_manager_config_t`:

-  :cpp:member:`rmt_sync_manager_config_t::tx_channel_array` points to the array of TX channels to be managed.
-  :cpp:member:`rmt_sync_manager_config_t::array_size` sets the number of channels to be managed.

:cpp:func:`rmt_new_sync_manager` can return a manager handle on success. This function could also fail due to various errors such as invalid arguments, etc. Specially, when the sync manager has been installed before, and there're no hardware resources to create another manager, this function will report :c:macro:`ESP_ERR_NOT_FOUND` error. In addition, if the sync manager is not supported by the hardware, it will report :c:macro:`ESP_ERR_NOT_SUPPORTED` error. Please refer to [`TRM <{IDF_TARGET_TRM_EN_URL}#rmt>`__] before using the sync manager feature.

Start Transmission Simultaneously
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For any managed TX channel, it won't start the machine until all the channels in the :cpp:member:`rmt_sync_manager_config_t::tx_channel_array` are called with :cpp:func:`rmt_transmit`. Before that, the channel is just put in a waiting state. Different channel usually take different time to finish the job if the transaction is different, which results in a loss of sync. So user needs to call :cpp:func:`rmt_sync_reset` to pull the channels back to the starting line again before restarting a simultaneous transmission.

Calling :cpp:func:`rmt_del_sync_manager` can recycle the sync manager and enable the channels to initiate transactions independently afterwards.

.. code:: c

    rmt_channel_handle_t tx_channels[2] = {NULL}; // declare two channels
    int tx_gpio_number[2] = {0, 2};
    // install channels one by one
    for (int i = 0; i < 2; i++) {
        rmt_tx_channel_config_t tx_chan_config = {
            .clk_src = RMT_CLK_SRC_DEFAULT,       // select source clock
            .gpio_num = tx_gpio_number[i],    // GPIO number
            .mem_block_symbols = 64,          // memory block size, 64 * 4 = 256Bytes
            .resolution_hz = 1 * 1000 * 1000, // 1MHz resolution
            .trans_queue_depth = 1,           // set the number of transactions that can pend in the background
        };
        ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &tx_channels[i]));
    }
    // install sync manager
    rmt_sync_manager_handle_t synchro = NULL;
    rmt_sync_manager_config_t synchro_config = {
        .tx_channel_array = tx_channels,
        .array_size = sizeof(tx_channels) / sizeof(tx_channels[0]),
    };
    ESP_ERROR_CHECK(rmt_new_sync_manager(&synchro_config, &synchro));

    ESP_ERROR_CHECK(rmt_transmit(tx_channels[0], led_strip_encoders[0], led_data, led_num * 3, &transmit_config));
    // tx_channels[0] won't start transmission until call of `rmt_transmit()` for tx_channels[1] returns
    ESP_ERROR_CHECK(rmt_transmit(tx_channels[1], led_strip_encoders[1], led_data, led_num * 3, &transmit_config));

Initiate RX Transaction
^^^^^^^^^^^^^^^^^^^^^^^

As also discussed in the `Enable and Disable channel <#enable-and-disable-channel>`__, the RX channel still doesn't get ready to receive RMT symbols even user calls :cpp:func:`rmt_enable`. User needs to specify the basic characteristics of the incoming signals in :cpp:type:`rmt_receive_config_t`:

-  :cpp:member:`rmt_receive_config_t::signal_range_min_ns` specifies the minimal valid pulse duration (either high or low level). A pulse whose width is smaller than this value will be treated as glitch and ignored by the hardware.
-  :cpp:member:`rmt_receive_config_t::signal_range_max_ns` specifies the maximum valid pulse duration (either high or low level). A pulse whose width is bigger than this value will be treated as **Stop Signal**, and the receiver will generate receive complete event immediately.

The RMT receiver will start the RX machine after user calls :cpp:func:`rmt_receive` with the provided configuration above. Note that, this configuration is transaction specific, which means, to start a new round of reception, user needs to sets the :cpp:type:`rmt_receive_config_t` again. The receiver saves the incoming signals into its internal memory block or DMA buffer, in the format of :cpp:type:`rmt_symbol_word_t`.

.. only:: SOC_RMT_SUPPORT_RX_PINGPONG

    Due to the limited size of memory block, the RMT receiver will notify the driver to copy away the accumulated symbols in a ping-pong way.

.. only:: not SOC_RMT_SUPPORT_RX_PINGPONG

    Due to the limited size of memory block, the RMT receiver can only save short frames whose length is not longer than the memory block capacity. Long frames will be truncated by the hardware, and the driver will report an error message: ``hw buffer too small, received symbols truncated``.

The copy destination should be provided in the ``buffer`` parameter of :cpp:func:`rmt_receive` function. If this buffer size is not sufficient, the receiver can continue to work but later incoming symbols will be dropped and report an error message: ``user buffer too small, received symbols truncated``. Please take care of the lifecycle of the ``buffer`` parameter, user shouldn't recycle the buffer before the receiver finished or stopped working.

The receiver will be stopped by the driver when it finishes working (i.e. received a signal whose duration is bigger than :cpp:member:`rmt_receive_config_t::signal_range_max_ns`). User needs to call :cpp:func:`rmt_receive` again to restart the receiver, is necessary. User can get the received data in the :cpp:member:`rmt_rx_event_callbacks_t::on_recv_done` callback. See also `Register Event Callbacks <#register-event-callbacks>`__ for more information.

.. code:: c

    static bool example_rmt_rx_done_callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data)
    {
        BaseType_t high_task_wakeup = pdFALSE;
        QueueHandle_t receive_queue = (QueueHandle_t)user_data;
        // send the received RMT symbols to the parser task
        xQueueSendFromISR(receive_queue, edata, &high_task_wakeup);
        // return whether any task is woken up
        return high_task_wakeup == pdTRUE;
    }

    QueueHandle_t receive_queue = xQueueCreate(1, sizeof(rmt_rx_done_event_data_t));
    rmt_rx_event_callbacks_t cbs = {
        .on_recv_done = example_rmt_rx_done_callback,
    };
    ESP_ERROR_CHECK(rmt_rx_register_event_callbacks(rx_channel, &cbs, receive_queue));

    // the following timing requirement is based on NEC protocol
    rmt_receive_config_t receive_config = {
        .signal_range_min_ns = 1250,     // the shortest duration for NEC signal is 560us, 1250ns < 560us, valid signal won't be treated as noise
        .signal_range_max_ns = 12000000, // the longest duration for NEC signal is 9000us, 12000000ns > 9000us, the receive won't stop early
    };

    rmt_symbol_word_t raw_symbols[64]; // 64 symbols should be sufficient for a standard NEC frame
    // ready to receive
    ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols), &receive_config));
    // wait for RX done signal
    rmt_rx_done_event_data_t rx_data;
    xQueueReceive(receive_queue, &rx_data, portMAX_DELAY);
    // parse the receive symbols
    example_parse_nec_frame(rx_data.received_symbols, rx_data.num_symbols);

RMT Encoder
^^^^^^^^^^^

An RMT encoder is part of the RMT TX transaction, whose responsibility is to generate and write the correct RMT symbols into hardware memory (or DMA buffer) at specific time. There're some special restrictions for an encoding function:

- An encoding function might be called for several times within a single transaction. This is because the target RMT memory block can't accommodate all the artifacts at once. We have to use the memory in a **ping-pong** way, thus the encoding session is divided into multiple parts. This requires the encoder to be **stateful**.
- The encoding function is running in the ISR context. To speed up the encoding session, it's high recommend to put the encoding function into IRAM. This can also avoid the cache miss during encoding.

To help get started with RMT driver faster, some commonly used encoders are provided out-of-the box. They can either work alone or chained together into a new encoder. See also `Composite Pattern <https://en.wikipedia.org/wiki/Composite_pattern>`__ for the principle behind. The driver has defined the encoder interface in :cpp:type:`rmt_encoder_t`, it contains the following functions:

-  :cpp:member:`rmt_encoder_t::encode` is the fundamental function of an encoder. This is where the encoding session happens. Please note, the :cpp:member:`rmt_encoder_t::encode` function might be called for multiple times within a single transaction. The encode function should return the state of current encoding session. The supported states are listed in the  :cpp:type:`rmt_encode_state_t`. If the result contains :cpp:enumerator:`RMT_ENCODING_COMPLETE`, it means the current encoder has finished work. If the result contains :cpp:enumerator:`RMT_ENCODING_MEM_FULL`, we need to yield from current session, as there's no space to save more encoding artifacts.
-  :cpp:member:`rmt_encoder_t::reset` should reset the encoder state back to initial. The RMT encoder is stateful, if RMT transmitter stopped manually without its corresponding encoder being reset, then the following encoding session can be wrong. This function is also called implicitly in :cpp:func:`rmt_disable`.
-  :cpp:member:`rmt_encoder_t::del` function should free the resources allocated by the encoder.

Copy Encoder
~~~~~~~~~~~~

A copy encoder is created by calling :cpp:func:`rmt_new_copy_encoder`. Copy encoder's main functionality is to copy the RMT symbols from user space into the driver layer. It's usually used to encode const data (i.e. data won't change at runtime after initialization), for example, the leading code in the IR protocol.

A configuration structure :cpp:type:`rmt_copy_encoder_config_t` should be provided in advance before calling :cpp:func:`rmt_new_copy_encoder`. Currently, this configuration is reserved for future expansion.

Bytes Encoder
~~~~~~~~~~~~~

A bytes encoder is created by calling :cpp:func:`rmt_new_bytes_encoder`. Bytes encoder's main functionality is to convert the user space byte stream into RMT symbols dynamically. It's usually used to encode dynamic data, for example, the address and command fields in the IR protocol.

A configuration structure :cpp:type:`rmt_bytes_encoder_config_t` should be provided in advance before calling :cpp:func:`rmt_new_bytes_encoder`:

-  :cpp:member:`rmt_bytes_encoder_config_t::bit0` and :cpp:member:`rmt_bytes_encoder_config_t::bit1` are necessary to tell to the encoder how to represent bit zero and bit one in the format of :cpp:type:`rmt_symbol_word_t`.
-  :cpp:member:`rmt_bytes_encoder_config_t::msb_first` sets the encoding order for of byte. If it is set to true, the encoder will encode the **Most Significant Bit** first. Otherwise, it will encode the **Least Significant Bit** first.

Besides the primitive encoders provided by the driver, user can implement his own encoder by chaining the existing encoders together. A common encoder chain is shown as follows:

.. blockdiag:: /../_static/diagrams/rmt/rmt_encoder_chain.diag
    :caption: RMT Encoder Chain
    :align: center

Customize RMT Encoder for NEC Protocol
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In this section, we will demonstrate on how to write an NEC encoder. The NEC IR protocol uses pulse distance encoding of the message bits. Each pulse burst is `562.5µs` in length, logical bits are transmitted as follows. It is worth mentioning, the bytes of data bits are sent least significant bit first.

-  Logical ``0``: a `562.5µs` pulse burst followed by a `562.5µs` space, with a total transmit time of `1.125ms`
-  Logical ``1``: a `562.5µs` pulse burst followed by a `1.6875ms` space, with a total transmit time of `2.25ms`

When a key is pressed on the remote controller, the message transmitted consists of the following, in order:

.. figure:: /../_static/ir_nec.png
    :align: center
    :alt: IR NEC Frame

    IR NEC Frame

-  `9ms` leading pulse burst (also called the "AGC pulse")
-  `4.5ms` space
-  8-bit address for the receiving device
-  8-bit logical inverse of the address
-  8-bit command
-  8-bit logical inverse of the command
-  a final `562.5µs` pulse burst to signify the end of message transmission

Then we can construct the NEC :cpp:member:`rmt_encoder_t::encode` function in the same order, for example:

.. code:: c

    // IR NEC scan code representation
    typedef struct {
        uint16_t address;
        uint16_t command;
    } ir_nec_scan_code_t;

    // construct a encoder by combining primitive encoders
    typedef struct {
        rmt_encoder_t base;           // the base "class", declares the standard encoder interface
        rmt_encoder_t *copy_encoder;  // use the copy_encoder to encode the leading and ending pulse
        rmt_encoder_t *bytes_encoder; // use the bytes_encoder to encode the address and command data
        rmt_symbol_word_t nec_leading_symbol; // NEC leading code with RMT representation
        rmt_symbol_word_t nec_ending_symbol;  // NEC ending code with RMT representation
        int state; // record the current encoding state (i.e. we're in which encoding phase)
    } rmt_ir_nec_encoder_t;

    static size_t rmt_encode_ir_nec(rmt_encoder_t *encoder, rmt_channel_handle_t channel, const void *primary_data, size_t data_size, rmt_encode_state_t *ret_state)
    {
        rmt_ir_nec_encoder_t *nec_encoder = __containerof(encoder, rmt_ir_nec_encoder_t, base);
        rmt_encode_state_t session_state = 0;
        rmt_encode_state_t state = 0;
        size_t encoded_symbols = 0;
        ir_nec_scan_code_t *scan_code = (ir_nec_scan_code_t *)primary_data;
        rmt_encoder_handle_t copy_encoder = nec_encoder->copy_encoder;
        rmt_encoder_handle_t bytes_encoder = nec_encoder->bytes_encoder;
        switch (nec_encoder->state) {
        case 0: // send leading code
            encoded_symbols += copy_encoder->encode(copy_encoder, channel, &nec_encoder->nec_leading_symbol,
                                                    sizeof(rmt_symbol_word_t), &session_state);
            if (session_state & RMT_ENCODING_COMPLETE) {
                nec_encoder->state = 1; // we can only switch to next state when current encoder finished
            }
            if (session_state & RMT_ENCODING_MEM_FULL) {
                state |= RMT_ENCODING_MEM_FULL;
                goto out; // yield if there's no free space to put other encoding artifacts
            }
        // fall-through
        case 1: // send address
            encoded_symbols += bytes_encoder->encode(bytes_encoder, channel, &scan_code->address, sizeof(uint16_t), &session_state);
            if (session_state & RMT_ENCODING_COMPLETE) {
                nec_encoder->state = 2; // we can only switch to next state when current encoder finished
            }
            if (session_state & RMT_ENCODING_MEM_FULL) {
                state |= RMT_ENCODING_MEM_FULL;
                goto out; // yield if there's no free space to put other encoding artifacts
            }
        // fall-through
        case 2: // send command
            encoded_symbols += bytes_encoder->encode(bytes_encoder, channel, &scan_code->command, sizeof(uint16_t), &session_state);
            if (session_state & RMT_ENCODING_COMPLETE) {
                nec_encoder->state = 3; // we can only switch to next state when current encoder finished
            }
            if (session_state & RMT_ENCODING_MEM_FULL) {
                state |= RMT_ENCODING_MEM_FULL;
                goto out; // yield if there's no free space to put other encoding artifacts
            }
        // fall-through
        case 3: // send ending code
            encoded_symbols += copy_encoder->encode(copy_encoder, channel, &nec_encoder->nec_ending_symbol,
                                                    sizeof(rmt_symbol_word_t), &session_state);
            if (session_state & RMT_ENCODING_COMPLETE) {
                nec_encoder->state = 0; // back to the initial encoding session
                state |= RMT_ENCODING_COMPLETE; // telling the caller the NEC encoding has finished
            }
            if (session_state & RMT_ENCODING_MEM_FULL) {
                state |= RMT_ENCODING_MEM_FULL;
                goto out; // yield if there's no free space to put other encoding artifacts
            }
        }
    out:
        *ret_state = state;
        return encoded_symbols;
    }

A full sample code can be found in :example:`peripherals/rmt/ir_nec_transceiver`. In the above snippet, we use a ``switch-case`` plus several ``goto`` statements to implement a `state machine <https://en.wikipedia.org/wiki/Finite-state_machine>`__ . With this pattern, user can construct a lot more complex IR protocols.

Power Management
^^^^^^^^^^^^^^^^

When power management is enabled (i.e. :ref:`CONFIG_PM_ENABLE` is on), the system will adjust the APB frequency before going into light sleep, thus potentially changing the resolution of RMT internal counter.

However, the driver can prevent the system from changing APB frequency by acquiring a power management lock of type :cpp:enumerator:`ESP_PM_APB_FREQ_MAX`. Whenever user creates an RMT channel that has selected :cpp:enumerator:`RMT_CLK_SRC_APB` as the clock source, the driver will guarantee that the power management lock is acquired after the channel enabled by :cpp:func:`rmt_enable`. Likewise, the driver releases the lock after :cpp:func:`rmt_disable` is called for the same channel. This also reveals that the :cpp:func:`rmt_enable` and :cpp:func:`rmt_disable` should appear in pairs.

If the channel clock source is selected to others like :cpp:enumerator:`RMT_CLK_SRC_XTAL`, then the driver won't install power management lock for it, which is more suitable for a low power application as long as the source clock can still provide sufficient resolution.

IRAM Safe
^^^^^^^^^

By default, the RMT interrupt will be deferred when the Cache is disabled for reasons like writing/erasing the main Flash. Thus the transaction done interrupt will not get executed in time, which is not expected in a real-time application. What's worse, when the RMT transaction relies on **ping-pong** interrupt to successively encode or copy RMT symbols, such delayed response can lead to an unpredictable result.

There's a Kconfig option :ref:`CONFIG_RMT_ISR_IRAM_SAFE` that will:

1. Enable the interrupt being serviced even when cache is disabled
2. Place all functions that used by the ISR into IRAM [2]_
3. Place driver object into DRAM (in case it's mapped to PSRAM by accident)

This Kconfig option will allow the interrupt to run while the cache is disabled but will come at the cost of increased IRAM consumption.

Thread Safety
^^^^^^^^^^^^^

The factory function :cpp:func:`rmt_new_tx_channel`, :cpp:func:`rmt_new_rx_channel` and :cpp:func:`rmt_new_sync_manager` are guaranteed to be thread safe by the driver, which means, user can call them from different RTOS tasks without protection by extra locks.
Other functions that take the :cpp:type:`rmt_channel_handle_t` and :cpp:type:`rmt_sync_manager_handle_t` as the first positional parameter, are not thread safe. which means the user should avoid calling them from multiple tasks.

Kconfig Options
^^^^^^^^^^^^^^^

- :ref:`CONFIG_RMT_ISR_IRAM_SAFE` controls whether the default ISR handler can work when cache is disabled, see also `IRAM Safe <#iram-safe>`__ for more information.
- :ref:`CONFIG_RMT_ENABLE_DEBUG_LOG` is used to enabled the debug log at the cost of increased firmware binary size.

Application Examples
--------------------

* RMT based RGB LED strip customized encoder: :example:`peripherals/rmt/led_strip`
* RMT IR NEC protocol encoding and decoding: :example:`peripherals/rmt/ir_nec_transceiver`
* RMT transactions in queue: :example:`peripherals/rmt/musical_buzzer`
* RMT based stepper motor with S-curve algorithm: : :example:`peripherals/rmt/stepper_motor`
* RMT infinite loop for driving DShot ESC: :example:`peripherals/rmt/dshot_esc`
* RMT simulate 1-wire protocol (take DS18B20 as example): :example:`peripherals/rmt/onewire_ds18b20`

API Reference
-------------

.. include-build-file:: inc/rmt_tx.inc
.. include-build-file:: inc/rmt_rx.inc
.. include-build-file:: inc/rmt_common.inc
.. include-build-file:: inc/rmt_encoder.inc
.. include-build-file:: inc/components/driver/include/driver/rmt_types.inc
.. include-build-file:: inc/components/hal/include/hal/rmt_types.inc


.. [1]
   Different ESP chip series might have different number of RMT channels. Please refer to the [`TRM <{IDF_TARGET_TRM_EN_URL}#rmt>`__] for details. The driver won't forbid you from applying for more RMT channels, but it will return error when there's no hardware resources available. Please always check the return value when doing `Resource Allocation <#resource-allocation>`__.

.. [2]
   Callback function (e.g. :cpp:member:`rmt_tx_event_callbacks_t::on_trans_done`) and the functions invoked by itself should also reside in IRAM, users need to take care of this by themselves.
