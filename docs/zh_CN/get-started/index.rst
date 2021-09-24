***********
快速入门
***********

:link_to_translation:`en:[English]`

.. 请保证 README.md 文件与该文件保持同步

本文档旨在指导用户搭建 {IDF_TARGET_NAME} 硬件开发的软件环境，通过一个简单的示例展示如何使用 ESP-IDF (Espressif IoT Development Framework) 配置菜单，并编译、下载固件至 {IDF_TARGET_NAME} 开发板等步骤。

.. include-build-file:: inc/version-note.inc

概述
============

{IDF_TARGET_NAME} SoC 芯片支持以下功能：

.. only:: esp32

    * 2.4 GHz Wi-Fi
    * 蓝牙 4.2
    * 高性能双核
    * 超低功耗协处理器
    * 多种外设

.. only:: esp32s2

    * 2.4 GHz Wi-Fi
    * 高性能单核
    * 运行 RISC-V 或 FSM 内核的超低功耗协处理器
    * 多种外设
    * 内置安全硬件
    * USB OTG 接口

{IDF_TARGET_NAME} 采用 40 nm 工艺制成，具有最佳的功耗性能、射频性能、稳定性、通用性和可靠性，适用于各种应用场景和不同功耗需求。

乐鑫为用户提供完整的软、硬件资源，进行 {IDF_TARGET_NAME} 硬件设备的开发。其中，乐鑫的软件开发环境 ESP-IDF 旨在协助用户快速开发物联网 (IoT) 应用，可满足用户对 Wi-Fi、蓝牙、低功耗等方面的要求。

准备工作
=============

硬件：

* 一款 **{IDF_TARGET_NAME}** 开发板
* **USB 数据线**  （A 转 Micro-B）
* 电脑（Windows、Linux 或 mac OS）

软件：

您可以选择下载并手动安装以下软件：

    * 设置 **工具链**，用于编译 {IDF_TARGET_NAME} 代码；
    * **编译构建工具** —— CMake 和 Ninja 编译构建工具，用于编译 {IDF_TARGET_NAME} **应用程序**；
    * 获取 **ESP-IDF** 软件开发框架。该框架已经基本包含 {IDF_TARGET_NAME} 使用的 API（软件库和源代码）和运行 **工具链** 的脚本；

或者，您也可以通过以下集成开发环境 (IDE) 中的官方插件完成安装流程：

    * `Eclipse 插件 <https://github.com/espressif/idf-eclipse-plugin>`_ (`安装 <https://github.com/espressif/idf-eclipse-plugin#installing-idf-plugin-using-update-site-url>`__)
    * `VS Code 插件 <https://github.com/espressif/vscode-esp-idf-extension>`_ (`安装 <https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/install.md>`__)

.. figure:: ../../_static/what-you-need.png
    :align: center
    :alt: {IDF_TARGET_NAME} 应用程序开发
    :figclass: align-center

    {IDF_TARGET_NAME} 应用程序开发


开发板简介
===========================

请点击下方连接，了解有关具体开发板的详细信息。

.. only:: esp32

    .. toctree::
        :maxdepth: 1

        ESP32-DevKitC <../hw-reference/esp32/get-started-devkitc>
        ESP-WROVER-KIT <../hw-reference/esp32/get-started-wrover-kit>
        ESP32-PICO-KIT <../hw-reference/esp32/get-started-pico-kit>
        ESP32-Ethernet-Kit <../hw-reference/esp32/get-started-ethernet-kit>
        ESP32-DevKit-S(-R) <../hw-reference/esp32/user-guide-devkits-r-v1.1>

.. only:: esp32s2

    .. toctree::
        :maxdepth: 1

        ESP32-S2-Saola-1 <../hw-reference/esp32s2/user-guide-saola-1-v1.2>
        ESP32-S2-DevKitM-1(U) <../hw-reference/esp32s2/user-guide-devkitm-1-v1>
        ESP32-S2-DevKitC-1 <../hw-reference/esp32s2/user-guide-s2-devkitc-1>
        ESP32-S2-Kaluga-Kit <../hw-reference/esp32s2/user-guide-esp32-s2-kaluga-1-kit>

