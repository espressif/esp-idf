测量模组功耗
============

:link_to_translation:`en:[English]`

{IDF_TARGET_SOC_BOOT_PIN:default="Not updated", esp32="IO0", esp32s2="IO0", esp32s3="IO0", esp32c3="IO9", esp32c2="IO9", "esp32c6"="IO9", "esp32h2"="IO9", "esp32p4"="IO35"}

开发功耗敏感型应用时，需要了解 `模组 <https://www.espressif.com/en/products/modules>`__ 在 Deep-sleep 模式、:doc:`其他节能模式 </api-reference/system/sleep_modes>` 和 Active 模式下的功耗。本节介绍如何测量运行此类应用程序时模组的功耗。


测量注意事项
------------

可以使用开发板进行测量吗？
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: esp32c6

    对于 {IDF_TARGET_NAME}，可以使用 `ESP32-C6-DevKitC-1 <https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/esp32c6/esp32-c6-devkitc-1/index.html>`__ 和 `ESP32-C6-DevKitM-1 <https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/esp32c6/esp32-c6-devkitm-1/index.html>`__ 等开发板。这些开发板配有排针，可以用来测量模组的功耗。

.. only:: esp32h2

    对于 {IDF_TARGET_NAME}，可以使用 `ESP32-H2-DevKitM-1 <https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/esp32h2/esp32-h2-devkitm-1/index.html>`__ 等开发板。这些开发板配有排针，可以用来测量模组的功耗。

.. only:: esp32c6 or esp32h2

    使用上述开发板，可以烧写 :example:`deep_sleep <system/deep_sleep>` 示例来测量模组在 Deep-sleep 模式下的功耗。也可以使用以下方法来测量配有 {IDF_TARGET_NAME} 芯片的裸模组的电流。

.. only:: esp32 or esp32s2 or esp32s3 or esp32c2 or esp32c3 or esp32p4

    对于 {IDF_TARGET_NAME}，不建议直接使用开发板来测量相应模组的功耗，因为即使烧写 :example:`deep_sleep <system/deep_sleep>` 示例，板上的某些电路仍会产生功耗。因此，在测量模组的电流前需要先切断电源电路。这种方法非常不便，测量成本高。


如何选择合适的电流表？
^^^^^^^^^^^^^^^^^^^^^^

在 :example:`deep_sleep <system/deep_sleep>` 示例中，模组每 20 秒被唤醒一次。在 Deep-sleep 模式下，模组中的电流仅为几微安 (μA)，而在 Active 模式下，电流的量级为毫安 (mA)。普通电流表无法快速动态切换测量范围，因此很难在如此大的动态范围内进行准确测量。

此外，普通电流表的内阻相对较高，压降较大，提供的电压可能会小于模组的最低电压，从而导致模组状态不稳定。

因此，用于测量 Deep-sleep 模式下电流的电流表应具有较小内阻，最好能够动态切换电流范围。我们推荐使用 `Joulescope 电流表 <https://www.joulescope.com/>`__ 和 Nordic 发布的 `Power Profiler Kit II <https://www.nordicsemi.com/Products/Development-hardware/Power-Profiler-Kit-2.?lang=en>`__。


Joulescope 电流表
"""""""""""""""""

Joulescope 电流表既能高速采样，又可以快速切换动态电流范围，即使测量设备的功耗变化很快，也可以准确测量其电流和功耗。Joulescope 可以精确测量从 A 到 nA 共九个数量级的设备电流。此外，Joulescope 在 1 A 时的总压降为 25 mV，可以保证模组正常运行。拥有以上两大特性， Joulescope 是测量模组在 Deep-sleep 模式和 wake-up 模式之间切换时电流的理想选择。

Joulescope 没有显示屏，如果需要查看所测模组的电流波形，请将其连接到计算机。具体请参照制造商提供的文档操作。


Nordic Power Profiler Kit II
""""""""""""""""""""""""""""

Nordic Power Profiler Kit II (PPK2) 具有先进模拟测量单元，动态测量范围大。低功耗嵌入式应用的功耗通常在几微安到 1 A 之间，PPK2 能够准确测量这个范围内的电流。根据测量范围的不同，分辨率在 100 nA 到 1 mA 之间变化。如此精准的分辨率足以检测到低功耗优化系统中常见的小波动。


硬件连接
--------

要测量裸模组的功耗，需要用 `ESP-Prog <https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/other/esp-prog/user_guide.html>`__ 来为模组烧写 :example:`deep_sleep <system/deep_sleep>` 示例，并在测量期间为模组供电。此外还需要一个合适的电流表（这里我们使用 Joulescope 电流表），一台计算机，当然还有一个带有必要跳线的裸模组。请参阅以下图示连接硬件。

