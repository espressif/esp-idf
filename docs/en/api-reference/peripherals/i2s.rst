Inter-IC Sound (I2S)
====================

:link_to_translation:`zh_CN:[中文]`

{IDF_TARGET_I2S_NUM:default="one", esp32="two", esp32s3="two"}

Introduction
------------

I2S (Inter-IC Sound) is a synchronous serial communication protocol usually used for transmitting audio data between two digital audio devices.

{IDF_TARGET_NAME} contains {IDF_TARGET_I2S_NUM} I2S peripheral(s). These peripherals can be configured to input and output sample data via the I2S driver.

An I2S bus that communicates in standard or TDM mode consists of the following lines:

- **MCLK:** Master clock line. It is an optional signal depending on the slave side, mainly used for offering a reference clock to the I2S slave device.
- **BCLK:** Bit clock line. The bit clock for data line.
- **WS:** Word (Slot) select line. It is usually used to identify the vocal tract except PDM mode.
- **DIN/DOUT:** Serial data input/output line. Data will loopback internally if DIN and DOUT are set to a same GPIO.

.. only:: SOC_I2S_SUPPORTS_PDM_TX or SOC_I2S_SUPPORTS_PDM_RX

    An I2S bus that communicates in PDM mode consists of the following lines:

    - **CLK:** PDM clock line.
    - **DIN/DOUT:** Serial data input/output line.

Each I2S controller has the following features that can be configured by the I2S driver:

- Operation as system master or slave
- Capable of acting as transmitter or receiver
- DMA controller that allows stream sampling of data without requiring the CPU to copy each data sample

.. only:: SOC_I2S_HW_VERSION_1

    Each controller supports single RX or TX simplex communication. As RX and TX channels share a clock, they can only be combined with the same configuration to establish a full-duplex communication.

.. only:: SOC_I2S_HW_VERSION_2

    Each controller has separate RX and TX channels. That means they are able to work under different clocks and slot configurations with separate GPIO pins. Note that although the internal MCLKs of TX channel and RX channel are separate on a controller, the output MCLK signal can only be attached to one channel. If independent MCLK output is required for each channel, they must be allocated on different I2S controllers.

I2S File Structure
------------------

.. figure:: ../../../_static/diagrams/i2s/i2s_file_structure.png
    :align: center
    :alt: I2S file structure

    I2S File Structure

**Public headers that need to be included in the I2S application are as follows:**

- ``i2s.h``: The header file that provides legacy I2S APIs (for apps using legacy driver).
- ``i2s_std.h``: The header file that provides standard communication mode specific APIs (for apps using new driver with standard mode).
- ``i2s_pdm.h``: The header file that provides PDM communication mode specific APIs (for apps using new driver with PDM mode).
- ``i2s_tdm.h``: The header file that provides TDM communication mode specific APIs (for apps using new driver with TDM mode).

.. note::

    The legacy driver cannot coexist with the new driver. Include ``i2s.h`` to use the legacy driver, or include the other three headers to use the new driver. The legacy driver might be removed in future.

**Public headers that have been included in the headers above are as follows:**

- ``i2s_types_legacy.h``: The header file that provides legacy public types that are only used in the legacy driver.
- ``i2s_types.h``: The header file that provides public types.
- ``i2s_common.h``: The header file that provides common APIs for all communication modes.

I2S Clock
---------

Clock Source
^^^^^^^^^^^^

- :cpp:enumerator:`i2s_clock_src_t::I2S_CLK_SRC_DEFAULT`: Default PLL clock.

.. only:: SOC_I2S_SUPPORTS_PLL_F160M

    - :cpp:enumerator:`i2s_clock_src_t::I2S_CLK_SRC_PLL_160M`: 160 MHz PLL clock.

.. only:: SOC_I2S_SUPPORTS_PLL_F96M

    - :cpp:enumerator:`i2s_clock_src_t::I2S_CLK_SRC_PLL_96M`: 96 MHz PLL clock.

.. only:: SOC_I2S_SUPPORTS_PLL_F240M

    - :cpp:enumerator:`i2s_clock_src_t::I2S_CLK_SRC_PLL_240M`: 240 MHz PLL clock.

.. only:: SOC_I2S_SUPPORTS_APLL

    - :cpp:enumerator:`i2s_clock_src_t::I2S_CLK_SRC_APLL`: Audio PLL clock, which is more precise than ``I2S_CLK_SRC_PLL_160M`` in high sample rate applications. Its frequency is configurable according to the sample rate. However, if APLL has been occupied by EMAC or other channels, the APLL frequency cannot be changed, and the driver will try to work under this APLL frequency. If this frequency cannot meet the requirements of I2S, the clock configuration will fail.

Clock Terminology
^^^^^^^^^^^^^^^^^

- **Sample rate**: The number of sampled data in one second per slot.
- **SCLK**: Source clock frequency. It is the frequency of the clock source.
- **MCLK**: Master clock frequency. BCLK is generated from this clock. The MCLK signal usually serves as a reference clock and is mostly needed to synchronize BCLK and WS between I2S master and slave roles.
- **BCLK**: Bit clock frequency. Every tick of this clock stands for one data bit on data pin. The slot bit width configured in :cpp:member:`i2s_std_slot_config_t::slot_bit_width` is equal to the number of BCLK ticks, which means there will be 8/16/24/32 BCLK ticks in one slot.
- **LRCK** / **WS**: Left/right clock or word select clock. For non-PDM mode, its frequency is equal to the sample rate.

.. note::

    Normally, MCLK should be the multiple of ``sample rate`` and BCLK at the same time. The field :cpp:member:`i2s_std_clk_config_t::mclk_multiple` indicates the multiple of MCLK to the ``sample rate``. In most cases, ``I2S_MCLK_MULTIPLE_256`` should be enough. However, if ``slot_bit_width`` is set to ``I2S_SLOT_BIT_WIDTH_24BIT``, to keep MCLK a multiple to the BCLK, :cpp:member:`i2s_std_clk_config_t::mclk_multiple` should be set to multiples that are divisible by 3 such as ``I2S_MCLK_MULTIPLE_384``. Otherwise, WS will be inaccurate.