.. _get-started-step-by-step:

详细安装步骤
=========================

请根据下方详细步骤，完成安装过程。

设置开发环境
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* :ref:`get-started-get-prerequisites` （:doc:`Windows <windows-setup>` 、:doc:`Linux <linux-setup>` 和 :doc:`macOS <macos-setup>`）
* :ref:`get-started-get-esp-idf`
* :ref:`get-started-set-up-tools`
* :ref:`get-started-set-up-env`

创建您的第一个工程
~~~~~~~~~~~~~~~~~~~~~~~~~~~

* :ref:`get-started-start-project`
* :ref:`get-started-connect`
* :ref:`get-started-configure`
* :ref:`get-started-build`
* :ref:`get-started-flash`
* :ref:`get-started-build-monitor`


.. _get-started-get-prerequisites:

第一步：安装准备
=============================

在正式开始创建工程前，请先完成工具的安装，具体步骤见下：

.. toctree::
    :hidden:

    Windows <windows-setup>
    Linux <linux-setup>
    macOS <macos-setup>

+-------------------+-------------------+-------------------+
| |windows-logo|    | |linux-logo|      | |macos-logo|      |
+-------------------+-------------------+-------------------+
| `Windows`_        | `Linux`_          | `mac OS`_         |
+-------------------+-------------------+-------------------+

.. |windows-logo| image:: ../../_static/windows-logo.png
    :target: ../get-started/windows-setup.html

.. |linux-logo| image:: ../../_static/linux-logo.png
    :target: ../get-started/linux-setup.html

.. |macos-logo| image:: ../../_static/macos-logo.png
    :target: ../get-started/macos-setup.html

.. _Windows: ../get-started/windows-setup.html
.. _Linux: ../get-started/linux-setup.html
.. _mac OS: ../get-started/macos-setup.html

.. _get-started-get-esp-idf:

第二步：获取 ESP-IDF
=================================

在围绕 {IDF_TARGET_NAME} 构建应用程序之前，请先获取乐鑫提供的软件库文件 `ESP-IDF 仓库 <https://github.com/espressif/esp-idf>`_。

获取 ESP-IDF 的本地副本：打开终端，切换到您要保存 ESP-IDF 的工作目录，使用 ``git clone`` 命令克隆远程仓库。针对不同操作系统的详细步骤，请见下文。

.. note::

    在本文档中，Linux 和 macOS 操作系统中 ESP-IDF 的默认安装路径为 ``~/esp``；Windows 操作系统的默认路径为 ``%userprofile%\esp``。您也可以将 ESP-IDF 安装在任何其他路径下，但请注意在使用命令行时进行相应替换。注意，ESP-IDF 不支持带有空格的路径。

Linux 和 macOS 操作系统
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

打开终端后运行以下命令：

.. include-build-file:: inc/git-clone-bash.inc

ESP-IDF 将下载至 ``~/esp/esp-idf``。

请前往 :doc:`/versions`，查看 ESP-IDF 不同版本的具体适用场景。

Windows 操作系统
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

除了安装必要工具外，第一步中介绍的 :ref:`get-started-windows-tools-installer` 也能同时下载 ESP-IDF 本地副本。

请前往 :doc:`/versions`，查看 ESP-IDF 不同版本的具体适用场景。

除了使用 ESP-IDF 工具安装器，您也可以参考 :ref:`指南 <get-esp-idf-windows-command-line>` 手动下载 ESP-IDF。

.. _get-started-set-up-tools:

第三步：设置工具
=================================

除了 ESP-IDF 本身，您还需要安装 ESP-IDF 使用的各种工具，比如编译器、调试器、Python 包等。

Windows 操作系统
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

请根据第一步中对 Windows (:ref:`get-started-windows-tools-installer`) 的介绍，安装所有必需工具。

除了使用 ESP-IDF 工具安装器，您也可以通过 **命令提示符** 窗口手动安装这些工具。具体步骤见下：

.. code-block:: batch

    cd %userprofile%\esp\esp-idf
    install.bat {IDF_TARGET_PATH_NAME}

或使用 Windows PowerShell

