*************************
IDF 组件管理器
*************************

:link_to_translation:`en:[English]`

IDF 组件管理器工具用于下载 ESP-IDF CMake 项目的依赖项，该下载在 CMake 运行期间自动完成。IDF 组件管理器可以从 `组件注册表 <https://components.espressif.com>`__ 或 Git 仓库获取组件。

要获取组件列表，请参阅 `<https://components.espressif.com/>`__.

有关 IDF 组件管理器的详细信息，请参阅 `IDF 组件管理器及 ESP 组件注册表文档 <https://docs.espressif.com/projects/idf-component-manager/en/latest/>`__。

在项目中使用 IDF 组件管理器
===================================

项目中各组件的依赖项定义在单独的清单文件中，命名为 ``idf_component.yml``，位于组件根目录。运行 ``idf.py create-manifest`` 可以为组件创建清单文件模板。默认情况下将为 main 组件创建清单文件。使用 ``--path`` 选项，可以显式指定创建清单文件的目录路径。使用 ``--component=my_component`` 选项可以指定组件名称，这样系统将会在 ``components`` 文件夹下为该组件创建清单文件。``create-manifest`` 命令支持以下运行方式：

- ``idf.py create-manifest`` 为 main 组件创建清单文件
- ``idf.py create-manifest --component=my_component`` 在 ``components`` 目录下，为组件 **my_component** 创建清单文件
- ``idf.py create-manifest --path="../../my_component"`` 在 ``my_component`` 目录下，为组件 **my_component** 创建清单文件

在向项目的某个组件添加新的清单时，必须先运行 ``idf.py reconfigure``，手动重新配置项目。随后，构建过程将跟踪 ``idf_component.yml`` 清单的变更，并在必要时自动触发 CMake。

要为 ESP-IDF 项目中的组件（如 ``my_component``）添加依赖项，可以运行命令 ``idf.py add-dependency DEPENDENCY``。``DEPENDENCY`` 参数代表一个由 IDF 组件管理器管理的额外组件，而 ``my_component`` 也依赖于这个组件。``DEPENDENCY`` 参数的格式为 ``namespace/name=1.0.0``，`namespace/name` 代表组件名称，`=1.0.0` 是组件的版本范围，详情请参阅 `版本文档 <https://docs.espressif.com/projects/idf-component-manager/en/latest/reference/versioning.html>`__。默认情况下，依赖项会添加到 main 组件。通过使用 ``--path`` 选项，可以显式指定包含清单的目录，也可以使用 ``--component=my_component``，在 ``components`` 文件夹中指定组件。``add-dependency`` 命令支持以下运行方式：

- ``idf.py add-dependency example/cmp`` 为 main 组件添加依赖项，依赖项为 ``example/cmp`` 的最新版本
- ``idf.py add-dependency --component=my_component example/cmp<=3.3.3`` 将依赖项添加到位于 ``components`` 目录下名为 ``my_component`` 的组件中，依赖项为版本号 ``<=3.3.3`` 的 ``example/cmp``
- ``idf.py add-dependency --path="../../my_component" example/cmp^3.3.3`` 将依赖项添加到位于目录 ``my_component`` 下名为 ``my_component`` 的组件中，依赖项为版本号 ``^3.3.3`` 的 ``example/cmp``

.. note::

    ``add-dependency`` 命令会从 `乐鑫组件注册表 <https://components.espressif.com/>`__ 将依赖项显式添加到你的项目中。

要更新 ESP-IDF 项目的依赖项，请运行命令 ``idf.py update-dependencies``。你也可以使用 ``--project-dir PATH`` 选项，指定项目目录的路径。

应用程序示例 :example:`build_system/cmake/component_manager` 使用了由组件管理器安装的组件。

对于不需要受管理依赖项的组件，则无需提供清单文件。

在 CMake 配置项目（如 ``idf.py reconfigure``）时，组件管理器会执行以下操作：

- 处理项目中每个组件的 ``idf_component.yml`` 清单，并递归解析依赖项。
- 在项目根目录中创建 ``dependencies.lock`` 文件，包含完整的依赖项列表。
- 将所有依赖项下载至 ``managed_components`` 目录。

请勿更改 ``dependencies.lock`` 锁文件和 ``managed_components`` 目录的内容。组件管理器运行时，会始终确保这些文件处于最新状态。如果意外修改了这些文件，可以通过使用 ``idf.py reconfigure`` 触发 CMake，重新运行组件管理器。

设置构建属性 ``DEPENDENCIES_LOCK`` 可以指定顶层 CMakeLists.txt 文件中的锁文件路径。例如，在 ``project(PROJECT_NAME)`` 前添加 ``idf_build_set_property(DEPENDENCIES_LOCK dependencies.lock.${IDF_TARGET})``，可以为不同目标生成不同锁文件。

从示例创建项目
================================

组件注册表中，部分组件包含示例项目。要从示例创建一个新项目，可以运行命令 ``idf.py create-project-from-example EXAMPLE``。``EXAMPLE`` 参数格式为 ``namespace/name=1.0.0:example``，`namespace/name` 代表组件名称，`=1.0.0` 是组件的版本范围（详情请参阅 `版本文档 <https://docs.espressif.com/projects/idf-component-manager/en/latest/reference/versioning.html>`__），而 `example` 代表示例名称。在 `乐鑫组件注册表 <https://components.espressif.com/>`__ 中，可以找到各组件的示例列表，以及启动组件示例的相应命令。

在清单文件中定义依赖项
=====================================

通过在文本编辑器直接编辑，你可以轻松定义清单文件 ``idf_component.yml`` 中的依赖项。以下是有关定义依赖项的简单示例：

你可以通过指定组件名称和版本范围，定义来自注册表的依赖项：

.. code-block:: yaml

    dependencies:
      # 定义来自注册表 (https://components.espressif.com/component/example/cmp) 的依赖项
      example/cmp: ">=1.0.0"

要从 Git 仓库定义依赖关系，请提供组件在仓库中的路径和仓库的 URL：

.. code-block:: yaml

    dependencies:
      # 从 Git 仓库定义依赖项
      test_component:
        path: test_component
        git: ssh://git@gitlab.com/user/components.git

在开发组件时，可以通过指定相对或绝对路径，使用本地目录中的组件：

.. code-block:: yaml

      dependencies:
        # 通过相对路径定义本地依赖项
        some_local_component:
          path: ../../projects/component

有关清单文件格式的详细信息，请参阅 `清单文件格式文档 <https://docs.espressif.com/projects/idf-component-manager/en/latest/reference/manifest_file.html>`__。

禁用组件管理器
===============================

将环境变量 ``IDF_COMPONENT_MANAGER`` 设置为 ``0``，可以显式禁用组件管理器。