.. _i2s-communication-mode:

I2S Communication Mode
----------------------

Overview of All Modes
^^^^^^^^^^^^^^^^^^^^^

=========  ========  ========  ========  ========  ========  ==========
 Target    Standard   PDM TX    PDM RX     TDM     ADC/DAC   LCD/Camera
=========  ========  ========  ========  ========  ========  ==========
ESP32      I2S 0/1    I2S 0     I2S 0      none     I2S 0      I2S 0
ESP32-S2    I2S 0     none      none       none     none       I2S 0
ESP32-C3    I2S 0     I2S 0     none      I2S 0     none       none
ESP32-C6    I2S 0     I2S 0     none      I2S 0     none       none
ESP32-S3   I2S 0/1    I2S 0     I2S 0    I2S 0/1    none       none
ESP32-H2    I2S 0     I2S 0     none      I2S 0     none       none
ESP32-P4   I2S 0~2    I2S 0     I2S 0    I2S 0~2    none       none
ESP32-C5    I2S 0     I2S 0     none      I2S 0     none       none
=========  ========  ========  ========  ========  ========  ==========

Standard Mode
^^^^^^^^^^^^^

In standard mode, there are always two sound channels, i.e., the left and right channels, which are called "slots". These slots support 8/16/24/32-bit width sample data. The communication format for the slots mainly includes the following:

- **Philips Format**: Data signal has one-bit shift comparing to the WS signal, and the duty of WS signal is 50%.

.. wavedrom:: /../_static/diagrams/i2s/std_philips.json

- **MSB Format**: Basically the same as Philips format, but without data shift.

.. wavedrom:: /../_static/diagrams/i2s/std_msb.json

- **PCM Short Format**: Data has one-bit shift and meanwhile the WS signal becomes a pulse lasting for one BCLK cycle.

.. wavedrom:: /../_static/diagrams/i2s/std_pcm.json


.. only:: SOC_I2S_SUPPORTS_PDM_TX

    PDM Mode (TX)
    ^^^^^^^^^^^^^

    PDM (Pulse-density Modulation) mode for the TX channel can convert PCM data into PDM format which always has left and right slots. PDM TX is only supported on I2S0 and it only supports 16-bit width sample data. It needs at least a CLK pin for clock signal and a DOUT pin for data signal (i.e., the WS and SD signal in the following figure; the BCK signal is an internal bit sampling clock, which is not needed between PDM devices). This mode allows users to configure the up-sampling parameters :cpp:member:`i2s_pdm_tx_clk_config_t::up_sample_fp` and :cpp:member:`i2s_pdm_tx_clk_config_t::up_sample_fs`. The up-sampling rate can be calculated by ``up_sample_rate = i2s_pdm_tx_clk_config_t::up_sample_fp / i2s_pdm_tx_clk_config_t::up_sample_fs``. There are two up-sampling modes in PDM TX:

    - **Fixed Clock Frequency**: In this mode, the up-sampling rate changes according to the sample rate. Setting ``fp = 960`` and ``fs = sample_rate / 100``, then the clock frequency (Fpdm) on CLK pin will be fixed to ``128 * 48 KHz = 6.144 MHz``. Note that this frequency is not equal to the sample rate (Fpcm).
    - **Fixed Up-sampling Rate**: In this mode, the up-sampling rate is fixed to 2. Setting ``fp = 960`` and ``fs = 480``, then the clock frequency (Fpdm) on CLK pin will be ``128 * sample_rate``.

    .. wavedrom:: /../_static/diagrams/i2s/pdm.json


.. only:: SOC_I2S_SUPPORTS_PDM_RX

    PDM Mode (RX)
    ^^^^^^^^^^^^^

    PDM (Pulse-density Modulation) mode for RX channel can receive PDM-format data and convert the data into PCM format. PDM RX is only supported on I2S0, and it only supports 16-bit width sample data. PDM RX needs at least a CLK pin for clock signal and a DIN pin for data signal. This mode allows users to configure the down-sampling parameter :cpp:member:`i2s_pdm_rx_clk_config_t::dn_sample_mode`. There are two down-sampling modes in PDM RX:

    - :cpp:enumerator:`i2s_pdm_dsr_t::I2S_PDM_DSR_8S`: In this mode, the clock frequency (Fpdm) on the WS pin is ``sample_rate (Fpcm) * 64``.
    - :cpp:enumerator:`i2s_pdm_dsr_t::I2S_PDM_DSR_16S`: In this mode, the clock frequency (Fpdm) on the WS pin is ``sample_rate (Fpcm) * 128``.


.. only:: SOC_I2S_SUPPORTS_TDM

    TDM Mode
    ^^^^^^^^

    TDM (Time Division Multiplexing) mode supports up to 16 slots. These slots can be enabled by :cpp:member:`i2s_tdm_slot_config_t::slot_mask`.

    .. only:: SOC_I2S_TDM_FULL_DATA_WIDTH

        Any data bit-width is supported no matter how many slots are enabled, which means there can be up to ``32 bit-width * 16 slots = 512 bit`` data in one frame.

    .. only:: not SOC_I2S_TDM_FULL_DATA_WIDTH

        But due to the hardware limitation, only up to 4 slots are supported while the slot is set to 32 bit-width, and 8 slots for 16 bit-width, 16 slots for 8 bit-width. The slot communication format of TDM is almost the same as the standard mode, yet with some small differences.

    - **Philips Format**: Data signal has one-bit shift comparing to the WS signal. And no matter how many slots are contained in one frame, the duty of WS signal always keeps 50%.

    .. wavedrom:: /../_static/diagrams/i2s/tdm_philips.json

    - **MSB Format**: Basically the same as the Philips format, but without data shift.

    .. wavedrom:: /../_static/diagrams/i2s/tdm_msb.json

    - **PCM Short Format**: Data has one-bit shift and the WS signal becomes a pulse lasting one BCLK cycle for every frame.

    .. wavedrom:: /../_static/diagrams/i2s/tdm_pcm_short.json

    - **PCM Long Format**: Data has one-bit shift and the WS signal lasts one-slot bit width for every frame. For example, the duty of WS will be 25% if there are four slots enabled, and 20% if there are five slots.

    .. wavedrom:: /../_static/diagrams/i2s/tdm_pcm_long.json

