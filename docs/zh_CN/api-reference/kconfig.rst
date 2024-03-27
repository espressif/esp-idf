项目配置
********
:link_to_translation:`en:[English]`

简介
=====

ESP-IDF 使用基于 kconfiglib_ 的 esp-idf-kconfig_ 包，而 kconfiglib_ 是 Kconfig_ 系统的 Python 扩展。Kconfig 提供了编译时的项目配置机制，以及多种类型的配置选项（如整数、字符串和布尔值等）。Kconfig 文件指定了选项之间的依赖关系、默认值、组合方式等。

了解所有可用功能，请查看 Kconfig_ 和 `kconfiglib 扩展`_。

.. _project-configuration-menu:

项目配置菜单
============

应用程序开发人员可以通过 ``idf.py menuconfig`` 构建目标，在终端中打开项目配置菜单。

更新后，此配置将保存在项目根目录的 ``sdkconfig`` 文件中。借助 ``sdkconfig``，应用程序构建目标将在构建目录中生成 ``sdkconfig.h`` 文件，并使得 ``sdkconfig`` 选项可用于项目构建系统和源文件。

使用 sdkconfig.defaults
==============================

在某些情况下，例如 ``sdkconfig`` 文件处于版本控制状态时，构建系统可能会不便于更改 ``sdkconfig`` 文件。要避免上述情况，可以在构建系统中创建 ``sdkconfig.defaults`` 文件。该文件可以手动或自动创建，且构建系统永远不会对其进行更改。该文件包含所有不同于默认选项的重要选项，其格式与 ``sdkconfig`` 文件格式相同。如果用户记得所有已更改的配置，则可以手动创建 ``sdkconfig.defaults``，或运行 ``idf.py save-defconfig`` 命令来自动生成此文件。

``sdkconfig.defaults`` 创建后，用户可以删除 ``sdkconfig`` 或将其添加到版本控制系统的忽略列表中（例如 ``git`` 的 ``.gitignore`` 文件）。项目构建目标将自动创建 ``sdkconfig`` 文件，填充 ``sdkconfig.defaults`` 文件中的设置，并将其他设置配置为默认值。请注意，构建时 ``sdkconfig.defaults`` 中的设置不会覆盖 ``sdkconfig`` 的已有设置。了解更多信息，请查看 :ref:`custom-sdkconfig-defaults`。

Kconfig 格式规定
====================

Kconfig 文件的格式规定如下：

- 在所有菜单中，选项名称的前缀需保持一致。目前，前缀长度应为至少 3 个字符。
- 每级采用 4 个空格的缩进方式，子项需比父项多缩进一级。例如， ``menu`` 缩进 0 个空格，``menu`` 中的 ``config`` 则缩进 4 个空格， ``config`` 中的 ``help`` 缩进 8 个空格， ``help`` 下的文本缩进 12 个空格。
- 行末不得出现尾随空格。
- 选项最长为 50 个字符。
- 每行最长为 120 个字符。

.. note::

    菜单中不同配置的 ``help`` 小节默认视为 reStructuredText 格式，以便生成相应选项的参考文档。

格式检查器
--------------

esp-idf-kconfig_ 软件包中的 ``kconfcheck`` 工具可以检查 Kconfig 文件是否符合上述格式规定。检查器会检查作为参数给出的所有 Kconfig 和 ``Kconfig.projbuild`` 文件，并生成一个后缀为 ``.new`` 的新文件，如有格式错误，便会在此文件中提供修改建议。注意，检查器不能解决所有格式问题，开发人员仍需终审并修改文件，使其通过测试。例如，在没有其他误导性格式的情况下，检查器能够更正缩进，但无法为菜单内选项提供常用的前缀。

``esp-idf-kconfig`` 软件包可以在 ESP-IDF 环境中使用。运行命令 ``python -m kconfcheck <path_to_kconfig_file>`` 即可调用检查工具。

如需了解更多内容，请参考 `esp-idf-kconfig 相关文档 <https://github.com/espressif/esp-idf-kconfig/blob/master/docs/DOCUMENTATION.md>`__。

.. _configuration-options-compatibility:

Kconfig 选项的向后兼容性
================================

标准 Kconfig_ 工具会忽略 ``sdkconfig`` 中的未知选项。因此，如果开发人员对某些选项进行了自定义设置，但这些选项在 ESP-IDF 新版本中重新命名，标准 Kconfig_ 工具将忽略原有设置。以下功能可以避免上述情况：

1. 工具链使用 ``kconfgen`` 预处理 ``sdkconfig`` 文件。例如， ``menuconfig`` 会读取这些文件，从而保留旧选项设置。
2. ``kconfgen`` 递归查找 ESP-IDF 目录中所有包含新旧 Kconfig 选项名称的 ``sdkconfig.rename`` 文件。在 ``sdkconfig`` 文件中，新选项将替换旧选项。针对单个目标的重命名可以放在特定目标的重命名文件 ``sdkconfig.rename.TARGET`` 中，其中 ``TARGET`` 是目标名称，例如 ``sdkconfig.rename.esp32s2``。
3. ``kconfgen`` 通过添加兼容性语句列表（即经过修改后，将旧选项的值设置为新选项的值），后处理 ``sdkconfig`` 文件，并生成所有构建结果（ ``sdkconfig.h``、 ``sdkconfig.cmake`` 以及 ``auto.conf``）。如果用户在其代码中仍然使用旧选项，此举可以防止用户代码出现问题。
4. ``kconfgen`` 会自动生成 :ref:`configuration-deprecated-options`。

.. _configuration-options-reference:

配置选项参考
==================

以下小节包含由 Kconfig 文件自动生成的 ESP-IDF 可用选项列表。请注意，由于所选选项不同，下列某些选项可能在 ``menuconfig`` 界面中默认不可见。

按照惯例，所有选项名称均为大写字母加下划线。当 Kconfig 生成 ``sdkconfig`` 和 ``sdkconfig.h`` 文件时，选项名称会以 ``CONFIG_`` 为前缀。因此，如果 Kconfig 文件定义了 ``ENABLE_FOO`` 选项且 ``menuconfig`` 中选择了该选项，则 ``sdkconfig`` 和 ``sdkconfig.h`` 文件也将定义 ``CONFIG_ENABLE_FOO``。在以下小节中，选项名称也以 ``CONFIG_`` 为前缀，与源代码相同。


.. include-build-file:: inc/kconfig.inc

.. _Kconfig: https://www.kernel.org/doc/Documentation/kbuild/kconfig-language.txt
.. _esp-idf-kconfig: https://pypi.org/project/esp-idf-kconfig/
.. _kconfiglib: https://github.com/ulfalizer/Kconfiglib
.. _kconfiglib 扩展: https://pypi.org/project/kconfiglib/#kconfig-extensions
