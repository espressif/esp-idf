USB OTG 控制台
**************

:link_to_translation:`en:[English]`

在集成了 USB 外设的芯片上，可以使用 USB 通信设备类 (CDC) 来实现串行控制台，而不是使用带有外部 USB-UART 桥接芯片的 UART，{IDF_TARGET_NAME} ROM 中包含 USB CDC 实现，支持一些基本功能，而无需应用程序包含 USB 协议栈：

* 双向串行控制台，可与 :doc:`IDF 监视器 <tools/idf-monitor>` 或其他串行监视器一起使用。
* 使用 ``esptool.py`` 和 ``idf.py flash`` 进行烧录。
* :doc:`设备固件更新 (DFU) <dfu>` 接口，可通过 ``dfu-util`` 和 ``dfu-util`` 烧录设备。

.. note::

    目前，此“USB 控制台”功能与 TinyUSB 协议栈不兼容，但 TinyUSB 可以提供自己的 CDC 实现。

硬件要求
========

如下所示，将 {IDF_TARGET_NAME} 连接到 USB 端口。

.. list-table::
    :header-rows: 1
    :widths: 50 50
    :align: center

    * - GPIO
      - USB
    * - 20
      - D+（绿色）
    * - 19
      - D-（白色）
    * - GND
      - GND（黑色）
    * -
      - +5V（红色）

一些开发板可能会提供用于内部 USB 外设的 USB 连接器，此时无需额外的连接。

.. only:: esp32s3

    默认情况下，:doc:`USB_SERIAL_JTAG <usb-serial-jtag-console>` 外设与 ESP32-S3 内部的 USB PHY 相连，USB OTG 外设需要连接外部 USB PHY 才能使用。由于 CDC 控制台由 USB OTG 外设提供，因而在默认情况下无法通过内部 PHY 使用控制台。

    通过烧录 ``USB_PHY_SEL`` eFuse， 永久切换内部 USB PHY 来与 USB OTG 外设配合使用，而不是 USB_SERIAL_JTAG。有关 USB_SERIAL_JTAG 和 USB OTG 的更多详细信息，请参阅 ESP32-S3 技术参考手册。

    请注意，USB_SERIAL_JTAG 也可提供 CDC 控制台，因此不能仅仅为了启用 CDC 控制台而从 USB_SERIAL_JTAG 切换到 USB CDC。


软件配置
========

使用 menuconfig 工具中的 ``CONFIG_ESP_CONSOLE_USB_CDC`` 选项可启用 USB 控制台功能（请参阅 :ref:`CONFIG_ESP_CONSOLE_UART`）。

启用该选项后，按常规方式构建项目。

上传应用程序
============

.. _usb_console_initial_upload:

初始上传
--------

如果 {IDF_TARGET_NAME} 尚未烧录可启用 USB 控制台的程序，则需要初始上传程序。有三种可选方案来执行初始上传。

应用程序将在完成初始上传后启动，系统界面中将显示 USB CDC 端口。

.. note::

    完成初始上传后，端口名称可能会更改，请在运行 ``idf.py monitor`` 之前再次检查端口列表。


通过 USB CDC，在 ROM 下载模式下进行初始上传
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* 按照以下步骤将 {IDF_TARGET_NAME} 调为下载模式：保持 GPIO0 低电平，同时打开复位键。许多开发板上的 "Boot" 按键与 GPIO0 相连，可以在按住 "Boot" 键的同时按 "Reset" 键。
* 系统界面中将显示串行端口。大多数操作系统（Windows 8 及更高版本、Linux、macOS）无需安装驱动程序，使用设备管理器 (Windows)，或列出 ``/dev/ttyACM*`` 设备 (Linux)， 或列出 ``/dev/cu*`` 设备 (macOS) 以查找端口名称。
* 运行 ``idf.py flash -p PORT``，上传应用程序。其中 ``PORT`` 是指在上一步中确定的端口。