.. only:: SOC_I2S_SUPPORTS_LCD_CAMERA

    LCD/Camera Mode
    ^^^^^^^^^^^^^^^

    LCD/Camera mode is only supported on I2S0 over a parallel bus. For LCD mode, I2S0 should work at master TX mode. For camera mode, I2S0 should work at slave RX mode. These two modes are not implemented by the I2S driver. Please refer to :doc:`/api-reference/peripherals/lcd/i80_lcd` for details about the LCD implementation. For more information, see **{IDF_TARGET_NAME} Technical Reference Manual** > **I2S Controller (I2S)** > LCD Mode [`PDF <{IDF_TARGET_TRM_EN_URL}#camlcdctrl>`__].

.. only:: SOC_I2S_SUPPORTS_ADC_DAC

    ADC/DAC Mode
    ^^^^^^^^^^^^

    ADC and DAC modes only exist on ESP32 and are only supported on I2S0. Actually, they are two sub-modes of LCD/Camera mode. I2S0 can be routed directly to the internal analog-to-digital converter (ADC) and digital-to-analog converter (DAC). In other words, ADC and DAC peripherals can read or write continuously via I2S0 DMA. As they are not actual communication modes, the I2S driver does not implement them.

Functional Overview
-------------------

The I2S driver offers the following services:

Resource Management
^^^^^^^^^^^^^^^^^^^

There are three levels of resources in the I2S driver:

- ``platform level``: Resources of all I2S controllers in the current target.
- ``controller level``: Resources in one I2S controller.
- ``channel level``: Resources of TX or RX channel in one I2S controller.

The public APIs are all channel-level APIs. The channel handle :cpp:type:`i2s_chan_handle_t` can help users to manage the resources under a specific channel without considering the other two levels. The other two upper levels' resources are private and are managed by the driver automatically. Users can call :cpp:func:`i2s_new_channel` to allocate a channel handle and call :cpp:func:`i2s_del_channel` to delete it.

Power Management
^^^^^^^^^^^^^^^^

When the power management is enabled (i.e., :ref:`CONFIG_PM_ENABLE` is on), the system will adjust or stop the source clock of I2S before entering Light-sleep, thus potentially changing the I2S signals and leading to transmitting or receiving invalid data.

The I2S driver can prevent the system from changing or stopping the source clock by acquiring a power management lock. When the source clock is generated from APB, the lock type will be set to :cpp:enumerator:`esp_pm_lock_type_t::ESP_PM_APB_FREQ_MAX` and when the source clock is APLL (if supported), it will be set to :cpp:enumerator:`esp_pm_lock_type_t::ESP_PM_NO_LIGHT_SLEEP`. Whenever the user is reading or writing via I2S (i.e., calling :cpp:func:`i2s_channel_read` or :cpp:func:`i2s_channel_write`), the driver guarantees that the power management lock is acquired. Likewise, the driver releases the lock after the reading or writing finishes.

Finite State Machine
^^^^^^^^^^^^^^^^^^^^

There are three states for an I2S channel, namely, ``registered``, ``ready``, and ``running``. Their relationship is shown in the following diagram:

.. figure:: ../../../_static/diagrams/i2s/i2s_state_machine.png
    :align: center
    :alt: I2S Finite State Machine

    I2S Finite State Machine

The ``<mode>`` in the diagram can be replaced by corresponding I2S communication modes, e.g., ``std`` for standard two-slot mode. For more information about communication modes, please refer to the :ref:`i2s-communication-mode` section.

Data Transport
^^^^^^^^^^^^^^

The data transport of the I2S peripheral, including sending and receiving, is realized by DMA. Before transporting data, please call :cpp:func:`i2s_channel_enable` to enable the specific channel. When the sent or received data reaches the size of one DMA buffer, the ``I2S_OUT_EOF`` or ``I2S_IN_SUC_EOF`` interrupt will be triggered. Note that the DMA buffer size is not equal to :cpp:member:`i2s_chan_config_t::dma_frame_num`. One frame here refers to all the sampled data in one WS circle. Therefore, ``dma_buffer_size = dma_frame_num * slot_num * slot_bit_width / 8``. For the data transmitting, users can input the data by calling :cpp:func:`i2s_channel_write`. This function helps users to copy the data from the source buffer to the DMA TX buffer and wait for the transmission to finish. Then it will repeat until the sent bytes reach the given size. For the data receiving, the function :cpp:func:`i2s_channel_read` waits to receive the message queue which contains the DMA buffer address. It helps users copy the data from the DMA RX buffer to the destination buffer.

Both :cpp:func:`i2s_channel_write` and :cpp:func:`i2s_channel_read` are blocking functions. They keeps waiting until the whole source buffer is sent or the whole destination buffer is loaded, unless they exceed the max blocking time, where the error code ``ESP_ERR_TIMEOUT`` returns. To send or receive data asynchronously, callbacks can be registered by  :cpp:func:`i2s_channel_register_event_callback`. Users are able to access the DMA buffer directly in the callback function instead of transmitting or receiving by the two blocking functions. However, please be aware that it is an interrupt callback, so do not add complex logic, run floating operation, or call non-reentrant functions in the callback.

Configuration
^^^^^^^^^^^^^

