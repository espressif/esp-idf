可重复构建
==========

:link_to_translation:`en:[English]`

简介
----

ESP-IDF 构建系统支持 `可重复构建 <https://reproducible-builds.org/docs/definition/>`_。

启用可重复构建后，使用 ESP-IDF 构建的应用程序不再依赖构建环境。即使以下变量发生变化，应用程序的 ``.elf`` 文件和 ``.bin`` 文件仍然保持不变：

- 项目所在目录
- ESP-IDF 所在目录 (``IDF_PATH``)
- 构建时间
- 工具链安装路径

构建不可重复的原因
------------------

即便使用相同的源代码和工具版本，以下几个原因依旧可能导致应用程序依赖构建环境：

- 在 C 代码中， ``__FILE__`` 预处理器宏展开为源文件的完整路径。
- ``__DATE__`` 和 ``__TIME__`` 预处理器宏展开为编译日期和时间。
- 当编译器生成目标文件时，会添加调试信息。这些信息能帮助 GDB 等调试器定位与机器代码中特定位置相对应的源代码，且通常包含相关源文件的路径。这些路径可能是绝对路径，并且包括前往 ESP-IDF 或项目的路径。

此外，输入顺序的不稳定性、构建系统的不确定性等因素也可能导致应用程序依赖构建环境。

在 ESP-IDF 中启用可重复构建
---------------------------

启用 :ref:`CONFIG_APP_REPRODUCIBLE_BUILD` 选项，可在 ESP-IDF 中实现可重复构建。

此选项默认处于禁用状态，可在 ``menuconfig`` 中启用。

也可以将该选项添加到 ``sdkconfig.defaults`` 中，并删除 ``sdkconfig`` 文件、重新运行构建。更多信息，请参阅 :ref:`custom-sdkconfig-defaults`。

如何实现可重复构建
------------------

ESP-IDF 可通过以下方式实现可重复构建：

- 在启用可重复构建时，ESP-IDF 的源代码不使用 ``__DATE__`` 和 ``__TIME__`` 宏。请注意，如果应用程序源代码使用了这些宏，则构建将无法重复。
- ESP-IDF 构建系统传递一组 ``-fmacro-prefix-map`` 和 ``-fdebug-prefix-map`` 标志，并用占位符替换基本路径：

    - 替换 ESP-IDF 的路径为 ``/IDF``
    - 替换项目路径为 ``/IDF_PROJECT``
    - 替换构建目录的路径为 ``/IDF_BUILD``
    - 替换组件路径为 ``/COMPONENT_NAME_DIR`` （其中 ``NAME`` 指的是组件的名称）
    - 替换工具链的路径为 ``/TOOLCHAIN``

- 如果启用 :ref:`CONFIG_APP_REPRODUCIBLE_BUILD`，则不会将构建日期和时间包括在 :ref:`应用程序元数据结构 <app-image-format-application-description>` 和 :ref:`引导加载程序元数据结构 <image-format-bootloader-description>` 中。
- ESP-IDF 构建系统在将源文件列表、组件列表和其他序列传递给 CMake 之前会对其进行排序。构建系统的其他各个部分，如链接器脚本生成器，也会先排序，从而确保无论环境如何，输出都一致。

可重复构建与调试
----------------

如前文所述，启用可重复构建时，调试信息部分中包含的文件名会被更改。因此，调试器 (GDB) 无法定位给定代码位置的源文件。

可以使用 GDB 的 ``set substitute-path`` 命令来解决这个问题。例如，添加以下命令到 GDB 初始化脚本中，可以将更改后的路径恢复为原始路径。

.. code-block:: none

    set substitute-path /COMPONENT_FREERTOS_DIR /home/user/esp/esp-idf/components/freertos

ESP-IDF 构建系统在构建过程中会自动生成一个包含 ``set substitute-path`` 命令列表的文件。该文件名为 ``prefix_map_gdbinit``，位于项目 ``build`` 目录中。

当使用 :ref:`idf.py gdb <jtag-debugging-with-idf-py>` 开始调试时，此额外的 ``gdbinit`` 文件会自动传递给 GDB。当手动启动 GDB 或从 IDE 启动 GDB 时，请使用 ``-x build/prefix_map_gdbinit`` 参数将此额外的 ``gdbinit`` 脚本传递给 GDB。

仍可能影响可重复构建的因素
--------------------------

请注意，构建应用程序还依赖于：

- ESP-IDF 版本
- 构建工具 (CMake, Ninja) 及交叉编译器的版本

:doc:`tools/idf-docker-image` 可避免这些因素影响构建。
