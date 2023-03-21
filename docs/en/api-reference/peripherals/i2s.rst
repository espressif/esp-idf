Inter-IC Sound (I2S)
====================

{IDF_TARGET_I2S_NUM:default="two", esp32s2="one", esp32c3="one"}

Introduction
------------

I2S (Inter-IC Sound) is a serial, synchronous communication protocol that is usually used for transmitting audio data between two digital audio devices.

{IDF_TARGET_NAME} contains {IDF_TARGET_I2S_NUM} I2S peripheral(s). These peripherals can be configured to input and output sample data via the I2S driver.

An I2S bus that communicate in Standard or TDM mode consists of the following lines:

- **MCLK:** Master clock line. It's an optional signal depends on slave side, mainly used for offering a reference clock to the I2S slave device.
- **BCLK:** Bit clock line. The bit clock for data line.
- **WS:** Word(Slot) select line. It is usually used to identify the vocal tract except PDM mode.
- **DIN/DOUT:** Serial data input/output line. (Data will loopback internally if din and dout are set to a same GPIO)

.. only:: SOC_I2S_SUPPORTS_PDM_TX or SOC_I2S_SUPPORTS_PDM_RX

    And for the I2S bus that communicate in PDM mode, the lines are:

    - **CLK:** PDM clock line.
    - **DIN/DOUT:** Serial data input/output line.

Each I2S controller has the following features that can be configured by the I2S driver:

- Operation as system master or slave
- Capable of acting as transmitter or receiver
- DMA controller that allows for streaming sample data without requiring the CPU to copy each data sample

.. only:: SOC_I2S_HW_VERSION_1

    Each controller can operate in simplex communication mode. Thus, the two controllers can be combined to establish full-duplex communication.

.. only:: SOC_I2S_HW_VERSION_2

    Each controller has separate rx and tx channel. That means they are able to work under different clock and slot configurations with separate GPIO pins. Note that although the internal MCLK of tx channel and rx channel are separate on a controller, the output MCLK signal can only be attached to one channel. If two different MCLK output is required, they must be allocated on different I2S controller.

I2S File Structure
------------------

.. figure:: ../../../_static/diagrams/i2s/i2s_file_structure.png
    :align: center
    :alt: I2S file structure

**Public headers that need to be included in the I2S application**

- ``i2s.h``: The header file of legacy I2S APIs (for apps using legacy driver).
- ``i2s_std.h``: The header file that provides standard communication mode specific APIs (for apps using new driver with standard mode).
- ``i2s_pdm.h``: The header file that provides PDM communication mode specific APIs (for apps using new driver with PDM mode).
- ``i2s_tdm.h``: The header file that provides TDM communication mode specific APIs (for apps using new driver with TDM mode).

.. note::

    The legacy driver can't coexist with the new driver. Including ``i2s.h`` to use the legacy driver or the other three headers to use the new driver. The legacy driver might be removed in future.

**Public headers that have been included in the headers above**

- ``i2s_types_legacy.h``: The legacy public types that only used in the legacy driver.
- ``i2s_types.h``: The header file that provides public types.
- ``i2s_common.h``: The header file that provides common APIs for all communication modes.

I2S Clock
---------

Clock Source
^^^^^^^^^^^^

- :cpp:enumerator:`i2s_clock_src_t::I2S_CLK_SRC_DEFAULT`: Default PLL clock.

.. only:: not esp32h2

    - :cpp:enumerator:`i2s_clock_src_t::I2S_CLK_SRC_PLL_160M`: 160 MHz PLL clock.

.. only:: esp32h2

    - :cpp:enumerator:`i2s_clock_src_t::I2S_CLK_SRC_PLL_96M`: 96 MHz PLL clock.

.. only:: SOC_I2S_SUPPORTS_APLL

    - :cpp:enumerator:`i2s_clock_src_t::I2S_CLK_SRC_APLL`: Audio PLL clock, more precise than ``I2S_CLK_SRC_PLL_160M`` in high sample rate applications. Its frequency is configurable according to the sample rate, but if APLL has been occupied by emac or other channels already, the APLL frequency is not allowed to change, the driver will try to work under this APLL frequency, if this APLL frequency can't meet the requirements of I2S, the clock configuration will fail.

Clock Terminology
^^^^^^^^^^^^^^^^^

- **sample rate**: The number of sampled data in one second per slot.
- **sclk**: Source clock frequency. It is the frequency of the clock source.
- **mclk**: Master clock frequency. ``bclk`` is generate from this clock, ``mclk`` is mostly needed in the case that requires the MCLK signal as a reference clock to synchronize BCLK and WS between I2S master role and slave role.
- **bclk**: Bit clock frequency. Every tick of this clock stands for one data bit on data pin. It means there will be 8/16/24/32 ``bclk`` ticks in one slot, because the number of ``bclk`` ticks in one slot is equal to the :cpp:member:`i2s_std_slot_config_t::slot_bit_width`.
- **lrck** / **ws**: Left/Right clock or word select clock. For non-PDM mode, its frequency is equal to the sample rate.

.. note::

    Normally ``mclk`` should be the multiple of ``sample rate`` and ``bclk`` at the same time. This field :cpp:member:`i2s_std_clk_config_t::mclk_multiple` means the multiple of ``mclk`` to the ``sample rate``. If ``slot_bit_width`` is set to ``I2S_SLOT_BIT_WIDTH_24BIT``, to keep ``mclk`` a multiple to the ``bclk``, :cpp:member:`i2s_std_clk_config_t::mclk_multiple` should be set to ``I2S_MCLK_MULTIPLE_384``, otherwise the ``ws`` will be inaccurate. But in the most other cases, ``I2S_MCLK_MULTIPLE_256`` should be enough.

.. _i2s-communication-mode:

I2S Communication Mode
----------------------

Overview of All Modes
^^^^^^^^^^^^^^^^^^^^^

=========  ========  ========  ========  ========  ========  ==========
 Target    Standard   PDM TX    PDM RX     TDM     ADC/DAC   LCD/Camera
=========  ========  ========  ========  ========  ========  ==========
ESP32      I2S 0/1    I2S 0     I2S 0      none     I2S 0      I2S 0
ESP32S2     I2S 0     none      none       none     none       I2S 0
ESP32C3     I2S 0     I2S 0     none      I2S0      none       none
ESP32S3    I2S 0/1    I2S 0     I2S 0    I2S 0/1    none       none
=========  ========  ========  ========  ========  ========  ==========

Standard Mode
^^^^^^^^^^^^^

Standard mode always has left and right two sound channels which are called 'slots'. These slots can support 8/16/24/32 bits width sample data. And the communication format for the slots mainly includes these following formats:

- **Philips Format**: Data signal have one bit shift comparing to the WS(word select) signal. And the duty of WS signal is 50%.

.. wavedrom:: /../_static/diagrams/i2s/std_philips.json

- **MSB Format**: Almost same as philips format, but its data have no shift.

.. wavedrom:: /../_static/diagrams/i2s/std_msb.json

- **PCM Short Format**: Data have one bit shift and meanwhile WS signal becomes a pulse lasting one BCLK(Bit Clock) cycle.

.. wavedrom:: /../_static/diagrams/i2s/std_pcm.json