.. figure:: /../_static/hardware_connection_power_measure.png
    :align: center
    :scale: 80%
    :alt: 硬件连接（点击放大）

    硬件连接（点击放大）

请将所测模组的 **UART TX**、 **UART RX**、 **SPI Boot**、 **Enable** 以及 **GND** 管脚与 ESP-Prog 上的相应管脚连接。将 ESP-Prog 上的 **VPROG** 管脚连接到 Joulescope 电流表的 **IN+** 端口，并将 **OUT+** 端口连接到所测模组的 **3.3 V** 管脚。有关不同模组中管脚的具体名称，请参考下面的列表。

.. list-table:: 基于 {IDF_TARGET_NAME} 芯片的模组管脚名称
    :header-rows: 1
    :widths: 50 50
    :align: center

    * - 模组管脚功能
      - 管脚名称

    * - UART TX
      - TXD0

    * - UART RX
      - RXD0

    * - SPI Boot
      - {IDF_TARGET_SOC_BOOT_PIN}

    * - Enable
      - EN

    * - 供电
      - 3V3

    * - 接地
      - GND

.. only:: esp32

    对于基于 ESP32 芯片的模组，UART TX 和 UART RX 管脚也可能是 U0TXD 和 U0RXD。

.. only:: esp32c2

    对于基于 ESP32-C2 芯片的模组，UART TX 和 UART RX 管脚也可能是 TXD 和 RXD。

.. only:: esp32c3

    对于基于 ESP32-C3 芯片的模组，UART TX 和 UART RX 管脚也可能是 TXD 和 RXD、TX 和 RX 或 TX0 和 RX0。

有关管脚名称的详细信息，请参阅 `模组技术规格书 <https://www.espressif.com/en/support/download/documents/modules>`__。


测量步骤
--------

下面以 ESP32-S3-WROOM-1 为例进行电流测量，其他模组也可参照以下步骤。芯片在不同模式下的具体功耗，请参阅相应 `芯片技术规格书 <https://www.espressif.com/en/support/download/documents/chips?keys=&field_download_document_type_tid%5B%5D=510>`__ 中的功耗章节。

参照以下步骤，可以测量 Deep-sleep 模式下的电流情况。

- 按照硬件连接章节的提示，连接上述设备。

- 将 :example:`deep_sleep <system/deep_sleep>` 示例烧写到模组中。详情请参阅 :doc:`在 Linux 和 macOS 系统中创建工程 </get-started/linux-macos-start-project>` （适用于运行 Linux 或 macOS 系统的计算机），也可以参考 :doc:`在 Windows 系统中创建工程 </get-started/windows-start-project>` （适用于运行 Windows 系统的计算机）。

.. only:: esp32 or esp32s2 or esp32s3

    请注意，在运行 ``idf.py menuconfig`` 配置示例时，需要先在 ``Example Configuration`` 中禁用 ``Enable touch wake up``，以降低底电流。

.. only:: esp32

    部分模组在 GPIO12 上连接了外部电阻（例如 ESP32-WROVER-E/IE），所以在进入 Deep-sleep 模式之前要调用 :cpp:func:`rtc_gpio_isolate`，将 GPIO12 管脚与外部电路隔离，从而进一步减小功耗。请注意，其他模组并不需要调用此函数，否则可能会显示结果异常。

- 默认情况下，模组每 20 秒唤醒一次（可以通过修改示例的代码来更改定时）。想要检查示例是否按照预期运行，可以运行 ``idf.py -p PORT monitor`` （请用你的串行端口名称替换 PORT）来监视模组的情况。

- 打开 Joulescope 软件查看如下图所示的电流波形。

观察波形可知，模组在 Deep-sleep 模式下的电流为 8.14 μA，在 Active 模式下的电流约为 23.88 mA。此外，Deep-sleep 模式下的平均功耗为 26.85 μW，Active 模式下的平均功耗则为 78.32 mW。

.. figure:: /../_static/current_measure_waveform.png
    :align: center
    :scale: 100%
    :alt: ESP32-S3-WROOM-1 的电流波形（点击放大）

    ESP32-S3-WROOM-1 的电流波形（点击放大）

观察下图可知，该模组在一个周期内的总功耗为 6.37 mW。

.. figure:: /../_static/power_measure_waveform.png
    :align: center
    :scale: 100%
    :alt: ESP32-S3-WROOM-1 的功耗（点击放大）

    ESP32-S3-WROOM-1 的功耗（点击放大）

通过参考不同模式下的功耗，可以估算应用程序的功耗，从而选择合适的电源。