Users can initialize a channel by calling corresponding functions (i.e., :func:`i2s_channel_init_std_mode`, :func:`i2s_channel_init_pdm_rx_mode`, :func:`i2s_channel_init_pdm_tx_mode`, or :func:`i2s_channel_init_tdm_mode`) to a specific mode. If the configurations need to be updated after initialization, users have to first call :cpp:func:`i2s_channel_disable` to ensure that the channel has stopped, and then call corresponding ``reconfig`` functions, like :cpp:func:`i2s_channel_reconfig_std_slot`, :cpp:func:`i2s_channel_reconfig_std_clock`, and :cpp:func:`i2s_channel_reconfig_std_gpio`.

IRAM Safe
^^^^^^^^^

By default, the I2S interrupt will be deferred when the cache is disabled for reasons like writing/erasing flash. Thus the EOF interrupt will not get executed in time.

To avoid such case in real-time applications, you can enable the Kconfig option :ref:`CONFIG_I2S_ISR_IRAM_SAFE` that:

1. Keeps the interrupt being serviced even when the cache is disabled.

2. Places driver object into DRAM (in case it is linked to PSRAM by accident).

This allows the interrupt to run while the cache is disabled, but comes at the cost of increased IRAM consumption.

Thread Safety
^^^^^^^^^^^^^

All the public I2S APIs are guaranteed to be thread safe by the driver, which means users can call them from different RTOS tasks without protection by extra locks. Notice that the I2S driver uses mutex lock to ensure the thread safety, thus these APIs are not allowed to be used in ISR.

Kconfig Options
^^^^^^^^^^^^^^^

- :ref:`CONFIG_I2S_ISR_IRAM_SAFE` controls whether the default ISR handler can work when the cache is disabled. See `IRAM Safe <#iram-safe>`__ for more information.
- :ref:`CONFIG_I2S_SUPPRESS_DEPRECATE_WARN` controls whether to suppress the compiling warning message while using the legacy I2S driver.
- :ref:`CONFIG_I2S_ENABLE_DEBUG_LOG` is used to enable the debug log output. Enable this option increases the firmware binary size.

Application Example
-------------------

The examples of the I2S driver can be found in the directory :example:`peripherals/i2s`. Here are some simple usages of each mode:

Standard TX/RX Usage
^^^^^^^^^^^^^^^^^^^^

- :example:`peripherals/i2s/i2s_codec/i2s_es8311` demonstrates how to use the I2S ES8311 audio codec with {IDF_TARGET_NAME} to play music or echo sounds, featuring high performance and low power multi-bit delta-sigma audio ADC and DAC, with options to customize music and adjust mic gain and volume.
- :example:`peripherals/i2s/i2s_basic/i2s_std` demonstrates how to use the I2S standard mode in either simplex or full-duplex mode on {IDF_TARGET_NAME}.

Different slot communication formats can be generated by the following helper macros for standard mode. As described above, there are three formats in standard mode, and their helper macros are:

- :c:macro:`I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG`
- :c:macro:`I2S_STD_PCM_SLOT_DEFAULT_CONFIG`
- :c:macro:`I2S_STD_MSB_SLOT_DEFAULT_CONFIG`

The clock config helper macro is:

- :c:macro:`I2S_STD_CLK_DEFAULT_CONFIG`

Please refer to :ref:`i2s-api-reference-i2s_std` for  information about STD API. And for more details, please refer to :component_file:`esp_driver_i2s/include/driver/i2s_std.h`.

STD TX Mode
~~~~~~~~~~~

Take 16-bit data width for example. When the data in a ``uint16_t`` writing buffer are:

+--------+--------+--------+--------+--------+--------+--------+--------+--------+
| data 0 | data 1 | data 2 | data 3 | data 4 | data 5 | data 6 | data 7 |  ...   |
+========+========+========+========+========+========+========+========+========+
| 0x0001 | 0x0002 | 0x0003 | 0x0004 | 0x0005 | 0x0006 | 0x0007 | 0x0008 |  ...   |
+--------+--------+--------+--------+--------+--------+--------+--------+--------+

Here is the table of the real data on the line with different :cpp:member:`i2s_std_slot_config_t::slot_mode` and :cpp:member:`i2s_std_slot_config_t::slot_mask`.

.. only:: esp32

    +----------------+-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    | data bit width | slot mode | slot mask | WS low   | WS high  | WS low   | WS high  | WS low   | WS high  | WS low   | WS high  |
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

        It is similar when the data is 32-bit width, but take care when using 8-bit and 24-bit data width. For 8-bit width, the written buffer should still use ``uint16_t`` (i.e., align with 2 bytes), and only the high 8 bits are valid while the low 8 bits are dropped. For 24-bit width, the buffer is supposed to use ``uint32_t`` (i.e., align with 4 bytes), and only the high 24 bits are valid while the low 8 bits are dropped.

        Besides, for 8-bit and 16-bit mono modes, the real data on the line is swapped. To get the correct data sequence, the writing buffer needs to swap the data every two bytes.

.. only:: esp32s2

    +----------------+-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    | data bit width | slot mode | slot mask | WS low   | WS high  | WS low   | WS high  | WS low   | WS high  | WS low   | WS high  |
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

        Similar for 8-bit and 32-bit data widths, the type of the buffer is better to be ``uint8_t`` and ``uint32_t``. But specially, when the data width is 24-bit, the data buffer should be aligned with 3-byte (i.e., every 3 bytes stands for a 24-bit data in one slot). Additionally, :cpp:member:`i2s_chan_config_t::dma_frame_num`, :cpp:member:`i2s_std_clk_config_t::mclk_multiple`, and the writing buffer size should be the multiple of ``3``, otherwise the data on the line or the sample rate will be incorrect.