.. only:: SOC_I2S_SUPPORTS_PDM_TX

    PDM Mode (TX)
    ^^^^^^^^^^^^^

    PDM(Pulse-density Modulation) mode for tx channel can convert PCM data into PDM format which always has left and right slots. PDM TX can only support 16 bits width sample data. PDM TX only needs CLK pin for clock signal and DOUT pin for data signal (i.e. WS and SD signal in the following figure, the BCK signal is an internal bit sampling clock, not needed between PDM devices). This mode allows user to configure the up-sampling parameters :cpp:member:`i2s_pdm_tx_clk_config_t::up_sample_fp` :cpp:member:`i2s_pdm_tx_clk_config_t::up_sample_fs`. The up-sampling rate can be calculated by ``up_sample_rate = fp / fs``, there are up-sampling modes in PDM TX:

    - **Fixed Clock Frequency**: In this mode the up-sampling rate will change according to the sample rate. Setting ``fp = 960`` and ``fs = sample_rate / 100``, then the clock frequency(Fpdm) on CLK pin will be fixed to 128 * 48 KHz = 6.144 MHz, note that this frequency is not equal to the sample rate(Fpcm).
    - **Fixed Up-sampling Rate**: In this mode the up-sampling rate is fixed to 2. Setting ``fp = 960`` and ``fs = 480``, then the clock frequency(Fpdm) on CLK pin will be ``128 * sample_rate``

    .. wavedrom:: /../_static/diagrams/i2s/pdm.json


.. only:: SOC_I2S_SUPPORTS_PDM_RX

    PDM Mode (RX)
    ^^^^^^^^^^^^^

    PDM(Pulse-density Modulation) mode for rx channel can receive PDM format data and convert the data into PCM format. PDM RX can only support 16 bits width sample data. PDM RX only need WS pin for clock signal and DIN pin for data signal. This mode allows user to configure the down-sampling parameter :cpp:member:`i2s_pdm_rx_clk_config_t::dn_sample_mode`, there are two down-sampling modes in PDM RX:

    - :cpp:enumerator:`i2s_pdm_dsr_t::I2S_PDM_DSR_8S`: In this mode, the clock frequency(Fpdm) on WS pin will be sample_rate(Fpcm) * 64.
    - :cpp:enumerator:`i2s_pdm_dsr_t::I2S_PDM_DSR_16S`: In this mode, the clock frequency(Fpdm) on WS pin will be sample_rate(Fpcm) * 128.


.. only:: SOC_I2S_SUPPORTS_TDM

    TDM Mode
    ^^^^^^^^

    TDM(Time Division Multiplexing) mode supports upto 16 slots, these slots can be enabled by :cpp:member:`i2s_tdm_slot_config_t::slot_mask`. But due to the hardware limitation, only upto 4 slots are supported while the slot is set to 32 bit-width, and 8 slots for 16 bit-width, 16 slots for 8 bit-width. The slot communication format of TDM is almost same as standard mode, but there are some small differences between them.

    - **Philips Format**: Data signal have one bit shift comparing to the WS(word select) signal. And no matter how many slots are contained in one frame, the duty of WS signal will always keep 50%.

    .. wavedrom:: /../_static/diagrams/i2s/tdm_philips.json

    - **MSB Format**: Almost same as philips format, but its data have no shift.

    .. wavedrom:: /../_static/diagrams/i2s/tdm_msb.json

    - **PCM Short Format**: Data have one bit shift and meanwhile WS signal becomes a pulse lasting one BCLK(Bit Clock) cycle for every frame.

    .. wavedrom:: /../_static/diagrams/i2s/tdm_pcm_short.json

    - **PCM Long Format**: Data have one bit shift and meanwhile WS signal will lasting one slot bit width for every frame. For example, if there are 4 slots enabled, then the duty of WS will be 25%, and if there are 5 slots, it will be 20%.

    .. wavedrom:: /../_static/diagrams/i2s/tdm_pcm_long.json

.. only:: SOC_I2S_SUPPORTS_LDC_CAMERA

    LCD/Camera Mode
    ^^^^^^^^^^^^^^^

    LCD/Camera mode are only supported on I2S0 over a parallel bus. For LCD mode, I2S0 should working at master tx mode. For camera mode, I2S0 should working at slave rx mode. These two modes are not implemented by I2S driver, please refer to :doc:`/api-reference/peripherals/lcd` for LCD implementation. For more information, see *{IDF_TARGET_NAME} Technical Reference Manual* > *I2S Controller (I2S)* > LCD Mode [`PDF <{IDF_TARGET_TRM_EN_URL}#camlcdctrl>`__].

.. only:: SOC_I2S_SUPPORTS_ADC_DAC

    ADC/DAC Mode
    ^^^^^^^^^^^^

    ADC and DAC modes only exist on ESP32 and are only supported on I2S0. Actually, they are two sub-modes of LCD/Camera mode. I2S0 can be routed directly to the internal analog-to-digital converter(ADC) and digital-to-analog converter(DAC). In other words, ADC and DAC peripherals can read or write continuously via I2S0 DMA. As they are not an actual communication mode, the I2S driver does not implement them.

Functional Overview
-------------------

The I2S driver offers following services:

Resources Management
^^^^^^^^^^^^^^^^^^^^

There are three levels' resources in I2S driver:

- ``platform level``: Resources of all I2S controllers in the current target.
- ``controller level``: Resources in one I2S controller.
- ``channel level``: Resources of tx or rx channel in one I2S controller.

The public APIs are all channel level APIs, the channel handle :cpp:type:`i2s_chan_handle_t` can help user to manage the resources under a specific channel without considering the other two levels. The other two upper levels' resources are private and will be managed by the driver automatically. Users can call :cpp:func:`i2s_new_channel` to allocate a channel handle and call :cpp:func:`i2s_del_channel` to delete it.

Power Management
^^^^^^^^^^^^^^^^

When the power management is enabled (i.e. :ref:`CONFIG_PM_ENABLE` is on), the system will adjust or stop the source clock of I2S before going into light sleep, thus potentially changing the I2S signals and leading to transmitting or receiving invalid data.

I2S driver can prevent the system from changing or stopping the source clock by acquiring a power management lock. When the source clock is generated from APB, the lock type will be set to :cpp:enumerator:`esp_pm_lock_type_t::ESP_PM_APB_FREQ_MAX` and when the source clock is APLL (if target support APLL), it will be set to :cpp:enumerator:`esp_pm_lock_type_t::ESP_PM_NO_LIGHT_SLEEP`. Whenever user is reading or writing via I2S (i.e. calling :cpp:func:`i2s_channel_read` or :cpp:func:`i2s_channel_write`), the driver will guarantee that the power management lock is acquired. Likewise, the driver releases the lock after reading or writing finished.

Finite-State Machine
^^^^^^^^^^^^^^^^^^^^

There are three states for an I2S channel, they are ``registered``, ``ready`` and ``running``. Their relationship is shown in the following diagram:

.. figure:: ../../../_static/diagrams/i2s/i2s_state_machine.png
    :align: center
    :alt: I2S Finite-State Machine

The ``<mode>`` in the diagram can be replaced by corresponding I2S communication mode like ``std`` for standard two-slot mode, for other information of communication mode, please refer to :ref:`i2s-communication-mode` section.

Data Transport
^^^^^^^^^^^^^^

