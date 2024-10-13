.. This file gets included from other .rst files in this folder.
.. It contains target-specific snippets.
.. Comments and '---' lines act as delimiters.
..
.. This is necessary mainly because RST doesn't support substitutions
.. (defined in RST, not in Python) inside code blocks. If that is ever implemented,
.. These code blocks can be moved back to the main .rst files, with target-specific
.. file names being replaced by substitutions.

.. gpio-summary

{IDF_TARGET_NAME} 芯片具有 31 个物理 GPIO 管脚（GPIO0 ~ GPIO30）。每个管脚都可用作一个通用 IO，或连接一个内部的外设 信号。通过 GPIO 交换矩阵和 IO MUX，可配置外设模块的输入信号来源于任何的 IO 管脚，并且外设模块的输 出信号也可连接到任意 IO 管脚。这些模块共同组成了芯片的 IO 控制。更多详细信息，请参阅 *{IDF_TARGET_NAME} 技术参考手册* > *IO MUX 和 GPIO 矩阵（GPIO、IO_MUX）* [`PDF <{IDF_TARGET_TRM_CN_URL}#iomuxgpio>`__]。

下表提供了各管脚的详细信息，部分 GPIO 具有特殊的使用限制，具体可参考表中的注释列。

.. list-table::
    :header-rows: 1
    :widths: 8 12 12 20

    * - GPIO
      - 模拟功能
      - LP GPIO
      - 注释

    * - GPIO0
      - ADC1_CH0
      - LP_GPIO0
      -

    * - GPIO1
      - ADC1_CH1
      - LP_GPIO1
      -

    * - GPIO2
      - ADC1_CH2
      - LP_GPIO2
      -

    * - GPIO3
      - ADC1_CH3
      - LP_GPIO3
      -

    * - GPIO4
      - ADC1_CH4
      - LP_GPIO4
      - Strapping 管脚

    * - GPIO5
      - ADC1_CH5
      - LP_GPIO5
      - Strapping 管脚

    * - GPIO6
      - ADC1_CH6
      - LP_GPIO6
      -

    * - GPIO7
      -
      - LP_GPIO7
      -

    * - GPIO8
      -
      -
      - Strapping 管脚

    * - GPIO9
      -
      -
      - Strapping 管脚

    * - GPIO10
      -
      -
      -

    * - GPIO11
      -
      -
      -

    * - GPIO12
      -
      -
      - USB-JTAG

    * - GPIO13
      -
      -
      - USB-JTAG

    * - GPIO14
      -
      -
      -

    * - GPIO15
      -
      -
      - Strapping 管脚

    * - GPIO16
      -
      -
      -

    * - GPIO17
      -
      -
      -

    * - GPIO18
      -
      -
      -

    * - GPIO19
      -
      -
      -

    * - GPIO20
      -
      -
      -

    * - GPIO21
      -
      -
      -

    * - GPIO22
      -
      -
      -

    * - GPIO23
      -
      -
      -

    * - GPIO24
      -
      -
      - SPI0/1

    * - GPIO25
      -
      -
      - SPI0/1

    * - GPIO26
      -
      -
      - SPI0/1

    * - GPIO27
      -
      -
      - SPI0/1

    * - GPIO28
      -
      -
      - SPI0/1

    * - GPIO29
      -
      -
      - SPI0/1

    * - GPIO30
      -
      -
      - SPI0/1

.. note::

    - Strapping 管脚：GPIO4、GPIO5、GPIO8、GPIO9 和 GPIO15 是 Strapping 管脚。更多信息请参考 `ESP32-C6 技术规格书 <{IDF_TARGET_DATASHEET_CN_URL}>`_。
    - SPI0/1：GPIO24 ～ GPIO30 通常用于 SPI flash，不推荐用于其他用途。
    - USB-JTAG：GPIO12 和 GPIO13 默认用于 USB-JTAG。如果将它们配置为普通 GPIO，驱动程序将禁用 USB-JTAG 功能。
    - 对于内置 SiP flash 的芯片型号，GPIO24 ~ GPIO30 专门用于连接 SiP flash; 且 GPIO10 ~ GPIO11 未引出至芯片管脚。因此，对于这类芯片只有 22 个 GPIO 管脚可用。
    - 对于无内置 SiP flash 的芯片型号，则 GPIO14 未引出至芯片管脚。

---
