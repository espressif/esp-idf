{IDF_TARGET_CORE_NUM:default="2", esp32s2="1", esp32c3="1"}

{IDF_TARGET_FEATURES:default="WiFi/BT/BLE, silicon revision 1, 2MB external flash", esp32="WiFi/BT/BLE, silicon revision 1, 2MB external flash", esp32s2="WiFi, silicon revision 0, 2MB external flash", esp32s3="This is esp32s3 chip with 2 CPU core(s), WiFi/BLE, silicon revision 0, 2MB external flash", esp32c3="WiFi/BLE, silicon revision 0, 2MB external flash"}

{IDF_TARGET_HEAP_SIZE:default="298968", esp32="298968", esp32s2="253900", esp32s3="390684", esp32c3="337332"}

现在您已经具备了使用 ESP-IDF 的所有条件，接下来将介绍如何开始您的第一个工程。

本指南将帮助您完成使用 ESP-IDF 的第一步。按照本指南，您将能使用 {IDF_TARGET_NAME} 开始创建第一个工程，并构建、烧录和监控设备输出。

.. note::

    如果您还没有安装 ESP-IDF，请前往 :ref:`get-started-step-by-step` 并按照说明操作，以获得使用本指南所需的所有软件。

开始创建工程
================

现在，您可以开始准备开发 {IDF_TARGET_NAME} 应用程序了。您可以从 ESP-IDF 中 :idf:`examples` 目录下的 :example:`get-started/hello_world` 工程开始。

.. important::

    ESP-IDF 编译系统不支持 ESP-IDF 路径或其工程路径中带有空格。

将 :example:`get-started/hello_world` 工程复制至您本地的 ``~/esp`` 目录下：

Windows 操作系统
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: batch

    cd %userprofile%\esp
    xcopy /e /i %IDF_PATH%\examples\get-started\hello_world hello_world

Linux 和 macOS 操作系统
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

    cd ~/esp
    cp -r $IDF_PATH/examples/get-started/hello_world .

.. note:: ESP-IDF 的 :idf:`examples` 目录下有一系列示例工程，您可以按照上述方法复制并运行其中的任何示例，也可以直接编译示例，无需进行复制。

连接设备
==============

现在，请将您的 {IDF_TARGET_NAME} 开发板连接到 PC，并查看开发板使用的串口。

通常，串口在不同操作系统下显示的名称有所不同：

- **Windows 操作系统：** ``COM1`` 等
- **Linux 操作系统：** 以 ``/dev/tty`` 开始
- **macOS 操作系统：** 以 ``/dev/cu.`` 开始

有关如何查看串口名称的详细信息，请见 :doc:`establish-serial-connection`。

.. note::

    请记住串口名，您会在后续步骤中使用。

配置工程
=============

请进入 ``hello_world`` 目录，设置 {IDF_TARGET_NAME} 为目标芯片，然后运行工程配置工具 ``menuconfig``。

Windows 操作系统
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: batch

    cd %userprofile%\esp\hello_world
    idf.py set-target {IDF_TARGET_PATH_NAME}
    idf.py menuconfig

Linux 和 macOS 操作系统
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

    cd ~/esp/hello_world
    idf.py set-target {IDF_TARGET_PATH_NAME}
    idf.py menuconfig

打开一个新工程后，应首先使用 ``idf.py set-target {IDF_TARGET_PATH_NAME}`` 设置“目标”芯片。注意，此操作将清除并初始化项目之前的编译和配置（如有）。 您也可以直接将“目标”配置为环境变量（此时可跳过该步骤）。更多信息，请见 :ref:`selecting-idf-target`。

如果之前的步骤都正确，则会显示下面的菜单：

.. figure:: ../../_static/project-configuration.png
    :align: center
    :alt: 工程配置 — 主窗口
    :figclass: align-center

    工程配置 — 主窗口

您可以通过此菜单设置项目的具体变量，包括 Wi-Fi 网络名称、密码和处理器速度等。``hello_world`` 示例项目会以默认配置运行，因此可以跳过使用 ``menuconfig`` 进行项目配置这一步骤。