The data transport of I2S peripheral, including sending and receiving, is realized by DMA. Before transporting data, please call :cpp:func:`i2s_channel_enable` to enable the specific channel. When the sent or received data reach the size of one DMA buffer, ``I2S_OUT_EOF`` or ``I2S_IN_SUC_EOF`` interrupt will be triggered. Note that the DMA buffer size is not equal to :cpp:member:`i2s_chan_config_t::dma_frame_num`, one frame here means all the sampled data in one WS circle. Therefore, ``dma_buffer_size = dma_frame_num * slot_num * slot_bit_width / 8``. For the transmit case, users can input the data by calling :cpp:func:`i2s_channel_write`. This function will help users to copy the data from the source buffer to the DMA tx buffer and wait for the transmission finished. Then it'll repeat until the sent bytes reach the given size. For the receive case, the function :cpp:func:`i2s_channel_read` will wait for receiving the message queue which contains the DMA buffer address, it will help users to copy the data from DMA rx buffer to the destination buffer.

Both :cpp:func:`i2s_channel_write` and :cpp:func:`i2s_channel_read` are blocking functions, they will keep waiting until the whole source buffer are sent or the whole destination buffer loaded, unless they exceed the max blocking time, then the error code `ESP_ERR_TIMEOUT` will return in this case. To send or receive data asynchronously, callbacks can be registered by  :cpp:func:`i2s_channel_register_event_callback`, users are able to access the DMA buffer directly in the callback function instead of transmitting or receiving by the two blocking functions. However, please be aware that it is an interrupt callback, don't do complex logic, floating operation or call non-reentrant functions in the callback.

Configuration Setting
^^^^^^^^^^^^^^^^^^^^^^

Users can initialize a channel by corresponding function (i.e. :func:`i2s_channel_init_std_mode`, :func:`i2s_channel_init_pdm_rx_mode`, :func:`i2s_channel_init_pdm_tx_mode` or :func:`i2s_channel_init_tdm_mode`), the channel will be initialized to the specific mode. If the configurations need to be updated after initialization, :cpp:func:`i2s_channel_disable` has to be called first to ensure the channel has stopped, and then calling corresponding 'reconfig' functions, like :cpp:func:`i2s_channel_reconfig_std_slot`, :cpp:func:`i2s_channel_reconfig_std_clock`, :cpp:func:`i2s_channel_reconfig_std_gpio`.

IRAM Safe
^^^^^^^^^

By default, the I2S interrupt will be deferred when the Cache is disabled for reasons like writing/erasing Flash. Thus the EOF interrupt will not get executed in time, which is not expected in a real-time application.

There's a Kconfig option :ref:`CONFIG_I2S_ISR_IRAM_SAFE` that will:

1. Enable the interrupt being serviced even when cache is disabled

