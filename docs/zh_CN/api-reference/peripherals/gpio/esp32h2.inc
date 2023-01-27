.. This file gets included from other .rst files in this folder.
.. It contains target-specific snippets.
.. Comments and '---' lines act as delimiters.
..
.. This is necessary mainly because RST doesn't support substitutions
.. (defined in RST, not in Python) inside code blocks. If that is ever implemented,
.. These code blocks can be moved back to the main .rst files, with target-specific
.. file names being replaced by substitutions.

.. gpio-summary

{IDF_TARGET_NAME} 芯片具有 28 个物理 GPIO 管脚（GPIO0 ~ GPIO27）。

每个管脚都可用作一个通用 IO，或连接一个内部的外设信号。通过 GPIO 交换矩阵和 IO MUX，可配置外设模块的输入信号来源于任何的 IO 管脚，并且外设模块的输出信号也可连接到任意 IO 管脚。这些模块共同组成了芯片的 IO 控制。更多详细信息，请参阅 *{IDF_TARGET_NAME} 技术参考手册* > *IO MUX 和 GPIO 矩阵（GPIO、IO_MUX）* [`PDF <{IDF_TARGET_TRM_CN_URL}#iomuxgpio>`__]。

下表提供了各管脚的详细信息，部分 GPIO 具有特殊的使用限制，具体可参考表中的注释列。

.. list-table::
    :header-rows: 1
    :widths: 8 12 20

    * - GPIO
      - 模拟功能
      - 注释

    * - GPIO0
      -
      -

    * - GPIO1
      - ADC1_CH0
      -

    * - GPIO2
      - ADC1_CH1
      - Strapping 管脚

    * - GPIO3
      - ADC1_CH2
      - Strapping 管脚

    * - GPIO4
      - ADC1_CH3
      -

    * - GPIO5
      - ADC1_CH4
      -

    * - GPIO6
      -
      -

    * - GPIO7
      -
      -

    * - GPIO8
      -
      - Strapping 管脚

    * - GPIO9
      -
      - Strapping 管脚

    * - GPIO10
      -
      -

    * - GPIO11
      -
      -

    * - GPIO12
      -
      -

    * - GPIO13
      -
      -

    * - GPIO14
      -
      -

    * - GPIO15
      -
      - SPI0/1

    * - GPIO16
      -
      - SPI0/1

    * - GPIO17
      -
      - SPI0/1

    * - GPIO18
      -
      - SPI0/1

    * - GPIO19
      -
      - SPI0/1

    * - GPIO20
      -
      - SPI0/1

    * - GPIO21
      -
      - SPI0/1

    * - GPIO22
      -
      -

    * - GPIO23
      -
      -

    * - GPIO24
      -
      -

    * - GPIO25
      -
      - Strapping 管脚

    * - GPIO26
      -
      - USB-JTAG

    * - GPIO27
      -
      - USB-JTAG

.. note::

    - Strapping 管脚：GPIO2、GPIO3、GPIO8、GPIO9 和 GPIO25 是 Strapping 管脚。更多信息请参考 `ESP32-H2 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32-h2_datasheet_cn.pdf>`_。
    - SPI0/1：GPIO15-21 通常用于 SPI flash，不推荐用于其他用途。
    - USB-JTAG：GPIO26 和 GPIO27 默认用于 USB-JTAG。用做 GPIO 时驱动程序将禁用 USB-JTAG。
    - 对于内置 SiP flash 的芯片型号，GPIO15 ~ GPIO21 专门用于连接 SiP flash; 因此，对于这类芯片只有 21 个 GPIO 管脚可用。

---
