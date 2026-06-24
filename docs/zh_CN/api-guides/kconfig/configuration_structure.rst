.. _configuration-structure:

配置文件的结构与关系
======================================

.. note::

    本文主要关注配置文件的结构。关于项目配置的更多信息，请参考 :ref:`Project Configuration Guide <project-configuration-guide>`。关于组件配置，请参考 :ref:`Component Configuration Guide <component-configuration-guide>`。

ESP-IDF 使用 `Kconfig language <https://docs.espressif.com/projects/esp-idf-kconfig/en/latest/kconfiglib/language.html>`_ 来配置项目。配置由配置选项（例如 ``CONFIG_IDF_TARGET``）及其取值（例如 ``esp32``）组成。每个配置选项在被写入例如 ``sdkconfig`` 文件时都会带上 ``CONFIG_`` 前缀，以便将其与其他变量（例如环境变量）区分开来。

在 ESP-IDF 的上下文中，配置由若干文件构成，其中最重要的是：

- ``Kconfig`` 和 ``Kconfig.projbuild`` 文件，用于定义配置选项及其相互关系，并给出其默认值（如果有）。
- ``sdkconfig`` 文件，包含当前使用的配置选项取值。
- ``sdkconfig.defaults`` 文件，包含用户为配置选项定义的默认值。
- ``sdkconfig.rename`` 文件，包含用于确保向后兼容的 ``OLD_NAME NEW_NAME`` 配对的配置名。该文件主要由组件或 ESP-IDF 开发者使用。

配置文件可分为两组：一组主要用于 **定义** 配置选项，另一组包含这些选项的 **取值**。第一组包括 ``Kconfig``、``Kconfig.projbuild`` 和 ``sdkconfig.rename`` 文件，第二组包括 ``sdkconfig``、``sdkconfig.defaults``、``sdkconfig.h`` 和 ``sdkconfig.cmake`` 文件。下面各节将对所有文件进行说明。

关于 ESP-IDF 中配置系统的更多信息，请参考 :ref:`Configuration Overview <configuration-overview>`。

.. _kconfig-files:

Kconfig 与 Kconfig.projbuild 文件
-----------------------------------

``Kconfig.*`` 文件存储配置选项及其属性与相互关系，也可能包含这些配置选项的默认值。每个项目都可以有自己的 ``Kconfig`` 和（或） ``Kconfig.projbuild`` 文件，用于为该项目定义配置选项。

``Kconfig`` 与 ``Kconfig.projbuild`` 文件之间唯一的区别在于其内容出现在配置界面 (menuconfig) 中的位置：

- ``Kconfig``：该文件的内容会出现在配置界面的 ``Component config`` 窗口中。
- ``Kconfig.projbuild``：该文件的内容会出现在配置界面的根窗口中。

``Kconfig`` 文件示例：

.. code-block:: kconfig

    mainmenu "Motors configuration"

        config SUBLIGHT_DRIVE_ENABLED
            bool "Enable sublight drive"
            default y
            help
                This option enables sublight on our spaceship.

关于 Kconfig 语言的更多信息，请参考 `Kconfig Documentation <https://docs.espressif.com/projects/esp-idf-kconfig/en/latest/kconfiglib/language.html>`_。

.. _sdkconfig-file:

sdkconfig 与 sdkconfig.old
^^^^^^^^^^^^^^^^^^^^^^^^^^^

在 ``sdkconfig`` 文件中，存储着 **当前分配** 给配置选项的 **取值**。该文件自动生成且不应手动编辑，因为配置选项之间可能存在相互关系（依赖与反向依赖），手动编辑会破坏这些关系。``sdkconfig`` 文件同时包含用户设定值与默认值，因此提供了所有可用配置选项及其当前取值的完整列表。

每一行符合下列模式之一：

- ``CONFIG_NAME=<value>``：配置名及其取值。
- ``# CONFIG_NAME is not set``：布尔配置 ``CONFIG_NAME`` 可见，但当前值为 n。对于非布尔配置，则会出现 ``CONFIG_NAME=""``。
- 其他以 # 开头的注释与空行。

``sdkconfig.old`` 文件是之前配置的备份。每次生成 ``sdkconfig`` 文件时都会生成该备份文件。

.. note::

    项目中还包含其他 sdkconfig.* 文件，即 ``sdkconfig.h``、``sdkconfig.cmake``、``sdkconfig.json``。这些文件与上面提到的 ``sdkconfig`` 文件内容相同，但格式不同。它们分别用于对应的工具（C/C++ 代码、CMake）。更多信息可查看 :ref:`configs-in-C-Cmake`。

.. _sdkconfig-rename-file:

sdkconfig.rename 与 sdkconfig.rename.<chip>
----------------------------------------------------

``sdkconfig.rename`` 文件由构建系统用于确保向后兼容。这些文件由组件或 ESP-IDF 开发者创建并维护，应用开发者一般无需编辑。

``sdkconfig.rename`` 文件的结构如下：