2. Place driver object into DRAM (in case it's linked to PSRAM by accident)

This will allow the interrupt to run while the cache is disabled but will come at the cost of increased IRAM consumption.

Thread Safety
^^^^^^^^^^^^^

All the public I2S APIs are guaranteed to be thread safe by the driver, which means, user can call them from different RTOS tasks without protection by extra locks. Notice that I2S driver uses mutex lock to ensure the thread safety, thus these APIs are not allowed to be used in ISR.

Kconfig Options
^^^^^^^^^^^^^^^

- :ref:`CONFIG_I2S_ISR_IRAM_SAFE` controls whether the default ISR handler can work when cache is disabled, see `IRAM Safe <#iram-safe>`__ for more information.
- :ref:`CONFIG_I2S_SUPPRESS_DEPRECATE_WARN` controls whether to suppress the compiling warning message while using the legacy I2S driver.
- :ref:`CONFIG_I2S_ENABLE_DEBUG_LOG` is used to enabled the debug log output. Enable this option will increase the firmware binary size.

Application Example
-------------------

The examples of the I2S driver can be found in the directory :example:`peripherals/i2s`.
Here are some simple usages of each mode:

Standard TX/RX Usage
^^^^^^^^^^^^^^^^^^^^

Different slot communication formats can be generated by following helper macros for standard mode. As described above, there are three formats in standard mode, their helper macros are:

- :c:macro:`I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG`
- :c:macro:`I2S_STD_PCM_SLOT_DEFAULT_CONFIG`
- :c:macro:`I2S_STD_MSB_SLOT_DEFAULT_CONFIG`

The clock config helper macro is:

- :c:macro:`I2S_STD_CLK_DEFAULT_CONFIG`

Please refer to :ref:`i2s-api-reference-i2s_std` for STD API information.
And for more details, please refer to :component_file:`driver/include/driver/i2s_std.h`.

STD TX Mode
~~~~~~~~~~~

Take 16-bit data width for example, when the data in a ``uint16_t`` writting buffer are:

+--------+--------+--------+--------+--------+--------+--------+--------+--------+
| data 0 | data 1 | data 2 | data 3 | data 4 | data 5 | data 6 | data 7 |  ...   |
+========+========+========+========+========+========+========+========+========+
| 0x0001 | 0x0002 | 0x0003 | 0x0004 | 0x0005 | 0x0006 | 0x0007 | 0x0008 |  ...   |
+--------+--------+--------+--------+--------+--------+--------+--------+--------+

Here is the table of the real data on the line with different :cpp:member:`i2s_std_slot_config_t::slot_mode` and :cpp:member:`i2s_std_slot_config_t::slot_mask`

.. only:: esp32

    +----------------+-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    | data bit width | slot mode | slot mask | ws low   | ws high  | ws low   | ws high  | ws low   | ws high  | ws low   | ws high  |
    +================+===========+===========+==========+==========+==========+==========+==========+==========+==========+==========+
    |                |  mono     |   left    | 0x0002   | 0x0000   | 0x0001   | 0x0000   | 0x0004   | 0x0000   | 0x0003   | 0x0000   |
    |     16 bit     |           +-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    |                |           |   right   | 0x0000   | 0x0002   | 0x0000   | 0x0001   | 0x0000   | 0x0004   | 0x0000   | 0x0003   |
    |                |           +-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    |                |           |   both    | 0x0002   | 0x0002   | 0x0001   | 0x0001   | 0x0004   | 0x0004   | 0x0003   | 0x0003   |
    |                +-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    |                |  stereo   |   left    | 0x0001   | 0x0001   | 0x0003   | 0x0003   | 0x0005   | 0x0005   | 0x0007   | 0x0007   |
    |                |           +-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    |                |           |   right   | 0x0002   | 0x0002   | 0x0004   | 0x0004   | 0x0006   | 0x0006   | 0x0008   | 0x0008   |
    |                |           +-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    |                |           |   both    | 0x0001   | 0x0002   | 0x0003   | 0x0004   | 0x0005   | 0x0006   | 0x0007   | 0x0008   |
    +----------------+-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+

    .. note::

        It's similar when the data is 32-bit width, but take care when using 8-bit and 24-bit data width. For 8-bit width, the written buffer should still using ``uint16_t`` (i.e. align with 2 bytes), and only the high 8 bits will be valid, the low 8 bits are dropped, and for 24-bit width, the buffer is supposed to use ``uint32_t`` (i.e. align with 4 bytes), and only the high 24 bits valid, the low 8 bits are dropped.

        Another point is that, for the ``8-bit`` and ``16-bit`` mono mode, the real data on the line are swapped. To get the correct sequence, the writing buffer need to swap the data every two bytes.

.. only:: esp32s2

    +----------------+-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    | data bit width | slot mode | slot mask | ws low   | ws high  | ws low   | ws high  | ws low   | ws high  | ws low   | ws high  |
    +================+===========+===========+==========+==========+==========+==========+==========+==========+==========+==========+
    |                |  mono     |   left    | 0x0001   | 0x0000   | 0x0002   | 0x0000   | 0x0003   | 0x0000   | 0x0004   | 0x0000   |
    |     16 bit     |           +-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    |                |           |   right   | 0x0000   | 0x0001   | 0x0000   | 0x0002   | 0x0000   | 0x0003   | 0x0000   | 0x0004   |
    |                |           +-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    |                |           |   both    | 0x0001   | 0x0001   | 0x0002   | 0x0002   | 0x0003   | 0x0003   | 0x0004   | 0x0004   |
    |                +-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    |                |  stereo   |   left    | 0x0001   | 0x0001   | 0x0003   | 0x0003   | 0x0005   | 0x0005   | 0x0007   | 0x0007   |
    |                |           +-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    |                |           |   right   | 0x0002   | 0x0002   | 0x0004   | 0x0004   | 0x0006   | 0x0006   | 0x0008   | 0x0008   |
    |                |           +-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    |                |           |   both    | 0x0001   | 0x0002   | 0x0003   | 0x0004   | 0x0005   | 0x0006   | 0x0007   | 0x0008   |
    +----------------+-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+

    .. note::

        Similar for 8-bit and 32-bit data width, the type of the buffer is better to be ``uint8_t`` and ``uint32_t`` type. But specially, when the data width is 24-bit, the data buffer should aligned with 3-byte(i.e. every 3 bytes stands for a 24-bit data in one slot), additionally, :cpp:member:`i2s_chan_config_t::dma_frame_num`, :cpp:member:`i2s_std_clk_config_t::mclk_multiple` and the writing buffer size should be the multiple of ``3``, otherwise the data on the line or the sample rate will be incorrect.

.. only:: esp32c3 or esp32s3 or esp32h2

    +----------------+-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    | data bit width | slot mode | slot mask | ws low   | ws high  | ws low   | ws high  | ws low   | ws high  | ws low   | ws high  |
    +================+===========+===========+==========+==========+==========+==========+==========+==========+==========+==========+
    |                |  mono     |   left    | 0x0001   | 0x0000   | 0x0002   | 0x0000   | 0x0003   | 0x0000   | 0x0004   | 0x0000   |
    |     16 bit     |           +-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    |                |           |   right   | 0x0000   | 0x0001   | 0x0000   | 0x0002   | 0x0000   | 0x0003   | 0x0000   | 0x0004   |
    |                |           +-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    |                |           |   both    | 0x0001   | 0x0001   | 0x0002   | 0x0002   | 0x0003   | 0x0003   | 0x0004   | 0x0004   |
    |                +-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    |                |  stereo   |   left    | 0x0001   | 0x0001   | 0x0003   | 0x0003   | 0x0005   | 0x0005   | 0x0007   | 0x0007   |
    |                |           +-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    |                |           |   right   | 0x0002   | 0x0002   | 0x0004   | 0x0004   | 0x0006   | 0x0006   | 0x0008   | 0x0008   |
    |                |           +-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    |                |           |   both    | 0x0001   | 0x0002   | 0x0003   | 0x0004   | 0x0005   | 0x0006   | 0x0007   | 0x0008   |
    +----------------+-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+

    .. note::

        Similar for 8-bit and 32-bit data width, the type of the buffer is better to be ``uint8_t`` and ``uint32_t`` type. But specially, when the data width is 24-bit, the data buffer should aligned with 3-byte(i.e. every 3 bytes stands for a 24-bit data in one slot), additionally, :cpp:member:`i2s_chan_config_t::dma_frame_num`, :cpp:member:`i2s_std_clk_config_t::mclk_multiple` and the writing buffer size should be the multiple of ``3``, otherwise the data on the line or the sample rate will be incorrect.

.. code-block:: c

    #include "driver/i2s_std.h"
    #include "driver/gpio.h"

    i2s_chan_handle_t tx_handle;
    /* Get the default channel configuration by helper macro.
     * This helper macro is defined in 'i2s_common.h' and shared by all the i2s communication mode.
     * It can help to specify the I2S role, and port id */
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    /* Allocate a new tx channel and get the handle of this channel */
    i2s_new_channel(&chan_cfg, &tx_handle, NULL);

    /* Setting the configurations, the slot configuration and clock configuration can be generated by the macros
     * These two helper macros is defined in 'i2s_std.h' which can only be used in STD mode.
     * They can help to specify the slot and clock configurations for initialization or updating */
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(48000),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = GPIO_NUM_4,
            .ws = GPIO_NUM_5,
            .dout = GPIO_NUM_18,
            .din = I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    /* Initialize the channel */
    i2s_channel_init_std_mode(tx_handle, &std_cfg);

    /* Before write data, start the tx channel first */
    i2s_channel_enable(tx_handle);
    i2s_channel_write(tx_handle, src_buf, bytes_to_write, bytes_written, ticks_to_wait);

    /* If the configurations of slot or clock need to be updated,
     * stop the channel first and then update it */
    // i2s_channel_disable(tx_handle);
    // std_cfg.slot_cfg.slot_mode = I2S_SLOT_MODE_MONO; // Default is stereo
    // i2s_channel_reconfig_std_slot(tx_handle, &std_cfg.slot_cfg);
    // std_cfg.clk_cfg.sample_rate_hz = 96000;
    // i2s_channel_reconfig_std_clock(tx_handle, &std_cfg.clk_cfg);

    /* Have to stop the channel before deleting it */
    i2s_channel_disable(tx_handle);
    /* If the handle is not needed any more, delete it to release the channel resources */
    i2s_del_channel(tx_handle);

STD RX Mode
~~~~~~~~~~~

Take 16-bit data width for example, when the data on the line are:

+--------+--------+--------+--------+--------+--------+--------+--------+--------+
| ws low | ws high| ws low | ws high| ws low | ws high| ws low | ws high|  ...   |
+========+========+========+========+========+========+========+========+========+
| 0x0001 | 0x0002 | 0x0003 | 0x0004 | 0x0005 | 0x0006 | 0x0007 | 0x0008 |  ...   |
+--------+--------+--------+--------+--------+--------+--------+--------+--------+

Here is the table of the data that received in the buffer with different :cpp:member:`i2s_std_slot_config_t::slot_mode` and :cpp:member:`i2s_std_slot_config_t::slot_mask`

.. only:: esp32

    +----------------+-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    | data bit width | slot mode | slot mask | data 0   | data 1   | data 2   | data 3   | data 4   | data 5   | data 6   | data 7   |
    +================+===========+===========+==========+==========+==========+==========+==========+==========+==========+==========+
    |                |  mono     |   left    | 0x0001   | 0x0000   | 0x0005   | 0x0003   | 0x0009   | 0x0007   | 0x000d   | 0x000b   |
    |                |           +-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    |     16 bit     |           |   right   | 0x0002   | 0x0000   | 0x0006   | 0x0004   | 0x000a   | 0x0008   | 0x000e   | 0x000c   |
    |                +-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    |                |  stereo   |   any     | 0x0001   | 0x0002   | 0x0003   | 0x0004   | 0x0005   | 0x0006   | 0x0007   | 0x0008   |
    +----------------+-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+

    .. note::

        The receive case is a little bit complicated on ESP32.
        Firstly, when the data width are ``8-bit`` or ``24-bit``, the received data will still align with two bytes or four bytes, which means the valid data are put in the high 8 bits in every two bytes and high 24 bits in every four bytes. For example, the received data will be ``0x5A00`` when the data on the line is ``0x5A`` in 8-bit width, and receive ``0x0000 5A00`` if the data ``0x00 005A`` on the line.
        Secondly, for ``8-bit`` and ``16-bit`` mono case, the data in buffer are swapped every two data, they may need to be swapped back manually to get the correct order.

.. only:: esp32s2

    +----------------+-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    | data bit width | slot mode | slot mask | data 0   | data 1   | data 2   | data 3   | data 4   | data 5   | data 6   | data 7   |
    +================+===========+===========+==========+==========+==========+==========+==========+==========+==========+==========+
    |                |  mono     |   left    | 0x0001   | 0x0003   | 0x0005   | 0x0007   | 0x0009   | 0x000b   | 0x000d   | 0x000f   |
    |                |           +-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    |     16 bit     |           |   right   | 0x0002   | 0x0004   | 0x0006   | 0x0008   | 0x000a   | 0x000c   | 0x000e   | 0x0010   |
    |                +-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    |                |  stereo   |   any     | 0x0001   | 0x0002   | 0x0003   | 0x0004   | 0x0005   | 0x0006   | 0x0007   | 0x0008   |
    +----------------+-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+

    .. note::

        ``8-bit``, ``24-bit`` and ``32-bit`` are similar as ``16-bit``, the data bit-width in the receiving buffer are equal to the data bit-width on the line. Additionally, when using ``24-bit`` data width, :cpp:member:`i2s_chan_config_t::dma_frame_num`, :cpp:member:`i2s_std_clk_config_t::mclk_multiple` and the receiving buffer size should be the multiple of ``3``, otherwise the data on the line or the sample rate will be incorrect.

.. only:: esp32c3 or esp32s3 or esp32h2

    +----------------+-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    | data bit width | slot mode | slot mask | data 0   | data 1   | data 2   | data 3   | data 4   | data 5   | data 6   | data 7   |
    +================+===========+===========+==========+==========+==========+==========+==========+==========+==========+==========+
    |                |  mono     |   left    | 0x0001   | 0x0003   | 0x0005   | 0x0007   | 0x0009   | 0x000b   | 0x000d   | 0x000f   |
    |                |           +-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    |     16 bit     |           |   right   | 0x0002   | 0x0004   | 0x0006   | 0x0008   | 0x000a   | 0x000c   | 0x000e   | 0x0010   |
    |                +-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    |                |  stereo   |   any     | 0x0001   | 0x0002   | 0x0003   | 0x0004   | 0x0005   | 0x0006   | 0x0007   | 0x0008   |
    +----------------+-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+

    .. note::

        ``8-bit``, ``24-bit`` and ``32-bit`` are similar as ``16-bit``, the data bit-width in the receiving buffer are equal to the data bit-width on the line. Additionally, when using ``24-bit`` data width, :cpp:member:`i2s_chan_config_t::dma_frame_num`, :cpp:member:`i2s_std_clk_config_t::mclk_multiple` and the receiving buffer size should be the multiple of ``3``, otherwise the data on the line or the sample rate will be incorrect.

.. code-block:: c

    #include "driver/i2s_std.h"
    #include "driver/gpio.h"

    i2s_chan_handle_t rx_handle;
    /* Get the default channel configuration by helper macro.
     * This helper macro is defined in 'i2s_common.h' and shared by all the i2s communication mode.
     * It can help to specify the I2S role, and port id */
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    /* Allocate a new rx channel and get the handle of this channel */
    i2s_new_channel(&chan_cfg, NULL, &rx_handle);

    /* Setting the configurations, the slot configuration and clock configuration can be generated by the macros
     * These two helper macros is defined in 'i2s_std.h' which can only be used in STD mode.
     * They can help to specify the slot and clock configurations for initialization or updating */
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(48000),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = GPIO_NUM_4,
            .ws = GPIO_NUM_5,
            .dout = I2S_GPIO_UNUSED,
            .din = GPIO_NUM_19,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    /* Initialize the channel */
    i2s_channel_init_std_mode(rx_handle, &std_cfg);

    /* Before read data, start the rx channel first */
    i2s_channel_enable(rx_handle);
    i2s_channel_read(rx_handle, desc_buf, bytes_to_read, bytes_read, ticks_to_wait);

    /* Have to stop the channel before deleting it */
    i2s_channel_disable(rx_handle);
    /* If the handle is not needed any more, delete it to release the channel resources */
    i2s_del_channel(rx_handle);


.. only:: SOC_I2S_SUPPORTS_PDM_TX

    PDM TX usage
    ^^^^^^^^^^^^

    For PDM mode in tx channel, the slot configuration helper macro is:

    - :c:macro:`I2S_PDM_TX_SLOT_DEFAULT_CONFIG`

    The clock configuration helper macro is:

    - :c:macro:`I2S_PDM_TX_CLK_DEFAULT_CONFIG`

    Please refer to :ref:`i2s-api-reference-i2s_pdm` for PDM TX API information.
    And for more details, please refer to :component_file:`driver/include/driver/i2s_pdm.h`.

    The PDM data width is fixed to 16-bit, when the data in a ``int16_t`` writing buffer are:

    +--------+--------+--------+--------+--------+--------+--------+--------+--------+
    | data 0 | data 1 | data 2 | data 3 | data 4 | data 5 | data 6 | data 7 |  ...   |
    +========+========+========+========+========+========+========+========+========+
    | 0x0001 | 0x0002 | 0x0003 | 0x0004 | 0x0005 | 0x0006 | 0x0007 | 0x0008 |  ...   |
    +--------+--------+--------+--------+--------+--------+--------+--------+--------+

    .. only:: esp32

        Here is the table of the real data on the line with different :cpp:member:`i2s_pdm_tx_slot_config_t::slot_mode` and :cpp:member:`i2s_pdm_tx_slot_config_t::slot_mask` (The PDM format on the line is transferred to PCM format for better comprehension).

        +-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+
        | slot mode | slot mask |  left    |  right   |  left    |  right   |  left    |  right   |  left    |  right   |
        +===========+===========+==========+==========+==========+==========+==========+==========+==========+==========+
        |  mono     |   left    | 0x0001   | 0x0000   | 0x0002   | 0x0000   | 0x0003   | 0x0000   | 0x0004   | 0x0000   |
        |           +-----------+----------+----------+----------+----------+----------+----------+----------+----------+
        |           |   right   | 0x0000   | 0x0001   | 0x0000   | 0x0002   | 0x0000   | 0x0003   | 0x0000   | 0x0004   |
        |           +-----------+----------+----------+----------+----------+----------+----------+----------+----------+
        |           |   both    | 0x0001   | 0x0001   | 0x0002   | 0x0002   | 0x0003   | 0x0003   | 0x0004   | 0x0004   |
        +-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+
        |  stereo   |   left    | 0x0001   | 0x0001   | 0x0003   | 0x0003   | 0x0005   | 0x0005   | 0x0007   | 0x0007   |
        |           +-----------+----------+----------+----------+----------+----------+----------+----------+----------+
        |           |   right   | 0x0002   | 0x0002   | 0x0004   | 0x0004   | 0x0006   | 0x0006   | 0x0008   | 0x0008   |
        |           +-----------+----------+----------+----------+----------+----------+----------+----------+----------+
        |           |   both    | 0x0001   | 0x0002   | 0x0003   | 0x0004   | 0x0005   | 0x0006   | 0x0007   | 0x0008   |
        +-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+

    .. only:: esp32c3 or esp32s3 or esp32h2

        Here is the table of the real data on the line with different :cpp:member:`i2s_pdm_tx_slot_config_t::slot_mode` and :cpp:member:`i2s_pdm_tx_slot_config_t::line_mode` (The PDM format on the line is transferred to PCM format for easier comprehension).

        +----------------+-----------+------+--------+--------+--------+--------+--------+--------+--------+--------+
        |    line mode   | slot mode | line |  left  |  right |  left  |  right |  left  |  right |  left  |  right |
        +================+===========+======+========+========+========+========+========+========+========+========+
        |                |    mono   | dout | 0x0001 | 0x0000 | 0x0002 | 0x0000 | 0x0003 | 0x0000 | 0x0004 | 0x0000 |
        | one-line codec +-----------+------+--------+--------+--------+--------+--------+--------+--------+--------+
        |                |   stereo  | dout | 0x0001 | 0x0002 | 0x0003 | 0x0004 | 0x0005 | 0x0006 | 0x0007 | 0x0008 |
        +----------------+-----------+------+--------+--------+--------+--------+--------+--------+--------+--------+
        |  one-line dac  |    mono   | dout | 0x0001 | 0x0001 | 0x0002 | 0x0002 | 0x0003 | 0x0003 | 0x0004 | 0x0004 |
        +----------------+-----------+------+--------+--------+--------+--------+--------+--------+--------+--------+
        |                |    mono   | dout | 0x0002 | 0x0002 | 0x0004 | 0x0004 | 0x0006 | 0x0006 | 0x0008 | 0x0008 |
        |                |           +------+--------+--------+--------+--------+--------+--------+--------+--------+
        |                |           | dout2| 0x0000 | 0x0000 | 0x0000 | 0x0000 | 0x0000 | 0x0000 | 0x0000 | 0x0000 |
        |  two-line dac  +-----------+------+--------+--------+--------+--------+--------+--------+--------+--------+
        |                |   stereo  | dout | 0x0002 | 0x0002 | 0x0004 | 0x0004 | 0x0006 | 0x0006 | 0x0008 | 0x0008 |
        |                |           +------+--------+--------+--------+--------+--------+--------+--------+--------+
        |                |           | dout2| 0x0001 | 0x0001 | 0x0003 | 0x0003 | 0x0005 | 0x0005 | 0x0007 | 0x0007 |
        +----------------+-----------+------+--------+--------+--------+--------+--------+--------+--------+--------+

        .. note::

            There are three line modes for PDM TX mode, they are ``I2S_PDM_TX_ONE_LINE_CODEC``, ``I2S_PDM_TX_ONE_LINE_DAC`` and ``I2S_PDM_TX_TWO_LINE_DAC``. One-line codec is for the PDM codecs those require clock signal, the PDM codec can differentiate the left and right slots by the clock level, and the other two are used to driver power amplifiers directly with a low-pass filter, they do not need the clock signal, so there are two lines to differentiate the left and right slots. Additionally, for the mono mode of one-line codec, the slot can be force to change to the right by setting the clock invert flag in gpio configuration.


    .. code-block:: c

        #include "driver/i2s_pdm.h"
        #include "driver/gpio.h"

        /* Allocate an I2S tx channel */
        i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
        i2s_new_channel(&chan_cfg, &tx_handle, NULL);

        /* Init the channel into PDM TX mode */
        i2s_pdm_tx_config_t pdm_tx_cfg = {
            .clk_cfg = I2S_PDM_TX_CLK_DEFAULT_CONFIG(36000),
            .slot_cfg = I2S_PDM_TX_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
            .gpio_cfg = {
                .clk = GPIO_NUM_5,
                .dout = GPIO_NUM_18,
                .invert_flags = {
                    .clk_inv = false,
                },
            },
        };
        i2s_channel_init_pdm_tx_mode(tx_handle, &pdm_tx_cfg);

        ...


.. only:: SOC_I2S_SUPPORTS_PDM_RX

    PDM RX usage
    ^^^^^^^^^^^^

    For PDM mode in RX channel, the slot configuration helper macro is:

    - :c:macro:`I2S_PDM_RX_SLOT_DEFAULT_CONFIG`

    The clock configuration helper macro is:

    - :c:macro:`I2S_PDM_RX_CLK_DEFAULT_CONFIG`

    Please refer to :ref:`i2s-api-reference-i2s_pdm` for PDM RX API information.
    And for more details, please refer to :component_file:`driver/include/driver/i2s_pdm.h`.

    The PDM data width is fixed to 16-bit, when the data on the line (The PDM format on the line is transferred to PCM format for easier comprehension) are:

    +--------+--------+--------+--------+--------+--------+--------+--------+--------+
    |  left  |  right |  left  |  right |  left  |  right |  left  |  right |  ...   |
    +========+========+========+========+========+========+========+========+========+
    | 0x0001 | 0x0002 | 0x0003 | 0x0004 | 0x0005 | 0x0006 | 0x0007 | 0x0008 |  ...   |
    +--------+--------+--------+--------+--------+--------+--------+--------+--------+

    Here is the table of the data that received in a 'int16_t' buffer with different :cpp:member:`i2s_pdm_rx_slot_config_t::slot_mode` and :cpp:member:`i2s_pdm_rx_slot_config_t::slot_mask`

    .. only:: esp32

        +-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+
        | slot mode | slot mask | data 0   | data 1   | data 2   | data 3   | data 4   | data 5   | data 6   | data 7   |
        +===========+===========+==========+==========+==========+==========+==========+==========+==========+==========+
        |  mono     |   left    | 0x0001   | 0x0003   | 0x0005   | 0x0007   | 0x0009   | 0x000b   | 0x000d   | 0x000f   |
        |           +-----------+----------+----------+----------+----------+----------+----------+----------+----------+
        |           |   right   | 0x0002   | 0x0004   | 0x0006   | 0x0008   | 0x000a   | 0x000c   | 0x000e   | 0x0010   |
        +-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+
        |  stereo   |   both    | 0x0001   | 0x0002   | 0x0003   | 0x0004   | 0x0005   | 0x0006   | 0x0007   | 0x0008   |
        +-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+

    .. only:: esp32s3

        +-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+
        | slot mode | slot mask | data 0   | data 1   | data 2   | data 3   | data 4   | data 5   | data 6   | data 7   |
        +===========+===========+==========+==========+==========+==========+==========+==========+==========+==========+
        |  mono     |   left    | 0x0001   | 0x0003   | 0x0005   | 0x0007   | 0x0009   | 0x000b   | 0x000d   | 0x000f   |
        |           +-----------+----------+----------+----------+----------+----------+----------+----------+----------+
        |           |   right   | 0x0002   | 0x0004   | 0x0006   | 0x0008   | 0x000a   | 0x000c   | 0x000e   | 0x0010   |
        +-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+
        |  stereo   |   both    | 0x0002   | 0x0001   | 0x0004   | 0x0003   | 0x0006   | 0x0005   | 0x0008   | 0x0007   |
        +-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+

        .. note::

            The right slot is received first in stereo mode. To switch the left and right slot in the buffer, please set the :cpp:member:`i2s_pdm_rx_gpio_config_t::invert_flags::clk_inv` to force invert the clock signal.

    .. code-block:: c

        #include "driver/i2s_pdm.h"
        #include "driver/gpio.h"

        i2s_chan_handle_t rx_handle;

        /* Allocate an I2S rx channel */
        i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
        i2s_new_channel(&chan_cfg, NULL, &rx_handle);

        /* Init the channel into PDM RX mode */
        i2s_pdm_rx_config_t pdm_rx_cfg = {
            .clk_cfg = I2S_PDM_RX_CLK_DEFAULT_CONFIG(36000),
            .slot_cfg = I2S_PDM_RX_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
            .gpio_cfg = {
                .clk = GPIO_NUM_5,
                .din = GPIO_NUM_19,
                .invert_flags = {
                    .clk_inv = false,
                },
            },
        };
        i2s_channel_init_pdm_rx_mode(rx_handle, &pdm_rx_cfg);

        ...


.. only:: SOC_I2S_SUPPORTS_TDM

    TDM TX/RX usage
    ^^^^^^^^^^^^^^^

    Different slot communication formats can be generated by following helper macros for TDM mode. As described above, there are four formats in TDM mode, their helper macros are:

    - :c:macro:`I2S_TDM_PHILIPS_SLOT_DEFAULT_CONFIG`
    - :c:macro:`I2S_TDM_MSB_SLOT_DEFAULT_CONFIG`
    - :c:macro:`I2S_TDM_PCM_SHORT_SLOT_DEFAULT_CONFIG`
    - :c:macro:`I2S_TDM_PCM_LONG_SLOT_DEFAULT_CONFIG`

    The clock config helper macro is:

    - :c:macro:`I2S_TDM_CLK_DEFAULT_CONFIG`

    Please refer to :ref:`i2s-api-reference-i2s_tdm` for TDM API information.
    And for more details, please refer to :component_file:`driver/include/driver/i2s_tdm.h`.

    TDM TX Mode
    ~~~~~~~~~~~

    .. code-block:: c

        #include "driver/i2s_tdm.h"
        #include "driver/gpio.h"

        /* Allocate an I2S tx channel */
        i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
        i2s_new_channel(&chan_cfg, &tx_handle, NULL);

        /* Init the channel into TDM mode */
        i2s_tdm_config_t tdm_cfg = {
            .clk_cfg = I2S_TDM_CLK_DEFAULT_CONFIG(44100),
            .slot_cfg = I2S_TDM_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO,
                        I2S_TDM_SLOT0 | I2S_TDM_SLOT1 | I2S_TDM_SLOT2 | I2S_TDM_SLOT3),
            .gpio_cfg = {
                .mclk = I2S_GPIO_UNUSED,
                .bclk = GPIO_NUM_4,
                .ws = GPIO_NUM_5,
                .dout = GPIO_NUM_18,
                .din = I2S_GPIO_UNUSED,
                .invert_flags = {
                    .mclk_inv = false,
                    .bclk_inv = false,
                    .ws_inv = false,
                },
            },
        };
        i2s_channel_init_tdm_mode(tx_handle, &tdm_cfg);

        ...

    TDM RX Mode
    ~~~~~~~~~~~

    .. code-block:: c

        #include "driver/i2s_tdm.h"
        #include "driver/gpio.h"

        /* Set the channel mode to TDM */
        i2s_chan_config_t chan_cfg = I2S_CHANNEL_CONFIG(I2S_ROLE_MASTER, I2S_COMM_MODE_TDM, &i2s_pin);
        i2s_new_channel(&chan_cfg, NULL, &rx_handle);

        /* Init the channel into TDM mode */
        i2s_tdm_config_t tdm_cfg = {
            .clk_cfg = I2S_TDM_CLK_DEFAULT_CONFIG(44100),
            .slot_cfg = I2S_TDM_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO,
                        I2S_TDM_SLOT0 | I2S_TDM_SLOT1 | I2S_TDM_SLOT2 | I2S_TDM_SLOT3),
            .gpio_cfg = {
                .mclk = I2S_GPIO_UNUSED,
                .bclk = GPIO_NUM_4,
                .ws = GPIO_NUM_5,
                .dout = I2S_GPIO_UNUSED,
                .din = GPIO_NUM_18,
                .invert_flags = {
                    .mclk_inv = false,
                    .bclk_inv = false,
                    .ws_inv = false,
                },
            },
        };
        i2s_channel_init_tdm_mode(rx_handle, &tdm_cfg);
        ...

