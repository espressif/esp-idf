.. include:: {IDF_TARGET_PATH_NAME}.inc
   :start-after: devkit-defs
   :end-before: ---


配置 {IDF_TARGET_NAME} 内置 JTAG 接口
=====================================

:link_to_translation:`en:[English]`

{IDF_TARGET_JTAG_PIN_Dneg:default="未更新!", esp32c3="GPIO18", esp32c6="GPIO12", esp32s3="GPIO19", esp32h2="GPIO26", esp32p4="GPIO24"}
{IDF_TARGET_JTAG_PIN_Dpos:default="未更新!", esp32c3="GPIO19", esp32c6="GPIO13", esp32s3="GPIO20", esp32h2="GPIO27", esp32p4="GPIO25"}

{IDF_TARGET_NAME} 内置了 JTAG 电路，无需额外的芯片即可进行调试。只需将 USB 线连接到 D+/D- 引脚即可。对于没有外露 USB 串行 JTAG 连接器的开发板，可以使用 USB 转接线。所需的连接方式将在下一节中介绍。

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

JTAG 通信在所有支持的平台上都可工作。Windows 和 Linux 需要额外步骤，具体请参考以下说明。

Windows
"""""""

Windows 用户可能会遇到 `LIBUSB_ERROR_NOT_FOUND` 错误，可使用以下任意方法安装驱动程序以解决此问题：

- 在 :doc:`ESP-IDF 安装管理器 (EIM) <../../get-started/windows-setup>` 图形用户界面 (GUI) ，点击 ``管理安装`` 下方的 ``打开管理面板``，之后点击 ``安装驱动程序``：

    .. figure:: ../../../_static/jtag-debugging-install-usb-drivers-eim-zh.png
      :align: center
      :alt: 在 EIM GUI 中安装驱动程序
      :figclass: align-center

      在 EIM GUI 中安装驱动程序

- 在 PowerShell 运行以下命令，通过 EIM 命令行安装驱动程序：

    .. code-block:: bash

      eim install-drivers

- 在 PowerShell 运行以下命令，通过 `idf-env <https://github.com/espressif/idf-env>`_ 安装驱动程序：

    .. code-block:: bash

      Invoke-WebRequest 'https://dl.espressif.com/dl/idf-env/idf-env.exe' -OutFile .\idf-env.exe; .\idf-env.exe driver install --espressif

Linux
"""""

在 Linux 上，需要添加 OpenOCD udev 规则。方法是将以下 `udev 规则文件 <https://github.com/espressif/openocd-esp32/blob/master/contrib/60-openocd.rules>`_ 放置在 ``/etc/udev/rules.d`` 文件夹中。
