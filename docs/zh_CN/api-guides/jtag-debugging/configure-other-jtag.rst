配置其他 JTAG 接口
==================

:link_to_translation:`en:[English]`

{IDF_TARGET_JTAG_SEL_EFUSE:default="Not Updated!", esp32s3="STRAP_JTAG_SEL", esp32c6="JTAG_SEL_ENABLE", esp32h2="JTAG_SEL_ENABLE", esp32p4="JTAG_SEL_ENABLE", esp32c5="JTAG_SEL_ENABLE", esp32c61="JTAG_SEL_ENABLE", esp32h4="JTAG_SEL_ENABLE", esp32h21="JTAG_SEL_ENABLE"}

关于适配 OpenOCD 和 {IDF_TARGET_NAME} 的 JTAG 接口选择问题，请参考 :ref:`jtag-debugging-selecting-jtag-adapter` 章节。然后按照以下步骤进行设置，使其正常工作。

.. only:: SOC_USB_SERIAL_JTAG_SUPPORTED

    配置 eFuse
    ^^^^^^^^^^^^^^^^

    {IDF_TARGET_NAME} JTAG 接口默认连接至 :doc:`内置 USB_SERIAL_JTAG 外设 <configure-builtin-jtag>`。要使用外部 JTAG 适配器，需将 JTAG 接口切换至 GPIO 管脚。你可以使用 ``idf.py`` 工具来烧录 eFuse，以完成接口转换。

    .. only:: esp32c3

        烧录 ``DIS_USB_JTAG`` eFuse 后，USB_SERIAL_JTAG 和 {IDF_TARGET_NAME} 的 JTAG 接口之间的连接将被永久禁用，此后你可以将 JTAG 接口连接到 |jtag-gpio-list|。注意，烧录后，USB_SERIAL_JTAG 的 USB CDC 功能仍然可用，即仍然可以通过 USB CDC 进行烧录和查看日志。

    .. only:: not esp32c3

        - 烧录 ``DIS_USB_JTAG`` eFuse 后，USB_SERIAL_JTAG 和 {IDF_TARGET_NAME} 的 JTAG 接口之间的连接将被永久禁用，此后你可以将 JTAG 接口连接到 |jtag-gpio-list|。注意，烧录后，USB_SERIAL_JTAG 的 USB CDC 功能仍然可用，即仍然可以通过 USB CDC 进行烧录和查看日志。
        - 烧录 ``{IDF_TARGET_JTAG_SEL_EFUSE}`` eFuse 后，JTAG 接口的选择将由 strapping 管脚 |jtag-sel-gpio| 来决定。{IDF_TARGET_NAME} 复位时，如果该 strapping 管脚为低电平，JTAG 接口将使用 |jtag-gpio-list|；如果为高电平，USB_SERIAL_JTAG 将被用作 JTAG 接口。

    .. warning::
        请注意，烧录 eFuse 是一项不可逆的操作，请在开始前谨慎考虑以上选项。

配置硬件
^^^^^^^^

1.  找到 JTAG 接口和 {IDF_TARGET_NAME} 板上需要相互连接并建立通信的所有管脚或信号。

    .. include:: {IDF_TARGET_PATH_NAME}.inc
        :start-after: jtag-pins
        :end-before: ---

2.  检查 {IDF_TARGET_NAME} 上用于 JTAG 通信的管脚是否被连接到了其它硬件上，这可能会影响 JTAG 的工作。

3.  连接 {IDF_TARGET_NAME} 和 JTAG 接口上的管脚或信号。


配置驱动
^^^^^^^^

你可能还需要安装软件驱动，才能使 JTAG 在计算机上正常工作，请参阅你所使用的 JTAG 适配器的有关文档，获取相关详细信息。

在 Linux 中，请务必将 `udev 规则文件 <https://github.com/espressif/openocd-esp32/blob/master/contrib/60-openocd.rules>`_ 复制到 ``/etc/udev/rules.d`` 目录中，以添加 OpenOCD udev 规则。

连接
^^^^

将 JTAG 接口连接到计算机，打开 {IDF_TARGET_NAME} 和 JTAG 接口板上的电源，然后检查计算机是否可以识别到 JTAG 接口。


如需继续设置调试环境，请前往 :ref:`jtag-debugging-run-openocd` 章节。
