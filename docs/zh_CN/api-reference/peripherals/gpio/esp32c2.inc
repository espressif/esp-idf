.. This file gets included from other .rst files in this folder.
.. It contains target-specific snippets.
.. Comments and '---' lines act as delimiters.
..
.. This is necessary mainly because RST doesn't support substitutions
.. (defined in RST, not in Python) inside code blocks. If that is ever implemented,
.. These code blocks can be moved back to the main .rst files, with target-specific
.. file names being replaced by substitutions.

.. gpio-summary

{IDF_TARGET_NAME} 芯片具有 21 个物理 GPIO 管脚（GPIO0 ~ GPIO20）。对于内置 SiP flash 的芯片型号，GPIO11 ~ GPIO17 专门用于连接 SiP flash。因此，对于这类芯片只有 14 个 GPIO 管脚可用。

每个管脚都可用作一个通用 IO，或连接一个内部的外设 信号。通过 GPIO 交换矩阵和 IO MUX，可配置外设模块的输入信号来源于任何的 IO 管脚，并且外设模块的输 出信号也可连接到任意 IO 管脚。这些模块共同组成了芯片的 IO 控制。更多详细信息，请参阅 *{IDF_TARGET_NAME} 技术参考手册* > *IO MUX 和 GPIO 矩阵（GPIO、IO_MUX）* [`PDF <{IDF_TARGET_TRM_CN_URL}#iomuxgpio>`__]。

下表提供了各管脚的详细信息，部分 GPIO 具有特殊的使用限制，具体可参考表中的注释列。

.. list-table::
    :header-rows: 1
    :widths: 12 12 22

    * - GPIO
      - 模拟功能
      - 注释

    * - GPIO0
      - ADC1_CH0
      - RTC

    * - GPIO1
      - ADC1_CH1
      - RTC

    * - GPIO2
      - ADC1_CH2
      - RTC

    * - GPIO3
      - ADC1_CH3
      - RTC

    * - GPIO4
      - ADC1_CH4
      - RTC

    * - GPIO5
      -
      - RTC

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
      - SPI0/1

    * - GPIO13
      -
      - SPI0/1

    * - GPIO14
      -
      - SPI0/1

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
      -

    * - GPIO19
      -
      -

    * - GPIO20
      -
      -

.. note::

    - Strapping 管脚：GPIO8 和 GPIO9 是 Strapping 管脚。更多信息请参考 `ESP8684 技术规格书 <{IDF_TARGET_DATASHEET_CN_URL}>`_。
    - SPI0/1：GPIO12-17 通常用于 SPI flash，不推荐用于其他用途。
    - RTC：GPIO0-5 可用于将芯片从 Deep-sleep 模式中唤醒，其他 GPIO 仅能将芯片从 Light-sleep 模式中唤醒。更多信息请参考 :ref:`唤醒源<api-reference-wakeup-source>` 章节。

---
