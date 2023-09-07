IDF Windows 安装程序
===========================

:link_to_translation:`en:[English]`

命令行参数
----------

IDF Windows 安装程序 ``esp-idf-tools-setup`` 提供以下命令行参数：

* ``/CONFIG=[PATH]`` - 指定 ``ini`` 配置文件的路径，覆盖安装程序的默认配置。默认值：``config.ini``。
* ``/GITCLEAN=[yes|no]`` - 在以离线模式安装时，执行 ``git clean`` 命令，并删除未跟踪的目录。默认值：``yes``。
* ``/GITRECURSIVE=[yes|no]`` - 递归克隆所有 Git 仓库子模块。默认值：``yes``。
* ``/GITREPO=[URL|PATH]`` - 指定克隆 ESP-IDF 仓库的 URL。默认值：``https://github.com/espressif/esp-idf.git``。
* ``/GITRESET=[yes|no]`` - 在安装过程中，启用或禁用对仓库的 ``git reset`` 操作。默认值：``yes``。
* ``/HELP`` - 显示 Inno Setup 安装程序提供的命令行选项。
* ``/IDFDIR=[PATH]`` - 指定安装目录的路径。默认值：``{userdesktop}\esp-idf}``。
* ``/IDFVERSION=[v4.3|v4.1|master]`` - 使用指定的 ESP-IDF 版本，如 v4.1、v4.2、master。默认值：``empty``，选取列表中的第一个版本。
* ``/IDFVERSIONSURL=[URL]`` - 使用 URL 下载 ESP-IDF 版本列表。默认值：``https://dl.espressif.com/dl/esp-idf/idf_versions.txt``。
* ``/LOG=[PATH]`` - 在指定目录中存储安装日志文件。默认值：``empty``。
* ``/OFFLINE=[yes|no]`` - 在离线模式下，使用 ``pip`` 执行。通过设置环境变量 ``PIP_NO_INDEX``，也可在离线模式下安装 Python 软件包。默认值：``no``。
* ``/USEEMBEDDEDPYTHON=[yes|no]`` - 使用嵌入式 Python 版本完成安装。将此参数设置为 ``no``，可以在安装程序中选择 Python 版本。默认值：``yes``。
* ``/PYTHONNOUSERSITE=[yes|no]`` - 在启动任意 Python 命令前，设置 ``PYTHONNOUSERSITE`` 变量，避免从 AppData\Roaming 加载 Python 软件包。默认值：``yes``。
* ``/PYTHONWHEELSURL=[URL]`` - 指定 PyPi 存储库的 URL，以解析二进制 Python Wheel 依赖关系。设置环境变量 ``PIP_EXTRA_INDEX_URL`` 可以实现相同效果。默认值：``https://dl.espressif.com/pypi``。
* ``/SKIPSYSTEMCHECK=[yes|no]`` - 跳过系统检查页面。默认值：``no``。
* ``/VERYSILENT /SUPPRESSMSGBOXES /SP- /NOCANCEL`` - 执行静默安装。

静默安装
--------

通过设置以下命令行参数，可以静默安装 ESP-IDF：

.. code-block:: batch

    esp-idf-tools-setup-x.x.exe /VERYSILENT /SUPPRESSMSGBOXES /SP- /NOCANCEL

在命令行中运行安装程序时，它会在后台启动一个独立的进程执行安装操作，而不会阻塞命令行的使用。通过以下 PowerShell 脚本可以等待安装程序完成：

.. code-block:: powershell

    esp-idf-tools-setup-x.x.exe /VERYSILENT /SUPPRESSMSGBOXES /SP- /NOCANCEL
    $InstallerProcess = Get-Process esp-idf-tools-setup
    Wait-Process -Id $InstallerProcess.id

自定义 Python 版本及 Python Wheel 位置
--------------------------------------------------

IDF 安装程序默认使用嵌入的 Python 版本，并参考预定义的 Python Wheel 镜像获取所需软件包。

以下参数支持自定义 Python 版本及 Python Wheel 位置：

.. code-block:: batch

    esp-idf-tools-setup-x.x.exe /USEEMBEDDEDPYTHON=no /PYTHONWHEELSURL=https://pypi.org/simple/
