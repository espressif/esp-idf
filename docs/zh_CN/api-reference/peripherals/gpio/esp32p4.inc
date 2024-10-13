.. This file gets included from other .rst files in this folder.
.. It contains target-specific snippets.
.. Comments and '---' lines act as delimiters.
..
.. This is necessary mainly because RST doesn't support substitutions
.. (defined in RST, not in Python) inside code blocks. If that is ever implemented,
.. These code blocks can be moved back to the main .rst files, with target-specific
.. file names being replaced by substitutions.

.. gpio-summary

{IDF_TARGET_NAME} 芯片具有 55 个物理 GPIO 管脚（GPIO0 ~ GPIO54）。每个管脚都可用作一个通用 IO，或连接一个内部的外设信号。通过 GPIO 交换矩阵和 IO MUX，可配置外设模块的输入信号来源于任何的 IO 管脚，并且外设模块的输出信号也可连接到任意 IO 管脚。这些模块共同组成了芯片的 IO 控制。更多详细信息，请参阅 *{IDF_TARGET_NAME} 技术参考手册* > *IO MUX 和 GPIO 矩阵（GPIO、IO_MUX）* [`PDF <{IDF_TARGET_TRM_CN_URL}#iomuxgpio>`__]。

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
      - TOUCH0
      - LP_GPIO2
      -

    * - GPIO3
      - TOUCH1
      - LP_GPIO3
      -

    * - GPIO4
      - TOUCH2
      - LP_GPIO4
      -

    * - GPIO5
      - TOUCH3
      - LP_GPIO5
      -

    * - GPIO6
      - TOUCH4
      - LP_GPIO6
      -

    * - GPIO7
      - TOUCH5
      - LP_GPIO7
      -

    * - GPIO8
      - TOUCH6
      - LP_GPIO8
      -

    * - GPIO9
      - TOUCH7
      - LP_GPIO9
      -

    * - GPIO10
      - TOUCH8
      - LP_GPIO10
      -

    * - GPIO11
      - TOUCH9
      - LP_GPIO11
      -

    * - GPIO12
      - TOUCH10
      - LP_GPIO12
      -

    * - GPIO13
      - TOUCH11
      - LP_GPIO13
      -

    * - GPIO14
      - TOUCH12
      - LP_GPIO14
      -

    * - GPIO15
      - TOUCH13
      - LP_GPIO15
      -

    * - GPIO16
      - ADC1_CH0
      -
      -

    * - GPIO17
      - ADC1_CH1
      -
      -

    * - GPIO18
      - ADC1_CH2
      -
      -

    * - GPIO19
      - ADC1_CH3
      -
      -

    * - GPIO20
      - ADC1_CH4
      -
      -

    * - GPIO21
      - ADC1_CH5
      -
      -

    * - GPIO22
      - ADC1_CH6
      -
      -

    * - GPIO23
      - ADC1_CH7
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
      -
      -
      -

    * - GPIO29
      -
      -
      -

    * - GPIO30
      -
      -
      -

    * - GPIO31
      -
      -
      -

    * - GPIO32
      -
      -
      -

    * - GPIO33
      -
      -
      -

    * - GPIO34
      -
      -
      - Strapping 管脚

    * - GPIO35
      -
      -
      - Strapping 管脚

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
      -

    * - GPIO40
      -
      -
      -

    * - GPIO41
      -
      -
      -

    * - GPIO42
      -
      -
      -

    * - GPIO43
      -
      -
      -

    * - GPIO44
      -
      -
      -

    * - GPIO45
      -
      -
      -

    * - GPIO46
      -
      -
      -

    * - GPIO47
      -
      -
      -

    * - GPIO48
      -
      -
      -

    * - GPIO49
      - ADC1_CH8
      -
      -

    * - GPIO50
      - ADC1_CH9
      -
      -

    * - GPIO51
      - ADC1_CH10，ANA_CMPR_CH0 外部参考电压
      -
      -

    * - GPIO52
      - ADC1_CH11，ANA_CMPR_CH0 同相输入
      -
      -

    * - GPIO53
      - ADC1_CH12，ANA_CMPR_CH1 外部参考电压
      -
      -

    * - GPIO54
      - ADC1_CH13，ANA_CMPR_CH1 同相输入
      -
      -

.. note::

    - Strapping 管脚：GPIO34, GPIO35、GPIO36、GPIO37 和 GPIO38 是 Strapping 管脚。更多信息请参考 `ESP32-P4 技术规格书 <{IDF_TARGET_DATASHEET_CN_URL}>`_。
    - USB-JTAG：GPIO24 和 GPIO25 默认用于 USB-JTAG。如果将它们配置为普通 GPIO，驱动程序将禁用 USB-JTAG 功能。

---
