************************************************
在 Windows 上安装 ESP-IDF 及工具链
************************************************

:link_to_translation:`zh_CN:[中文]`

本章节将介绍如何使用 ESP-IDF 安装管理器 (EIM) 在 Windows 上安装 ESP-IDF 及其所需工具。

.. note::

   本章节描述了 ESP-IDF v6.0 及以上版本的默认和推荐安装方式。ESP-IDF 仍支持 :doc:`Windows 上更新 ESP-IDF 的传统方法 <windows-setup-update-legacy>`。


第一步：安装 EIM
=================

通过 `WinGet <https://learn.microsoft.com/en-us/windows/package-manager/winget/>`__ 安装 EIM 的图形用户界面 (GUI) 或命令行界面 (CLI)：

- GUI:
    .. code-block:: bash

        winget install Espressif.EIM

- CLI:
    .. code-block:: bash

        winget install Espressif.EIM-CLI

.. note::

   通过 WinGet 安装 EIM 便于后续升级更新。

   您也可以从 `Espressif 下载页面 <https://dl.espressif.com/dl/eim/>`__ 下载适用于 Windows 的 EIM 安装包，该页面提供了 CLI 和 GUI 版本的在线和离线安装程序。


第二步：使用 EIM 安装 ESP-IDF
==============================

.. include:: eim-install-idf.rst

.. toctree::
    :hidden:
    :maxdepth: 1
    :caption: Legacy Installation

    windows-setup-update-legacy