.. only:: esp32

    .. attention::

        如果您使用的是 ESP32-DevKitC（板载 ESP32-SOLO-1 模组）或 ESP32-DevKitM-1（板载 ESP32-MINI-1(1U) 模组），请在烧写示例程序前，前往 ``menuconfig`` 中使能单核模式（:ref:`CONFIG_FREERTOS_UNICORE`）。

.. note::

    您终端窗口中显示出的菜单颜色可能会与上图不同。您可以通过选项 ``--style`` 来改变外观。请运行 ``idf.py menuconfig --help`` 命令，获取更多信息。

编译工程
=========================

请使用以下命令，编译烧录工程：

.. code-block:: batch

    idf.py build

运行以上命令可以编译应用程序和所有 ESP-IDF 组件，接着生成引导加载程序、分区表和应用程序二进制文件。

.. code-block:: none

    $ idf.py build
    Running cmake in directory /path/to/hello_world/build
    Executing "cmake -G Ninja --warn-uninitialized /path/to/hello_world"...
    Warn about uninitialized values.
    -- Found Git: /usr/bin/git (found version "2.17.0")
    -- Building empty aws_iot component due to configuration
    -- Component names: ...
    -- Component paths: ...

    ... (more lines of build system output)

    [527/527] Generating hello_world.bin
    esptool.py v2.3.1

    Project build complete. To flash, run this command:
    ../../../components/esptool_py/esptool/esptool.py -p (PORT) -b 921600 write_flash --flash_mode dio --flash_size detect --flash_freq 40m 0x10000 build/hello_world.bin  build 0x1000 build/bootloader/bootloader.bin 0x8000 build/partition_table/partition-table.bin
    or run 'idf.py -p PORT flash'

如果一切正常，编译完成后将生成 .bin 文件。


烧录到设备
=============================

请使用以下命令，将刚刚生成的二进制文件 (bootloader.bin、partition-table.bin 和 hello_world.bin) 烧录至您的 {IDF_TARGET_NAME} 开发板：

.. code-block:: bash

    idf.py -p PORT [-b BAUD] flash

请将 PORT 替换为 {IDF_TARGET_NAME} 开发板的串口名称。

您还可以将 BAUD 替换为您希望的烧录波特率。默认波特率为 ``460800``。

更多有关 idf.py 参数的详情，请见 :ref:`idf.py`。

.. note::

    勾选 ``flash`` 选项将自动编译并烧录工程，因此无需再运行 ``idf.py build``。

烧录过程中可能遇到的问题
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

{IDF_TARGET_STRAP_GPIO:default="GPIO0", esp32="GPIO0", esp32s2="GPIO0", esp32s3="GPIO0", esp32c3="GPIO9"}

如果在运行给定命令时出现如“连接失败”这样的错误，造成该错误的原因之一可能是运行 ``esptool.py`` 时出现错误。``esptool.py`` 是构建系统调用的程序，用于重置芯片、与 ROM 引导加载器交互以及烧录固件的工具。可以按照以下步骤进行手动复位，轻松解决该问题。如果问题仍未解决，请参考 `Troubleshooting <https://github.com/espressif/esptool#bootloader-wont-respond>`_. 获取更多信息。

``esptool.py`` 通过使 USB 转串口转接器芯片（如 FTDI 或 CP210x）的 DTR 和 RTS 控制线生效来自动复位 {IDF_TARGET_NAME}（请参考 :doc:`establish-serial-connection` 获取更多详细信息)。DTR 和 RTS 控制线又连接到 {IDF_TARGET_NAME} 的 ``{IDF_TARGET_STRAP_GPIO}`` 和 ``CHIP_PU`` (EN) 管脚上，因此 DTR 和 RTS 的电压电平变化会使 {IDF_TARGET_NAME} 进入固件下载模式。相关示例可查看 ESP32 DevKitC 开发板的 `原理图 <https://dl.espressif.com/dl/schematics/esp32_devkitc_v4-sch-20180607a.pdf>`_。

一般来说，使用官方的 ESP-IDF 开发板不会出现问题。但是，``esptool.py`` 在以下情况下不能自动重置硬件。

