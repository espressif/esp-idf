.. This file gets included from other .rst files in this folder.
.. It contains target-specific snippets.
.. Comments and '---' lines act as delimiters.
..
.. This is necessary mainly because RST doesn't support substitutions
.. (defined in RST, not in Python) inside code blocks. If that is ever implemented,
.. These code blocks can be moved back to the main .rst files, with target-specific
.. file names being replaced by substitutions.

.. gpio-summary

{IDF_TARGET_NAME} 芯片具有 34 个物理 GPIO 管脚（GPIO0 ~ GPIO19、GPIO21 ~ GPIO23、GPIO25 ~ GPIO27 和 GPIO32 ~ GPIO39）。每个管脚都可用作一个通用 IO，或连接一个内部的外设信号。通过 IO MUX、RTC IO MUX 和 GPIO 交换矩阵，可配置外设模块的输入信号来源于任何的 IO 管脚，并且外设模块的输出信号也可连接到任意 IO 管脚。这些模块共同组成了芯片的 IO 控制。更多详细信息，请参阅 *{IDF_TARGET_NAME} 技术参考手册* > *IO MUX 和 GPIO 矩阵（GPIO、IO_MUX）* [`PDF <{IDF_TARGET_TRM_CN_URL}#iomuxgpio>`__]。

下表提供了各管脚的详细信息，部分 GPIO 具有特殊的使用限制，具体可参考表中的注释列。

.. list-table::
    :header-rows: 1
    :widths: 8 12 12 20

    * - GPIO
      - 模拟功能
      - RTC GPIO
      - 注释

    * - GPIO0
      - ADC2_CH1
      - RTC_GPIO11
      - Strapping 管脚

    * - GPIO1
      -
      -
      - TXD

    * - GPIO2
      - ADC2_CH2
      - RTC_GPIO12
      - Strapping 管脚

    * - GPIO3
      -
      -
      - RXD

    * - GPIO4
      - ADC2_CH0
      - RTC_GPIO10
      -

    * - GPIO5
      -
      -
      - Strapping 管脚

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
      - ADC2_CH5
      - RTC_GPIO15
      - Strapping 管脚；JTAG

    * - GPIO13
      - ADC2_CH4
      - RTC_GPIO14
      - JTAG

    * - GPIO14
      - ADC2_CH6
      - RTC_GPIO16
      - JTAG

    * - GPIO15
      - ADC2_CH3
      - RTC_GPIO13
      - Strapping 管脚；JTAG

    * - GPIO16
      -
      -
      - SPI0/1

    * - GPIO17
      -
      -
      - SPI0/1

    * - GPIO18
      -
      -
      -

    * - GPIO19
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

    * - GPIO25
      - ADC2_CH8
      - RTC_GPIO6
      -

    * - GPIO26
      - ADC2_CH9
      - RTC_GPIO7
      -

    * - GPIO27
      - ADC2_CH7
      - RTC_GPIO17
      -

    * - GPIO32
      - ADC1_CH4
      - RTC_GPIO9
      -

    * - GPIO33
      - ADC1_CH5
      - RTC_GPIO8
      -

    * - GPIO34
      - ADC1_CH6
      - RTC_GPIO4
      - GPI

    * - GPIO35
      - ADC1_CH7
      - RTC_GPIO5
      - GPI

    * - GPIO36
      - ADC1_CH0
      - RTC_GPIO0
      - GPI

    * - GPIO37
      - ADC1_CH1
      - RTC_GPIO1
      - GPI

    * - GPIO38
      - ADC1_CH2
      - RTC_GPIO2
      - GPI

    * - GPIO39
      - ADC1_CH3
      - RTC_GPIO3
      - GPI

.. note::

    - Strapping 管脚：GPIO0、GPIO2、GPIO5、GPIO12 (MTDI) 和 GPIO15 (MTDO) 是 Strapping 管脚。更多信息请参考 `ESP32 技术规格书 <{IDF_TARGET_DATASHEET_CN_URL}>`_。
    - SPI0/1：GPIO6-11 和 GPIO16-17 通常连接到模组内集成的 SPI flash 和 PSRAM，因此不能用于其他用途。
    - JTAG：GPIO12-15 通常用于在线调试。
    - GPI：GPIO34-39 只能设置为输入模式，不具备软件使能的上拉或下拉功能。
    - TXD & RXD 通常用于烧录和调试。
    - ADC2：使用 Wi-Fi 时不能使用 ADC2 管脚。因此，如果您在使用 Wi-Fi 时无法从 ADC2 GPIO 获取值，可以考虑使用 ADC1 GPIO 来解决该问题。更多详情请参考 :ref:`ADC 连续转换模式下的硬件限制 <hardware_limitations_adc_continuous>` 以及 :ref:`ADC 单次转换模式下的硬件限制<hardware_limitations_adc_oneshot>`。
    - 使用 ADC 或睡眠模式下使用 Wi-Fi 和蓝牙时，请不要使用 GPIO36 和 GPIO39 的中断。有关问题的详细描述，请参考 `ESP32 ECO 和 Bug 解决方法 <https://espressif.com/sites/default/files/documentation/eco_and_workarounds_for_bugs_in_esp32_cn.pdf>`_ > 中的第 3.11 节。

---
