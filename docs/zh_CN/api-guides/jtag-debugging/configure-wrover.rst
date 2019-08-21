配置 WROVER 上的 JTAG 接口
==========================
:link_to_translation:`en:[English]`

所有版本的 ESP-WROVER-KIT 板子都内置了 JTAG 调试功能，要使其正常工作，还需要设置相关跳帽来启用 JTAG 功能，设置 SPI 闪存电压和配置 USB 驱动程序。具体步骤请参考以下说明。


配置硬件
^^^^^^^^

1.  根据 :doc:`../../hw-reference/get-started-wrover-kit` 文档中 :ref:`get-started-esp-wrover-kit-v4.1-setup-options` 章节所描述的信息，设置 JP8 便可以启用 JTAG 功能。 

2.  检查 ESP32 上用于 JTAG 通信的引脚是否被接到了其它硬件上，这可能会影响 JTAG 的工作。

    +---+---------------+-----------+
    |   | ESP32 引脚    | JTAG 信号 |
    +===+===============+===========+
    | 1 | CHIP_PU       | TRST_N    |
    +---+---------------+-----------+
    | 2 | MTDO / GPIO15 | TDO       |
    +---+---------------+-----------+
    | 3 | MTDI / GPIO12 | TDI       |
    +---+---------------+-----------+
    | 4 | MTCK / GPIO13 | TCK       |
    +---+---------------+-----------+
    | 5 | MTMS / GPIO14 | TMS       |
    +---+---------------+-----------+


配置 USB 驱动
^^^^^^^^^^^^^

安装和配置 USB 驱动，这样 OpenOCD 才能够与 ESP-WROVER-KIT 板上的 JTAG 接口通信，并且使用 UART 接口上传待烧写的镜像文件。请根据你的操作系统按照以下步骤进行安装配置。

.. note:: ESP-WROVER-KIT 使用了 FT2232 芯片实现了 JTAG 适配器，所以以下说明同样适用于其他基于 FT2232 的 JTAG 适配器。


Windows
"""""""

1.  使用标准 USB A / micro USB B 线将 ESP-WROVER-KIT 与计算机相连接，并打开板子的电源。

2.  等待 Windows 识别出 ESP-WROVER-KIT 并且为其安装驱动。如果驱动没有被自动安装，请前往 `官网 <http://www.ftdichip.com/Drivers/D2XX.htm>`_ 下载并手动安装。

3.  从 `Zadig 官网 <http://zadig.akeo.ie/>`_ 下载 Zadig 工具（Zadig_X.X.exe）并运行。

4.  在 Zadig 工具中，进入 “Options” 菜单中选中 “List All Devices”。

5.  检查设备列表，其中应该包含两条与 ESP-WROVER-KIT 相关的条目：“Dual RS232-HS (Interface 0)” 和 “Dual RS232-HS (Interface 1)”。驱动的名字应该是 “FTDIBUS (vxxxx)” 并且 USB ID 为：0403 6010。

    .. figure:: ../../../_static/jtag-usb-configuration-zadig.jpg
        :align: center
        :alt: Configuration of JTAG USB driver in Zadig tool
        :figclass: align-center

        在 Zadig 工具中配置 JTAG USB 驱动

6.  第一个设备 “Dual RS232-HS（Interface 0）” 连接到了 ESP32 的 JTAG 端口，此设备原来的 “FTDIBUS (vxxxx)” 驱动需要替换成 "WinUSB (v6xxxxx)"。为此，请选择 “Dual RS232-HS (Interface 0)” 并将驱动重新安装为 “WinUSB (v6xxxxx)”，具体可以参考上图。

.. note::

    请勿更改第二个设备 “Dual RS232-HS（Interface 1）” 的驱动，它被连接到 ESP32 的串口（UART），用于上传应用程序映像给 ESP32 进行烧写。

现在，ESP-WROVER-KIT 的 JTAG 接口应该可以被 OpenOCD 使用了，想要进一步设置调试环境，请前往 :ref:`jtag-debugging-run-openocd` 章节。


