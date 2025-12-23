************************************************
在 Linux 上安装 ESP-IDF 及工具链
************************************************

:link_to_translation:`zh_CN:[中文]`

本章节将介绍如何使用 ESP-IDF 安装管理器 (EIM) 在 Linux 发行版（例如 Ubuntu）上安装 ESP-IDF 及其所需工具。

.. note::

   本章节描述了 ESP-IDF v6.0 及以上版本的默认和推荐安装方式。如使用 ESP-IDF v6.0 之前版本的默认安装方式，请参考 :doc:`Linux 上的传统安装方法 <linux-macos-setup-legacy>`。


第一步：安装依赖包（可选）
==========================

如计划通过 :ref:`APT <install-eim-linux-apt>` 安装 EIM，可跳过本步骤。

如使用其他方式安装 EIM，请根据 Linux 发行版安装相应的 `依赖包 <https://docs.espressif.com/projects/idf-im-ui/en/latest/prerequisites.html#linux>`_。

.. note::

    ESP-IDF  要求 Python 版本至少为 3.10。

    但如需使用 EIM 进行 `离线安装 <https://docs.espressif.com/projects/idf-im-ui/en/latest/offline_installation.html>`_，则必须使用 **Python 3.11 或更高版本**。


第二步：安装 EIM
=================

您可以使用以下任一方法安装 EIM：

- `通过 APT 安装（Debian 发行版）`_
- `通过 DNF 安装（RPM 发行版）`_
- `下载 EIM 安装包`_

通过 APT 或 DNF 安装 EIM 便于后续升级更新。


.. _install-eim-linux-apt:

通过 APT 安装（Debian 发行版）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

将 EIM 仓库添加到 APT 源列表，以便后续使用 APT 安装 EIM：

.. code-block:: bash

    echo "deb [trusted=yes] https://dl.espressif.com/dl/eim/apt/ stable main" | sudo tee /etc/apt/sources.list.d/espressif.list

    sudo apt update

然后，通过 APT 安装 EIM 的命令行界面 (CLI) 和图形用户界面 (GUI)，或仅安装命令行界面 (CLI)：

- GUI 和 CLI：
    .. code-block:: bash

        sudo apt install eim

- 仅 CLI：
    .. code-block:: bash

        sudo apt install eim-cli


通过 DNF 安装（RPM 发行版）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

将 EIM 仓库添加到 DNF 源列表，以便后续使用 DNF 安装 EIM：

.. code-block:: bash

    sudo tee /etc/yum.repos.d/espressif-eim.repo << 'EOF'
    [eim]
    name=ESP-IDF Installation Manager
    baseurl=https://dl.espressif.com/dl/eim/rpm/$basearch
    enabled=1
    gpgcheck=0
    EOF

然后，通过 DNF 安装 EIM 的命令行界面 (CLI) 和图形用户界面 (GUI)，或仅安装命令行界面 (CLI)：

- GUI 和 CLI：
    .. code-block:: bash

        sudo dnf install eim

- 仅 CLI：
    .. code-block:: bash

        sudo dnf install eim-cli


下载 EIM 安装包
~~~~~~~~~~~~~~~~

您也可以从 `Espressif 下载页面 <https://dl.espressif.com/dl/eim/>`__ 下载适用于 Linux 的 EIM 安装包，该页面提供了 CLI 和 GUI 版本的在线和离线安装程序。


第三步：使用 EIM 安装 ESP-IDF
=================================

.. include:: eim-install-idf.rst