.. only:: not (esp32 or esp32s2)

    +----------------+-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    | data bit width | slot mode | slot mask | WS low   | WS high  | WS low   | WS high  | WS low   | WS high  | WS low   | WS high  |
    +================+===========+===========+==========+==========+==========+==========+==========+==========+==========+==========+
    |                |  mono     |   left    | 0x0001   | 0x0000   | 0x0002   | 0x0000   | 0x0003   | 0x0000   | 0x0004   | 0x0000   |
    |     16 bit     |           +-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    |                |           |   right   | 0x0000   | 0x0001   | 0x0000   | 0x0002   | 0x0000   | 0x0003   | 0x0000   | 0x0004   |
    |                |           +-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    |                |           |   both    | 0x0001   | 0x0001   | 0x0002   | 0x0002   | 0x0003   | 0x0003   | 0x0004   | 0x0004   |
    |                +-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    |                |  stereo   |   left    | 0x0001   | 0x0000   | 0x0003   | 0x0000   | 0x0005   | 0x0000   | 0x0007   | 0x0000   |
    |                |           +-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    |                |           |   right   | 0x0000   | 0x0002   | 0x0000   | 0x0004   | 0x0000   | 0x0006   | 0x0000   | 0x0008   |
    |                |           +-----------+----------+----------+----------+----------+----------+----------+----------+----------+
    |                |           |   both    | 0x0001   | 0x0002   | 0x0003   | 0x0004   | 0x0005   | 0x0006   | 0x0007   | 0x0008   |
    +----------------+-----------+-----------+----------+----------+----------+----------+----------+----------+----------+----------+

    .. note::

        Similar for 8-bit and 32-bit data widths, the type of the buffer is better to be ``uint8_t`` and ``uint32_t``. But specially, when the data width is 24-bit, the data buffer should be aligned with 3-byte (i.e., every 3 bytes stands for a 24-bit data in one slot). Additionally, :cpp:member:`i2s_chan_config_t::dma_frame_num`, :cpp:member:`i2s_std_clk_config_t::mclk_multiple`, and the writing buffer size should be the multiple of ``3``, otherwise the data on the line or the sample rate will be incorrect.

.. code-block:: c

    #include "driver/i2s_std.h"
    #include "driver/gpio.h"

    i2s_chan_handle_t tx_handle;
    /* Get the default channel configuration by the helper macro.
     * This helper macro is defined in `i2s_common.h` and shared by all the I2S communication modes.
     * It can help to specify the I2S role and port ID */
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    /* Allocate a new TX channel and get the handle of this channel */
    i2s_new_channel(&chan_cfg, &tx_handle, NULL);

    /* Setting the configurations, the slot configuration and clock configuration can be generated by the macros
     * These two helper macros are defined in `i2s_std.h` which can only be used in STD mode.
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

    /* Before writing data, start the TX channel first */
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

Taking 16-bit data width for example, when the data on the line are:

+--------+--------+--------+--------+--------+--------+--------+--------+--------+
| WS low | WS high| WS low | WS high| WS low | WS high| WS low | WS high|  ...   |
+========+========+========+========+========+========+========+========+========+
| 0x0001 | 0x0002 | 0x0003 | 0x0004 | 0x0005 | 0x0006 | 0x0007 | 0x0008 |  ...   |
+--------+--------+--------+--------+--------+--------+--------+--------+--------+

Here is the table of the data received in the buffer with different :cpp:member:`i2s_std_slot_config_t::slot_mode` and :cpp:member:`i2s_std_slot_config_t::slot_mask`.

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

        The receive case is a little bit complicated on ESP32. Firstly, when the data width is 8-bit or 24-bit, the received data will still align with two bytes or four bytes, which means that the valid data are put in the high 8 bits in every two bytes and high 24 bits in every four bytes. For example, the received data will be ``0x5A00`` when the data on the line is ``0x5A`` in 8-bit width, and ``0x0000 5A00`` if the data on the line is ``0x00 005A``. Secondly, for the 8-bit or 16-bit mono case, the data in buffer is swapped every two data, so it may be necessary to manually swap the data back to the correct order.

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

        8-bit, 24-bit, and 32-bit are similar as 16-bit, where the data bit-width in the receiving buffer is equal to the data bit-width on the line. Additionally, when using 24-bit data width, :cpp:member:`i2s_chan_config_t::dma_frame_num`, :cpp:member:`i2s_std_clk_config_t::mclk_multiple`, and the receiving buffer size should be the multiple of ``3``, otherwise the data on the line or the sample rate will be incorrect.

.. only:: not (esp32 or esp32s2)

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

        8-bit, 24-bit, and 32-bit are similar as 16-bit, the data bit-width in the receiving buffer is equal to the data bit-width on the line. Additionally, when using 24-bit data width, :cpp:member:`i2s_chan_config_t::dma_frame_num`, :cpp:member:`i2s_std_clk_config_t::mclk_multiple`, and the receiving buffer size should be the multiple of ``3``, otherwise the data on the line or the sample rate will be incorrect.

.. code-block:: c

    #include "driver/i2s_std.h"
    #include "driver/gpio.h"

    i2s_chan_handle_t rx_handle;
    /* Get the default channel configuration by helper macro.
     * This helper macro is defined in `i2s_common.h` and shared by all the I2S communication modes.
     * It can help to specify the I2S role and port ID */
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    /* Allocate a new RX channel and get the handle of this channel */
    i2s_new_channel(&chan_cfg, NULL, &rx_handle);

    /* Setting the configurations, the slot configuration and clock configuration can be generated by the macros
     * These two helper macros are defined in `i2s_std.h` which can only be used in STD mode.
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

    /* Before reading data, start the RX channel first */
    i2s_channel_enable(rx_handle);
    i2s_channel_read(rx_handle, desc_buf, bytes_to_read, bytes_read, ticks_to_wait);

    /* Have to stop the channel before deleting it */
    i2s_channel_disable(rx_handle);
    /* If the handle is not needed any more, delete it to release the channel resources */
    i2s_del_channel(rx_handle);


