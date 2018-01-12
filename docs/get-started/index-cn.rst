***********
快速入门
***********

本文档旨在指导用户创建 ESP32 的软件环境。本文将通过一个简单的例子来说明如何使用 ESP-IDF (Espressif IoT Development Framework)，包括配置、编译、下载固件到开发板等步骤。

概述
======

ESP32 是一套 Wi-Fi (2.4 GHz) 和蓝牙 (4.2) 双模解决方案，集成了高性能的 CPU 内核、超低功耗协处理器和丰富的外设。ESP32 采用 40 nm 工艺制成，具有最佳的功耗性能、射频性能、稳定性、通用性和可靠性，适用于各种应用和不同功耗需求。

乐鑫为用户提供完整的软、硬件资源进行 ESP32 设备的开发。乐鑫所研发的软件开发环境 ESP-IDF 能够帮助用户快速开发物联网 (IoT) 应用，满足用户对于 Wi-Fi、蓝牙、低功耗等性能的需求。

准备工作
=========

开发 ESP32 应用程序需要准备：


* **电脑**：安装 Windows、Linux 或者 Mac 操作系统
* **工具链**：用于编译 ESP32 **应用程序**
* **ESP-IDF**：包含 ESP32 API 和用于操作 **工具链** 的脚本
* **文本编辑器**：编写 C 语言程序，例如 `Eclipse <http://www.eclipse.org/>`_
* **ESP32 开发板** 和将其连接到 **电脑** 的 **USB 线**

.. figure:: ../_static/what-you-need.png
    :align: center
    :alt: Development of applications for ESP32
    :figclass: align-center

    开发应用程序

开发环境的准备工作包括以下三部分：

1. 设置 **工具链**
2. 从 GitHub 上获取 **ESP-IDF**
3. 安装和配置 **Eclipse**

如果你偏好使用其它编辑器，可以跳过最后一步。

环境设置好后，就可以开始开发应用程序了。整个过程可以概括为如下四步：

1. 配置 **工程** 并编写代码
2. 编译 **工程** 并链接成一个 **应用程序**
3. 烧写 **应用程序** 到 **ESP32**
4. 监视/调试 **应用程序**

下文将全程指导你操作完成这些步骤。

开发板指南
==========

如果你有下列任一 ESP32 开发板，请点击对应的链接，对照指南进行操作就可以让你的板子跑起来。

.. toctree::
    :maxdepth: 1

    ESP32 DevKitC <../get-started/get-started-devkitc>
    ESP-WROVER-KIT <../get-started/get-started-wrover-kit>
    ESP32-PICO-KIT <../get-started/get-started-pico-kit>

如果你使用其它开发板，请查看下面的内容。

.. _get-started-setup-toolchain-cn:

设置工具链
===============

用 ESP32 进行开发最快的方法是安装预编译的工具链。请根据你的操作系点击对应的链接，并按照链接中的指导进行安装。

.. toctree::
    :hidden:

    Windows <../get-started/windows-setup-cn>
    Linux <../get-started/linux-setup-cn> 
    MacOS <../get-started/macos-setup>

+-------------------+-------------------+-------------------+
| |windows-logo|    | |linux-logo|      | |macos-logo|      |
+-------------------+-------------------+-------------------+
| `Windows`_        | `Linux`_          | `Mac OS`_         |
+-------------------+-------------------+-------------------+

.. |windows-logo| image:: ../_static/windows-logo.png
    :target: ../get-started/windows-setup-cn.html

.. |linux-logo| image:: ../_static/linux-logo.png
    :target: ../get-started/linux-setup-cn.html

.. |macos-logo| image:: ../_static/macos-logo.png
    :target: ../get-started/macos-setup.html

.. _Windows: ../get-started/windows-setup-cn.html
.. _Linux: ../get-started/linux-setup-cn.html
.. _Mac OS: ../get-started/macos-setup.html

.. note::

    我们使用 ``~/esp`` 目录来安装预编译的工具链、ESP-IDF 和示例程序。你也可以使用其它目录，但是需要注意调整相应的指令。

你可以安装预编译的工具链或者自定义你的环境，这完全取决于个人经验和偏好。如果你要自定义环境，请参考 :ref:`get-started-customized-setup`。

工具链设置完成后，就可以 :ref:`get-started-get-esp-idf-cn` 了。

.. _get-started-get-esp-idf-cn:

获取 ESP-IDF
=================

.. highlight:: bash

工具链（包括用于编译和构建应用程序的程序）安装完后，你还需要 ESP32 相关的 API/库。API/库在 `ESP-IDF 仓库 <https://github.com/espressif/esp-idf>`_ 中。要获取这些 API/库，打开一个终端，进入某个你希望存放 ESP-IDF 的目录，然后 ``git clone`` 以下指令： ::

    cd ~/esp
    git clone --recursive https://github.com/espressif/esp-idf.git

