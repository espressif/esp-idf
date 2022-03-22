Inter-IC Sound (I2S)
====================

{IDF_TARGET_I2S_NUM:default="two", esp32s2="one", esp32c3="one"}

Overview
--------

I2S (Inter-IC Sound) is a serial, synchronous communication protocol that is usually used for transmitting audio data between two digital audio devices.

{IDF_TARGET_NAME} contains {IDF_TARGET_I2S_NUM} I2S peripheral(s). These peripherals can be configured to input and output sample data via the I2S driver.

An I2S bus consists of the following lines:

- Master clock line (operational)
- Bit clock line
- Channel select line
- Serial data line

Each I2S controller has the following features that can be configured using the I2S driver:

- Operation as system master or slave
- Capable of acting as transmitter or receiver
- DMA controller that allows for streaming sample data without requiring the CPU to copy each data sample

Each controller can operate in half-duplex communication mode. Thus, the two controllers can be combined to establish full-duplex communication.

.. only:: esp32

    I2S0 output can be routed directly to the digital-to-analog converter's (DAC) output channels (GPIO 25 & GPIO 26) to produce direct analog output without involving any external I2S codecs. I2S0 can also be used for transmitting PDM (Pulse-density modulation) signals.

.. only:: esp32 or esp32s2

    The I2S peripherals also support LCD mode for communicating data over a parallel bus, as used by some LCD displays and camera modules. LCD mode has the following operational modes:

    - LCD master transmitting mode
    - Camera slave receiving mode
    - ADC/DAC mode

    For more information, see *{IDF_TARGET_NAME} Technical Reference Manual* > *I2S Controller (I2S)* > LCD Mode [`PDF <{IDF_TARGET_TRM_EN_URL}#camlcdctrl>`__].

.. only:: SOC_I2S_SUPPORTS_APLL

    .. note::

        For high accuracy clock applications, use the APLL_CLK clock source, which has the frequency range of 16 ~ 128 MHz. You can enable the APLL_CLK clock source by setting :cpp:member:`i2s_config_t::use_apll` to ``TRUE``.

        If :cpp:member:`i2s_config_t::use_apll` = ``TRUE`` and :cpp:member:`i2s_config_t::fixed_mclk` > ``0``, then the master clock output frequency for I2S will be equal to the value of :cpp:member:`i2s_config_t::fixed_mclk`, which means that the mclk frequency is provided by the user, instead of being calculated by the driver.

        The clock rate of the word select line, which is called audio left-right clock rate (LRCK) here, is always the divisor of the master clock output frequency and for which the following is always true: 0 < MCLK/LRCK/channels/bits_per_sample < 64.


Functional Overview
-------------------


Installing the Driver
^^^^^^^^^^^^^^^^^^^^^

Install the I2S driver by calling the function :cpp:func:`i2s_driver_install` and passing the following arguments:

- Port number
- The initialized :cpp:type:`i2s_config_t` struct defining the communication parameters
- Event queue size and handle

An ``ESP_OK`` return value from :cpp:func:`i2s_driver_install` indictes I2S has started.

Configuration example:

.. only:: not SOC_I2S_SUPPORTS_TDM

    .. code-block:: c

        static const int i2s_num = 0; // i2s port number

        i2s_config_t i2s_config = {
            .mode = I2S_MODE_MASTER | I2S_MODE_TX,
            .sample_rate = 44100,
            .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
            .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
            .communication_format = I2S_COMM_FORMAT_STAND_I2S
            .tx_desc_auto_clear = false,
            .dma_desc_num = 8,
            .dma_frame_num = 64,
            .use_apll = false,
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1  // Interrupt level 1, default 0
        };

        i2s_driver_install(i2s_num, &i2s_config, 0, NULL);

.. only:: SOC_I2S_SUPPORTS_TDM

    .. code-block:: c

        static const int i2s_num = 0; // i2s port number

        i2s_config_t i2s_config = {
            .mode = I2S_MODE_MASTER | I2S_MODE_TX,
            .sample_rate = 44100,
            .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
            .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
            .communication_format = I2S_COMM_FORMAT_STAND_I2S,
            .tx_desc_auto_clear = false,
            .dma_desc_num = 8,
            .dma_frame_num = 64,
            .bits_per_chan = I2S_BITS_PER_SAMPLE_16BIT
        };

        i2s_driver_install(i2s_num, &i2s_config, 0, NULL);

