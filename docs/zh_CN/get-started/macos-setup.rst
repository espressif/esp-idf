************************************************
在 macOS 上安装 ESP-IDF 及工具链
************************************************

:link_to_translation:`en:[English]`

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

    EIM 所需的 Python 版本，请参考 `EIM 文档 <https://docs.espressif.com/projects/idf-im-ui/en/latest/prerequisites.html#python-version>`_。


第二步：安装 EIM
=======================

您可以使用以下任一方法安装 EIM：

- `下载 EIM`_
- `通过 Homebrew 安装 EIM`_

如果通过 Homebrew 安装 EIM，后续运行单个命令，即可轻松升级更新 EIM。


下载 EIM
~~~~~~~~~

.. figure:: ../../_static/get-started-eim-download.drawio.png
   :alt: EIM 下载页面
   :align: left
   :width: 35%
   :target: https://dl.espressif.com/dl/eim/

可选择下载在线或离线安装程序的图形用户界面 (GUI) 或命令行界面 (CLI) 版本。


通过 Homebrew 安装 EIM
~~~~~~~~~~~~~~~~~~~~~~~~~

将 EIM 仓库添加到 `Homebrew <https://brew.sh/>`__ 中，以便后续使用 Homebrew 安装 EIM：

.. code-block:: bash

    brew tap espressif/eim

然后，通过 Homebrew 安装 EIM 的图形用户界面 (GUI) 或命令行界面 (CLI)：

- GUI：
    .. code-block:: bash

        brew install --cask eim-gui

- CLI：
    .. code-block:: bash

        brew install eim


第三步：使用 EIM 安装 ESP-IDF
==============================

.. include:: eim-install-idf.rst

.. toctree::
    :hidden:
    :maxdepth: 1
    :caption: Legacy Installation

    linux-macos-setup-legacy
