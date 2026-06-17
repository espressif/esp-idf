************************************************
在 Windows 上安装 ESP-IDF 及工具链
************************************************

:link_to_translation:`en:[English]`

本章节将介绍如何使用 ESP-IDF 安装管理器 (EIM) 在 Windows 上安装 ESP-IDF 及其所需工具。

.. note::

   本章节描述了 ESP-IDF v6.0 及以上版本的默认和推荐安装方式。ESP-IDF 仍支持 :doc:`Windows 上更新 ESP-IDF 的传统方法 <windows-setup-update-legacy>`。


第一步：安装依赖包（可选）
==========================

安装 ESP-IDF 时，EIM 会自动检查所需的依赖包。如果检查到依赖包缺失，EIM 会在用户确认后自动安装缺失的依赖包。

如果自动安装失败，可以手动安装以下依赖包：

- `Git <https://git-scm.com/install/windows>`_
- `Python <https://www.python.org/downloads/windows/>`_

  .. note::

    Python 3.10 是 ESP-IDF 支持的最低版本。

    EIM 所需的 Python 版本，请参考 `EIM 文档 <https://docs.espressif.com/projects/idf-im-ui/en/latest/prerequisites.html#python-version>`_。


第二步：安装 EIM
=================

您可以使用以下任一方法安装 EIM：

- 下载 EIM

  .. figure:: ../../_static/get-started-eim-download.drawio.png
     :alt: EIM 下载页面
     :align: left
     :width: 35%
     :target: https://dl.espressif.com/dl/eim/

  可选择下载在线或离线安装程序的图形用户界面 (GUI) 或命令行界面 (CLI) 版本。

- 使用 `WinGet 包管理器 <https://learn.microsoft.com/zh-cn/windows/package-manager/>`_ 安装 EIM

  - 安装图形用户界面 (GUI) 或命令行界面 (CLI)：

    - GUI:

      .. code-block:: bash

          winget install Espressif.EIM
    - CLI:

      .. code-block:: bash

          winget install Espressif.EIM-CLI

  - 通过运行单个命令，轻松升级更新 EIM。


第三步：使用 EIM 安装 ESP-IDF
==============================

.. include:: eim-install-idf.rst

.. toctree::
    :hidden:
    :maxdepth: 1
    :caption: Legacy Installation

    windows-setup-update-legacy
