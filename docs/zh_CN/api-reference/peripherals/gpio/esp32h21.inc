.. This file gets included from other .rst files in this folder.
.. It contains target-specific snippets.
.. Comments and '---' lines act as delimiters.
..
.. This is necessary mainly because RST doesn't support substitutions
.. (defined in RST, not in Python) inside code blocks. If that is ever implemented,
.. These code blocks can be moved back to the main .rst files, with target-specific
.. file names being replaced by substitutions.

.. gpio-summary

{IDF_TARGET_NAME} 芯片具有 26 个物理 GPIO 管脚（GPIO0 ~ GPIO25）。每个管脚都可用作一个通用 IO，或连接一个内部的外设信号。通过 GPIO 交换矩阵和 IO MUX，可配置外设模块的输入信号来源于任何的 IO 管脚，并且外设模块的输出信号也可连接到任意 IO 管脚。这些模块共同组成了芯片的 IO 控制。更多详细信息，请参阅 *{IDF_TARGET_NAME} 技术参考手册* > *IO MUX 和 GPIO 矩阵（GPIO、IO_MUX）* [`PDF <{IDF_TARGET_TRM_CN_URL}#iomuxgpio>`__]。

下表提供了各管脚的详细信息，部分 GPIO 具有特殊的使用限制，具体可参考表中的注释列。

.. list-table::
    :header-rows: 1
    :widths: 8 12 12 20

    * - GPIO
      - 模拟功能
      - LP GPIO
      - 注释

    * - GPIO0
      -
      -
      -

    * - GPIO1
      - ADC1_CH0
      -
      -

    * - GPIO2
      - ADC1_CH1
      -
      -

    * - GPIO3
      - ADC1_CH2
      -
      -

    * - GPIO4
      - ADC1_CH3
      -
      -

    * - GPIO5
      - ADC1_CH4
      - LP_GPIO0
      -

    * - GPIO6
      -
      - LP_GPIO1
      -

    * - GPIO7
      -
      - LP_GPIO2
      -

    * - GPIO8
      -
      - LP_GPIO3
      - Strapping 管脚

    * - GPIO9
      -
      - LP_GPIO4
      -

    * - GPIO10
      -
      - LP_GPIO5
      -

    * - GPIO11
      -
      - LP_GPIO6
      -

    * - GPIO12
      -
      -
      -

    * - GPIO13
      -
      -
      - Strapping 管脚

    * - GPIO14
      -
      -
      - Strapping 管脚

    * - GPIO15
      -
      -
      -

    * - GPIO16
      -
      -
      -

    * - GPIO17
      -
      -
      - USB-JTAG

    * - GPIO18
      -
      -
      - USB-JTAG

    * - GPIO19
      -
      -
      - SPI0/1

    * - GPIO20
      -
      -
      - SPI0/1

    * - GPIO21
      -
      -
      - SPI0/1

    * - GPIO22
      -
      -
      - SPI0/1

    * - GPIO23
      -
      -
      - SPI0/1

    * - GPIO24
      -
      -
      - SPI0/1

    * - GPIO25
      -
      -
      - SPI0/1

.. note::

    - Strapping 管脚：GPIO8、GPIO13 和 GPIO14 是 Strapping 管脚。更多信息请参考 `ESP32-H21 技术规格书 <{IDF_TARGET_DATASHEET_CN_URL}>`_。
    - SPI0/1：GPIO19 ~ GPIO25 通常用于 SPI flash，不推荐用于其他用途。
    - USB-JTAG：GPIO17 和 GPIO18 默认用于 USB-JTAG。如果将它们配置为普通 GPIO，驱动程序将禁用 USB-JTAG 功能。

---
