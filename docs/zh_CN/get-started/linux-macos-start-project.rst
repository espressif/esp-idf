*****************************************************
在 Linux 或 macOS 上通过命令行构建项目
*****************************************************

本指南将介绍如何使用命令行在 {IDF_TARGET_NAME} 上创建新项目，构建、烧录和监控设备输出。


.. _get-started-set-up-env-linux-macos:

激活环境
========

.. note::

    本章节描述了 ESP-IDF v6.0 及以上版本激活环境的默认和推荐步骤。如果您使用 :doc:`Linux 和 macOS 上的传统安装方法 <linux-macos-setup-legacy>`，请跳过本章节。

在使用 ESP-IDF 工具之前，需要先激活 ESP-IDF 开发环境。您可以通过图形用户界面 (GUI) 或命令行界面 (CLI) 两种方式来激活。

- `使用 EIM GUI 激活`_
- `使用 EIM CLI 激活`_

使用 EIM GUI 激活
~~~~~~~~~~~~~~~~~

.. include:: eim-gui-activate-env.rst

使用 EIM CLI 激活
~~~~~~~~~~~~~~~~~

ESP-IDF 安装完成后，EIM 命令行界面会打印一条命令提示，该命令用于激活 ESP-IDF 环境，例如：

.. code-block:: bash
    :emphasize-lines: 6

    You have successfully installed ESP-IDF
    for using the ESP-IDF tools inside the terminal, you will find activation scripts inside the base install folder
    sourcing the activation script will setup environment in the current terminal session
    ============================================
    to activate the environment, run the following command in your terminal:
        source "/Users/username/.espressif/tools/activate_idf_v5.4.2.sh"
    ============================================

在终端中运行上述高亮命令：

.. code-block:: bash

    source "/Users/username/.espressif/tools/activate_idf_v5.4.2.sh"

至此，您已成功在终端中激活 ESP-IDF 环境。后续所有 ESP-IDF 相关命令都需在已激活的终端中运行。


开始创建项目
================

现在，可以准备开发 {IDF_TARGET_NAME} 应用程序了。可以从 ESP-IDF 中 :idf:`examples` 目录下的 :example:`get-started/hello_world` 项目开始。

.. important::

    ESP-IDF 编译系统不支持 ESP-IDF 路径或其项目路径中带有空格。

将 :example:`get-started/hello_world` 项目复制至本地的 ``~/esp`` 目录下：

.. code-block:: bash

    cd ~/esp
    cp -r $IDF_PATH/examples/get-started/hello_world .

.. note:: ESP-IDF 的 :idf:`examples` 目录下有一系列示例项目，可以按照上述方法复制并运行其中的任何示例，也可以直接编译示例，无需进行复制。

.. _get-started-connect-linux-macos:

连接设备
==============

现在，请将 {IDF_TARGET_NAME} 开发板连接到 PC，并查看开发板使用的串口。

通常，串口在不同操作系统下显示的名称有所不同：

- **Linux 操作系统：** 以 ``/dev/tty`` 开头
- **macOS 操作系统：** 以 ``/dev/cu.`` 开头

有关如何查看串口名称的详细信息，请见 :doc:`establish-serial-connection`。

.. note::

    请记住串口名，以便后续使用。

.. _get-started-configure-linux-macos:

配置项目
=============

请进入 ``hello_world`` 目录，设置 {IDF_TARGET_NAME} 为目标芯片，然后运行项目配置工具 ``menuconfig``。

.. code-block:: bash

    cd ~/esp/hello_world
    idf.py set-target {IDF_TARGET_PATH_NAME}
    idf.py menuconfig

打开一个新项目后，应首先使用 ``idf.py set-target {IDF_TARGET_PATH_NAME}`` 设置“目标”芯片。注意，此操作将清除并初始化项目之前的编译和配置（如有）。也可以直接将“目标”配置为环境变量（此时可跳过该步骤）。更多信息，请见 :ref:`selecting-idf-target`。

正确操作上述步骤后，系统将显示以下菜单：

.. figure:: ../../_static/project-configuration.png
    :align: center
    :alt: 项目配置 — 主窗口

    项目配置 — 主窗口

可以通过此菜单设置项目的具体变量，包括 Wi-Fi 网络名称、密码和处理器速度等。``hello_world`` 示例项目会以默认配置运行，因此在这一项目中，可以跳过使用 ``menuconfig`` 进行项目配置这一步骤。

.. only:: esp32

    .. attention::

        如果使用的是 ESP32-DevKitC（板载 ESP32-SOLO-1 模组）或 ESP32-DevKitM-1（板载 ESP32-MINI-1/1U 模组），请在烧写示例程序前，前往 ``menuconfig`` 中使能单核模式 (:ref:`CONFIG_FREERTOS_UNICORE`)。

.. note::

    终端窗口中显示出的菜单颜色可能会与上图不同。可以通过选项 ``--style`` 来改变外观。请运行 ``idf.py menuconfig --help`` 命令，获取更多信息。

.. only:: esp32 or esp32s2 or esp32s3

    如果使用的是支持的开发板，可以通过板级支持包 (BSP) 来协助开发。更多信息，请见 `其他提示 <#additional-tips>`__。

.. only:: esp32s2

    控制台输出配置
    ----------------------------

    如需使用 USB 烧录 {IDF_TARGET_NAME}，请将控制台的输出通道改为 USB。对于 {IDF_TARGET_NAME}，默认的控制台输出通道为 UART。

    1. 前往选项 ``Channel for console output``。

        ``Component config`` > ``ESP System Settings`` > ``Channel for console output``

    2. 将默认选项 UART 改为：

        ``USB CDC``

    3. 保存设置，退出 ``menuconfig`` 界面。

.. include:: start-project.rst
