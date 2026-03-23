************************************************
在 Linux 上安装 ESP-IDF 及工具链
************************************************

:link_to_translation:`en:[English]`

本章节将介绍如何使用 ESP-IDF 安装管理器 (EIM) 在 Linux 发行版（例如 Ubuntu）上安装 ESP-IDF 及其所需工具。

.. note::

   本章节描述了 ESP-IDF v6.0 及以上版本的默认和推荐安装方式。如使用 ESP-IDF v6.0 之前版本的默认安装方式，请参考 :doc:`Linux 上的传统安装方法 <linux-macos-setup-legacy>`。


第一步：安装依赖包（可选）
==========================

如计划通过 :ref:`APT <install-eim-linux-apt>` 安装 EIM，可跳过本步骤。

如使用其他方式安装 EIM，请根据 Linux 发行版安装相应的 `依赖包 <https://docs.espressif.com/projects/idf-im-ui/en/latest/prerequisites.html#linux>`_。

.. note::

    ESP-IDF  要求 Python 版本至少为 3.10。

    EIM 所需的 Python 版本，请参考 `EIM 文档 <https://docs.espressif.com/projects/idf-im-ui/en/latest/prerequisites.html#python-version>`_。


第二步：安装 EIM
=================

您可以使用以下任一方法安装 EIM：

- `下载 EIM`_
- `通过 APT 安装（Debian 发行版）`_
- `通过 DNF 安装（RPM 发行版）`_
- `通过 Homebrew 安装（Linux）`_

通过 APT 或 DNF 安装 EIM 便于后续升级更新。


下载 EIM
~~~~~~~~~

.. figure:: ../../_static/get-started-eim-download.drawio.png
   :alt: EIM 下载页面
   :align: left
   :width: 35%
   :target: https://dl.espressif.com/dl/eim/

可选择下载在线或离线安装程序的图形用户界面 (GUI) 或命令行界面 (CLI) 版本。


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


.. _install-eim-linux-homebrew:

通过 Homebrew 安装（Linux）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

EIM 也可以使用 `Homebrew <https://brew.sh/>`_ 在 Linux 上安装。此方法适用于大多数 Linux 发行版，如果您更喜欢 Homebrew 而不是系统包管理器，这是一个很好的替代方案。

首先，如果您尚未安装 Homebrew，请安装它：

.. code-block:: bash

    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

然后添加 Espressif tap 并安装 EIM：

.. code-block:: bash

    # 添加 Espressif tap
    brew tap espressif/eim

    # 仅安装 CLI
    brew install eim

    # 或安装 GUI（含 CLI）
    brew install --cask eim-gui

稍后更新 EIM：

.. code-block:: bash

    brew upgrade eim
    # 或
    brew upgrade --cask eim-gui

.. note::

    GUI 版本需要图形环境，可能还有其他取决于您 Linux 发行版的依赖项。


第三步：使用 EIM 安装 ESP-IDF
=================================

.. include:: eim-install-idf.rst
