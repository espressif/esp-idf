GPIO & RTC GPIO
===============

:link_to_translation:`en:[English]`

概述
--------

.. only:: esp32

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

        - Strapping 管脚：GPIO0、GPIO2、GPIO5、GPIO12 (MTDI) 和 GPIO15 (MTDO) 是 Strapping 管脚。更多信息请参考 `ESP32 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_cn.pdf>`_。
        - SPI0/1：GPIO6-11 和 GPIO16-17 通常连接到模组内集成的 SPI flash 和 PSRAM，因此不能用于其他用途。
        - JTAG：GPIO12-15 通常用于在线调试。
        - GPI：GPIO34-39 只能设置为输入模式，不具备软件使能的上拉或下拉功能。
        - TXD & RXD 通常用于烧录和调试。
        - ADC2：使用 Wi-Fi 时不能使用 ADC2 管脚。因此，如果您在使用 Wi-Fi 时无法从 ADC2 GPIO 获取值，可以考虑使用 ADC1 GPIO 来解决该问题。更多详情请参考 `ADC 限制 <https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html#adc-limitations>`_。


.. only:: esp32s2

    {IDF_TARGET_NAME} 芯片具有 43 个物理 GPIO 管脚（GPIO0 ~ GPIO21 和 GPIO26 ~ GPIO46）。每个管脚都可用作一个通用 IO，或连接一个内部的外设信号。通过 IO MUX、RTC IO MUX 和 GPIO 交换矩阵，可配置外设模块的输入信号来源于任何的 IO 管脚，并且外设模块的输出信号也可连接到任意 IO 管脚。这些模块共同组成了芯片的 IO 控制。更多详细信息，请参阅 *{IDF_TARGET_NAME} 技术参考手册* > *IO MUX 和 GPIO 矩阵（GPIO、IO_MUX）* [`PDF <{IDF_TARGET_TRM_CN_URL}#iomuxgpio>`__]。

    下表提供了各管脚的详细信息，部分 GPIO 具有特殊的使用限制，具体可参考表中的注释列。

    .. list-table::
       :header-rows: 1
       :widths: 8 12 12 20

       * - GPIO
         - 模拟功能
         - RTC GPIO
         - 注释

       * - GPIO0
         -
         - RTC_GPIO0
         - Strapping 管脚

       * - GPIO1
         - ADC1_CH0
         - RTC_GPIO1
         -

       * - GPIO2
         - ADC1_CH1
         - RTC_GPIO2
         -

       * - GPIO3
         - ADC1_CH2
         - RTC_GPIO3
         -

       * - GPIO4
         - ADC1_CH3
         - RTC_GPIO4
         -

       * - GPIO5
         - ADC1_CH4
         - RTC_GPIO5
         -

       * - GPIO6
         - ADC1_CH5
         - RTC_GPIO6
         -

       * - GPIO7
         - ADC1_CH6
         - RTC_GPIO7
         -

       * - GPIO8
         - ADC1_CH7
         - RTC_GPIO8
         -

       * - GPIO9
         - ADC1_CH8
         - RTC_GPIO9
         -

       * - GPIO10
         - ADC1_CH9
         - RTC_GPIO10
         -

       * - GPIO11
         - ADC2_CH0
         - RTC_GPIO11
         -

       * - GPIO12
         - ADC2_CH1
         - RTC_GPIO12
         -

       * - GPIO13
         - ADC2_CH2
         - RTC_GPIO13
         -

       * - GPIO14
         - ADC2_CH3
         - RTC_GPIO14
         -

       * - GPIO15
         - ADC2_CH4
         - RTC_GPIO15
         -

       * - GPIO16
         - ADC2_CH5
         - RTC_GPIO16
         -

       * - GPIO17
         - ADC2_CH6
         - RTC_GPIO17
         -

       * - GPIO18
         - ADC2_CH7
         - RTC_GPIO18
         -

       * - GPIO19
         - ADC2_CH8
         - RTC_GPIO19
         -

       * - GPIO20
         - ADC2_CH9
         - RTC_GPIO20
         -

       * - GPIO21
         -
         - RTC_GPIO21
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

       * - GPIO29
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
         -

       * - GPIO37
         -
         -
         -

       * - GPIO38
         -
         -
         -

       * - GPIO39
         -
         -
         - JTAG

       * - GPIO40
         -
         -
         - JTAG

       * - GPIO41
         -
         -
         - JTAG

       * - GPIO42
         -
         -
         - JTAG

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
         - Strapping 管脚

       * - GPIO46
         -
         -
         - GPI；Strapping 管脚

    .. note::

        - Strapping 管脚：GPIO0、GPIO45、和 GPIO46 是 Strapping 管脚。更多信息请参考 `ESP32-S2 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32-s2_datasheet_cn.pdf>`_。
        - SPI0/1：GPIO26-32 通常用于 SPI flash 和 PSRAM，不推荐用于其他用途。
        - JTAG：GPIO39-42 通常用于在线调试。
        - GPI：GPIO46 固定为下拉，只能设置为输入模式。