.. code-block:: powershell

    cd ~/esp/esp-idf
    ./install.ps1 {IDF_TARGET_PATH_NAME}

Linux 和 macOS 操作系统
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

    cd ~/esp/esp-idf
    ./install.sh {IDF_TARGET_PATH_NAME}

或使用 Fish shell

.. code-block:: fish

    cd ~/esp/esp-idf
    ./install.fish {IDF_TARGET_PATH_NAME}

.. note::
    通过一次性指定多个目标，可为多个目标芯片同时安装工具，如运行 ``./install.sh esp32,esp32c3,esp32s3``。
    通过运行 ``./install.sh`` 或 ``./install.sh all`` 可一次性为所有支持的目标芯片安装工具。

下载工具备选方案
~~~~~~~~~~~~~~~~~~~~~~~~~~

ESP-IDF 工具安装器会下载 Github 发布版本中附带的一些工具，如果访问 Github 较为缓慢，则可以设置一个环境变量，实现优先选择 Espressif 的下载服务器进行 Github 资源下载。

.. 注解:: 该设置只影响从 Github 发布版本中下载的单个工具，它并不会改变访问任何 Git 仓库的 URL。

Windows 操作系统
-----------------

如果希望在运行 ESP-IDF 工具安装器或在使用命令行安装工具时优先选择 Espressif 下载服务器，可通过以下方式设置：打开系统控制面板，然后点击高级设置，添加一个新的环境变量（类型为用户或系统都可以，名称为 ``IDF_GITHUB_ASSETS``，值为 ``dl.espressif.com/github_assets``），最后点击确定。

如果在添加新的环境变量前命令行窗口或 ESP-IDF 工具安装器窗口已经打开，请关闭这些窗口后重新打开。

当设置好这个新的环境变量后，ESP-IDF 工具安装器以及命令行安装程序将会优先选择 Espressif 下载服务器。

.. 在 ESP-IDF 工具安装器的二进制文件更新后（导入复选框），这段需要重新更新

Linux 和 macOS 操作系统
--------------------------

要在安装工具时优先选择 Espressif 下载服务器，请在运行 ``install.sh`` 时使用以下命令：

.. code-block:: bash

    cd ~/esp/esp-idf
    export IDF_GITHUB_ASSETS="dl.espressif.com/github_assets"
    ./install.sh

自定义工具安装路径
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

本步骤中介绍的脚本将 ESP-IDF 所需的编译工具默认安装在用户根文件夹中，即 Linux 和 macOS 系统中的 ``$HOME/.espressif`` 和 Windows 系统的 ``%USERPROFILE%\.espressif``。您也可以选择将工具安装到其他目录中，但请在运行安装脚本前，重新设置环境变量 ``IDF_TOOLS_PATH``。注意，请确保您的用户已经具备了读写该路径的权限。

如果修改了 ``IDF_TOOLS_PATH`` 变量，请确保该变量在每次执行“安装脚本” （``install.bat``、``install.ps1`` 或 ``install.sh``）和导出脚本 （``export.bat``、``export.ps1`` 或 ``export.sh``）时均保持一致。

.. _get-started-set-up-env:


第四步：设置环境变量
=======================================

此时，您刚刚安装的工具尚未添加至 PATH 环境变量，无法通过“命令窗口”使用这些工具。因此，必须设置一些环境变量，这可以通过 ESP-IDF 提供的另一个脚本完成。

Windows 操作系统
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Windows 安装器（:ref:`get-started-windows-tools-installer` ）可在“开始”菜单创建一个 "ESP-IDF Command Prompt" 快捷方式。该快捷方式可以打开命令提示符窗口，并设置所有环境变量。您可以点击该快捷方式，然后继续下一步。

此外，如果您希望在当下命令提示符窗口使用 ESP-IDF，请使用下方代码：

.. code-block:: batch

    %userprofile%\esp\esp-idf\export.bat

或使用 Windows PowerShell

.. code-block:: powershell

    .$HOME/esp/esp-idf/export.ps1

Linux 和 macOS 操作系统
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

请在您需要运行 ESP-IDF 的“命令提示符”窗口运行以下命令：