- 您的硬件没有连接到 ``{IDF_TARGET_STRAP_GPIO}`` 和 ``CIHP_PU`` 的 DTR 和 RTS 控制线。
- DTR 和 RTS 控制线的配置方式不同
- 根本没有这样的串行控制线路

根据您硬件的种类，也可以将您 {IDF_TARGET_NAME} 开发板手动设置成固件下载模式（复位）。

- 对于 Espressif 的开发板，您可以参考对应开发板的入门指南或用户指南。例如，可以通过按住 **Boot** 按钮 (``{IDF_TARGET_STRAP_GPIO}``) 再按住 **EN** 按钮(``CHIP_PU``) 来手动复位 ESP-IDF 开发板。
- 对于其他类型的硬件，可以尝试将 ``{IDF_TARGET_STRAP_GPIO}`` 拉低。

常规操作
~~~~~~~~~~~~~~~~

在烧录过程中，您会看到类似如下的输出日志：

.. only:: esp32

    .. code-block:: none

        ...
        esptool.py --chip esp32 -p /dev/ttyUSB0 -b 460800 --before=default_reset --after=hard_reset write_flash --flash_mode dio --flash_freq 40m --flash_size 2MB 0x8000 partition_table/partition-table.bin 0x1000 bootloader/bootloader.bin 0x10000 hello_world.bin
        esptool.py v3.0-dev
        Serial port /dev/ttyUSB0
        Connecting........_
        Chip is ESP32D0WDQ6 (revision 0)
        Features: WiFi, BT, Dual Core, Coding Scheme None
        Crystal is 40MHz
        MAC: 24:0a:c4:05:b9:14
        Uploading stub...
        Running stub...
        Stub running...
        Changing baud rate to 460800
        Changed.
        Configuring flash size...
        Compressed 3072 bytes to 103...
        Writing at 0x00008000... (100 %)
        Wrote 3072 bytes (103 compressed) at 0x00008000 in 0.0 seconds (effective 5962.8 kbit/s)...
        Hash of data verified.
        Compressed 26096 bytes to 15408...
        Writing at 0x00001000... (100 %)
        Wrote 26096 bytes (15408 compressed) at 0x00001000 in 0.4 seconds (effective 546.7 kbit/s)...
        Hash of data verified.
        Compressed 147104 bytes to 77364...
        Writing at 0x00010000... (20 %)
        Writing at 0x00014000... (40 %)
        Writing at 0x00018000... (60 %)
        Writing at 0x0001c000... (80 %)
        Writing at 0x00020000... (100 %)
        Wrote 147104 bytes (77364 compressed) at 0x00010000 in 1.9 seconds (effective 615.5 kbit/s)...
        Hash of data verified.

        Leaving...
        Hard resetting via RTS pin...
        Done

.. only:: esp32s2

    .. code-block:: none

        ...
        esptool.py --chip esp32s2 -p /dev/ttyUSB0 -b 460800 --before=default_reset --after=hard_reset write_flash --flash_mode dio --flash_freq 40m --flash_size 2MB 0x8000 partition_table/partition-table.bin 0x1000 bootloader/bootloader.bin 0x10000 hello_world.bin
        esptool.py v3.0-dev
        Serial port /dev/ttyUSB0
        Connecting....
        Chip is ESP32-S2
        Features: WiFi
        Crystal is 40MHz
        MAC: 18:fe:34:72:50:e3
        Uploading stub...
        Running stub...
        Stub running...
        Changing baud rate to 460800
        Changed.
        Configuring flash size...
        Compressed 3072 bytes to 103...
        Writing at 0x00008000... (100 %)
        Wrote 3072 bytes (103 compressed) at 0x00008000 in 0.0 seconds (effective 3851.6 kbit/s)...
        Hash of data verified.
        Compressed 22592 bytes to 13483...
        Writing at 0x00001000... (100 %)
        Wrote 22592 bytes (13483 compressed) at 0x00001000 in 0.3 seconds (effective 595.1 kbit/s)...
        Hash of data verified.
        Compressed 140048 bytes to 70298...
        Writing at 0x00010000... (20 %)
        Writing at 0x00014000... (40 %)
        Writing at 0x00018000... (60 %)
        Writing at 0x0001c000... (80 %)
        Writing at 0x00020000... (100 %)
        Wrote 140048 bytes (70298 compressed) at 0x00010000 in 1.7 seconds (effective 662.5 kbit/s)...
        Hash of data verified.

        Leaving...
        Hard resetting via RTS pin...
        Done

