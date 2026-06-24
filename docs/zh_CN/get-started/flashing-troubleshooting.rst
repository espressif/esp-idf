烧录故障排除
========================

:link_to_translation:`en:[English]`

连接失败
-----------------

{IDF_TARGET_STRAP_GPIO:default="[NEEDS TO BE UPDATED]", esp32="GPIO0", esp32s2="GPIO0", esp32s3="GPIO0", esp32c2="GPIO9", esp32c3="GPIO9", esp32c6="GPIO9", esp32h2="GPIO9", esp32p4="GPIO35", esp32c5="GPIO28", esp32c61="GPIO9"}

如果在运行给定命令时出现如“连接失败”这样的错误，造成该错误的原因之一可能是运行 ``esptool`` 时出现错误。 ``esptool`` 是构建系统调用的程序，用于重置芯片、与 ROM 引导加载器交互以及烧录固件的工具。可以按照以下步骤进行手动复位，轻松解决该问题。如果问题仍未解决，请参考 `esptool 故障排除 <https://docs.espressif.com/projects/esptool/en/latest/esp32/troubleshooting.html>`_ 获取更多信息。

``esptool`` 通过使 USB 至 UART 桥（如 FTDI 或 CP210x）的 DTR 和 RTS 控制线生效来自动复位 {IDF_TARGET_NAME}（请参考 :doc:`establish-serial-connection` 获取更多详细信息）。DTR 和 RTS 控制线又连接到 {IDF_TARGET_NAME} 的 ``{IDF_TARGET_STRAP_GPIO}`` 和 ``CHIP_PU`` (EN) 管脚上，因此 DTR 和 RTS 的电压电平变化会使 {IDF_TARGET_NAME} 进入固件下载模式。相关示例可查看 ESP32 DevKitC 开发板的 `原理图 <https://dl.espressif.com/dl/schematics/esp32_devkitc_v4-sch-20180607a.pdf>`_。

一般来说，使用官方的 ESP-IDF 开发板不会出现问题。但是， ``esptool`` 在以下情况下不能自动重置硬件：

- 硬件未连接到 ``{IDF_TARGET_STRAP_GPIO}`` 和 ``CIHP_PU`` 的 DTR 和 RTS 控制线。
- DTR 和 RTS 控制线的配置方式不同。
- 不存在这样的串行控制线路。

根据硬件的种类，也可以将 {IDF_TARGET_NAME} 开发板手动设置为固件下载模式（复位）。

- 对于乐鑫开发板，可以参考对应开发板的入门指南或用户指南。例如，可以通过按住 ``Boot`` 按钮 (``{IDF_TARGET_STRAP_GPIO}``) 再按住 ``EN`` 按钮 (``CHIP_PU``) 来手动复位 ESP-IDF 开发板。
- 对于其他类型的硬件，可以尝试将 ``{IDF_TARGET_STRAP_GPIO}`` 拉低。
