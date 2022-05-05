配置其它 JTAG 接口
==================
:link_to_translation:`en:[English]`

关于适配 OpenOCD 和 {IDF_TARGET_NAME} 的 JTAG 接口选择问题，请参考 :ref:`jtag-debugging-selecting-jtag-adapter` 章节，确保 JTAG 适配器能够与 OpenOCD 和 {IDF_TARGET_NAME} 一同工作。然后按照以下三个步骤进行设置，使其正常工作。


.. only:: SOC_USB_SERIAL_JTAG_SUPPORTED

   配置 eFuses
   ^^^^^^^^^^^^^^^^

   {IDF_TARGET_NAME} JTAG 接口默认连接至 :doc:`内置 USB_SERIAL_JTAG 外设 <configure-builtin-jtag>`。要使用外部 JTAG 适配器，需将 JTAG 接口切换至 GPIO 管脚。您可以使用 `espefuse.py` 工具来烧录 eFuse，以完成接口转换。

   烧录 eFuse 是一项不可逆的操作，所以在开始之前，请考虑以下两点：

   - 烧录 `DIS_USB_JTAG` eFuse 后，USB_SERIAL_JTAG 和 CPU 的 JTAG 接口之间的连接将被永久禁用，此后您可以将 JTAG 接口连接到 |jtag-gpio-list|。注意，烧录后，USB_SERIAL_JTAG 的 USB CDC 功能仍然可用，即仍然可以通过 USB CDC 进行烧录和 log 查看。
 
   - 烧录 ``JTAG_SEL_ENABLE`` eFuse 后，JTAG 接口的选择将由 strapping 管脚 |jtag-sel-gpio| 来决定。{IDF_TARGET_NAME} 复位时，如果该 strapping 管脚为低电平，JTAG 接口将使用 |jtag-gpio-list|，如果为高电平，USB_SERIAL_JTAG 将被用作 JTAG 接口。


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

您可能还需要安装软件驱动，才能使 JTAG 在计算机上正常工作，请参阅您所使用的 JTAG 适配器的有关文档，获取相关详细信息。


连接
^^^^

将 JTAG 接口连接到计算机，打开 {IDF_TARGET_NAME} 和 JTAG 接口板上的电源，然后检查计算机是否可以识别到 JTAG 接口。


要继续设置调试环境，请前往 :ref:`jtag-debugging-run-openocd` 章节。
