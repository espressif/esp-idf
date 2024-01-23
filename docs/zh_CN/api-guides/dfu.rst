通过 USB 升级设备固件
=======================================

:link_to_translation:`en:[English]`

一般情况下，{IDF_TARGET_NAME} 的固件是通过芯片的串口烧录。但是，通过串口烧录 {IDF_TARGET_NAME} 需要连接 USB 转串口转换器（如 CP210x 或 FTDI），详细信息可参阅 :doc:`与 {IDF_TARGET_NAME} 创建串口连接<../get-started/establish-serial-connection>`。{IDF_TARGET_NAME} 包含一个 USB OTG 外设，使其可以通过 USB 将 {IDF_TARGET_NAME} 直接连接到主机，即不需要 USB 转串口转换器也可完成烧录。

设备固件升级 (DFU) 是一种通过通用串行总线 (USB) 升级设备固件的机制。但是，启用安全启动 (Secure Boot) 或 flash 加密会禁用 ROM 中的 USB-OTG USB 堆栈，则无法通过该端口上的模拟串口或 DFU 进行更新。

- 入门指南中的 :ref:`get-started-get-prerequisites` 介绍了 DFU 的软件要求。
- :ref:`api_guide_dfu_build` 章节介绍了如何使用 ESP-IDF 构建固件。
- :ref:`api_guide_dfu_flash` 章节介绍了如何烧录固件。


USB 连接
--------------

{IDF_TARGET_NAME} 的内部 USB PHY（收发器）与 GPIO 的连接如下表所示：

.. list-table::
   :header-rows: 1
   :widths: 25 20

   * - GPIO
     - USB

   * - 20
     - D+（绿色）

   * - 19
     - D-（白色）

   * - GND
     - GND（黑色）

   * - +5V
     - +5V（红色）

.. warning::

    一些连接线采用非标准颜色连接，有时调换下 D+ 和 D- 的连接，驱动程序就能正常工作。如果无法检测到你的设备，请尝试下调换 D+ 和 D- 的连接线。

.. only:: esp32s3

    默认情况下，{IDF_TARGET_NAME} 内部 USB PHY 与 :doc:`USB_SERIAL_JTAG<usb-serial-jtag-console>` 模块连接，此时 USB OTG 外设只有在连接外部 USB PHY 时才能使用。DFU 是通过 USB OTG 外设提供，因此在默认的设置下，无法通过内部 USB PHY 使用 DFU。

    然而，用户可以烧录 ``USB_PHY_SEL`` eFuse 使得内部 USB PHY 与 USB OTG 连接，而不是连接 USB_SERIAL_JTAG。有关 USB_SERIAL_JTAG 和 USB OTG 的更多详细信息，请参阅 *{IDF_TARGET_NAME} 技术参考手册* [`PDF <{IDF_TARGET_TRM_CN_URL}>`__]。

.. note::

    {IDF_TARGET_NAME} 芯片需要处于引导加载程序模式才能被检测为 DFU 设备并烧录。可以通过下拉 GPIO0（例如按下 BOOT 按钮）、拉低 RESET 片刻并释放 GPIO0 来实现。


.. _api_guide_dfu_build:

构建 DFU 镜像
----------------------

可以通过运行以下命令构建 DFU 镜像，该命令会在工程的 ``build`` 目录下生成 ``dfu.bin`` 文件::

    idf.py dfu

.. note::

    在运行 ``idf.py dfu`` 命令前，请记得通过 ``idf.py set-target`` 命令设置目标芯片。否则，你创建的镜像可能不是针对目标芯片，或者收到类似 ``unknown target 'dfu'`` 的错误消息。


.. _api_guide_dfu_flash:

烧录 DFU 镜像
------------------------------------

运行以下命令将 DFU 镜像下载到 {IDF_TARGET_NAME} 中::

    idf.py dfu-flash

该命令依赖于 `dfu-util <http://dfu-util.sourceforge.net/>`_。关于如何安装 ``dfu-util``，请参考 :ref:`get-started-get-prerequisites`。对于 Windows 和 Linux 用户，``dfu-util`` 还需进行额外设置。Windows 用户请参考 :ref:`api_guide_dfu_flash_win`，Linux 用户请参考 :ref:`api_guide_dfu_flash_udev`。macOS 用户无需额外设置即可使用 ``dfu-util``。