* 以 ``#`` 开头的行和空行会被忽略。
* 其他所有行应符合以下格式之一：
    * ``CONFIG_DEPRECATED_NAME CONFIG_NEW_NAME``，其中 ``CONFIG_DEPRECATED_NAME`` 是旧版配置项名称，在较新版本的 ESP-IDF 中已被重命名为 ``CONFIG_NEW_NAME``。
    * ``CONFIG_DEPRECATED_NAME !CONFIG_NEW_INVERTED_NAME``，其中 ``CONFIG_NEW_INVERTED_NAME`` 是在较新版本的 ESP-IDF 中引入的配置项，其值由 ``CONFIG_DEPRECATED_NAME`` 的逻辑值取反得到。

如果同一弃用选项名存在多个映射（即同一弃用选项名被多次重命名），则以最后一次出现的映射为准。只有将配置报告详细级别设置为 ``verbose`` （例如通过 ``KCONFIG_REPORT_VERBOSITY`` 环境变量）时，配置系统才会报告该情况。

该文件的主要用途是在较新的 ESP-IDF 版本更改配置名时，确保向后兼容。

示例：

``sdkconfig.rename``：

.. code-block:: kconfig

    # 旧名称            新名称
    CONFIG_WARP_DRIVE     CONFIG_HYPERDRIVE
    CONFIG_ENABLE_WARP_DRIVE !CONFIG_DISABLE_HYPERDRIVE

``sdkconfig``：

.. code-block:: kconfig

    (...)
    CONFIG_HYPERDRIVE=y
    CONFIG_DISABLE_HYPERDRIVE=n
    (...)
    # 为向后兼容而保留的弃用选项
    CONFIG_WARP_DRIVE=y
    CONFIG_ENABLE_WARP_DRIVE=y
    # 弃用选项结束

.. _sdkconfig-defaults-file:

sdkconfig.defaults 与 sdkconfig.defaults.<chip>
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Kconfig 语言提供了为配置设置默认值的方式：在 ``Kconfig`` 文件中使用 ``default`` 选项。然而，输入的 ``Kconfig`` 文件可能位于不同的项目中、处于版本控制之下，或出于其他原因不便直接编辑。在这种情况下，可以使用 ``sdkconfig.defaults`` 文件。该文件的结构与 ``sdkconfig`` 文件相同；每一行都是完整的配置名（包含 ``CONFIG_`` 前缀）及其取值。通过 ``default`` 选项设置的默认值会被此处的取值覆盖。

也可以只对特定目标覆盖默认值。在这种情况下，可以创建 ``sdkconfig.defaults.<chip>`` 文件，其中 ``<chip>`` 为目标名称（例如 ``esp32s2``）。此时也必须同时提供 ``sdkconfig.defaults`` 文件，否则 ``sdkconfig.defaults.<chip>`` 文件会被忽略。不过，``sdkconfig.defaults`` 文件可以为空。

如何生成 ``sdkconfig.defaults`` 文件：

1. ``cd`` 进入项目文件夹。
2. 在 ``idf.py menuconfig`` 中完成所需配置。
3. 运行 ``idf.py save-defconfig``。这将生成 ``sdkconfig.defaults`` 文件，内容为所有与默认值不同的取值。

.. note::

    ``sdkconfig`` 文件中的用户设定值优先于 ``sdkconfig.defaults`` 文件。换言之，如果用户更改了某个同样在 ``sdkconfig.defaults`` 文件中设置过的配置选项取值，则会忽略 ``sdkconfig.defaults`` 文件中的取值：

    ``sdkconfig.defaults``

    .. code-block:: kconfig

        CONFIG_SUBLIGHT_SPEED=42

    ``sdkconfig``

    .. code-block:: kconfig

        # 用户更改了该值（例如通过 menuconfig 修改） -> 将忽略来自 sdkconfig.defaults 的取值
        CONFIG_SUBLIGHT_SPEED=10

也可以通过设置环境变量来覆盖该文件名。关于如何自定义文件名，以及当存在多个默认值文件时的处理顺序，请参阅构建系统文档中的 :ref:`Custom Sdkconfig Defaults <custom-sdkconfig-defaults>` 章节。

示例：

``Kconfig``：

.. code-block:: kconfig

    (...)
    config SUBLIGHT_SPEED
        int "Sublight speed"
        default 10
    (...)

``sdkconfig.defaults``：

.. code-block:: text

    CONFIG_SUBLIGHT_SPEED=42

当运行 ``idf.py menuconfig`` 等命令时，``SUBLIGHT_SPEED`` 将被设置为 42。如果在图形界面中更改了该值，则会使用图形界面中的值并将其保存到 ``sdkconfig`` 文件中。

sdkconfig.ci
^^^^^^^^^^^^

部分 IDF 示例包含 ``sdkconfig.ci`` 文件。它是持续集成 (CI) 测试框架的一部分，在正常构建过程中会被忽略。
