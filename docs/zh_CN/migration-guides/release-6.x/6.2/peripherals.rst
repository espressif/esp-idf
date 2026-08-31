外设驱动
========

:link_to_translation:`en:[English]`

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