.. only:: esp32c3

    {IDF_TARGET_NAME} 芯片具有 22 个物理 GPIO 管脚（GPIO0 ~ GPIO21）。每个管脚都可用作一个通用 IO，或连接一个内部的外设信号。通过 GPIO 交换矩阵和 IO MUX，可配置外设模块的输入信号来源于任何的 IO 管脚，并且外设模块的输出信号也可连接到任意 IO 管脚。这些模块共同组成了芯片的 IO 控制。更多详细信息，请参阅 *{IDF_TARGET_NAME} 技术参考手册* > *IO MUX 和 GPIO 矩阵（GPIO、IO_MUX）* [`PDF <{IDF_TARGET_TRM_CN_URL}#iomuxgpio>`__]。

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
         - Strapping 管脚；RTC

       * - GPIO3
         - ADC1_CH3
         - RTC

       * - GPIO4
         - ADC1_CH4
         - RTC

       * - GPIO5
         - ADC2_CH0
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
         - USB-JTAG

       * - GPIO19
         -
         - USB-JTAG

       * - GPIO20
         -
         -

       * - GPIO21
         -
         -

    .. note::

        - Strapping 管脚：GPIO2、GPIO8、和 GPIO9 是 Strapping 管脚。更多信息请参考 `ESP32-C3 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_cn.pdf>`_。
        - SPI0/1：GPIO12-17 通常用于 SPI flash 和 PSRAM，不推荐用于其他用途。
        - USB-JTAG：GPIO18 和 GPIO19 默认用于 USB-JTAG。用做 GPIO 时驱动程序将禁用 USB-JTAG。
        - RTC：GPIO0-5 可以在 Deep-sleep 模式时使用。


