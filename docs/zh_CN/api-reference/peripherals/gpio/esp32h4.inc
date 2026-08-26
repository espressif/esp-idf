.. This file gets included from other .rst files in this folder.
.. It contains target-specific snippets.
.. Comments and '---' lines act as delimiters.
..
.. This is necessary mainly because RST doesn't support substitutions
.. (defined in RST, not in Python) inside code blocks. If that is ever implemented,
.. These code blocks can be moved back to the main .rst files, with target-specific
.. file names being replaced by substitutions.

.. gpio-summary

{IDF_TARGET_NAME} 芯片具有 40 个物理 GPIO 管脚（GPIO0 ~ GPIO39）。每个管脚都可用作一个通用 IO，或连接一个内部的外设信号。通过 GPIO 交换矩阵和 IO MUX，可配置外设模块的输入信号来源于任何的 IO 管脚，并且外设模块的输出信号也可连接到任意 IO 管脚。这些模块共同组成了芯片的 IO 控制。更多详细信息，请参阅 *{IDF_TARGET_NAME} 技术参考手册* > *IO MUX 和 GPIO 矩阵（GPIO、IO_MUX）* [`PDF <{IDF_TARGET_TRM_CN_URL}#iomuxgpio>`__]。

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
      - LP_GPIO0
      -

    * - GPIO1
      -
      - LP_GPIO1
      -

    * - GPIO2
      -
      - LP_GPIO2
      -

    * - GPIO3
      -
      - LP_GPIO3
      -

    * - GPIO4
      -
      - LP_GPIO4
      -

    * - GPIO5
      -
      - LP_GPIO5
      -

    * - GPIO6
      -
      -
      - SPI0/1

    * - GPIO7
      -
      -
      - SPI0/1

    * - GPIO8
      -
      -
      - SPI0/1

    * - GPIO9
      -
      -
      - SPI0/1

    * - GPIO10
      -
      -
      - SPI0/1

    * - GPIO11
      -
      -
      - SPI0/1

    * - GPIO12
      -
      -
      - SPI0/1

    * - GPIO13
      -
      -
      - USB-JTAG

    * - GPIO14
      -
      -
      - USB-JTAG

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
      - Strapping 管脚

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
      -

    * - GPIO25
      -
      -
      -

    * - GPIO26
      -
      -
      -

    * - GPIO27
      -
      -
      -

    * - GPIO28
      - ADC1_CH0
      -
      -

    * - GPIO29
      - ADC1_CH1
      -
      -

    * - GPIO30
      - ADC1_CH2
      -
      -

    * - GPIO31
      - ADC1_CH3
      -
      -

    * - GPIO32
      - ADC1_CH4
      -
      -

    * - GPIO33
      -
      -
      -

    * - GPIO34
      -
      -
      -

    * - GPIO35
      -
      -
      -

    * - GPIO36
      -
      -
      - Strapping 管脚

    * - GPIO37
      -
      -
      - Strapping 管脚

    * - GPIO38
      -
      -
      -

    * - GPIO39
      -
      -
      -

.. note::

    - Strapping 管脚：GPIO17、GPIO36 和 GPIO37 是 Strapping 管脚。更多信息请参考 `{IDF_TARGET_NAME} 技术规格书 <{IDF_TARGET_DATASHEET_CN_URL}>`_。
    - SPI0/1：GPIO6 ~ GPIO12 通常用于 SPI flash 和 PSRAM，不推荐用于其他用途。
    - USB-JTAG：GPIO13 和 GPIO14 默认用于 USB-JTAG。如果将它们配置为普通 GPIO，驱动程序将禁用 USB-JTAG 功能。

---