.. code-block:: bash

    . $HOME/esp/esp-idf/export.sh

注意，命令开始的 "." 与路径之间应有一个空格！

如果您需要经常运行 ESP-IDF，您可以为执行 ``export.sh`` 创建一个别名，具体步骤如下：

1. 复制并粘贴以下命令到 shell 配置文件中（``.profile``， ``.bashrc``， ``.zprofile`` 等）

   .. code-block:: bash

        alias get_idf='. $HOME/esp/esp-idf/export.sh'

2. 通过重启终端窗口或运行 ``source [path to profile]``，如 ``source ~/.bashrc`` 来刷新配置文件。

现在您可以在任何终端窗口中运行 ``get_idf`` 来设置或刷新 esp-idf 环境。

这里不建议您直接将 ``export.sh`` 添加到 shell 的配置文件。因为这会导致在每个终端会话中都激活 IDF 虚拟环境（包括无需使用 IDF 的情况），从而破坏使用虚拟环境的目的，并可能影响其他软件的使用。

.. _get-started-start-project:

第五步：开始创建工程
========================================

现在，您可以开始准备开发 {IDF_TARGET_NAME} 应用程序了。您可以从 ESP-IDF 中 :idf:`examples` 目录下的 :example:`get-started/hello_world` 工程开始。

将 :example:`get-started/hello_world` 复制至您本地的 ``~/esp`` 目录下：

Linux 和 macOS 操作系统
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

    cd ~/esp
    cp -r $IDF_PATH/examples/get-started/hello_world .

Windows 操作系统
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: batch

    cd %userprofile%\esp
    xcopy /e /i %IDF_PATH%\examples\get-started\hello_world hello_world

ESP-IDF 的 :idf:`examples` 目录下有一系列示例工程，都可以按照上面的方法进行创建。您可以按照上述方法复制并运行其中的任何示例，也可以直接编译示例，无需进行复制。

.. important::

    ESP-IDF 编译系统不支持带有空格的路径。

.. _get-started-connect:

第六步：连接设备
==========================================

现在，请将您的 {IDF_TARGET_NAME} 开发板连接到 PC，并查看开发板使用的串口。

通常，串口在不同操作系统下显示的名称有所不同：

- **Windows 操作系统：** ``COM1`` 等
- **Linux 操作系统：** 以 ``/dev/tty`` 开始
- **macOS 操作系统：** 以 ``/dev/cu.`` 开始

有关如何查看串口名称的详细信息，请见 :doc:`establish-serial-connection`。

.. note::

    请记住串口名，您会在下面的步骤中用到。


.. _get-started-configure:

第七步：配置
=========================

请进入 :ref:`get-started-start-project` 中提到的 ``hello_world`` 目录，并运行工程配置工具 ``menuconfig``。

Linux 和 macOS 操作系统
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

    cd ~/esp/hello_world
    idf.py set-target {IDF_TARGET_PATH_NAME}
    idf.py menuconfig

Windows 操作系统
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: batch

    cd %userprofile%\esp\hello_world
    idf.py set-target {IDF_TARGET_PATH_NAME}
    idf.py menuconfig

打开一个新项目后，应首先设置“目标”芯片 ``idf.py set-target {IDF_TARGET_PATH_NAME}``。注意，此操作将清除并初始化项目之前的编译和配置（如有）。 您也可以直接将“目标”配置为环境变量（则可跳过该步骤）。更多信息，请见 :ref:`selecting-idf-target`。

如果之前的步骤都正确，则会显示下面的菜单：

.. figure:: ../../_static/project-configuration.png
    :align: center
    :alt: 工程配置 — 主窗口
    :figclass: align-center

    工程配置 — 主窗口

您可以通过此菜单设置项目的具体变量，包括 Wi-Fi 网络名称、密码和处理器速度等. ``hello_world`` 示例项目会以默认配置运行，因此可以跳过使用 ``menuconfig`` 进行项目配置这一步骤。

.. only:: esp32

    .. attention::

        如果您使用的是 ESP32-DevKitC（板载 ESP32-SOLO-1 模组），请在烧写示例程序前，前往 ``menuconfig`` 中使能单核模式（:ref:`CONFIG_FREERTOS_UNICORE`）。