.. only:: SOC_I2S_SUPPORTS_PDM_TX

    PDM TX Usage
    ^^^^^^^^^^^^

    - :example:`peripherals/i2s/i2s_basic/i2s_pdm` demonstrates how to use the PDM TX mode on {IDF_TARGET_NAME}, including the necessary hardware setup and configuration.

    For PDM mode in TX channel, the slot configuration helper macro is:

    - :c:macro:`I2S_PDM_TX_SLOT_DEFAULT_CONFIG`

    The clock configuration helper macro is:

    - :c:macro:`I2S_PDM_TX_CLK_DEFAULT_CONFIG`

    Please refer to :ref:`i2s-api-reference-i2s_pdm` for information about PDM TX API. And for more details, please refer to :component_file:`esp_driver_i2s/include/driver/i2s_pdm.h`.

    The PDM data width is fixed to 16-bit. When the data in an ``int16_t`` writing buffer is:

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

    .. only:: not esp32

        Here is the table of the real data on the line with different :cpp:member:`i2s_pdm_tx_slot_config_t::slot_mode` and :cpp:member:`i2s_pdm_tx_slot_config_t::line_mode` (The PDM format on the line is transferred to PCM format for easier comprehension).

        +----------------+-----------+------+--------+--------+--------+--------+--------+--------+--------+--------+
        |    line mode   | slot mode | line |  left  |  right |  left  |  right |  left  |  right |  left  |  right |
        +================+===========+======+========+========+========+========+========+========+========+========+
        |                |    mono   | dout | 0x0001 | 0x0000 | 0x0002 | 0x0000 | 0x0003 | 0x0000 | 0x0004 | 0x0000 |
        | one-line Codec +-----------+------+--------+--------+--------+--------+--------+--------+--------+--------+
        |                |   stereo  | dout | 0x0001 | 0x0002 | 0x0003 | 0x0004 | 0x0005 | 0x0006 | 0x0007 | 0x0008 |
        +----------------+-----------+------+--------+--------+--------+--------+--------+--------+--------+--------+
        |  one-line DAC  |    mono   | dout | 0x0001 | 0x0001 | 0x0002 | 0x0002 | 0x0003 | 0x0003 | 0x0004 | 0x0004 |
        +----------------+-----------+------+--------+--------+--------+--------+--------+--------+--------+--------+
        |                |    mono   | dout | 0x0002 | 0x0002 | 0x0004 | 0x0004 | 0x0006 | 0x0006 | 0x0008 | 0x0008 |
        |                |           +------+--------+--------+--------+--------+--------+--------+--------+--------+
        |                |           | dout2| 0x0000 | 0x0000 | 0x0000 | 0x0000 | 0x0000 | 0x0000 | 0x0000 | 0x0000 |
        |  two-line DAC  +-----------+------+--------+--------+--------+--------+--------+--------+--------+--------+
        |                |   stereo  | dout | 0x0002 | 0x0002 | 0x0004 | 0x0004 | 0x0006 | 0x0006 | 0x0008 | 0x0008 |
        |                |           +------+--------+--------+--------+--------+--------+--------+--------+--------+
        |                |           | dout2| 0x0001 | 0x0001 | 0x0003 | 0x0003 | 0x0005 | 0x0005 | 0x0007 | 0x0007 |
        +----------------+-----------+------+--------+--------+--------+--------+--------+--------+--------+--------+

        .. note::

            There are three line modes for PDM TX mode, i.e., ``I2S_PDM_TX_ONE_LINE_CODEC``, ``I2S_PDM_TX_ONE_LINE_DAC``, and ``I2S_PDM_TX_TWO_LINE_DAC``. One-line codec is for the PDM codecs that require clock signal. The PDM codec can differentiate the left and right slots by the clock level. The other two modes are used to drive power amplifiers directly with a low-pass filter. They do not need the clock signal, so there are two lines to differentiate the left and right slots. Additionally, for the mono mode of one-line codec, users can force change the slot to the right by setting the clock invert flag in GPIO configuration.


    .. code-block:: c

        #include "driver/i2s_pdm.h"
        #include "driver/gpio.h"

        /* Allocate an I2S TX channel */
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

    PDM RX Usage
    ^^^^^^^^^^^^

    - :example:`peripherals/i2s/i2s_recorder` demonstrates how to record audio from a digital MEMS microphone using the I2S peripheral in PDM data format and save it to an SD card in ``.wav`` file format on {IDF_TARGET_NAME} development boards.
    - :example:`peripherals/i2s/i2s_basic/i2s_pdm` demonstrates how to use the PDM RX mode on {IDF_TARGET_NAME}, including the necessary hardware setup and configuration.

    For PDM mode in RX channel, the slot configuration helper macro is:

    - :c:macro:`I2S_PDM_RX_SLOT_DEFAULT_CONFIG`

    The clock configuration helper macro is:

    - :c:macro:`I2S_PDM_RX_CLK_DEFAULT_CONFIG`

    Please refer to :ref:`i2s-api-reference-i2s_pdm` for information about PDM RX API. And for more details, please refer to :component_file:`esp_driver_i2s/include/driver/i2s_pdm.h`.

    The PDM data width is fixed to 16-bit. When the data on the line (The PDM format on the line is transferred to PCM format for easier comprehension) is:

    +--------+--------+--------+--------+--------+--------+--------+--------+--------+
    |  left  |  right |  left  |  right |  left  |  right |  left  |  right |  ...   |
    +========+========+========+========+========+========+========+========+========+
    | 0x0001 | 0x0002 | 0x0003 | 0x0004 | 0x0005 | 0x0006 | 0x0007 | 0x0008 |  ...   |
    +--------+--------+--------+--------+--------+--------+--------+--------+--------+

    Here is the table of the data received in a ``int16_t`` buffer with different :cpp:member:`i2s_pdm_rx_slot_config_t::slot_mode` and :cpp:member:`i2s_pdm_rx_slot_config_t::slot_mask`.

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

            The right slot is received first in stereo mode. To switch the left and right slots in the buffer, please set the :cpp:member:`i2s_pdm_rx_gpio_config_t::invert_flags::clk_inv` to force invert the clock signal.

            Specially, ESP32-S3 supports up to 4 data lines in PDM RX mode, where each data line can be connected to two PDM MICs (left and right slots). This means that the PDM RX on ESP32-S3 can support up to 8 PDM MICs. To enable multiple data lines, set the bits in :cpp:member:`i2s_pdm_rx_gpio_config_t::slot_mask` to enable corresponding slots first, and then set the data GPIOs in :cpp:type:`i2s_pdm_rx_gpio_config_t`.

    .. code-block:: c

        #include "driver/i2s_pdm.h"
        #include "driver/gpio.h"

        i2s_chan_handle_t rx_handle;

        /* Allocate an I2S RX channel */
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

    TDM TX/RX Usage
    ^^^^^^^^^^^^^^^

    - :example:`peripherals/i2s/i2s_codec/i2s_es7210_tdm` demonstrates how to use the I2S TDM mode on {IDF_TARGET_NAME} to record four MICs connected to ES7210 codec, with the recorded voice saved to an SD card in ``wav`` format.
    - :example:`peripherals/i2s/i2s_basic/i2s_tdm` demonstrates how to use the TDM mode in simplex or full-duplex mode on {IDF_TARGET_NAME}.

    Different slot communication formats can be generated by the following helper macros for TDM mode. As described above, there are four formats in TDM mode, and their helper macros are:

    - :c:macro:`I2S_TDM_PHILIPS_SLOT_DEFAULT_CONFIG`
    - :c:macro:`I2S_TDM_MSB_SLOT_DEFAULT_CONFIG`
    - :c:macro:`I2S_TDM_PCM_SHORT_SLOT_DEFAULT_CONFIG`
    - :c:macro:`I2S_TDM_PCM_LONG_SLOT_DEFAULT_CONFIG`

    The clock config helper macro is:

    - :c:macro:`I2S_TDM_CLK_DEFAULT_CONFIG`

    Please refer to :ref:`i2s-api-reference-i2s_tdm` for information about TDM API. And for more details, please refer to :component_file:`esp_driver_i2s/include/driver/i2s_tdm.h`.

    .. note::

        Due to hardware limitation, when setting the clock configuration for a slave role, please be aware that :cpp:member:`i2s_tdm_clk_config_t::bclk_div` should not be smaller than 8. Increasing this field can reduce the lagging of the data sent from the slave. In the high sample rate case, the data might lag behind for more than one BCLK which leads to data malposition. Users may gradually increase :cpp:member:`i2s_tdm_clk_config_t::bclk_div` to correct it.

        As :cpp:member:`i2s_tdm_clk_config_t::bclk_div` is the division of MCLK to BCLK, increasing it also increases the MCLK frequency. Therefore, the clock calculation may fail if MCLK is too high to divide from the source clock. This means that a larger value for :cpp:member:`i2s_tdm_clk_config_t::bclk_div` is not necessarily better.

    TDM TX Mode
    ~~~~~~~~~~~

    .. code-block:: c

        #include "driver/i2s_tdm.h"
        #include "driver/gpio.h"

        /* Allocate an I2S TX channel */
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

