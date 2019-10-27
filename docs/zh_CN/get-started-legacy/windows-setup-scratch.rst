******************************************************
从零开始设置 Windows 环境下的工具链（传统 GNU Make）
******************************************************

.. include:: ../gnu-make-legacy.rst

手动安装所有工具能更好地控制整个安装流程，同时也方便高阶用户进行自定义安装。此外，经验不足的用户还可以参考 :doc:`预编译环境 <windows-setup>` 中的步骤进行准备。

使用预编译环境对工具链进行快速标准设置，请参照 :doc:`windows-setup`。


.. _configure-windows-toolchain-from-scratch-legacy:

从零开始配置工具链和环境
==============================================

本流程包括：首先，安装 MSYS2_；其次，安装 ESP-IDF 所需的 Python 工具包；最后，下载并安装 Xtensa 工具链。

* 请前往 MSYS2_ 安装器页面，并下载 ``msys2-i686-xxxxxxx.exe`` 安装器（我们仅支持 32-bit MSYS 环境，32 位和 64 位 Window 均可使用）。截止至本文最新更新之时，最新版安装器为 ``msys2-i686-20161025.exe``。

* 完成所有安装步骤。**最后一步时，请不要勾选 "Run MSYS2 32-bit now"。**

* 安装完成后，请从“开始”菜单中找到 "MSYS2 MinGW 32-bit"，运行“命令提示符”窗口。

    * 为什么要特别打开这个终端窗口？MSYS2 会对不同环境进行区分。默认的 "MSYS" 环境与 Cygwin 相仿，会为 Windows 系统的 API 调用增加一个转换层。但为了使用支持 COM 端口的原生 Python，我们需要准备好 "MinGW" 环境。

* GitHub 上的 ESP-IDF 仓库的 `tools` 目录下可以找到名为 ``windows_install_prerequisites.sh`` 的脚本。如果您还没有本地 ESP-IDF 副本，也可以从以下地址进行下载（请下载 raw 格式）：:idf_raw:`tools/windows/windows_install_prerequisites.sh`，并将其保存至您的电脑。

* 请在 MSYS2 终端窗口中指定该脚本的保存路径。注意，路径格式与 Window 路径相同，但需使用正斜杠 (``/``) 而不是反斜杠 (``\``)。例 ``C:/Users/myuser/Downloads/windows_install_prerequisites.sh``。当然，您也可以直接打开查看该脚本的内容。

* ``windows_install_prerequisites.sh`` 脚本将帮您下载并安装支持 ESP-IDF 的软件包和 ESP32 工具链。


疑难解答
~~~~~~~~~~~~~~~

* MSYS 可能在脚本运行过程中进行自动升级，导致无法使用。此时，您会看到以下错误信息::

    *** fatal error - cygheap base mismatch detected - 0x612E5408/0x612E4408

  这个问题很大可能是由于 cygwin DLL 版本不兼容。

  这种情况下，请完全关闭终端窗口（相当于终止所有进程），并重新打开一个窗口。然后，请在新窗口中重新运行 ``windows_install_prerequisites.sh`` （小技巧：您可以使用“向上箭头”找到之前运行的命令）。此时，更新流程将重启。

* 注意，MSYS2 是一个“滚动”发行版，因此安装脚本可能会为您安装比“预编译环境”中更新的软件包。因此，如果遇到与 MSYS2 安装包有关的错误，请前往 `MSYS2-packages 问题列表`_ 页面寻找答案。如果未找到所需答案，请 `提交一个 IDF Issue`_。


中国地区的 MSYS2 镜像
~~~~~~~~~~~~~~~~~~~~~~

中国地区有一些（非官方）的 MSYS2 镜像，这可以大幅提高中国地区的下载速度。

如需添加这些镜像，请在运行安装脚本前修改以下两个 MSYS2 镜像列表文件。镜像文件的保存路径为 ``/etc/pacman.d``，比如 ``c:\msys2\etc\pacman.d``。

请在 ``mirrorlist.mingw32`` 最上方增加如下语句::

    Server = https://mirrors.ustc.edu.cn/msys2/mingw/i686/
    Server = http://mirror.bit.edu.cn/msys2/REPOS/MINGW/i686

请在 ``mirrorlist.msys`` 最上方增加如下语句::

    Server = http://mirrors.ustc.edu.cn/msys2/msys/$arch
    Server = http://mirror.bit.edu.cn/msys2/REPOS/MSYS2/$arch


HTTP 代理
~~~~~~~~~~

您可以在运行“设置脚本”之前，在终端中设置 ``http_proxy`` 变量，从而允许使用 HTTP 代理下载 MSYS 和 PIP::

    export http_proxy='http://http.proxy.server:PORT'

或者修改证书::

    export http_proxy='http://user:password@http.proxy.server:PORT'

如需始终用代理使用 MSYS，请在 MSYS 目录中增加 ``/etc/profile``。


其他设置：下载工具链
============================================

.. include:: /_build/inc/download-links.inc

如果您已经安装了 MSYS2，或者想要以不同的方式执行操作，您可以在此处下载工具链：

|download_link_win32|

.. note::

	如果您已完成了 :ref:`configure-windows-toolchain-from-scratch-legacy` 中介绍的所有步骤，则已经拥有了工具链，这里无需重复下载。

.. important::

	仅拥有工具链 *尚无法* 让您在 Windows 中使用 ESP-IDF。除此之外，您还至少需要安装 GNU make、bash 和 sed。上述环境已经包括这些配置，此外还有一个主机编译器（这样才能使用 menuconfig）。


后续步骤
==========

继续设置开发环境，请前往 :ref:`get-started-get-esp-idf-legacy` 章节。

.. _updating-existing-windows-environment-legacy:

更新环境
========================

当 IDF 有更新时，有时需要安装新的工具链或为 Windows MSYS2 环境添加新的系统要求。

此时，您无需重新配置环境，仅需更新现有 Windows 环境和工具链即可。

- 将 IDF 更新至您希望的版本。
- 请运行 IDF 仓库中的 ``tools/windows/windows_install_prerequisites.sh`` 脚本。该脚本将帮您安装所有新的软件包，并下载更新工具链至最新版本。

注意，该脚本在更新 MSYS2 时也会遇到 疑难解答_ 中介绍的问题。

如需要同时支持多个 IDF 版本，您可以在不同的目录下配置独立的 MSYS2 环境。或者，您还可以下载多个工具链，并将其解压缩到不同的目录下，然后使用 PATH 环境变量指定默认工具链。

.. _MSYS2: https://msys2.github.io/
.. _MSYS2-packages 问题列表: https://github.com/Alexpux/MSYS2-packages/issues/
.. _提交一个 IDF Issue: https://github.com/espressif/esp-idf/issues/new

