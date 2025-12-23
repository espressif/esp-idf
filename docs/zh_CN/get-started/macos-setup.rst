************************************************
在 macOS 上安装 ESP-IDF 及工具链
************************************************

:link_to_translation:`zh_CN:[中文]`

本章节将介绍如何使用 ESP-IDF 安装管理器 (EIM) 在 macOS 上安装 ESP-IDF 及其所需工具。

.. note::

   本章节描述了 ESP-IDF v6.0 及以上版本的默认和推荐安装方式。如使用 ESP-IDF v6.0 之前版本的默认安装方式，请参考 :doc:`macOS 上的传统安装方法 <linux-macos-setup-legacy>`。


第一步：安装依赖包
====================

通过 `Homebrew <https://brew.sh/>`__ 安装所需的依赖包：

.. code-block:: bash

    brew install libgcrypt glib pixman sdl2 libslirp dfu-util cmake python

.. note::

    ESP-IDF  要求 Python 版本至少为 3.10。

    但如需使用 EIM 进行 `离线安装 <https://docs.espressif.com/projects/idf-im-ui/en/latest/offline_installation.html>`_，则必须使用 **Python 3.11 或更高版本**。


第二步：安装 EIM
=======================

将 EIM 仓库添加到 Homebrew 中，以便后续使用 Homebrew 安装 EIM：

.. code-block:: bash

    brew tap espressif/eim

然后，通过 Homebrew 安装 EIM 的图形用户界面 (GUI) 或命令行界面 (CLI)：

- GUI：
    .. code-block:: bash

        brew install --cask eim-gui

- CLI：
    .. code-block:: bash

        brew install eim

.. note::

   通过 Homebrew 安装 EIM 便于后续升级更新。

   您也可以从 `Espressif 下载页面 <https://dl.espressif.com/dl/eim/>`__ 下载适用于 macOS 的 EIM 安装包，该页面提供了 CLI 和 GUI 版本的在线和离线安装程序。


第三步：使用 EIM 安装 ESP-IDF
==============================

.. include:: eim-install-idf.rst

.. toctree::
    :hidden:
    :maxdepth: 1
    :caption: Legacy Installation

    linux-macos-setup-legacy