通过 USB DFU，在 ROM 下载模式下进行初始上传
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* 按照以下步骤将 {IDF_TARGET_NAME} 调为下载模式：保持 GPIO0 低电平，同时打开复位键。许多开发板上的 "Boot" 按键与 GPIO0 相连，可以在按住 "Boot" 键的同时按 "Reset" 键。
* 运行 ``idf.py dfu-flash``。

有关烧录 DFU 的详细信息，请参阅 :ref:`api_guide_dfu_flash`。

通过 UART 进行初始上传
^^^^^^^^^^^^^^^^^^^^^^^^^

在有 USB-UART 桥接器的开发板上，可通过 UART 上传应用程序。运行 ``idf.py flash -p PORT``，其中 ``PORT`` 是 USB-UART 桥接器提供的串行端口的名称。

后续使用
--------

完成应用程序初始上传后，按常规方式运行 ``idf.py flash`` 和 ``idf.py monitor``。

限制
====

USB 控制台功能有一些限制。开发中的应用程序类型和工作流程不同，受限制的程度也有所不同。由于 USB CDC 是在软件中实现的，相比通过 UART 启用的控制台，通过 USB CDC 启用的会更加脆弱和复杂，而下面多数限制也因此产生。

1. 如果应用程序崩溃，某些情况下可能无法通过 USB CDC 发送紧急处理程序的输出。如果 CDC 驱动程序使用的内存已损坏，或者存在其他系统级问题，则 CDC 可能无法通过 USB 发送紧急处理程序的消息。在许多情况下，即便应用程序崩溃，USB CDC 依旧正常运转，但无法保证其输出如 UART 一般可靠。此外，如果应用程序在 USB CDC 驱动程序启动前进入循环启动，控制台同样无法输出。

2. 如果应用程序意外重置了 USB 外设管脚，或是禁用了 USB 外设，则 USB CDC 设备将从系统中消失。修复应用程序中的问题后，请按照 :ref:`usb_console_initial_upload` 流程重新烧录应用程序。

3. 如果应用程序进入 Light-sleep 模式（包括自动 Light-sleep 模式）或是 Deep-sleep 模式，则 USB CDC 设备将从系统中消失。

4. 在试图优化应用程序内存使用时，请牢记：USB CDC 驱动程序保留了一定量的 RAM 并增加了应用程序代码大小。

5. 默认情况下，当使用 USB CDC 时，低级别的 ``esp_rom_printf`` 功能和 ``ESP_EARLY_LOG`` 功能都被禁用。可以通过 :ref:`CONFIG_ESP_CONSOLE_USB_CDC_SUPPORT_ETS_PRINTF` 选项启用这些功能。启用此选项后，可以使用 ``esp_rom_printf`` 功能，但 IRAM 的使用量也随之增加。请注意，与 UART 相比，通过 USB CDC 使用 ``esp_rom_printf`` 和 ``ESP_EARLY_LOG`` 功能的成本要高得多，因此日志记录机制在中断处理程序中尤其不适用于“printf 调试”。

6. 如果在开发一个使用 TinyUSB 协议栈的应用程序时，无法使用 USB 控制台功能，其主要原因可能如下：

   * 此功能依赖于 {IDF_TARGET_NAME} ROM 中其他的 USB CDC 软件协议栈。
   * ROM CDC 协议栈使用的 USB 描述符可能与 TinyUSB 使用的描述符有所不同。
   * 在开发使用 USB 外设的应用程序时，USB 的功能有时很可能无法工作、或是无法完全工作，这可能是由 USB 描述符配置错误、USB 协议栈使用有误等原因引起的。此时为了更好的开发体验，可使用 UART 控制台进行烧录和监控。

7. 在使用 JTAG 调试应用程序时，如果 CPU 在断点处停止，则 USB CDC 可能会停止工作。USB CDC 的操作依赖于来自 USB 外设的周期性中断。如果主机在一段时间内未收到来自 USB 设备端的有效响应，则可能会断开连接设备。实际的等待时间取决于操作系统和驱动程序，范围从几百毫秒到几秒不等。
