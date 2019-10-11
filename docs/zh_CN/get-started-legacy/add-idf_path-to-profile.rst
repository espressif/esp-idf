在用户配置文件中添加 IDF_PATH (传统 GNU Make)
=============================================
:link_to_translation:`en:[English]`

.. include:: ../gnu-make-legacy.rst

为了在系统多次重新启动时保留 “IDF_PATH” 环境变量的设置，请按照以下说明将其添加到用户配置文件中。

.. _add-idf_path-to-profile-windows-legacy:


Windows
-------

用户配置文件脚本存放在 ``C:/msys32/etc/profile.d/`` 目录中。每次打开 MSYS2 窗口时，系统都执行这些脚本。


#. 在 ``C:/msys32/etc/profile.d/`` 目录下创建一个新的脚本文件。将其命名为 ``export_idf_path.sh``。

#. 确定 ESP-IDF 目录的路径。路径与系统配置有关，例如 ``C:\msys32\home\user-name\esp\esp-idf``。
#. 在脚本中加入 ``export`` 命令，e.g.::

       export IDF_PATH="C:/msys32/home/user-name/esp/esp-idf"

  请将原始 Windows 路径中将反斜杠替换为正斜杠。

#. 保存脚本。

#. 关闭 MSYS2 窗口并再次打开。输入以下命令检查是否设置了 ``IDF_PATH``::

       printenv IDF_PATH

将此前在脚本文件中输入的路径打印出来。

如果您不想在用户配置文件中永久设置 ``IDF_PATH``，则应在打开 MSYS2 窗口时手动输入::
    
    export IDF_PATH="C:/msys32/home/user-name/esp/esp-idf"

如您在安装用于 ESP32 开发的软件时，从 :ref:`get-started-setup-path-legacy` 小节跳转到了这里，请返回到 :ref:`get-started-start-project-legacy` 小节。

.. _add-idf_path-to-profile-linux-macos-legacy:

Linux and MacOS
---------------

在 ``~/.profile`` 文件中加入以下指令，创建 ``IDF_PATH``：

    export IDF_PATH=~/esp/esp-idf

注销并重新登录以使此更改生效。

.. note::

    如果将 ``/bin/bash``  已设为登录 shell，并且 ``.bash_profile`` 和 ``.profile`` 同时存在，则更新 ``.bash_profile``。
    
运行以下命令以确保 ``IDF_PATH`` 已经设置好::

    printenv IDF_PATH

此前在 ``~/.profile`` 文件中输入（或者手动设置）的路径应该被打印出来。

如果不想永久设置 ``IDF_PATH``，每次重启或者注销时在终端窗口中手动输入::

    export IDF_PATH=~/esp/esp-idf

如果您从 :ref:`get-started-setup-path-legacy` 小节跳转到了这里，在安装用于 ESP32 开发的软件时，返回到 :ref:`get-started-start-project-legacy` 小节。
