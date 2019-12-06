在用户配置文件中添加 IDF_PATH（传统 GNU Make）
==============================================
:link_to_translation:`en:[English]`

.. include:: ../gnu-make-legacy.rst

为了在系统多次启动时，保留 ``IDF_PATH`` 环境变量的设置，请按照以下说明将该变量的设置增加至用户配置文件。

.. _add-idf_path-to-profile-windows-legacy:

Windows 操作系统
-----------------

用户配置文件脚本保存在 ``C:/msys32/etc/profile.d/`` 路径下，每次打开 MSYS2 窗口时均会执行。

#. 在 ``C:/msys32/etc/profile.d/`` 目录下创建一个新的脚本文件，并将其命名为 ``export_idf_path.sh``。

#. 指定您的 ESP-IDF 保存路径，这通常与您的系统配置有关，比如 ``C:\msys32\home\user-name\esp\esp-idf``。

#. 在脚本文件中加入下方 ``export`` 命令行，例::

    export IDF_PATH="C:/msys32/home/user-name/esp/esp-idf"

   注意请将反斜杠替换为 Windows 操作系统路径要求的正斜杠。

#. 保存脚本文件。

#. 关闭 MSYS2 窗口，并重新打开。运行以下命令，检查 ``IDF_PATH`` 是否成功设置::

    printenv IDF_PATH

   如果设置成功，则此时将打印之前在脚本中输入的路径。

   如果您不希望永久设置 ``IDF_PATH`` 变量，也可以在每次打开 MSYS2 窗口时进行手动设置::

    export IDF_PATH="C:/msys32/home/user-name/esp/esp-idf"

如果您是在安装 ESP32 软件的过程中从 :ref:`get-started-setup-path-legacy` 章节跳转至此，请返回 :ref:`get-started-start-project-legacy` 章节。

.. _add-idf_path-to-profile-linux-macos-legacy:

Linux 和 MacOS 操作系统
-------------------------

在 ``~/.profile`` 文件中增加以下命令，设置 ``IDF_PATH`` 变量::

    export IDF_PATH=~/esp/esp-idf

退出并重新登陆，检查设置是否生效。

.. note::

    如果您已将 ``/bin/bash`` 设置为登录 shell，且同时存在 ``.bash_profile`` 和 ``.profile``，则请更新 ``.bash_profile`` 。

运行以下命令，检查 ``IDF_PATH`` 是否成功设置::

    printenv IDF_PATH

如果设置成功，则此时将打印之前在 ``~/.profile`` 文件中输入（或手动设置）的路径。

如果您不希望永久设置 ``IDF_PATH`` 变量，也可以在每次重新打开终端窗口时进行手动设置::

    export IDF_PATH=~/esp/esp-idf

如果您是在安装 ESP32 软件的过程中从 :ref:`get-started-setup-path-legacy` 章节跳转至此，请返回 :ref:`get-started-start-project-legacy` 章节。