Setting Communication Pins
^^^^^^^^^^^^^^^^^^^^^^^^^^

Once the driver is installed, configure the physical GPIO pins to which the I2S signals will be routed. This is accomplished by calling the function :cpp:func:`i2s_set_pin` with the following arguments:

- Port number
- The structure :cpp:type:`i2s_pin_config_t` which defines the GPIO pin numbers for the MCK, BCK, WS, DATA out, and DATA in signals. To keep a current pin allocatopm pin for a specific signal, or to indicate an unused signal,  pass the macro :c:macro:`I2S_PIN_NO_CHANGE`. See the example below.

.. note::

    MCK only takes effect in `I2S_MODE_MASTER` mode.

.. code-block:: c

    static const i2s_pin_config_t pin_config = {
        .mck_io_num = 0,
        .bck_io_num = 4,
        .ws_io_num = 5,
        .data_out_num = 18,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    i2s_set_pin(I2S_NUM, &pin_config);

Running I2S Communication
^^^^^^^^^^^^^^^^^^^^^^^^^

To send data:

- Prepare the data for sending
- Call the function :cpp:func:`i2s_write` and pass the data buffer address and data length to it

The function will write the data to the DMA Tx buffer, and the data will be transmitted automatically by the I2S peripheral.

.. code-block:: c

    i2s_write(I2S_NUM, samples_data, ((bits+8)/16)*SAMPLE_PER_CYCLE*4, &i2s_bytes_write, 100);

To retrieve received data, use the function :cpp:func:`i2s_read`. It will retrieve the data from the DMA Rx buffer once the data is received by the I2S peripheral.

.. code-block:: c

    i2s_read(I2S_NUM, data_recv, ((bits+8)/16)*SAMPLE_PER_CYCLE*4, &i2s_bytes_read, 100);

You can temporarily halt the I2S driver by calling  :cpp:func:`i2s_stop`, which disables the I2S Tx/Rx units until   :cpp:func:`i2s_start` is called. The driver automatically starts the I2S peripheral after :cpp:func:`i2s_driver_install` is called,  eliminating the need to call :cpp:func:`i2s_start`.


Deleting the Driver
^^^^^^^^^^^^^^^^^^^

Once I2S communication is no longer required, the driver can be removed to free allocated resources by calling :cpp:func:`i2s_driver_uninstall`.


Application Example
-------------------

Code examples for the I2S driver can be found in the directory :example:`peripherals/i2s`.

.. only:: SOC_I2S_SUPPORTS_ADC or SOC_I2S_SUPPORTS_DAC

    Additionally, there are two short configuration examples for the I2S driver.

.. only:: not SOC_I2S_SUPPORTS_ADC or SOC_I2S_SUPPORTS_DAC

    Additionally, there is a short configuration examples for the I2S driver.

I2S configuration
^^^^^^^^^^^^^^^^^

Example for general usage:

.. only:: not SOC_I2S_SUPPORTS_TDM

    .. code-block:: c

        #include "driver/i2s.h"

        static const int i2s_num = 0; // i2s port number

        i2s_config_t i2s_config = {
            .mode = I2S_MODE_MASTER | I2S_MODE_TX,
            .sample_rate = 44100,
            .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
            .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
            .communication_format = I2S_COMM_FORMAT_STAND_I2S
            .tx_desc_auto_clear = false,
            .dma_desc_num = 8,
            .dma_frame_num = 64,
            .use_apll = false,
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1  // Interrupt level 1, default 0
        };

        static const i2s_pin_config_t pin_config = {
            .bck_io_num = 4,
            .ws_io_num = 5,
            .data_out_num = 18,
            .data_in_num = I2S_PIN_NO_CHANGE
        };

        i2s_driver_install(i2s_num, &i2s_config, 0, NULL);   //install and start i2s driver
        i2s_set_pin(i2s_num, &pin_config);

        ...
        /* You can reset parameters by calling 'i2s_set_clk'
         *
         * The low 16 bits are the valid data bits in one chan and the high 16 bits are
         * the total bits in one chan. If high 16 bits is smaller than low 16 bits, it will
         * be set to a same value as low 16 bits.
         */
        uint32_t bits_cfg = (I2S_BITS_PER_CHAN_32BIT << 16) | I2S_BITS_PER_SAMPLE_16BIT;
        i2s_set_clk(i2s_num, 22050, bits_cfg, I2S_CHANNEL_STEREO);
        ...

        i2s_driver_uninstall(i2s_num); //stop & destroy i2s driver

.. only:: SOC_I2S_SUPPORTS_TDM

    .. code-block:: c

        #include "driver/i2s.h"

        static const int i2s_num = 0; // i2s port number

        i2s_config_t i2s_config = {
            .mode = I2S_MODE_MASTER | I2S_MODE_TX,
            .sample_rate = 44100,
            .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
            .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
            .communication_format = I2S_COMM_FORMAT_STAND_I2S
            .tx_desc_auto_clear = false,
            .dma_desc_num = 8,
            .dma_frame_num = 64
        };

        static const i2s_pin_config_t pin_config = {
            .bck_io_num = 4,
            .ws_io_num = 5,
            .data_out_num = 18,
            .data_in_num = I2S_PIN_NO_CHANGE
        };

        i2s_driver_install(i2s_num, &i2s_config, 0, NULL);   //install and start i2s driver
        i2s_set_pin(i2s_num, &pin_config);

        ...
        /* You can reset parameters by calling 'i2s_set_clk'
         *
         * The low 16 bits are the valid data bits in one chan and the high 16 bits are
         * the total bits in one chan. If high 16 bits is smaller than low 16 bits, it will
         * be set to a same value as low 16 bits.
         */
        uint32_t bits_cfg = (I2S_BITS_PER_CHAN_32BIT << 16) | I2S_BITS_PER_SAMPLE_16BIT;
        i2s_set_clk(i2s_num, 22050, bits_cfg, I2S_CHANNEL_STEREO);
        ...

        i2s_driver_uninstall(i2s_num); //stop & destroy i2s driver

    I2S on {IDF_TARGET_NAME} support TDM mode, up to 16 channels are available in TDM mode. If you want to use TDM mode, set field ``channel_format`` of :cpp:type:`i2s_config_t` to ``I2S_CHANNEL_FMT_MULTIPLE``. Then enable the channels by setting ``chan_mask`` using masks in :cpp:type:`i2s_channel_t`, the number of active channels and total channels will be calculate automatically. You can also set a particular total channel number for it, but it shouldn't be smaller than the largest channel you use.

    If active channels are discrete, the inactive channels within total channels will be filled by a constant automatically. But if ``skip_msk`` is enabled, these inactive channels will be skiped.

    .. code-block:: c

        #include "driver/i2s.h"

        static const int i2s_num = 0; // i2s port number

        i2s_config_t i2s_config = {
            .mode = I2S_MODE_MASTER | I2S_MODE_TX,
            .sample_rate = 44100,
            .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
            .channel_format = I2S_CHANNEL_FMT_MULTIPLE,
            .communication_format = I2S_COMM_FORMAT_STAND_I2S
            .tx_desc_auto_clear = false,
            .dma_desc_num = 8,
            .dma_frame_num = 64,
            .chan_mask = I2S_TDM_ACTIVE_CH0 | I2S_TDM_ACTIVE_CH2
        };

        static const i2s_pin_config_t pin_config = {
            .bck_io_num = 4,
            .ws_io_num = 5,
            .data_out_num = 18,
            .data_in_num = I2S_PIN_NO_CHANGE
        };

        i2s_driver_install(i2s_num, &i2s_config, 0, NULL);   //install and start i2s driver
        i2s_set_pin(i2s_num, &pin_config);

        ...
        /* You can reset parameters by calling 'i2s_set_clk'
         *
         * The low 16 bits are the valid data bits in one chan and the high 16 bits are
         * the total bits in one chan. If high 16 bits is smaller than low 16 bits, it will
         * be set to a same value as low 16 bits.
         */
        uint32_t bits_cfg = (I2S_BITS_PER_CHAN_32BIT << 16) | I2S_BITS_PER_SAMPLE_16BIT;
        i2s_set_clk(i2s_port_t i2s_num, 22050, bits_cfg, I2S_TDM_ACTIVE_CH0 | I2S_TDM_ACTIVE_CH1); // set clock
        ...

        i2s_driver_uninstall(i2s_num); //stop & destroy i2s driver

.. only:: SOC_I2S_SUPPORTS_ADC or SOC_I2S_SUPPORTS_DAC

    Configuring I2S to use internal DAC for analog output
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    .. code-block:: c

        #include "driver/i2s.h"
        #include "freertos/queue.h"

        static const int i2s_num = 0; // i2s port number

        static const i2s_config_t i2s_config = {
            .mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN,
            .sample_rate = 44100,
            .bits_per_sample = 16, /* the DAC module will only take the 8bits from MSB */
            .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
            .intr_alloc_flags = 0, // default interrupt priority
            .dma_desc_num = 8,
            .dma_frame_num = 64,
            .use_apll = false
        };

        ...

            i2s_driver_install(i2s_num, &i2s_config, 0, NULL);   //install and start i2s driver

            i2s_set_pin(i2s_num, NULL); //for internal DAC, this will enable both of the internal channels

            //You can call i2s_set_dac_mode to set built-in DAC output mode.
            //i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);

            i2s_set_sample_rates(i2s_num, 22050); //set sample rates

            i2s_driver_uninstall(i2s_num); //stop & destroy i2s driver


Application Notes
^^^^^^^^^^^^^^^^^

For the applications that need a high frequency sample rate, sometimes the massive throughput of receiving data may cause data lost. Users can receive data loss event by the event queue, it will trigger an ``I2S_EVENT_RX_Q_OVF`` event.:

    .. code-block:: c

        QueueHandle_t evt_que;
        i2s_driver_install(i2s_num, &i2s_config, 10, &evt_que);
        ...
        i2s_event_t evt;
        xQueueReceive(evt_que, &evt, portMAX_DELAY);
        if (evt.type == I2S_EVENT_RX_Q_OVF) {
            printf("RX data dropped\n");
        }

Please follow these steps to calculate the parameters that can prevent data lost:

1. Determine the interrupt interval. Generally, when data lost happened, the interval should be the bigger the better, it can help to reduce the interrupt times, i.e., ``dma_frame_num`` should be as big as possible while the DMA buffer size won't exceed its maximum value 4092. The relationships are::

    interrupt_interval(unit: sec) = dma_frame_num / sample_rate
    dma_buffer_size = dma_frame_num * channel_num * data_bit_width / 8 <= 4092

2. Determine the ``dma_desc_num``. The ``dma_desc_num`` is decided by the max time of ``i2s_read`` polling cycle, all the data should be stored between two ``i2s_read``. This cycle can be measured by a timer or an outputting gpio signal. The relationship should be::

    dma_desc_num > polling_cycle / interrupt_interval

3. Determine the receiving buffer size. The receiving buffer that offered by user in ``i2s_read`` should be able to take all the data in all dma buffers, that means it should be bigger than the total size of all the dma buffers::

    recv_buffer_size > dma_desc_num * dma_buffer_size

For example, if there is a I2S application::

    sample_rate = 144000 Hz
    data_bit_width = 32 bits
    channel_num = 2
    polling_cycle = 10ms

Then we need to calculate ``dma_frame_num``, ``dma_desc_num`` and ``recv_buf_size`` according to the known values::

    dma_frame_num * channel_num * data_bit_width / 8 = dma_buffer_size <= 4092
    dma_frame_num <= 511
    interrupt_interval = dma_frame_num / sample_rate = 511 / 144000 = 0.003549 s = 3.549 ms
    dma_desc_num > polling_cycle / interrupt_interval = cell(10 / 3.549) = cell(2.818) = 3
    recv_buffer_size > dma_desc_num * dma_buffer_size = 3 * 4092 = 12276 bytes


API Reference
-------------

.. include-build-file:: inc/i2s.inc
.. include-build-file:: inc/i2s_types.inc