Full-duplex
^^^^^^^^^^^

Full-duplex mode will register tx and rx channel in an I2S port at the same time, and they will share the BCLK and WS signal. Currently STD and TDM communication mode are able to adopt full-duplex mode in following way, but PDM full-duplex is not supported because PDM TX and RX clock are not same.

Note that one handle can only stand for one channel, the slot and clock configurations for both tx and rx channel should be set one by one.

Here is an example of how to allocate a pair of full-duplex channels:

.. code-block:: c

    #include "driver/i2s_std.h"
    #include "driver/gpio.h"

    i2s_chan_handle_t tx_handle;
    i2s_chan_handle_t rx_handle;

    /* Allocate a pair of I2S channel */
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    /* Allocate for tx and rx channel at the same time, then they will work in full-duplex mode */
    i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle);

    /* Set the configurations for BOTH TWO channels, since tx and rx channel have to be same in full-duplex mode */
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(32000),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = GPIO_NUM_4,
            .ws = GPIO_NUM_5,
            .dout = GPIO_NUM_18,
            .din = GPIO_NUM_19,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    i2s_channel_init_std_mode(tx_handle, &std_cfg);
    i2s_channel_init_std_mode(rx_handle, &std_cfg);

    i2s_channel_enable(tx_handle);
    i2s_channel_enable(rx_handle);

    ...

