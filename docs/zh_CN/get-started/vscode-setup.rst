********************************
VS Code IDE 快速入门
********************************

:link_to_translation:`en:[English]`

我们支持 VS Code，并且致力于为所有与 ESP-IDF 相关的操作提供完善的端到端支持，包括构建、烧录、监控、调试、追踪、core-dump、以及系统追踪查看器等操作。

快速安装指南
===============

推荐您从 `VS Code 插件市场 <https://marketplace.visualstudio.com/items?itemName=espressif.esp-idf-extension>`_ 中下载 ESP-IDF VS Code 插件，或根据 `快速安装指南 <https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/install.md>`_ 安装 ESP-IDF VS Code 插件。

查看 ESP-IDF VS Code 插件 `教程 <https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/toc.md>` 了解如何使用所有功能。

支持如下功能
==================

* **安装程序**：帮助您迅速安装 ESP-IDF 及其相关工具链。
* **构建**：通过一键构建和多目标构建，轻松构建并部署您的应用程序。
* **烧录**：UART 和 JTAG 均可完成烧录。
* **监控**：内置终端带有监控功能，您可以在 VS Code 中启用 IDF 监控命令，操作方法和传统终端一样。
* `调试 <https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/debugging.md>`：提供立即可用的硬件调试功能，同时支持事后剖析调试如 core-dump 功能，分析 bug 更加方便。
* **GUI 菜单配置**：提供简化的用户界面，用于配置您的芯片。
* **应用程序追踪 & 堆追踪**：支持从应用程序中收集跟踪，并提供简化的用户界面分析跟踪。
* **系统视图查看器**：读取并显示 *.svdat* 文件到用户追踪界面，同时支持多个内核追踪视图。
* **IDF 二进制大小分析**：为分析二进制文件大小提供用户界面。
* `Rainmaker Cloud <https://rainmaker.espressif.com/>`_：我们有内置的 Rainmaker Cloud 支持，您可以轻松编辑/读取连接的物联网设备的状态。
* **代码覆盖**：我们有内置的代码覆盖支持，将用颜色突出显示已经覆盖的行。我们也会在 IDE 内部直接渲染现有的 HTML 报告。


Bugs 问题 & 功能请求
=======================

如果您在使用 VS Code 或其某些功能上遇到问题，建议您在 `论坛 <http://esp32.com/viewforum.php?f=40>`_ 或是 `github <https://github.com/espressif/vscode-esp-idf-extension/issues/new/choose>`__ 上提出您的问题，我们开发团队会对问题进行解答。

我们也欢迎您提出新的功能需求，正是由于用户要求新功能或是建议对现有功能进行改善，才成就我们今天所具备的大多数功能。欢迎您在 `github <http://github.com/espressif/vscode-esp-idf-extension/issues/new/choose>`__ 上提出功能请求。
