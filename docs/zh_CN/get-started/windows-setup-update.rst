*************************************************
在 Windows 环境下更新 ESP-IDF 工具
*************************************************

.. _get-started-install_bat-windows:

使用脚本安装 ESP-IDF 工具
====================================

请从 Windows “命令提示符”窗口，切换至 ESP-IDF 的安装目录。然后运行::

	install.bat

对于 Powershell，请切换至 ESP-IDF 的安装目录。然后运行::

	install.ps1

该命令可下载安装 ESP-IDF 所需的工具。如您已经安装了某个版本的工具，则该命令将无效。

该工具的下载安装位置由 ESP-IDF 工具安装器的设置决定，默认情况下为： ``C:\Users\username\.espressif``。

.. _get-started-export_bat-windows:

使用“导出脚本”将 ESP-IDF 工具添加至 PATH
=================================================================================

ESP-IDF 工具安装器将在“开始菜单”为 “ESP-IDF 命令提示符” 创建快捷方式。点击该快捷方式可打开 Windows 命令提示符窗口，您可在该窗口使用所有已安装的工具。

有些情况下，您正在使用的 ESP-IDF 版本可能并未创建命令提示符快捷方式，此时您可以根据下方步骤将 ESP-IDF 工具添加至 PATH。

首先，请打开需要使用 ESP-IDF 的命令提示符窗口，切换至 ESP-IDF 的安装路径，然后执行 ``export.bat``::

	cd %userprofile%\esp\esp-idf
	export.bat

对于 Powershell 用户，请同样切换至 ESP-IDF 的安装路径，然后执行 ``export.ps1``::

	cd ~/esp/esp-idf
	export.ps1

运行完成后，您就可以通过命令提示符使用 ESP-IDF 工具了。