.. only:: SOC_I2S_HW_VERSION_1

    Simplex Mode
    ^^^^^^^^^^^^

    To allocate a channel handle in simplex mode, :cpp:func:`i2s_new_channel` should be called for each channel. The clock and gpio pins of TX/RX channel on {IDF_TARGET_NAME} are not separate, therefore TX and RX channel can't coexist on a same I2S port in simplex mode.

    .. code-block:: c

        #include "driver/i2s_std.h"
        #include "driver/gpio.h"

        i2s_chan_handle_t tx_handle;
        i2s_chan_handle_t rx_handle;

        i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
        i2s_new_channel(&chan_cfg, &tx_handle, NULL);
        i2s_std_config_t std_tx_cfg = {
            .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(48000),
            .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
            .gpio_cfg = {
                .mclk = GPIO_NUM_0,
                .bclk = GPIO_NUM_4,
                .ws = GPIO_NUM_5,
                .dout = GPIO_NUM_18,
                .din = I2S_GPIO_UNUSED,
                .invert_flags = {
                    .mclk_inv = false,
                    .bclk_inv = false,
                    .ws_inv = false,
                },
            },
        };
        /* Initialize the channel */
        i2s_channel_init_std_mode(tx_handle, &std_tx_cfg);
        i2s_channel_enable(tx_handle);

        /* rx channel will be registered on another I2S, if no other available I2S unit found
         * it will return ESP_ERR_NOT_FOUND */
        i2s_new_channel(&chan_cfg, NULL, &rx_handle);
        i2s_std_config_t std_rx_cfg = {
            .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(16000),
            .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO),
            .gpio_cfg = {
                .mclk = I2S_GPIO_UNUSED,
                .bclk = GPIO_NUM_6,
                .ws = GPIO_NUM_7,
                .dout = I2S_GPIO_UNUSED,
                .din = GPIO_NUM_19,
                .invert_flags = {
                    .mclk_inv = false,
                    .bclk_inv = false,
                    .ws_inv = false,
                },
            },
        };
        i2s_channel_init_std_mode(rx_handle, &std_rx_cfg);
        i2s_channel_enable(rx_handle);

