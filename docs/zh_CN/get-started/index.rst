***********
快速入门
***********

:link_to_translation:`en:[English]`

本文档旨在指导用户搭建 ESP32 硬件开发的软件环境，通过一个简单的示例展示如何使用 ESP-IDF (Espressif IoT Development Framework) 配置菜单，并编译、下载固件至 ESP32 开发板等步骤。

.. include:: /_build/inc/version-note.inc

概述
============

ESP32 SoC 芯片支持以下功能：

* 2.4 GHz Wi-Fi
* 蓝牙 4.2
* 高性能双核
* 超低功耗协处理器
* 多种外设

ESP32 采用 40 nm 工艺制成，具有最佳的功耗性能、射频性能、稳定性、通用性和可靠性，适用于各种应用场景和不同功耗需求。

乐鑫为用户提供完整的软、硬件资源，进行 ESP32 硬件设备的开发。其中，乐鑫的软件开发环境 ESP-IDF 旨在协助用户快速开发物联网 (IoT) 应用，可满足用户对 Wi-Fi、蓝牙、低功耗等方面的要求。

准备工作
=============

硬件：

* 一款 **ESP32** 开发板
* **USB 数据线**  (A 转 Micro-B)
* PC（Windows、Linux 或 Mac OS）

软件：

* 设置 **工具链**，用于编译 ESP32 代码；
* **编译工具** —— CMake 和 Ninja 编译工具，用于编译 ESP32 **应用程序**；
* 获取 **ESP-IDF** 软件开发框架。该框架已经基本包含 ESP32 使用的 API（软件库和源代码）和运行 **工具链** 的脚本；
* 安装 C 语言编程（**工程**）的 **文本编辑器**，例如 `Eclipse <https://www.eclipse.org/>`_。


.. figure:: ../../_static/what-you-need.png
    :align: center
    :alt: ESP32 应用程序开发
    :figclass: align-center

    ESP32 应用程序开发


开发板简介
===========================

请点击下方连接，了解有关具体开发板的详细信息。

.. toctree::
    :maxdepth: 1

    ESP32-DevKitC <../hw-reference/get-started-devkitc>
    ESP-WROVER-KIT <../hw-reference/get-started-wrover-kit>
    ESP32-PICO-KIT <../hw-reference/get-started-pico-kit>
    ESP32-Ethernet-Kit <../hw-reference/get-started-ethernet-kit>


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
    MacOS <macos-setup>

+-------------------+-------------------+-------------------+
| |windows-logo|    | |linux-logo|      | |macos-logo|      |
+-------------------+-------------------+-------------------+
| `Windows`_        | `Linux`_          | `Mac OS`_         |
+-------------------+-------------------+-------------------+

.. |windows-logo| image:: ../../_static/windows-logo.png
    :target: ../get-started/windows-setup.html

.. |linux-logo| image:: ../../_static/linux-logo.png
    :target: ../get-started/linux-setup.html

.. |macos-logo| image:: ../../_static/macos-logo.png
    :target: ../get-started/macos-setup.html

.. _Windows: ../get-started/windows-setup.html
.. _Linux: ../get-started/linux-setup.html
.. _Mac OS: ../get-started/macos-setup.html

.. _get-started-get-esp-idf:


第二步：获取 ESP-IDF
=================================

在围绕 ESP32 构建应用程序之前，请先获取乐鑫提供的软件库文件 `ESP-IDF 仓库 <https://github.com/espressif/esp-idf>`_。

获取 ESP-IDF 的本地副本：打开终端，切换到您要保存 ESP-IDF 的工作目录，使用 ``git clone`` 命令克隆远程仓库。针对不同操作系统的详细步骤，请见下文。

.. note::

    在本文档中，Linux 和 MacOS 操作系统中 ESP-IDF 的默认安装路径为 ``~/esp``；Windows 操作系统的默认路径为 ``%userprofile%\esp``。您也可以将 ESP-IDF 安装在任何其他路径下，但请注意在使用命令行时进行相应替换。注意，ESP-IDF 不支持带有空格的路径。

Linux 和 MacOS 操作系统
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

打开终端，后运行以下命令：

.. include:: /_build/inc/git-clone-bash.inc

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
    install.bat

或使用 Windows PowerShell

.. code-block:: powershell

    cd ~/esp/esp-idf
    ./install.ps1

