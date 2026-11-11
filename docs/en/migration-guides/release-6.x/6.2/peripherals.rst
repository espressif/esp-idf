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

PPA
---

Blocking Operation Behavior Change
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For any blocking PPA operation, now the registered ``on_trans_done`` callback function will not be invoked upon completion. Users should perform necessary actions after the operation returns by themselves manually. For non-blocking PPA operations, all behaviors remain unchanged.