.. only:: SOC_I2S_HW_VERSION_2

    Simplex Mode
    ^^^^^^^^^^^^

    To allocate a channel in simplex mode, :cpp:func:`i2s_new_channel` should be called for each channel. The clock and gpio pins of TX/RX channel on {IDF_TARGET_NAME} are separate, they can be configured in different modes and clocks, and they are able to coexist on a same I2S port in simplex mode. So PDM duplex can be realized by registering PDM TX simplex and PDM RX simplex on a same I2S port. But in this way, PDM TX/RX might work with different clocks, take care when configuring the gpio pins and clocks.

    The following example offers a use case for the simplex mode, but note that, although the internal MCLK signals for tx and rx channel are separate, the output MCLK can only be bound to one of them if they are from a same controller, if both channel initialized MCLK, it depends on which is initialized later.

    .. code-block:: c

        #include "driver/i2s_std.h"
        #include "driver/gpio.h"

        i2s_chan_handle_t tx_handle;
        i2s_chan_handle_t rx_handle;
        i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
        i2s_new_channel(&chan_cfg, &tx_handle, NULL);
        i2s_std_config_t std_tx_cfg = {
            .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(48000),
            .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
            .gpio_cfg = {
                .mclk = GPIO_NUM_0,
                .bclk = GPIO_NUM_4,
                .ws = GPIO_NUM_5,
                .dout = GPIO_NUM_18,
                .din = I2S_GPIO_UNUSED,
                .invert_flags = {
                    .mclk_inv = false,
                    .bclk_inv = false,
                    .ws_inv = false,
                },
            },
        };
        /* Initialize the channel */
        i2s_channel_init_std_mode(tx_handle, &std_tx_cfg);
        i2s_channel_enable(tx_handle);

        /* rx channel will be registered on another I2S, if no other available I2S unit found
         * it will return ESP_ERR_NOT_FOUND */
        i2s_new_channel(&chan_cfg, NULL, &rx_handle); // Both rx and tx channel will be registered on I2S0, but they can work with different configurations.
        i2s_std_config_t std_rx_cfg = {
            .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(16000),
            .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO),
            .gpio_cfg = {
                .mclk = I2S_GPIO_UNUSED,
                .bclk = GPIO_NUM_6,
                .ws = GPIO_NUM_7,
                .dout = I2S_GPIO_UNUSED,
                .din = GPIO_NUM_19,
                .invert_flags = {
                    .mclk_inv = false,
                    .bclk_inv = false,
                    .ws_inv = false,
                },
            },
        };
        i2s_channel_init_std_mode(rx_handle, &std_rx_cfg);
        i2s_channel_enable(rx_handle);