.. only:: esp32s3

    {IDF_TARGET_NAME} 芯片具有 45 个物理 GPIO 管脚（GPIO0 ~ GPIO21 和 GPIO26 ~ GPIO48）。每个管脚都可用作一个通用 IO，或连接一个内部外设信号。通过 GPIO 交换矩阵、IO MUX 和 RTC IO MUX，可配置外设模块的输入信号来源于任何的 GPIO 管脚，并且外设模块的输出信号也可连接到任意 GPIO 管脚。这些模块共同组成了芯片的输入输出控制。更多详细信息，请参阅 *{IDF_TARGET_NAME} 技术参考手册* > *IO MUX 和 GPIO 矩阵（GPIO、IO_MUX）* [`PDF <{IDF_TARGET_TRM_CN_URL}#iomuxgpio>`__]。

    下表提供了各管脚的详细信息，部分 GPIO 具有特殊的使用限制，具体可参考表中的注释列。

    .. list-table::
       :header-rows: 1
       :widths: 8 12 12 20

       * - GPIO
         - 模拟功能
         - RTC GPIO
         - 注释

       * - GPIO0
         -
         - RTC_GPIO0
         - Strapping 管脚

       * - GPIO1
         - ADC1_CH0
         - RTC_GPIO1
         -

       * - GPIO2
         - ADC1_CH1
         - RTC_GPIO2
         -

       * - GPIO3
         - ADC1_CH2
         - RTC_GPIO3
         - Strapping 管脚

       * - GPIO4
         - ADC1_CH3
         - RTC_GPIO4
         -

       * - GPIO5
         - ADC1_CH4
         - RTC_GPIO5
         -

       * - GPIO6
         - ADC1_CH5
         - RTC_GPIO6
         -

       * - GPIO7
         - ADC1_CH6
         - RTC_GPIO7
         -

       * - GPIO8
         - ADC1_CH7
         - RTC_GPIO8
         -

       * - GPIO9
         - ADC1_CH8
         - RTC_GPIO9
         -

       * - GPIO10
         - ADC1_CH9
         - RTC_GPIO10
         -

       * - GPIO11
         - ADC2_CH0
         - RTC_GPIO11
         -

       * - GPIO12
         - ADC2_CH1
         - RTC_GPIO12
         -

       * - GPIO13
         - ADC2_CH2
         - RTC_GPIO13
         -

       * - GPIO14
         - ADC2_CH3
         - RTC_GPIO14
         -

       * - GPIO15
         - ADC2_CH4
         - RTC_GPIO15
         -

       * - GPIO16
         - ADC2_CH5
         - RTC_GPIO16
         -

       * - GPIO17
         - ADC2_CH6
         - RTC_GPIO17
         -

       * - GPIO18
         - ADC2_CH7
         - RTC_GPIO18
         -

       * - GPIO19
         - ADC2_CH8
         - RTC_GPIO19
         - USB-JTAG

       * - GPIO20
         - ADC2_CH9
         - RTC_GPIO20
         - USB-JTAG

       * - GPIO21
         -
         - RTC_GPIO21
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

       * - GPIO29
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
         - SPI0/1

       * - GPIO34
         -
         -
         - SPI0/1

       * - GPIO35
         -
         -
         - SPI0/1

       * - GPIO36
         -
         -
         - SPI0/1

       * - GPIO37
         -
         -
         - SPI0/1

       * - GPIO38
         -
         -
         -

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
         - Strapping 管脚

       * - GPIO46
         -
         -
         - Strapping 管脚

       * - GPIO47
         -
         -
         -

       * - GPIO48
         -
         -
         -

    .. Note::

        - Strapping 管脚：GPIO0、GPIO3、GPIO45 和 GPIO46 是 Strapping 管脚。更多信息请参考 `ESP32-S3 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_cn.pdf>`_。
        - SPI0/1：GPIO26-32 通常用于 SPI flash 和 PSRAM，不推荐用于其他用途。当使用八线 flash 或八线 PSRAM 或同时使用两者时，GPIO33~37 会连接到 SPIIO4 ~ SPIIO7 和 SPIDQS。因此，对于内嵌 ESP32-S3R8 或 ESP32-S3R8V 芯片的开发板，GPIO33~37 也不推荐用于其他用途。
        - USB-JTAG：GPIO19 和 GPIO20 默认用于 USB-JTAG。用做 GPIO 时驱动程序将禁用 USB-JTAG。

.. only:: esp32c2

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

        - Strapping 管脚：GPIO8 和 GPIO9 是 Strapping 管脚。更多信息请参考 `ESP8684 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp8684_datasheet_cn.pdf>`_。
        - SPI0/1：GPIO12-17 通常用于 SPI flash，不推荐用于其他用途。
        - RTC：GPIO0-5 可以在 Deep-sleep 模式时使用。


.. only:: SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

    当 GPIO 连接到 "RTC" 低功耗和模拟子系统时，{IDF_TARGET_NAME} 芯片还单独支持 "RTC GPIO"。可在以下情况时使用这些管脚功能：

    .. list::

        - 处于 Deep-sleep 模式时
        :SOC_ULP_SUPPORTED: - :doc:`超低功耗协处理器 (ULP) <../../api-reference/system/ulp>` 运行时
        - 使用 ADC/DAC 等模拟功能时

应用示例
-------------------

GPIO 输出和输入中断示例：:example:`peripherals/gpio/generic_gpio`。

API 参考 - 普通 GPIO
---------------------------

.. include-build-file:: inc/gpio.inc
.. include-build-file:: inc/gpio_types.inc


.. only:: SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

    API 参考 - RTC GPIO
    ------------------------

    .. include-build-file:: inc/rtc_io.inc
    .. include-build-file:: inc/rtc_io_types.inc
