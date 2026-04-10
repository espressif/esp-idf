.. This file gets included from other .rst files in this folder.
.. It contains target-specific snippets.
.. Comments and '---' lines act as delimiters.
..
.. This is necessary mainly because RST doesn't support substitutions
.. (defined in RST, not in Python) inside code blocks. If that is ever implemented,
.. These code blocks can be moved back to the main .rst files, with target-specific
.. file names being replaced by substitutions.

.. gpio-summary

{IDF_TARGET_NAME} 芯片具有 60 个物理 GPIO 管脚（GPIO0 ~ GPIO61，除 GPIO29、GPIO41 外）。每个管脚都可用作一个通用 IO，或连接一个内部的外设信号。通过 GPIO 交换矩阵和 IO MUX，可配置外设模块的输入信号来源于任何的 IO 管脚，并且外设模块的输出信号也可连接到任意 IO 管脚。这些模块共同组成了芯片的 IO 控制。更多详细信息，请参阅 *{IDF_TARGET_NAME} 技术参考手册* > *IO MUX 和 GPIO 矩阵（GPIO、IO_MUX）* [`PDF <{IDF_TARGET_TRM_CN_URL}#iomuxgpio>`__]。

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
      - DAC0
      - LP_GPIO4
      -

    * - GPIO5
      - DAC1
      - LP_GPIO5
      -

    * - GPIO6
      - TOUCH0
      - LP_GPIO6
      -

    * - GPIO7
      - TOUCH1
      - LP_GPIO7
      -

    * - GPIO8
      - TOUCH2
      -
      -

    * - GPIO9
      - TOUCH3
      -
      -

    * - GPIO10
      - TOUCH4
      -
      -

    * - GPIO11
      - TOUCH5
      -
      -

    * - GPIO12
      - TOUCH6
      -
      -

    * - GPIO13
      - TOUCH7
      -
      -

    * - GPIO14
      - TOUCH8
      -
      -

    * - GPIO15
      - TOUCH9
      -
      -

    * - GPIO16
      - TOUCH10
      -
      -

    * - GPIO17
      - TOUCH11
      -
      -

    * - GPIO18
      - TOUCH12
      -
      -

    * - GPIO19
      - TOUCH13
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
      - SPI0/1

    * - GPIO27
      -
      -
      - SPI0/1

    * - GPIO28
      -
      -
      - SPI0/1

    * - GPIO30
      -
      -
      - SPI0/1

    * - GPIO31
      -
      -
      - SPI0/1

    * - GPIO32
      -
      -
      - SPI0/1

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
      - Strapping 管脚

    * - GPIO39
      -
      -
      - Strapping 管脚

    * - GPIO40
      -
      -
      - Strapping 管脚

    * - GPIO42
      - ADC1_CH0_N
      -
      -

    * - GPIO43
      - ADC1_CH0_P
      -
      -

    * - GPIO44
      - ADC1_CH1_N
      -
      -

    * - GPIO45
      - ADC1_CH1_P
      -
      -

    * - GPIO46
      - ADC1_CH2_N
      -
      -

    * - GPIO47
      - ADC1_CH2_P
      -
      -

    * - GPIO48
      - ADC1_CH3_N
      -
      -

    * - GPIO49
      - ADC1_CH3_P
      -
      -

    * - GPIO50
      - ADC2_CH0_N
      -
      -

    * - GPIO51
      - ADC2_CH0_P
      -
      -

    * - GPIO52
      - ADC2_CH1_N
      -
      -

    * - GPIO53
      - ADC2_CH1_P
      -
      -

    * - GPIO54
      - ADC2_CH2_N
      -
      -

    * - GPIO55
      - ADC2_CH2_P
      -
      -

    * - GPIO56
      - ADC2_CH3_N
      -
      -

    * - GPIO57
      - ADC2_CH3_P
      -
      -

    * - GPIO58
      -
      -
      - UART0

    * - GPIO59
      -
      -
      - UART0

    * - GPIO60
      -
      -
      - Strapping 管脚

    * - GPIO61
      -
      -
      - Strapping 管脚

.. note::

    - Strapping 管脚：GPIO36 ~ GPIO40、GPIO60 ~ GPIO61 是 Strapping 管脚。更多信息请参考 `{IDF_TARGET_NAME} 技术规格书 <{IDF_TARGET_DATASHEET_CN_URL}>`_。
    - SPI0/1：GPIO26 ~ GPIO32 通常用于 SPI flash，不推荐用于其他用途。

---