Linux 和 MacOS 操作系统
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

    cd ~/esp/esp-idf
    ./install.sh

自定义工具安装路径
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

本步骤中介绍的脚本将 ESP-IDF 所需的编译工具默认安装在用户根文件夹中，即 Linux 和 MacOS 系统中的 ``$HOME/.espressif`` 和 Windows 系统的 ``%USERPROFILE%\.espressif``。此外，您可以可以将工具安装到其他目录中，但请在运行安装脚本前，重新设置环境变量 ``IDF_TOOLS_PATH``。注意，请确保您的用户已经具备了读写该路径的权限。

如果修改了 ``IDF_TOOLS_PATH`` 变量，请确保该变量在每次执行“安装脚本” (``install.bat``、``install.ps1`` 或 ``install.sh``) 和导出脚本 (``export.bat``、``export.ps1`` 或 ``export.sh``) 均保持一致。

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

Linux 和 MacOS 操作系统
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

请在您需要运行 ESP-IDF 的“命令提示符”窗口运行以下命令：

.. code-block:: bash

    . $HOME/esp/esp-idf/export.sh

注意，命令开始的 "." 与路径之间应有一个空格！

此外，您也可以将这行代码增加至您的 ``.profile`` 或 ``.bash_profile`` 脚本中，这样您就可以在任何命令窗口使用 ESP-IDF 工具了。

.. _get-started-start-project:

第五步：开始创建工程
========================================

现在，您可以开始准备开发 ESP32 应用程序了。您可以从 ESP-IDF 中 :idf:`examples` 目录下的 :example:`get-started/hello_world` 工程开始。

将 :example:`get-started/hello_world` 复制至您本地的 ``~/esp`` 目录下：

Linux 和 MacOS 操作系统
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

现在，请将您的 ESP32 开发板连接到 PC，并查看开发板使用的串口。

通常，串口在不同操作系统下显示的名称有所不同：

- **Windows 操作系统：** ``COM1`` 等
- **Linux 操作系统：** 以 ``/dev/tty`` 开始
- **MacOS 操作系统：** 以 ``/dev/cu.`` 开始

有关如何查看串口名称的详细信息，请见 :doc:`establish-serial-connection`。

.. note::

    请记住串口名，您会在下面的步骤中用到。


.. _get-started-configure:

第七步：配置
=========================

请进入 :ref:`get-started-start-project` 中提到的 ``hello_world`` 目录，并运行工程配置工具 ``menuconfig``。

Linux 和 MacOS 操作系统
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

    cd ~/esp/hello_world
    idf.py menuconfig

如果您的默认 Python 版本为 3.0 及以上，可能需要运行 ``python2 $(which idf.py) menuconfig``。

Windows 操作系统
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: batch

    cd %userprofile%\esp\hello_world
    idf.py menuconfig

如果之前的步骤都正确，则会显示下面的菜单：

.. figure:: ../../_static/project-configuration.png
    :align: center
    :alt: 工程配置 — 主窗口
    :figclass: align-center

工程配置 — 主窗口

``menuconfig`` 工具的常见操作见下。

* 上下箭头：移动
* ``回车``：进入子菜单
* ``ESC 键``：返回上级菜单或退出
* ``英文问号``：调出帮助菜单（退出帮助菜单，请按回车键）。
* ``空格``或 ``Y 键``：选择 ``[*]`` 配置选项；``N 键``：禁用 ``[*]`` 配置选项
* ``英文问号`` （查询配置选项）：调出有关该选项的帮助菜单
* ``/ 键``：寻找配置工程

.. attention::

    如果您使用的是 ESP32-DevKitC（板载 ESP32-SOLO-1 模组），请在烧写示例程序前，前往 ``menuconfig`` 中使能单核模式（:ref:`CONFIG_FREERTOS_UNICORE`）。

.. _get-started-build:

第八步：编译工程
=========================

请使用以下命令，编译烧录工程：::

    idf.py build

运行以上命令可以编译应用程序和所有 ESP-IDF 组件，接着生成 bootloader、分区表和应用程序二进制文件。

.. code-block:: none

    $ idf.py build
    Running cmake in directory /path/to/hello_world/build
    Executing "cmake -G Ninja --warn-uninitialized /path/to/hello_world"...
    Warn about uninitialized values.
    -- Found Git:/usr/bin/git (found version "2.17.0")
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

