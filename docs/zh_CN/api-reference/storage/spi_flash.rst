.. include:: ../../../en/api-reference/storage/spi_flash.rst


Flash 特性支持情况
-----------------------------------

不同厂家的 Flash 特性通过不同的方式来操作，因此需要特殊的驱动支持。当前驱动支持大多数厂家 Flash 24 位地址范围内的快速/慢速读，以及二线模式（ DIO / DOUT ），因为他们不需要任何厂家自定义的命令。

当前驱动支持以下厂家/型号的 Flash 的四线模式（ QIO / QOUT ）：

1. ISSI
2. GD
3. MXIC
4. FM
5. Winbond
6. XMC
7. BOYA

当前驱动支持以下厂家/型号的 Flash 的 32 位地址范围的访问：

1. W25Q256

如果有需要，也可以自定义 Flash 芯片驱动，参见 :doc:`spi_flash_override_driver` 。但此功能仅供专业用户使用。

.. toctree::
   :hidden:

   自定义 Flash 芯片驱动 <spi_flash_override_driver>