ESP-IDF 将会被下载到 ``~/esp/esp-idf``。

.. note::

    注意这里有个 ``--recursive`` 选项。如果你克隆 ESP-IDF 时没有带这个选项，你还需要运行额外的命令来获取子模块： ::

        cd ~/esp/esp-idf
        git submodule update --init


.. _get-started-setup-path-cn:

设置 ESP-IDF 路径
=====================

工具链程序使用环境变量 ``IDF_PATH`` 来访问 ESP-IDF。这个变量应该设置在你的 PC 中，否则工程将不能编译。你可以在每次 PC 重启时手工设置，也可以通过在用户配置文件中定义 ``IDF_PATH`` 变量来永久性设置。要永久性设置，请参考 :doc:`add-idf_path-to-profile` 文档中 :ref:`Windows <add-idf_path-to-profile-windows>` 或 :ref:`Linux and MacOS <add-idf_path-to-profile-linux-macos>` 相关的指导进行操作。


.. _get-started-start-project-cn:

创建一个工程
===============

现在可以开始创建 ESP32 应用程序了。为了快速开始，我们这里以 IDF 的 :idf:`examples` 目录下的 :example:`get-started/hello_world` 工程为例进行说明。

将 :example:`get-started/hello_world` 拷贝到 ``~/esp`` 目录： ::

    cd ~/esp
    cp -r $IDF_PATH/examples/get-started/hello_world

ESP-IDF 的 :idf:`examples` 目录下有一系列示例工程，都可以按照上面的方法进行创建。

.. important::

    esp-idf 构建系统不支持在路径中存在空格。

.. _get-started-connect-cn:

连接
=======

还有几个步骤就完成了。在继续后续操作前，先将 ESP32 开发板连接到 PC，然后检查串口号，看看它能否正常通信。如果你不知道如何操作，请查看 Establish Serial Connection with ESP32 中的相关指导。请注意一下端口号，我们在下一步中会用到。

.. _get-started-configure-cn:

配置
=========

在终端窗口中，输入 ``cd ~/esp/hello_world`` 进入 ``hello_world`` 所在目录，然后启动工程配置工具 ``menuconfig``： ::

    cd ~/esp/hello_world
    make menuconfig

如果之前的步骤都正确，则会显示下面的菜单：

.. figure:: ../_static/project-configuration.png
    :align: center
    :alt: Project configuration - Home window
    :figclass: align-center

    工程配置 - 主窗口
    
在菜单中，进入 ``Serial flasher config`` > ``Default serial port`` 配置串口（工程将会加载到该串口上）。输入回车确认选择，选择 ``< Save >`` 保存配置，然后选择 ``< Exit >`` 退出应用程序。

.. note::

   在 Windows 系统中，端口号的名称类似 COM1，在 MacOS 中以 ``/dev/cu.`` 开始，而在 Linux 系统中，以 ``/dev/tty`` 开始。
   （详细内容可以参考章节 :doc:`establish-serial-connection`。）

下面是一些使用 ``menuconfig`` 的小技巧：

* 使用 up & down 组合键在菜单中上下移动
* 使用 Enter 键进入一个子菜单，Escape 键退出子菜单或退出整个菜单
* 输入 ``?`` 查看帮助信息，Enter 键退出帮助屏幕
* 使用空格键或 ``Y`` 和 ``N`` 键来使能 (Yes) 和禁止 (No) 带有复选框 "``[*]``" 的配置项
* 当光标在某个配置项上面高亮时，输入 ``?`` 可以直接查看该项的帮助信息
* 输入 ``/`` 搜索配置项

.. note::

    如果你是 **Arch Linux** 用户，需要进入 ``SDK tool configuration`` 将 ``Python 2 interpreter`` 从 ``python`` 修改为 ``python2``。


.. _get-started-build-flash-cn:

编译和烧写
===============

现在可以编译和烧写应用程序了，执行指令： ::

    make flash

这条命令会编译应用程序和所有的 ESP-IDF 组件，生成 bootloader、分区表和应用程序 bin 文件，并将这些 bin 文件烧写到 ESP32 板子上。

.. highlight:: none