请使用以下命令，将刚刚生成的二进制文件烧录至您的 ESP32 开发板：

    ``idf.py -p PORT [-b BAUD] flash``

请将 PORT 替换为 ESP32 开发板的串口名称，具体可见 :ref:`get-started-connect`。

您还可以将 BAUD 替换为您希望的烧录波特率。默认波特率为 ``460800``。

更多有关 idf.py 参数的详情，请见 :ref:`idf.py`。

.. note::

    勾选 ``flash`` 选项将自动编译并烧录工程，因此无需再运行 ``idf.py build``。

.. code-block:: none

    Running esptool.py in directory [...]/esp/hello_world
    Executing "python [...]/esp-idf/components/esptool_py/esptool/esptool.py -b 460800 write_flash @flash_project_args"...
    esptool.py -b 460800 write_flash --flash_mode dio --flash_size detect --flash_freq 40m 0x1000 bootloader/bootloader.bin 0x8000 partition_table/partition-table.bin 0x10000 hello-world.bin
    esptool.py v2.3.1
    Connecting....
    Detecting chip type... ESP32
    Chip is ESP32D0WDQ6 (revision 1)
    Features: WiFi, BT, Dual Core
    Uploading stub...
    Running stub...
    Stub running...
    Changing baud rate to 460800
    Changed.
    Configuring flash size...
    Auto-detected Flash size: 4MB
    Flash params set to 0x0220
    Compressed 22992 bytes to 13019...
    Wrote 22992 bytes (13019 compressed) at 0x00001000 in 0.3 seconds (effective 558.9 kbit/s)...
    Hash of data verified.
    Compressed 3072 bytes to 82...
    Wrote 3072 bytes (82 compressed) at 0x00008000 in 0.0 seconds (effective 5789.3 kbit/s)...
    Hash of data verified.
    Compressed 136672 bytes to 67544...
    Wrote 136672 bytes (67544 compressed) at 0x00010000 in 1.9 seconds (effective 567.5 kbit/s)...
    Hash of data verified.

    Leaving...
    Hard resetting via RTS pin...

如果一切顺利，烧录完成后，开发板将会复位，应用程序 "hello_world" 开始运行。

.. note::

    （目前不支持）如果您希望使用 Eclipse IDE，而非 ``idf.py``，请参考 :doc:`Eclipse 指南 <eclipse-setup>`。


.. _get-started-build-monitor:

第十步：监视器
======================

您可以使用 ``make monitor`` 命令，监视 “hello_world” 的运行情况。注意，不要忘记将 PORT 替换为您的串口名称。

运行该命令后，:doc:`IDF 监视器 <../api-guides/tools/idf-monitor>` 应用程序将启动：::

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
    I (211) cpu_start: Starting scheduler on APP CPU.
    Restarting in 9 seconds...
    Restarting in 8 seconds...
    Restarting in 7 seconds...

您可使用快捷键 ``Ctrl+]``，退出 IDF 监视器。

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

**恭喜，您已完成 ESP32 的入门学习！**

现在，您可以尝试一些其他 :idf:`examples`，或者直接开发自己的应用程序。

更新 ESP-IDF
================

乐鑫会不时推出更新版本的 ESP-IDF，修复 bug 或提出新的特性。因此，您在使用时，也应注意更新您本地的版本。最简单的方法是：直接删除您本地的 ``esp-idf`` 文件夹，然后按照 :ref:`get-started-get-esp-idf` 中的指示，重新完成克隆。 

此外，您可以仅更新变更部分。具体方式，请前往 :ref:`更新 <updating>` 章节查看。

注意，更新完成后，请执行 ``install.sh`` （Windows 系统中为 ``install.bat``）脚本，避免新版 ESP-IDF 所需的工具也有所更新。具体请参考 :ref:`get-started-set-up-tools`。

一旦重新安装好工具，请使用“导出脚本”更新环境，具体请参考 :ref:`get-started-set-up-env`。

相关文档
=================

.. toctree::
    :maxdepth: 1

    establish-serial-connection
    eclipse-setup
    ../api-guides/tools/idf-monitor
    toolchain-setup-scratch
    ../get-started-legacy/index

.. _Stable version: https://docs.espressif.com/projects/esp-idf/zh_CN/stable/
.. _Releases page: https://github.com/espressif/esp-idf/releases
