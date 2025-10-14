通过 USB 升级设备固件
=======================================

:link_to_translation:`en:[English]`

.. only:: not SOC_USB_SERIAL_JTAG_SUPPORTED

    一般情况下，{IDF_TARGET_NAME} 的固件是通过芯片的串口烧录。但是，通过串口烧录 {IDF_TARGET_NAME} 需要连接 USB 转串口转换器（如 CP210x 或 FTDI），详细信息可参阅 :doc:`与 {IDF_TARGET_NAME} 创建串口连接 <../get-started/establish-serial-connection>`。{IDF_TARGET_NAME} 包含一个 USB OTG 外设，使其能够通过 USB 将 {IDF_TARGET_NAME} 直接连接到主机，即不需要 USB 转串口转换器也可完成烧录。

.. only:: SOC_USB_SERIAL_JTAG_SUPPORTED

    一般情况下，{IDF_TARGET_NAME} 的固件是通过芯片的串口或 USB_SERIAL_JTAG 烧录，详细信息可参阅 :doc:`与 {IDF_TARGET_NAME} 创建串口连接 <../get-started/establish-serial-connection>`。{IDF_TARGET_NAME} 还包含一个 USB OTG 外设，使其能够通过 USB 设备固件升级直接连接到主机。

.. only:: esp32s3

    默认情况下，:doc:`USB_SERIAL_JTAG<usb-serial-jtag-console>` 模块连接到 {IDF_TARGET_NAME} 的内部 USB PHY，而 USB OTG 外设只有在连接外部 USB PHY 时才能使用。由于 DFU 是通过 USB OTG 外设提供的，因此在默认的设置下，无法通过内部 USB PHY 使用 DFU。

    然而，你可以通过烧录 ``USB_PHY_SEL`` eFuse，将内部 USB PHY 永久切换为支持 USB OTG 外设的模式，不再用于 USB_SERIAL_JTAG。有关 USB_SERIAL_JTAG 和 USB OTG 的更多信息，请参阅 *{IDF_TARGET_NAME} 技术参考手册* [`PDF <{IDF_TARGET_TRM_CN_URL}>`__]。

设备固件升级 (DFU) 是一种通过通用串行总线 (USB) 升级设备固件的机制。但是，启用安全启动 (Secure Boot) 或 flash 加密会禁用 ROM 中的 USB-OTG USB 堆栈，则无法通过该端口上的模拟串口或 DFU 进行更新。

- 入门指南中的 :ref:`get-started-get-prerequisites` 介绍了 DFU 的软件要求。
- :ref:`api_guide_dfu_build` 章节介绍了如何使用 ESP-IDF 构建固件。
- :ref:`api_guide_dfu_flash` 章节介绍了如何烧录固件。


USB 连接
--------------

.. only:: esp32p4

    {IDF_TARGET_NAME} 将 USB D+ 和 D- 信号连接到其专用引脚。为了实现 USB 设备功能，这些引脚必须连接到 USB 总线，如，通过 Micro-B 接口、USB-C 接口进行连接，或直接连接到标准 A 型插头。

.. only:: esp32s2 or esp32s3

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

    一些连接线采用非标准颜色连接，且一些驱动程序能够在对调了 D+ 和 D- 连接的情况下正常工作。因此如果无法检测到设备，请尝试下调换 D+ 和 D- 的连接线。

.. note::

    {IDF_TARGET_NAME} 芯片需要处于引导加载程序模式才能被检测为 DFU 设备并烧录。有关如何进入引导加载程序模式的更多信息，请参阅 `Boot Mode Selection <https://docs.espressif.com/projects/esptool/en/latest/{IDF_TARGET_PATH_NAME}/advanced-topics/boot-mode-selection.html#select-bootloader-mode>`_。


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

Udev 是 Linux 内核的设备管理器，允许在没有 ``sudo`` 的情况下运行 ``dfu-util`` （和 ``idf.py dfu-flash``）从而访问芯片。

创建文件 ``/etc/udev/rules.d/40-dfuse.rules``，并在文件中添加如下内容::

    SUBSYSTEMS=="usb", ATTRS{idVendor}=="303a", ATTRS{idProduct}=="00??", GROUP="plugdev", MODE="0666"

.. note::

    请检查 ``groups`` 命令的输出。加入上面指定的 `GROUP` 组获取访问权限。你可以为此使用其他现有的组（例如，在某些系统上使用 `uucp` 而不是 `plugdev`）或为此创建一个新的组。

你可以选择重启计算机使之前的设置生效，或者手动运行 ``sudo udevadm trigger``，强制 Udev 触发新规则。


.. _api_guide_dfu_flash_win:

USB 驱动（仅限 Windows）
------------------------

``dfu-util`` 使用 `libusb` 访问设备。在 Windows 上，必须先安装 `WinUSB` 驱动程序才能使设备正常工作。详情请参阅 `libusb wiki <https://github.com/libusb/libusb/wiki/Windows#How_to_use_libusb_on_Windows>`_。

.. only:: esp32s2

    开发板驱动程序可以从 https://github.com/espressif/esp-win-usb-drivers/releases 下载。文件需要解压并 `安装 <https://learn.microsoft.com/zh-cn/windows-hardware/drivers/ifs/using-an-inf-file-to-install-a-file-system-filter-driver#right-click-install>`_。进行以上操作，可以为正确的设备接口更改或安装 WinUSB 驱动程序。

.. note::

    如果该功能无法正常运作，请手动分配驱动程序；若设备正常工作，请跳过以下章节。

USB 驱动（仅限 Windows）- 手动分配驱动程序
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

可以使用 `Zadig 工具 <https://zadig.akeo.ie/>`_ 手动分配驱动程序。在运行工具前，请确保设备处于下载模式，且在安装驱动程序之前已检测到 {IDF_TARGET_NAME} 设备。

Zadig 工具可能会检测到 {IDF_TARGET_NAME} 的多个 USB 接口。请 **仅为** 没有安装驱动程序的接口（可能是接口 2）安装 `WinUSB` 驱动程序，不要重新安装其他接口的驱动程序。

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