::

    esptool.py v2.0-beta2
    Flashing binaries to serial port /dev/ttyUSB0 (app at offset 0x10000)...
    esptool.py v2.0-beta2
    Connecting........___
    Uploading stub...
    Running stub...
    Stub running...
    Changing baud rate to 921600
    Changed.
    Attaching SPI flash...
    Configuring flash size...
    Auto-detected Flash size: 4MB
    Flash params set to 0x0220
    Compressed 11616 bytes to 6695...
    Wrote 11616 bytes (6695 compressed) at 0x00001000 in 0.1 seconds (effective 920.5 kbit/s)...
    Hash of data verified.
    Compressed 408096 bytes to 171625...
    Wrote 408096 bytes (171625 compressed) at 0x00010000 in 3.9 seconds (effective 847.3 kbit/s)...
    Hash of data verified.
    Compressed 3072 bytes to 82...
    Wrote 3072 bytes (82 compressed) at 0x00008000 in 0.0 seconds (effective 8297.4 kbit/s)...
    Hash of data verified.

    Leaving...
    Hard resetting...

如果没有任何问题，在编译过程结束后将能看到类似上面的消息。最后，板子将会复位，应用程序 "hello_world" 开始启动。

如果你想使用 Eclipse IDE 而不是运行 ``make``，请参考 :doc:`Eclipse guide <eclipse-setup>`。

.. _get-started-build-monitor-cn:

监视器
=======

如果要查看 "hello_world" 程序是否真的在运行，输入命令 ``make monitor``。这个命令会启动 IDF Monitor 程序： ::

    $ make monitor
    MONITOR
    --- idf_monitor on /dev/ttyUSB0 115200 ---
    --- Quit: Ctrl+] | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
    ets Jun  8 2016 00:22:57

    rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
    ets Jun  8 2016 00:22:57
    ...

在启动消息和诊断消息后，你就能看到 "Hello world!" 程序所打印的消息： ::

    ...
    Hello world!
    Restarting in 10 seconds...
    I (211) cpu_start: Starting scheduler on APP CPU.
    Restarting in 9 seconds...
    Restarting in 8 seconds...
    Restarting in 7 seconds...

要退出监视器，请使用快捷键 ``Ctrl+]``。

.. note::

    如果串口打印的不是上面显示的消息而是类似下面的乱码： ::

        e���)(Xn@�y.!��(�PW+)��Hn9a؅/9�!�t5��P�~�k��e�ea�5�jA
        ~zY��Y(1�,1�� e���)(Xn@�y.!Dr�zY(�jpi�|�+z5Ymvp

    或者监视器程序启动失败，那么可能你的开发板用的是 26 MHz 晶振，而 ESP-IDF 默认的是 40 MHz 晶振。请退出监视器，回到 :ref:`配置 <get-started-configure-cn>`，将 :ref:`CONFIG_ESP32_XTAL_FREQ_SEL` 改为 26 MHz，然后再次 :ref:`编译和烧写 <get-started-build-flash-cn>`。请在 ``make menuconfig`` 的 Component config --> ESP32-specific --> Main XTAL frequency 中配置。

要一次性执行 ``make flash`` 和 ``make monitor``，输入 ``make flash monitor``。参考文档 :doc:`IDF Monitor <idf-monitor>` 里的快捷键和更多内容。

你已完成 ESP32 的入门！

现在你可以尝试其他的示例工程 :idf:`examples`，或者直接开发自己的应用程序。 

更新 ESP-IDF
=============

使用 ESP-IDF 一段时间后，你可能想要进行升级来获得新的性能或者对 bug 进行修复。最简单的更新方式是删除已有的 ``esp-idf`` 文件夹然后再克隆一个，即重复 :ref:`get-started-get-esp-idf-cn` 里的操作。

另外一种方法是只更新有改动的部分，如果你不容易登陆 GitHub，那么这种方法比较合适。执行以下命令： ::

    cd ~/esp/esp-idf
    git pull
    git submodule update --init --recursive

``git pull`` 指令是从 ESP-IDF 仓库中获取合并更新。``git submodule update --init --recursive`` 用来更新现有的子模块或拷贝新的子模块。在 GitHub 上，子模块链接到其他仓库，所以需要这个额外的指令来下载到你的电脑里。

如果你想使用某一版本的 ESP-IDF，比如 `v2.1` 版本，请执行以下指令： ::

    cd ~/esp
    git clone https://github.com/espressif/esp-idf.git esp-idf-v2.1
    cd esp-idf-v2.1/
    git checkout v2.1
    git submodule update --init --recursive

然后 :doc:`add-idf_path-to-profile`，这样工具链脚本就能够知道这一版本的 ESP-IDF 的具体位置。


相关文档
=================

.. toctree::
    :maxdepth: 1

    ../get-started/add-idf_path-to-profile
    ../get-started/establish-serial-connection
    ../get-started/make-project
    ../get-started/eclipse-setup
    ../get-started/idf-monitor
    ../get-started/toolchain-setup-scratch
