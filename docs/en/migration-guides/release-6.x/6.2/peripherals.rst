Peripherals
===========

:link_to_translation:`zh_CN:[中文]`

ADC
---

ADC eFuse calibration internals have moved to the ``esp_hal_ana_conv`` component. The internal headers ``esp_efuse_rtc_calib.h`` and ``esp_efuse_rtc_table.h`` have been removed. Applications should use the public :doc:`ADC calibration APIs </api-reference/peripherals/adc/adc_calibration>` and must not depend on internal HAL interfaces.

DMA
---

Unified ``dma_burst_size`` Default
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:cpp:member:`uhci_controller_config_t::dma_burst_size`, :cpp:member:`async_memcpy_config_t::dma_burst_size`, and :cpp:member:`async_crc_config_t::dma_burst_size` now treat ``0`` as unset and fall back to that driver's own recommended default. This matches I2S, SPI, and LCD, and closes a hole where a zero-initialized config struct could only disable the burst — there was no way to ask for the driver default.

``1`` is newly defined as "disable the data burst". A single-beat burst has no benefit over non-burst mode.

.. list-table::
   :header-rows: 1
   :widths: 40 60

   * - ``dma_burst_size``
     - Meaning
   * - ``0``
     - Fall back to the driver's recommended default
   * - ``1``
     - Disable data burst
   * - ``N`` (``N > 1``, power of 2)
     - User-specified burst size

.. only:: SOC_I2S_SUPPORTS_APLL

    I2S
    ---

    Adjustable APLL Mode and Clock Jitter
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    The clock configuration structures for I2S STD, TDM, PDM TX, and PDM RX now provide ``flags.clk_src_fixed`` (``false`` by default) to select the APLL access mode:

    - ``false`` (default): Use exclusive adjustable mode. The driver can change the APLL frequency during initialization, clock reconfiguration, and rate tuning to obtain a more suitable division ratio and reduce divider-induced clock jitter. Compatible STD/TDM full-duplex channels can share this adjustable mode as one unit.
    - ``true``: Use the fixed, shareable mode. Once the APLL frequency is selected, subsequent reconfiguration and rate tuning normally change only the I2S divider.

    Because adjustable mode acquires APLL exclusively and prioritizes integer division for the master channel, existing applications that select ``I2S_CLK_SRC_APLL`` may encounter the following errors or behavior changes:

    - ``ESP_ERR_INVALID_STATE`` when initializing a second APLL user: adjustable mode does not allow APLL to be shared with other peripherals, another I2S controller, an unrelated simplex I2S channel on the same controller, or a second PDM channel.
    - ``ESP_ERR_INVALID_ARG`` in rare asymmetric full-duplex configurations where no APLL frequency provides an integer divider for the master channel while remaining within the divider range of the slave channel.
    - Clock reconfiguration or ``i2s_channel_tune_rate`` updates the APLL frequency (including overwriting a preconfigured APLL frequency) rather than keeping it fixed.

    If you encounter these errors, need to share APLL with other peripherals or unrelated I2S channels, or want to keep the APLL frequency fixed after initialization, explicitly set ``flags.clk_src_fixed = true``. Note that in fixed mode, changing the sample rate after initialization may require fractional division and introduce period jitter. For mode selection, full-duplex behavior, and the causes and mitigation of clock jitter, see :ref:`i2s-apll-access-mode` and :ref:`i2s-clock-jitter`.

PPA
---

Blocking Operation Behavior Change
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For any blocking PPA operation, now the registered ``on_trans_done`` callback function will not be invoked upon completion. Users should perform necessary actions after the operation returns by themselves manually. For non-blocking PPA operations, all behaviors remain unchanged.
