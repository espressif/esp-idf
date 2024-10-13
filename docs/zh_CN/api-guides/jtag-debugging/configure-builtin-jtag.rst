.. include:: {IDF_TARGET_PATH_NAME}.inc
   :start-after: devkit-defs
   :end-before: ---


配置 {IDF_TARGET_NAME} 内置 JTAG 接口
=====================================

:link_to_translation:`en:[English]`

{IDF_TARGET_JTAG_PIN_Dneg:default="未更新!", esp32c3="GPIO18", esp32c6="GPIO12", esp32s3="GPIO19", esp32h2="GPIO26"}
{IDF_TARGET_JTAG_PIN_Dpos:default="未更新!", esp32c3="GPIO19", esp32c6="GPIO13", esp32s3="GPIO20", esp32h2="GPIO27"}

{IDF_TARGET_NAME} 配备了内置的 JTAG 电路，无需额外芯片，即可进行调试。只需将 USB 电缆连接到 D+/D- 管脚即可。必要的连接如下所示。

配置硬件
^^^^^^^^

.. list-table:: {IDF_TARGET_NAME} 管脚及 USB 信号
    :widths: 25 75
    :header-rows: 1

    * - {IDF_TARGET_NAME} 管脚
      - USB 信号
    * - {IDF_TARGET_JTAG_PIN_Dneg}
      - D-
    * - {IDF_TARGET_JTAG_PIN_Dpos}
      - D+
    * - 5V
      - V_BUS
    * - GND
      - 接地

请不要将用于 USB 通信的 {IDF_TARGET_NAME} 管脚连接到可能会干扰 JTAG 操作的其他硬件上。

配置 USB 驱动程序
^^^^^^^^^^^^^^^^^

JTAG 通信在所有支持的平台上都可工作。Windows 用户可能会遇到 `LIBUSB_ERROR_NOT_FOUND` 错误。请使用 2.8 版本（或更新版本）的 :ref:`get-started-windows-tools-installer`，并选择驱动程序“Espressif - WinUSB 支持 JTAG (ESP32-C3/S3)”以解决此问题。如果您不想重新运行安装程序，则可以通过 `idf-env <https://github.com/espressif/idf-env>`_ 实现相同的效果。请在 PowerShell 运行以下命令::

    Invoke-WebRequest 'https://dl.espressif.com/dl/idf-env/idf-env.exe' -OutFile .\idf-env.exe; .\idf-env.exe driver install --espressif

在 Linux 上，需要添加 OpenOCD udev 规则。方法是将以下 `udev 规则文件 <https://github.com/espressif/openocd-esp32/blob/master/contrib/60-openocd.rules>`_ 放置在 ``/etc/udev/rules.d`` 文件夹中。
