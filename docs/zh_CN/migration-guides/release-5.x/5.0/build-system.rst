迁移构建系统至 ESP-IDF v5.0
===================================

从 GNU Make 构建系统迁移至 ESP-IDF v5.0
-----------------------------------------------

ESP-IDF v5.0 已不再支持基于 Make 的工程，请参考 :ref:`从 ESP-IDF GNU Make 构建系统迁移到 CMake 构建系统 <migrating_from_make>` 进行迁移。

更新片段文件语法
----------------------------

ESP-IDF v5.0 中将不再支持 ESP-IDF v3.x 中链接器脚本片段文件的旧式语法。在迁移的过程中需注意以下几点：

- 必须缩进，缩进不当的文件会产生解析异常；旧版本不强制缩进，但之前的文档和示例均遵循了正确的缩进语法。
- 条件改用 ``if...elif...else`` 结构，可以参照 :ref:`之前的章节<ldgen-conditional-placements>`。
- 映射片段和其他片段类型一样，需有名称。


明确指定组件依赖
-----------------------------------------

在之前的 ESP-IDF 版本中，除了 :ref:`通用组件依赖项 <component-common-requirements>`，还有一些组件总是作为公共依赖项，在构建时添加至每个组件中，如：

* ``driver``
* ``efuse``
* ``esp_timer``
* ``lwip``
* ``vfs``
* ``esp_wifi``
* ``esp_event``
* ``esp_netif``
* ``esp_eth``
* ``esp_phy``

这意味着可以直接包含这些组件的头文件，而无需在 ``idf_component_register`` 中将它们指定为依赖。此行为是由各种常见组件的传递依赖关系引起的。

在 ESP-IDF v5.0 中，此行为已修复，这些组件不再默认作为公共依赖项添加。

如果组件所依赖的某个组件不属于通用组件依赖项，则必须显式地声明此依赖关系。可以通过在组件的 ``CMakeLists.txt`` 中的 ``idf_component_register`` 调用中添加 ``REQUIRES <component_name>`` 或 ``PRIV_REQUIRES <component_name>`` 来完成。有关指定组件依赖的更多信息，请参阅 :ref:`组件依赖 <component-requirements>`。

设置 ``COMPONENT_DIRS`` 和 ``EXTRA_COMPONENT_DIRS`` 变量
-----------------------------------------------------------------

.. highlight:: cmake

为了实现构建项目时的路径能够包含空格，ESP-IDF v5.0 做了一系列改进，其中包括改进了 CMakeLists.txt 文件中的 ``COMPONENT_DIRS`` 和 ``EXTRA_COMPONENT_DIRS`` 变量。

ESP-IDF v5.0 版本中，不再支持添加不存在的目录到变量 ``COMPONENT_DIRS`` 或 ``EXTRA_COMPONENT_DIRS`` 中，否则会出现报错。

同时，ESP-IDF v5.0 中也不再支持使用字符串拼接的方式定义 ``COMPONENT_DIRS`` 或 ``EXTRA_COMPONENT_DIRS`` 变量。这些变量应该定义为 CMake 列表。 例如::

    set(EXTRA_COMPONENT_DIRS path1 path2)
    list(APPEND EXTRA_COMPONENT_DIRS path3)

不支持::

    set(EXTRA_COMPONENT_DIRS "path1 path2")
    set(EXTRA_COMPONENT_DIRS "${EXTRA_COMPONENT_DIRS} path3")

将这些变量定义为 CMake 列表的方式兼容之前的 ESP-IDF 版本。

更新 target_link_libraries 用法
---------------------------------------------------

ESP-IDF v5.0 修复了组件的 CMake 变量传播问题。此问题导致本应该只应用于某一组件的编译器标志和定义应用到了项目中的每个组件。

该修复也带来一定的副作用，从 ESP-IDF v5.0 开始，用户项目在使用 ``target_link_libraries`` 时必须明确指定 ``project_elf``，同时自定义 CMake 项目必须指定 ``PRIVATE``、``PUBLIC`` 或 ``INTERFACE`` 参数。这是一项重大变更，不兼容以前的 ESP-IDF 版本。

例如::

    target_link_libraries(${project_elf} PRIVATE "-Wl,--wrap=esp_panic_handler")

不支持::

    target_link_libraries(${project_elf} "-Wl,--wrap=esp_panic_handler")

更新 CMake 版本
-----------------------

在 ESP-IDF v5.0 中，最低 CMake 版本已更新到 3.16，并且不再支持低于 3.16 的版本。如果你的操作系统没有安装 CMake，请运行 ``tools/idf_tools.py install cmake`` 来安装合适的版本。

该变更会影响到使用系统提供的 CMake 以及自定义 CMake 的 ESP-IDF 用户。

重新定义特定目标配置文件的应用顺序
-------------------------------------------------------------------------------------------

.. highlight:: none

ESP-IDF v5.0 重新安排了特定目标配置文件和 SDKCONFIG_DEFAULTS 中所有其他文件的应用顺序。现在，特定目标的配置文件将在引入它的文件之后、在 SDKCONFIG_DEFAULTS 中后续的其他文件之前应用。

例如::

    如果 ``SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig_devkit1"``，且同一文件夹内有 ``sdkconfig.defaults.esp32`` 文件，那么文件的应用顺序为：(1) sdkconfig.defaults (2) sdkconfig.defaults.esp32 (3) sdkconfig_devkit1

如果某个键在不同的特定目标配置文件中有不同的值，那么后者的值会覆盖前者。例如在以上案例中，如果某个键在 ``sdkconfig.defaults.esp32`` 和 ``sdkconfig_devkit1`` 中的值不同，则在 ``sdkconfig_devkit1`` 中的值会覆盖在 ``sdkconfig.defaults.esp32`` 中的值。

如果确实需要设置特定目标的配置值，请将其放到后应用的特定目标文件中，如 ``sdkconfig_devkit1.esp32``。
