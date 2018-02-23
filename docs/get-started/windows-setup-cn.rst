***************************************
Windows 平台工具链的标准设置
***************************************

引言
============

Windows 没有内置的 "make" 环境，因此如果要安装工具链，你需要一个 GNU 兼容环境。我们这里使用 MSYS2_ 来提供该环境。你不需要一直使用这个环境（你可以使用 :doc:`Eclipse <eclipse-setup>` 或其它前端工具），但是它是在后台运行的。

工具链的设置
===============

快速设置的方法是从 dl.espressif.com 下载集成在一起的工具链和 MSYS2 压缩文件：

https://dl.espressif.com/dl/esp32_win32_msys2_environment_and_toolchain-20180110.zip

将 zip 压缩文件解压到 ``C:\`` (或其它路径，这里假设是 ``C:\``)，它会使用预先准备的环境创建一个 ``msys32`` 目录。

检出
============

运行 ``C:\msys32\mingw32.exe`` 打开一个 MSYS2 的终端窗口。该窗口的环境是一个 bash shell。

.. figure:: ../_static/msys2-terminal-window.png
    :align: center
    :alt: MSYS2 MINGW32 shell window
    :figclass: align-center

    MSYS2 终端窗口

后续步骤将会使用这个窗口来为 ESP32 设置开发环境。

后续步骤
==========

要继续设置开发环境，请参考 :ref:`get-started-get-esp-idf-cn` 一节。

更新环境
========================

当 IDF 更新时，有时需要新的工具链，或者将新的需求添加到 Windows MSYS2 环境中。要将旧版本的预编译环境中的数据移动到新版本：

- 把旧的 MSYS2 环境（即 ``C:\msys32``）移动/重命名为不同的目录（即 ``C:\msys32_old``）。
- 按照前文所述步骤下载新的预编译环境。
- 将新的 MSYS2 环境解压缩到 ``C:\msys32`` （或其他位置）。
- 找到旧的 ``C:\msys32_old\home`` 目录并把它移到 ``C:\msys32``。
- 如果你不再需要 ``C:\msys32_old`` 可以将它删除。

你可以在系统上拥有独立的不同的 MSYS2 环境，前提是在不同的目录中。

或者，:ref:`你也可以更新现有的环境而不是下载新环境 <updating-existing-windows-environment>`，但是这样更复杂。

相关文档
=================

.. toctree::
    :maxdepth: 1

    windows-setup-scratch


.. _MSYS2: https://msys2.github.io/