Application Notes
-----------------

How to Prevent Data Lost
^^^^^^^^^^^^^^^^^^^^^^^^

For the applications that need a high frequency sample rate, sometimes the massive throughput of receiving data may cause data lost. Users can receive data lost event by registering isr callback function to receive event queue:

    .. code-block:: c

        static IRAM_ATTR bool i2s_rx_queue_overflow_callback(i2s_chan_handle_t handle, i2s_event_data_t *event, void *user_ctx)
        {
            // handle rx queue overflow event ...
            return false;
        }

        i2s_event_callbacks_t cbs = {
            .on_recv = NULL,
            .on_recv_q_ovf = i2s_rx_queue_overflow_callback,
            .on_sent = NULL,
            .on_send_q_ovf = NULL,
        };
        TEST_ESP_OK(i2s_channel_register_event_callback(rx_handle, &cbs, NULL));

Please follow these steps to prevent data lost:

1. Determine the interrupt interval. Generally, when data lost happened, the interval should be the bigger the better, it can help to reduce the interrupt times, i.e., ``dma_frame_num`` should be as big as possible while the DMA buffer size won't exceed its maximum value 4092. The relationships are::

    interrupt_interval(unit: sec) = dma_frame_num / sample_rate
    dma_buffer_size = dma_frame_num * slot_num * data_bit_width / 8 <= 4092

2. Determine the ``dma_desc_num``. The ``dma_desc_num`` is decided by the max time of ``i2s_channel_read`` polling cycle, all the received data are supposed to be stored between two ``i2s_channel_read``. This cycle can be measured by a timer or an outputting gpio signal. The relationship is::

    dma_desc_num > polling_cycle / interrupt_interval

3. Determine the receiving buffer size. The receiving buffer that offered by user in ``i2s_channel_read`` should be able to take all the data in all dma buffers, that means it should be bigger than the total size of all the dma buffers::

    recv_buffer_size > dma_desc_num * dma_buffer_size

For example, if there is an I2S application, and the known values are::

    sample_rate = 144000 Hz
    data_bit_width = 32 bits
    slot_num = 2
    polling_cycle = 10ms

Then the parameters ``dma_frame_num``, ``dma_desc_num`` and ``recv_buf_size`` can be calculated according to the given known values::

    dma_frame_num * slot_num * data_bit_width / 8 = dma_buffer_size <= 4092
    dma_frame_num <= 511
    interrupt_interval = dma_frame_num / sample_rate = 511 / 144000 = 0.003549 s = 3.549 ms
    dma_desc_num > polling_cycle / interrupt_interval = cell(10 / 3.549) = cell(2.818) = 3
    recv_buffer_size > dma_desc_num * dma_buffer_size = 3 * 4092 = 12276 bytes


API Reference
-------------

.. _i2s-api-reference-i2s_std:

Standard Mode
^^^^^^^^^^^^^

.. include-build-file:: inc/i2s_std.inc

.. only:: SOC_I2S_SUPPORTS_PDM

    .. _i2s-api-reference-i2s_pdm:

    PDM Mode
    ^^^^^^^^

    .. include-build-file:: inc/i2s_pdm.inc

.. only:: SOC_I2S_SUPPORTS_TDM

    .. _i2s-api-reference-i2s_tdm:

    TDM Mode
    ^^^^^^^^

    .. include-build-file:: inc/i2s_tdm.inc

.. _i2s-api-reference-i2s_driver:

I2S Driver
^^^^^^^^^^

.. include-build-file:: inc/i2s_common.inc

.. _i2s-api-reference-i2s_types:

I2S Types
^^^^^^^^^

.. include-build-file:: inc/components/driver/include/driver/i2s_types.inc
.. include-build-file:: inc/components/hal/include/hal/i2s_types.inc