Linux
"""""

1.  使用标准 USB A / micro USB B 线将 ESP-WROVER-KIT 与计算机相连接，并打开板子的电源。

.. highlight:: none

2.  打开终端，输入 ``ls -l /dev/ttyUSB*`` 命令检查操作系统是否能够识别板子的 USB 端口。类似识别结果如下：

    ::

        user-name@computer-name:~/esp$ ls -l /dev/ttyUSB*
        crw-rw---- 1 root dialout 188, 0 Jul 10 19:04 /dev/ttyUSB0
        crw-rw---- 1 root dialout 188, 1 Jul 10 19:04 /dev/ttyUSB1


3.  根据 `OpenOCD README 文档 <https://sourceforge.net/p/openocd/code/ci/master/tree/README>`_ 中 “Permissions delegation” 小节的介绍，设置这两个 USB 端口的访问权限。

4.  注销并重新登录 Linux 系统，然后重新插拔板子的电源使之前的改动生效。在终端再次输入 ``ls -l /dev/ttyUSB*`` 命令进行验证，查看这两个设备的组所有者是否已经从 ``dialout`` 更改为 ``plugdev``:

    ::

        user-name@computer-name:~/esp$ ls -l /dev/ttyUSB*
        crw-rw-r-- 1 root plugdev 188, 0 Jul 10 19:07 /dev/ttyUSB0
        crw-rw-r-- 1 root plugdev 188, 1 Jul 10 19:07 /dev/ttyUSB1

    如果看到类似的输出结果，并且你也是 ``plugdev`` 组的成员， 那么设置工作就完成了。

    具有较低编号的 ``/dev/ttyUSBn`` 接口用于 JTAG 通信，另一路接口被连接到 ESP32 的串口（UART），用于上传应用程序映像给 ESP32 进行烧写。

现在，ESP-WROVER-KIT 的 JTAG 接口应该可以被 OpenOCD 使用了，想要进一步设置调试环境，请前往 :ref:`jtag-debugging-run-openocd` 章节。


MacOS
"""""

在 macOS 上，同时使用 FT2232 的 JTAG 接口和串口还需另外进行其它操作。当操作系统加载 FTDI 串口驱动的时候，它会对 FT2232 芯片的两个通道做相同的操作。但是，这两个通道中只有一个是被用作串口，而另一个用于 JTAG，如果操作系统已经为用于 JTAG 的通道加载了 FTDI 串口驱动的话，OpenOCD 将无法连接到芯片。有两个方法可以解决这个问题： 

1. 在启动 OpenOCD 之前手动卸载 FTDI 串口驱动程序，然后启动 OpenOCD，再加载串口驱动程序。

2. 修改 FTDI 驱动程序的配置，使其不会为 FT2232 芯片的通道 B 进行自我加载，该通道用于 ESP-WROVER-KIT 板上的 JTAG 通道。 

手动卸载驱动程序
................

1. 从 `FTDI 官网 <http://www.ftdichip.com/Drivers/VCP.htm>`_ 安装驱动。

2. 使用 USB 线连接 ESP-WROVER-KIT。

3. 卸载串口驱动 ::

    sudo kextunload -b com.FTDI.driver.FTDIUSBSerialDriver

   有时，您可能还需要卸载苹果的 FTDI 驱动::

    sudo kextunload -b com.apple.driver.AppleUSBFTDI

4. 运行 OpenOCD::

    openocd -f interface/ftdi/esp32_devkitj_v1.cfg -f board/esp-wroom-32.cfg

5. 在另一个终端窗口，再一次加载 FTDI 串口驱动::

    sudo kextload -b com.FTDI.driver.FTDIUSBSerialDriver

.. note::

   如果你需要重启 OpenOCD，则无需再次卸载 FTDI 驱动程序，只需停止 OpenOCD 并再次启动它。只有在重新连接 ESP-WROVER-KIT 或者切换了电源的情况下才需要再次卸载驱动。

你也可以根据自身需求，将此过程包装进 shell 脚本中。

修改 FTDI 驱动
..............

简而言之，这种方法需要修改 FTDI 驱动程序的配置文件，这样可以防止为 FT2232H 的通道 B 自动加载串口驱动。

.. note:: 其他板子可能将通道 A 用于 JTAG，因此请谨慎使用此选项。

.. warning:: 此方法还需要操作系统禁止对驱动进行签名验证，因此可能无法被所有的用户所接受。


1. 使用文本编辑器打开 FTDI 驱动的配置文件（注意 ``sudo``）::

    sudo nano /Library/Extensions/FTDIUSBSerialDriver.kext/Contents/Info.plist

2. 找到并删除以下几行::

        <key>FT2232H_B</key>
        <dict>
            <key>CFBundleIdentifier</key>
            <string>com.FTDI.driver.FTDIUSBSerialDriver</string>
            <key>IOClass</key>
            <string>FTDIUSBSerialDriver</string>
            <key>IOProviderClass</key>
            <string>IOUSBInterface</string>
            <key>bConfigurationValue</key>
            <integer>1</integer>
            <key>bInterfaceNumber</key>
            <integer>1</integer>
            <key>bcdDevice</key>
            <integer>1792</integer>
            <key>idProduct</key>
            <integer>24592</integer>
            <key>idVendor</key>
            <integer>1027</integer>
        </dict>

3. 保存并关闭文件

4. 禁用驱动的签名认证：

   1. 点击苹果的 logo，选择 “Restart...”

   2. 重启后当听到响铃时，立即按下键盘上的 CMD+R 组合键

   3. 进入恢复模式后，打开终端

   4. 运行命令::

       csrutil enable --without kext

   5. 再一次重启系统

完成这些步骤后，可以同时使用串口和 JTAG 接口了。

想要进一步设置调试环境，请前往 :ref:`jtag-debugging-run-openocd` 章节。