.. only:: esp32s3

    .. code-block:: none

        ...
        esptool.py esp32s3 -p /dev/ttyUSB0 -b 460800 --before=default_reset --after=hard_reset write_flash --flash_mode dio --flash_freq 80m --flash_size 2MB 0x0 bootloader/bootloader.bin 0x10000 hello_world.bin 0x8000 partition_table/partition-table.bin
        esptool.py v3.2-dev
        Serial port /dev/ttyUSB0
        Connecting....
        Chip is ESP32-S3
        Features: WiFi, BLE
        Crystal is 40MHz
        MAC: 7c:df:a1:e0:00:64
        Uploading stub...
        Running stub...
        Stub running...
        Changing baud rate to 460800
        Changed.
        Configuring flash size...
        Flash will be erased from 0x00000000 to 0x00004fff...
        Flash will be erased from 0x00010000 to 0x00039fff...
        Flash will be erased from 0x00008000 to 0x00008fff...
        Compressed 18896 bytes to 11758...
        Writing at 0x00000000... (100 %)
        Wrote 18896 bytes (11758 compressed) at 0x00000000 in 0.5 seconds (effective 279.9 kbit/s)...
        Hash of data verified.
        Compressed 168208 bytes to 88178...
        Writing at 0x00010000... (16 %)
        Writing at 0x0001a80f... (33 %)
        Writing at 0x000201f1... (50 %)
        Writing at 0x00025dcf... (66 %)
        Writing at 0x0002d0be... (83 %)
        Writing at 0x00036c07... (100 %)
        Wrote 168208 bytes (88178 compressed) at 0x00010000 in 2.4 seconds (effective 569.2 kbit/s)...
        Hash of data verified.
        Compressed 3072 bytes to 103...
        Writing at 0x00008000... (100 %)
        Wrote 3072 bytes (103 compressed) at 0x00008000 in 0.1 seconds (effective 478.9 kbit/s)...
        Hash of data verified.

        Leaving...
        Hard resetting via RTS pin...
        Done


.. only:: esp32c3

    .. code-block:: none

        ...
        esptool.py --chip esp32c3 -p /dev/ttyUSB0 -b 460800 --before=default_reset --after=hard_reset write_flash --flash_mode dio --flash_freq 80m --flash_size 2MB 0x8000 partition_table/partition-table.bin 0x0 bootloader/bootloader.bin 0x10000 hello_world.bin
        esptool.py v3.0
        Serial port /dev/ttyUSB0
        Connecting....
        Chip is ESP32-C3
        Features: Wi-Fi
        Crystal is 40MHz
        MAC: 7c:df:a1:40:02:a4
        Uploading stub...
        Running stub...
        Stub running...
        Changing baud rate to 460800
        Changed.
        Configuring flash size...
        Compressed 3072 bytes to 103...
        Writing at 0x00008000... (100 %)
        Wrote 3072 bytes (103 compressed) at 0x00008000 in 0.0 seconds (effective 4238.1 kbit/s)...
        Hash of data verified.
        Compressed 18960 bytes to 11311...
        Writing at 0x00000000... (100 %)
        Wrote 18960 bytes (11311 compressed) at 0x00000000 in 0.3 seconds (effective 584.9 kbit/s)...
        Hash of data verified.
        Compressed 145520 bytes to 71984...
        Writing at 0x00010000... (20 %)
        Writing at 0x00014000... (40 %)
        Writing at 0x00018000... (60 %)
        Writing at 0x0001c000... (80 %)
        Writing at 0x00020000... (100 %)
        Wrote 145520 bytes (71984 compressed) at 0x00010000 in 2.3 seconds (effective 504.4 kbit/s)...
        Hash of data verified.

        Leaving...
        Hard resetting via RTS pin...
        Done


如果一切顺利，烧录完成后，开发板将会复位，应用程序 "hello_world" 开始运行。