Full-duplex mode registers TX and RX channel in an I2S port at the same time, and the channels share the BCLK and WS signals. Currently, STD and TDM communication modes supports full-duplex mode in the following way, but PDM full-duplex is not supported because due to different PDM TX and RX clocks.

Note that one handle can only stand for one channel. Therefore, it is still necessary to configure the slot and clock for both TX and RX channels one by one.

Here is an example of how to allocate a pair of full-duplex channels:

.. code-block:: c

    #include "driver/i2s_std.h"
    #include "driver/gpio.h"

    i2s_chan_handle_t tx_handle;
    i2s_chan_handle_t rx_handle;

    /* Allocate a pair of I2S channel */
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    /* Allocate for TX and RX channel at the same time, then they will work in full-duplex mode */
    i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle);

    /* Set the configurations for BOTH TWO channels, since TX and RX channel have to be same in full-duplex mode */
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

    To allocate a channel handle in simplex mode, :cpp:func:`i2s_new_channel` should be called for each channel. The clock and GPIO pins of TX/RX channel on {IDF_TARGET_NAME} are not independent, so the TX and RX channel cannot coexist on the same I2S port in simplex mode.

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

        /* RX channel will be registered on another I2S, if no other available I2S unit found
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

    To allocate a channel in simplex mode, :cpp:func:`i2s_new_channel` should be called for each channel. The clock and GPIO pins of TX/RX channel on {IDF_TARGET_NAME} are independent, so they can be configured with different modes and clocks, and are able to coexist on the same I2S port in simplex mode. PDM duplex can be realized by registering PDM TX simplex and PDM RX simplex on the same I2S port. But in this way, PDM TX/RX might work with different clocks, so take care when configuring the GPIO pins and clocks.

    The following example offers a use case for the simplex mode, but note that although the internal MCLK signals for TX and RX channel are separate, the output MCLK can only be bound to one of them if they are from the same controller. If MCLK has been initialized by both channels, it will be bound to the channel that initializes later.

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

        /* RX channel will be registered on another I2S, if no other available I2S unit found
         * it will return ESP_ERR_NOT_FOUND */
        i2s_new_channel(&chan_cfg, NULL, &rx_handle); // Both RX and TX channel will be registered on I2S0, but they can work with different configurations.
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

