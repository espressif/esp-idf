外设驱动
========

:link_to_translation:`en:[English]`

ADC
---

ADC eFuse 校准的内部实现已迁移至 ``esp_hal_ana_conv`` 组件。内部头文件 ``esp_efuse_rtc_calib.h`` 和 ``esp_efuse_rtc_table.h`` 已被移除。应用程序应使用公开的 :doc:`ADC 校准 API </api-reference/peripherals/adc/adc_calibration>`，不应依赖内部 HAL 接口。

DMA
---

统一 ``dma_burst_size`` 的默认值语义
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:cpp:member:`uhci_controller_config_t::dma_burst_size`、:cpp:member:`async_memcpy_config_t::dma_burst_size` 和 :cpp:member:`async_crc_config_t::dma_burst_size` 现在把 ``0`` 视为未设置，并回落到该驱动自己推荐的默认值。这与 I2S、SPI、LCD 的约定一致，也补上了原先的漏洞：配置结构体零初始化时只能关掉 burst，无法表达“使用驱动默认值”。

``1`` 是新引入的含义，表示显式关闭数据突发传输。单 beat 突发相对于非突发没有收益。

.. list-table::
   :header-rows: 1
   :widths: 40 60

   * - ``dma_burst_size``
     - 含义
   * - ``0``
     - 回落到该驱动推荐的默认值
   * - ``1``
     - 关闭数据突发传输
   * - ``N`` （``N > 1``，且为 2 的幂）
     - 用户指定的突发大小

PPA
---

阻塞操作行为变更
~~~~~~~~~~~~~~~~~

对于任何阻塞的 PPA 操作，现在注册的 ``on_trans_done`` 回调函数不会在操作完成后被调用。用户需要手动在操作返回后执行必要的操作。对于非阻塞的 PPA 操作，所有行为都与之前一致。