.. 注解::

    您终端窗口中显示出的菜单颜色可能会与上图不同。您可以通过选项 ``--style`` 来改变外观。更多信息，请运行 ``idf.py menuconfig --help`` 命令。

.. _get-started-build:

第八步：编译工程
=========================

请使用以下命令，编译烧录工程：

.. code-block:: batch

    idf.py build

运行以上命令可以编译应用程序和所有 ESP-IDF 组件，接着生成 bootloader、分区表和应用程序二进制文件。

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

   [527/527] Generating hello-world.bin
   esptool.py v2.3.1

   Project build complete. To flash, run this command:
   ../../../components/esptool_py/esptool/esptool.py -p (PORT) -b 921600 write_flash --flash_mode dio --flash_size detect --flash_freq 40m 0x10000 build/hello-world.bin  build 0x1000 build/bootloader/bootloader.bin 0x8000 build/partition_table/partition-table.bin
   or run 'idf.py -p PORT flash'

如果一切正常，编译完成后将生成 .bin 文件。


.. _get-started-flash:

第九步：烧录到设备
=============================

请使用以下命令，将刚刚生成的二进制文件烧录（bootloader.bin, partition-table.bin 和 hello-world.bin）至您的 {IDF_TARGET_NAME} 开发板：

.. code-block:: bash

    idf.py -p PORT [-b BAUD] flash

请将 PORT 替换为 {IDF_TARGET_NAME} 开发板的串口名称，具体可见 :ref:`get-started-connect`。

您还可以将 BAUD 替换为您希望的烧录波特率。默认波特率为 ``460800``。

更多有关 idf.py 参数的详情，请见 :ref:`idf.py`。

.. note::

    勾选 ``flash`` 选项将自动编译并烧录工程，因此无需再运行 ``idf.py build``。


烧录过程中可能遇到的问题
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果在运行给定命令时出现如“连接失败”这样的错误，原因之一则可能是运行 ``esptool.py`` 出现错误。``esptool.py`` 是编译系统调用的程序，用于重置芯片、与 ROM 引导加载器交互以及烧录固件的工具。解决该问题的一个简单的方法就是按照以下步骤进行手动复位。如果问题仍未解决，请参考 `Troubleshooting <https://github.com/espressif/esptool#bootloader-wont-respond>`_ 获取更多信息。


``esptool.py`` 通过使 USB 转串口转接器芯片（如 FTDI 或 CP210x）的 DTR 和 RTS 控制线生效来自动复位 {IDF_TARGET_NAME}（请参考 :doc:`establish-serial-connection` 获取更多详细信息)。DTR 和 RTS 控制线又连接到 {IDF_TARGET_NAME} 的 ``GPIO0`` 和 ``CHIP_PU`` (EN) 管脚上，因此 DTR 和 RTS 的电压水平变化会使 {IDF_TARGET_NAME} 进入固件下载模式。相关示例可查看 ESP32 DevKitC 开发板的 `原理图 <https://dl.espressif.com/dl/schematics/esp32_devkitc_v4-sch-20180607a.pdf>`_。

一般来说，使用官方的 esp-idf 开发板不会出现问题。但是，``esptool.py`` 在以下情况下不能自动重置硬件。

- 您的硬件没有连接到 ``GPIO0`` 和 ``CIHP_PU`` 的 DTR 和 RTS 控制线。
- DTR 和 RTS 控制线的配置方式不同
- 根本没有这样的串行控制线路

根据您硬件的种类，也可以将您 {IDF_TARGET_NAME} 开发板手动设置成固件下载模式（复位）。
- 对于 Espressif 的开发板，您可以参考对应开发板的入门指南或用户指南。例如，可以通过按住 **Boot** 按钮 (``GPIO0``) 再按住 **EN** 按钮(``CHIP_PU``) 来手动复位 esp-idf 开发板。
- 对于其他类型的硬件，可以尝试将 ``GPIO0`` 拉低。


常规操作
~~~~~~~~~~~~~~~~

在烧录过程中，您会看到类似如下的输出日志：