如果您希望使用 Eclipse 或是 VS Code IDE，而非 ``idf.py``，请参考 :doc:`Eclipse 指南 <eclipse-setup>`，以及 :doc:`VS Code 指南 <vscode-setup>`。

监视输出
===============

您可以使用 ``idf.py -p PORT monitor`` 命令，监视 “hello_world” 工程的运行情况。注意，不要忘记将 PORT 替换为您的串口名称。

运行该命令后，:doc:`IDF 监视器 <../api-guides/tools/idf-monitor>` 应用程序将启动：::

    $ idf.py -p <PORT> monitor
    Running idf_monitor in directory [...]/esp/hello_world/build
    Executing "python [...]/esp-idf/tools/idf_monitor.py -b 115200 [...]/esp/hello_world/build/hello_world.elf"...
    --- idf_monitor on <PORT> 115200 ---
    --- Quit: Ctrl+] | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
    ets Jun  8 2016 00:22:57

    rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
    ets Jun  8 2016 00:22:57
    ...

此时，您就可以在启动日志和诊断日志之后，看到打印的 “Hello world!” 了。

.. code-block:: none

        ...
        Hello world!
        Restarting in 10 seconds...
        This is {IDF_TARGET_PATH_NAME} chip with {IDF_TARGET_CORE_NUM} CPU core(s), {IDF_TARGET_FEATURES}
    Minimum free heap size: {IDF_TARGET_HEAP_SIZE} bytes
        Restarting in 9 seconds...
        Restarting in 8 seconds...
        Restarting in 7 seconds...

您可使用快捷键 ``Ctrl+]``，退出 IDF 监视器。

.. only:: esp32

    如果 IDF 监视器在烧录后很快发生错误，或打印信息全是乱码（如下），很有可能是因为您的开发板采用了 26 MHz 晶振，而 ESP-IDF 默认支持大多数开发板使用的 40 MHz 晶振。

    .. figure:: ../../_static/get-started-garbled-output.png
        :align: center
        :alt: 乱码输出
        :figclass: align-center

    此时，您可以：

    1. 退出监视器。
    2. 返回 `menuconfig`。
    3. 进入 ``Component config`` --> ``ESP32-specific`` --> ``Main XTAL frequency`` 进行配置，将 :ref:`CONFIG_ESP32_XTAL_FREQ_SEL` 设置为 26 MHz。
    4. 重新 `编译和烧录` 应用程序。

.. note::

    您也可以运行以下命令，一次性执行构建、烧录和监视过程：

    ``idf.py -p PORT flash monitor``

此外，

- 请前往 :doc:`IDF 监视器 <../api-guides/tools/idf-monitor>`，了解更多使用 IDF 监视器的快捷键和其他详情。
- 请前往 :ref:`idf.py`，查看更多 ``idf.py`` 命令和选项。

**恭喜，您已完成 {IDF_TARGET_NAME} 的入门学习！**

现在，您可以尝试一些其他 :idf:`examples`，或者直接开发自己的应用程序。

.. 重要::

	一些示例程序不支持 {IDF_TARGET_NAME}，因为 {IDF_TARGET_NAME} 中不包含所需的硬件。

	在编译示例程序前请查看 README 文件中 ``Supported Targets`` 表格。如果表格中包含 {IDF_TARGET_NAME}， 或者不存在这个表格，那么即表示 {IDF_TARGET_NAME} 支持这个示例程序。


其他提示
===============

权限问题 /dev/ttyUSB0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

使用某些 Linux 版本向 {IDF_TARGET_NAME} 烧录固件时，可能会出现 ``Failed to open port /dev/ttyUSB0`` 错误消息。此时可以将用户添加至 :ref:`Linux Dialout 组<linux-dialout-group>`。

兼容的 Python 版本
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ESP-IDF 支持 Python 3.7 及以上版本，建议升级操作系统到最新版本从而更新 Python。也可选择从 `sources <https://www.python.org/downloads/>`_ 安装最新版 Python，或使用 Python 管理系统如 `pyenv <https://github.com/pyenv/pyenv>`_ 对版本进行升级管理。