.. only:: SOC_I2S_SUPPORTS_ETM

    I2S ETM Usage
    ^^^^^^^^^^^^^

    {IDF_TARGET_NAME} supports I2S ETM (Event Task Matrix), which allows to trigger other ETM tasks via I2S ETM events, or to control the start/stop by I2S ETM tasks.

    The I2S ETM APIs can be found in ``driver/i2s_etm.h``, the following example shows how to use GPIO to start/stop I2S channel via ETM:

    .. code-block:: c

        #include "driver/i2s_etm.h"
        // ...
        i2s_chan_handle_t tx_handle;
        // Initialize I2S channel
        // ......
        int ctrl_gpio = 4;
        // Initialize GPIO
        // ......
        /* Register GPIO ETM events */
        gpio_etm_event_config_t gpio_event_cfg = {
            .edges = {GPIO_ETM_EVENT_EDGE_POS, GPIO_ETM_EVENT_EDGE_NEG},
        };
        esp_etm_event_handle_t gpio_pos_event_handle;
        esp_etm_event_handle_t gpio_neg_event_handle;
        gpio_new_etm_event(&gpio_event_cfg, &gpio_pos_event_handle, &gpio_neg_event_handle);
        gpio_etm_event_bind_gpio(gpio_pos_event_handle, ctrl_gpio);
        gpio_etm_event_bind_gpio(gpio_neg_event_handle, ctrl_gpio);
        /* Register I2S ETM tasks */
        i2s_etm_task_config_t i2s_start_task_cfg = {
            .task_type = I2S_ETM_TASK_START,
        };
        esp_etm_task_handle_t i2s_start_task_handle;
        i2s_new_etm_task(tx_handle, &i2s_start_task_cfg, &i2s_start_task_handle);
        i2s_etm_task_config_t i2s_stop_task_cfg = {
            .task_type = I2S_ETM_TASK_STOP,
        };
        esp_etm_task_handle_t i2s_stop_task_handle;
        i2s_new_etm_task(tx_handle, &i2s_stop_task_cfg, &i2s_stop_task_handle);
        /* Bind GPIO events to I2S ETM tasks */
        esp_etm_channel_config_t etm_config = {};
        esp_etm_channel_handle_t i2s_etm_start_chan = NULL;
        esp_etm_channel_handle_t i2s_etm_stop_chan = NULL;
        esp_etm_new_channel(&etm_config, &i2s_etm_start_chan);
        esp_etm_new_channel(&etm_config, &i2s_etm_stop_chan);
        esp_etm_channel_connect(i2s_etm_start_chan, gpio_pos_event_handle, i2s_start_task_handle);
        esp_etm_channel_connect(i2s_etm_stop_chan, gpio_neg_event_handle, i2s_stop_task_handle);
        esp_etm_channel_enable(i2s_etm_start_chan);
        esp_etm_channel_enable(i2s_etm_stop_chan);
        /* Enable I2S channel first before starting I2S channel */
        i2s_channel_enable(tx_handle);
        // (Optional) Able to load the data into the internal DMA buffer here,
        // but tx_channel does not start yet, will timeout when the internal buffer is full
        // i2s_channel_write(tx_handle, data, data_size, NULL, 0);
        /* Start I2S channel by setting the GPIO to high */
        gpio_set_level(ctrl_gpio, 1);
        // Write data ......
        // i2s_channel_write(tx_handle, data, data_size, NULL, 1000);
        /* Stop I2S channel by setting the GPIO to low */
        gpio_set_level(ctrl_gpio, 0);

        /* Free resources */
        i2s_channel_disable(tx_handle);
        esp_etm_channel_disable(i2s_etm_start_chan);
        esp_etm_channel_disable(i2s_etm_stop_chan);
        esp_etm_del_event(gpio_pos_event_handle);
        esp_etm_del_event(gpio_neg_event_handle);
        esp_etm_del_task(i2s_start_task_handle);
        esp_etm_del_task(i2s_stop_task_handle);
        esp_etm_del_channel(i2s_etm_start_chan);
        esp_etm_del_channel(i2s_etm_stop_chan);
        // De-initialize I2S and GPIO
        // ......

Application Notes
-----------------

How to Prevent Data Lost
^^^^^^^^^^^^^^^^^^^^^^^^

For applications that need a high frequency sample rate, the massive data throughput may cause data lost. Users can receive data lost event by registering the ISR callback function to receive the event queue:

    .. code-block:: c

        static IRAM_ATTR bool i2s_rx_queue_overflow_callback(i2s_chan_handle_t handle, i2s_event_data_t *event, void *user_ctx)
        {
            // handle RX queue overflow event ...
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

1. Determine the interrupt interval. Generally, when data lost happens, the bigger the interval, the better, which  helps to reduce the interrupt times. This means ``dma_frame_num`` should be as big as possible while the DMA buffer size is below the maximum value of 4092. The relationships are::

    interrupt_interval(unit: sec) = dma_frame_num / sample_rate
    dma_buffer_size = dma_frame_num * slot_num * data_bit_width / 8 <= 4092

2. Determine ``dma_desc_num``. ``dma_desc_num`` is decided by the maximum time of ``i2s_channel_read`` polling cycle. All the received data is supposed to be stored between two ``i2s_channel_read``. This cycle can be measured by a timer or an outputting GPIO signal. The relationship is::

    dma_desc_num > polling_cycle / interrupt_interval

3. Determine the receiving buffer size. The receiving buffer offered by users in ``i2s_channel_read`` should be able to take all the data in all DMA buffers, which means that it should be larger than the total size of all the DMA buffers::

    recv_buffer_size > dma_desc_num * dma_buffer_size

For example, if there is an I2S application, and the known values are::

    sample_rate = 144000 Hz
    data_bit_width = 32 bits
    slot_num = 2
    polling_cycle = 10 ms

Then the parameters ``dma_frame_num``, ``dma_desc_num``, and ``recv_buf_size`` can be calculated as follows::

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

.. include-build-file:: inc/components/esp_driver_i2s/include/driver/i2s_types.inc
.. include-build-file:: inc/components/hal/include/hal/i2s_types.inc