.. only:: esp32

    .. code-block:: none

        ...
        esptool.py --chip esp32 -p /dev/ttyUSB0 -b 460800 --before=default_reset --after=hard_reset write_flash --flash_mode dio --flash_freq 40m --flash_size 2MB 0x8000 partition_table/partition-table.bin 0x1000 bootloader/bootloader.bin 0x10000 hello-world.bin
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
        esptool.py --chip esp32s2 -p /dev/ttyUSB0 -b 460800 --before=default_reset --after=hard_reset write_flash --flash_mode dio --flash_freq 40m --flash_size 2MB 0x8000 partition_table/partition-table.bin 0x1000 bootloader/bootloader.bin 0x10000 hello-world.bin
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

如果一切顺利，烧录完成后，开发板将会复位，应用程序 "hello_world" 开始运行。

如果您希望使用 Eclipse 或是 VS Code IDE，而非 ``idf.py``，请参考 :doc:`Eclipse 指南 <eclipse-setup>`，以及 :doc:`VS Code 指南 <vscode-setup>`。


.. _get-started-build-monitor:

第十步：监视器
===============

您可以使用 ``idf.py -p PORT monitor`` 命令，监视 “hello_world” 的运行情况。注意，不要忘记将 PORT 替换为您的串口名称。

运行该命令后，:doc:`IDF 监视器 <../api-guides/tools/idf-monitor>` 应用程序将启动::

    $ idf.py -p /dev/ttyUSB0 monitor
    Running idf_monitor in directory [...]/esp/hello_world/build
    Executing "python [...]/esp-idf/tools/idf_monitor.py -b 115200 [...]/esp/hello_world/build/hello-world.elf"...
    --- idf_monitor on /dev/ttyUSB0 115200 ---
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
    This is esp32 chip with 2 CPU cores, WiFi/BT/BLE, silicon revision 1, 2MB external flash
    Restarting in 9 seconds...
    Restarting in 8 seconds...
    Restarting in 7 seconds...

您可使用快捷键 ``Ctrl+]``，退出 IDF 监视器。

.. only:: esp32

    如果 IDF 监视器在烧录后很快发生错误，或打印信息全是乱码（见下），很有可能是因为您的开发板采用了 26 MHz 晶振，而 ESP-IDF 默认支持大多数开发板使用的 40 MHz 晶振。

    .. figure:: ../../_static/get-started-garbled-output.png
        :align: center
        :alt: 乱码输出
        :figclass: align-center

    此时，您可以：

    1. 退出监视器。
    2. 打开 :ref:`menuconfig <get-started-configure>`。
    3. 进入 ``Component config`` --> ``ESP32-specific`` --> ``Main XTAL frequency`` 进行配置，将 :ref:`CONFIG_ESP32_XTAL_FREQ_SEL` 设置为 26 MHz。
    4. 然后，请重新 :ref:`编译和烧录 <get-started-flash>` 应用程序。

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


更新 ESP-IDF
================

乐鑫会不定期推出更新版本的 ESP-IDF，修复 bug 或提供新的功能。因此，您在使用时，也应注意更新您本地的版本。最简单的方法是：直接删除您本地的 ``esp-idf`` 文件夹，然后按照 :ref:`get-started-get-esp-idf` 中的指示，重新完成克隆。

此外，您可以仅更新变更部分。具体方式，请前往 :ref:`更新 <updating>` 章节查看。

注意，更新完成后，请再次运行安装脚本，以防新版 ESP-IDF 所需的工具也有所更新。具体请参考 :ref:`get-started-set-up-tools`。

一旦重新安装好工具，请使用导出脚本更新环境，具体请参考 :ref:`get-started-set-up-env`。

相关文档
=================

.. toctree::
    :maxdepth: 1

    establish-serial-connection
    eclipse-setup
    vscode-setup
    ../api-guides/tools/idf-monitor
    toolchain-setup-scratch
    :esp32: ../get-started-legacy/index

.. _Stable version: https://docs.espressif.com/projects/esp-idf/zh_CN/stable/
.. _Releases page: https://github.com/espressif/esp-idf/releases