如果连接了不止一个开发板，且这些开发板使用的芯片相同，则可以使用 ``idf.py dfu-list`` 列出所有可用设备，例如::

    Found Runtime: [303a:0002] ver=0723, devnum=4, cfg=1, intf=2, path="1-10", alt=0, name="UNKNOWN", serial="0"
    Found Runtime: [303a:0002] ver=0723, devnum=6, cfg=1, intf=2, path="1-2", alt=0, name="UNKNOWN", serial="0"

然后，可以通过 ``--path`` 参数选择所需的设备进行烧录。例如，以上设备可以通过下面的命令分别进行烧录::

    idf.py dfu-flash --path 1-10
    idf.py dfu-flash --path 1-2

.. note::

    供应商和产品标识符的设置是基于使用 ``idf.py set-target`` 命令时所选的目标芯片，在调用 ``idf.py dfu-flash`` 时无法选择。

请参考 :ref:`api_guide_dfu_flash_errors` 及其解决方案。


.. _api_guide_dfu_flash_udev:

Udev 规则（仅限 Linux）
--------------------------------

Udev 是 Linux 内核的设备管理器，允许用户在没有 ``sudo`` 的情况下运行 ``dfu-util`` （和 ``idf.py dfu-flash``）从而访问芯片。

创建文件 ``/etc/udev/rules.d/40-dfuse.rules``，并在文件中添加如下内容::

    SUBSYSTEMS=="usb", ATTRS{idVendor}=="303a", ATTRS{idProduct}=="00??", GROUP="plugdev", MODE="0666"

.. note::

    请检查 ``groups`` 命令的输出。用户必须是上面指定的 `GROUP` 的成员。你可以为此使用其他现有的组（例如，在某些系统上使用 `uucp` 而不是 `plugdev`）或为此创建一个新的组。

你可以选择重启计算机使之前的设置生效，或者手动运行 ``sudo udevadm trigger``，强制 Udev 触发新规则。


.. _api_guide_dfu_flash_win:

USB 驱动（仅限 Windows）
-------------------------------

``dfu-util`` 使用 `libusb` 来访问设备。你需要在 Windows 上使用 `WinUSB` 驱动程序注册设备。

更多详细信息，请参考 `libusb wiki <https://github.com/libusb/libusb/wiki/Windows#How_to_use_libusb_on_Windows>`_。

可以通过 `Zadig 工具 <https://zadig.akeo.ie/>`_ 安装驱动程序。请确保在运行该工具之前设备处于下载模式，并确保在安装驱动程序之前检测到 {IDF_TARGET_NAME} 设备。Zadig 工具可能会检测到 {IDF_TARGET_NAME} 的多个 USB 接口。请只为没有安装驱动的接口（可能是接口 2）安装 WinUSB 驱动，不要重新安装其他接口驱动。

.. warning::

    不建议在 Windows 的设备管理器中手动安装驱动程序，可能会造成无法正常烧录。


.. _api_guide_dfu_flash_errors:

常见错误及已知问题
------------------------------

- 出现 ``dfu-util: command not found`` 错误可能是因为该工具尚未安装或是无法在终端使用。检查是否已经安装该工具的一种简单方法是运行 ``dfu-util --version`` 命令。请参考 :ref:`get-started-get-prerequisites` 安装 ``dfu-util``。

- 出现 ``No DFU capable USB device available`` 错误的原因可能是在 Windows 上没有正确安装 USB 驱动程序（请参考 :ref:`api_guide_dfu_flash_win`），或是未在 Linux 上设置 Udev 规则（请参考 :ref:`api_guide_dfu_flash_udev`），或是设备未处于引导加载程序模式。

- 在 Windows 上使用 ``dfu-util`` 第一次烧录失败，并出现 ``Lost device after RESET?`` 错误信息。出现此问题时，请重新烧录一次，再次烧录应该会成功。


.. only:: SOC_SUPPORTS_SECURE_DL_MODE

    安全下载模式
    --------------------

    启用安全下载模式后，DFU 不再可用。请参见 :doc:`../security/flash-encryption`，了解详细信息。
