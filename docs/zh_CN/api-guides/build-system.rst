构建系统
********************

:link_to_translation:`en:[English]`

本文档主要介绍 ESP-IDF 构建系统的实现原理以及 ``组件`` 等相关概念。如需了解如何组织和构建新的 ESP-IDF 项目或组件，请阅读本文档。


概述
====

一个 ESP-IDF 项目可以看作是多个不同组件的集合，例如一个显示当前湿度的网页服务器会包含以下组件：

- ESP-IDF 基础库，包括 libc、ROM bindings 等
- Wi-Fi 驱动
- TCP/IP 协议栈
- FreeRTOS 操作系统
- 网页服务器
- 湿度传感器的驱动
- 负责将上述组件整合到一起的主程序

ESP-IDF 可以显式地指定和配置每个组件。在构建项目的时候，构建系统会前往 ESP-IDF 目录、项目目录和用户自定义组件目录（可选）中查找所有组件，允许用户通过文本菜单系统配置 ESP-IDF 项目中用到的每个组件。在所有组件配置结束后，构建系统开始编译整个项目。


概念
----

- ``项目`` 特指一个目录，其中包含了构建可执行应用程序所需的全部文件和配置，以及其他支持型文件，例如分区表、数据/文件系统分区和引导程序。

- ``项目配置`` 保存在项目根目录下名为 ``sdkconfig`` 的文件中，可以通过 ``idf.py menuconfig`` 进行修改，且一个项目只能包含一个项目配置。

- ``应用程序`` 是由 ESP-IDF 构建得到的可执行文件。一个项目通常会构建两个应用程序：项目应用程序（可执行的主文件，即用户自定义的固件）和引导程序（启动并初始化项目应用程序）。

- ``组件`` 是模块化且独立的代码，会被编译成静态库（.a 文件）并链接到应用程序。部分组件由 ESP-IDF 官方提供，其他组件则来源于其它开源项目。

- ``目标`` 特指运行构建后应用程序的硬件设备。运行 `idf.py --list-targets` 可以查看当前 ESP-IDF 版本中支持目标的完整列表。

请注意，以下内容并不属于项目的组成部分：

- ``ESP-IDF`` 并不是项目的一部分，它独立于项目，通过 ``IDF_PATH`` 环境变量（保存 ``esp-idf`` 目录的路径）链接到项目，从而将 IDF 框架与项目分离。

- 交叉编译工具链并不是项目的组成部分，它应该被安装在系统 PATH 环境变量中。


使用构建系统
============

.. _idf.py:

idf.py
--------

``idf.py`` 命令行工具提供了一个前端，可以帮助你轻松管理项目的构建过程，它管理了以下工具：

- CMake_，配置待构建的项目
- Ninja_，用于构建项目
- `esptool.py`_，烧录目标硬件设备

可通过 ``idf.py`` 配置构建系统，具体可参考 :doc:`相关文档 <tools/idf-py>`。


直接使用 CMake
--------------

为了方便，:ref:`idf.py` 已经封装了 CMake_ 命令，但是你愿意，也可以直接调用 CMake。

.. highlight:: bash

当 ``idf.py`` 在执行某些操作时，它会打印出其运行的每条命令以便参考。例如运行 ``idf.py build`` 命令与在 bash shell（或者 Windows Command Prompt）中运行以下命令是相同的::

    mkdir -p build
    cd build
    cmake .. -G Ninja   # 或者 'Unix Makefiles'
    ninja

在上面的命令列表中，``cmake`` 命令对项目进行配置，并生成用于最终构建工具的构建文件。在这个例子中，最终构建工具是 Ninja_: 运行 ``ninja`` 来构建项目。

没有必要多次运行 ``cmake``。第一次构建后，往后每次只需运行 ``ninja`` 即可。如果项目需要重新配置，``ninja`` 会自动重新调用 ``cmake``。

若在 CMake 中使用 ``ninja`` 或 ``make``，则多数 ``idf.py`` 子命令也会有其对应的目标，例如在构建目录下运行 ``make menuconfig`` 或 ``ninja menuconfig`` 与运行 ``idf.py menuconfig`` 是相同的。

.. Note::

    如果你已经熟悉了 CMake_，那么可能会发现 ESP-IDF 的 CMake 构建系统不同寻常，为了减少样板文件，该系统封装了 CMake 的许多功能。请参考 :ref:`write-pure-component` 以编写更多 “CMake 风格”的组件。


.. _flash-with-ninja-or-make:

使用 Ninja/Make 来烧录
^^^^^^^^^^^^^^^^^^^^^^

可以直接使用 ninja 或 make 运行如下命令来构建项目并烧录::

    ninja flash

或::

    make app-flash

可用的目标还包括：``flash``、``app-flash`` （仅用于 app）、``bootloader-flash`` （仅用于 bootloader）。

以这种方式烧录时，可以通过设置 ``ESPPORT`` 和 ``ESPBAUD`` 环境变量来指定串口设备和波特率。可以在操作系统或 IDE 项目中设置该环境变量，或者直接在命令行中进行设置::

    ESPPORT=/dev/ttyUSB0 ninja flash

.. Note::

  在命令的开头为环境变量赋值属于 Bash shell 的语法，可在 Linux 、macOS 和 Windows 的类 Bash shell 中运行，但在 Windows Command Prompt 中无法运行。

或::

    make -j3 app-flash ESPPORT=COM4 ESPBAUD=2000000

.. Note::

  在命令末尾为变量赋值属于 ``make`` 的语法，适用于所有平台的 ``make``。


在 IDE 中使用 CMake
-------------------

还可以使用集成了 CMake 的 IDE，仅需将项目 ``CMakeLists.txt`` 文件的路径告诉 IDE 即可。集成 CMake 的 IDE 通常会有自己的构建工具（CMake 称之为“生成器”），它是组成 IDE 的一部分，用来构建源文件。

向 IDE 中添加除 ``build`` 目标以外的自定义目标（如添加 “flash” 目标到 IDE）时，建议调用 ``idf.py`` 命令来执行这些“特殊”的操作。

有关将 ESP-IDF 同 CMake 集成到 IDE 中的详细信息，请参阅 :ref:`build_system_metadata`。

.. _setting-python-interpreter:


设置 Python 解释器
------------------

ESP-IDF 适用于 Python 3.8 以上版本。

``idf.py`` 和其他的 Python 脚本会使用默认的 Python 解释器运行，如 ``python``。你可以通过 ``python3 $IDF_PATH/tools/idf.py ...`` 命令切换到别的 Python 解释器，或者通过设置 shell 别名或其他脚本来简化该命令。

如果直接使用 CMake，运行 ``cmake -D PYTHON=python3 ...``，CMake 会使用传入的值覆盖默认的 Python 解释器。

如果使用集成 CMake 的 IDE，可以在 IDE 的图形用户界面中给名为 ``PYTHON`` 的 CMake cache 变量设置新的值来覆盖默认的 Python 解释器。

如果想在命令行中更优雅地管理 Python 的各个版本，请查看 pyenv_ 或 virtualenv_ 工具，它们会帮助你更改默认的 python 版本。


.. _example-project-structure:

示例项目
========

.. highlight:: none

示例项目的目录树结构可能如下所示::

    - myProject/
                 - CMakeLists.txt
                 - sdkconfig
                 - bootloader_components/ - boot_component/ - CMakeLists.txt
                                                            - Kconfig
                                                            - src1.c
                 - components/ - component1/ - CMakeLists.txt
                                             - Kconfig
                                             - src1.c
                               - component2/ - CMakeLists.txt
                                             - Kconfig
                                             - src1.c
                                             - include/ - component2.h
                 - main/       - CMakeLists.txt
                               - src1.c
                               - src2.c

                 - build/

该示例项目 "myProject" 包含以下组成部分：

- 顶层项目 CMakeLists.txt 文件，这是 CMake 用于学习如何构建项目的主要文件，可以在这个文件中设置项目全局的 CMake 变量。顶层项目 CMakeLists.txt 文件会导入 :idf_file:`/tools/cmake/project.cmake` 文件，由它负责实现构建系统的其余部分。该文件最后会设置项目的名称，并定义该项目。

- "sdkconfig" 项目配置文件，执行 ``idf.py menuconfig`` 时会创建或更新此文件，文件中保存了项目中所有组件（包括 ESP-IDF 本身）的配置信息。 ``sdkconfig`` 文件可能会也可能不会被添加到项目的源码管理系统中。

- 可选的 "bootloader_components" 目录中包含了需要在引导加载项目中进行编译和链接的组件。并不是每个项目都需要这种自定义组件，但此类组件在引导加载程序需要修改以嵌入新功能时可能很有用。

- 可选的 "components" 目录中包含了项目的部分自定义组件，并不是每个项目都需要这种自定义组件，但它有助于构建可复用的代码或者导入第三方（不属于 ESP-IDF）的组件。或者，你也可以在顶层 CMakeLists.txt 中设置 ``EXTRA_COMPONENT_DIRS`` 变量以查找其他指定位置处的组件。

- "main" 目录是一个特殊的组件，它包含项目本身的源代码。"main" 是默认名称，CMake 变量 ``COMPONENT_DIRS`` 默认包含此组件，但你可以修改此变量。有关详细信息，请参阅 :ref:`重命名 main 组件 <rename-main>`。如果项目中源文件较多，建议将其归于组件中，而不是全部放在 "main" 中。

- "build" 目录是存放构建输出的地方，如果没有此目录，``idf.py`` 会自动创建。CMake 会配置项目，并在此目录下生成临时的构建文件。随后，在主构建进程的运行期间，该目录还会保存临时目标文件、库文件以及最终输出的二进制文件。此目录通常不会添加到项目的源码管理系统中，也不会随项目源码一同发布。

每个组件目录都包含一个 ``CMakeLists.txt`` 文件，里面会定义一些变量以控制该组件的构建过程，以及其与整个项目的集成。更多详细信息请参阅 :ref:`component-directories`。

每个组件还可以包含一个 ``Kconfig`` 文件，它用于定义 ``menuconfig`` 时展示的 :ref:`component-configuration` 选项。某些组件可能还会包含 ``Kconfig.projbuild`` 和 ``project_include.cmake`` 特殊文件，它们用于 :ref:`override_project_config`。


项目 CMakeLists 文件
====================

每个项目都有一个顶层 ``CMakeLists.txt`` 文件，包含整个项目的构建设置。默认情况下，项目 CMakeLists 文件会非常小。


最小 CMakeLists 文件示例
------------------------

.. highlight:: cmake

最小项目::

        cmake_minimum_required(VERSION 3.16)
        include($ENV{IDF_PATH}/tools/cmake/project.cmake)
        project(myProject)


.. _project-mandatory-parts:

必要部分
--------

每个项目都要按照上面显示的顺序添加上述三行代码：

- ``cmake_minimum_required(VERSION 3.16)`` 必须放在 CMakeLists.txt 文件的第一行，它会告诉 CMake 构建该项目所需要的最小版本号。ESP-IDF 支持 CMake 3.16 或更高的版本。
- ``include($ENV{IDF_PATH}/tools/cmake/project.cmake)`` 会导入 CMake 的其余功能来完成配置项目、检索组件等任务。
- ``project(myProject)`` 会创建项目本身，并指定项目名称。该名称会作为最终输出的二进制文件的名字，即 ``myProject.elf`` 和 ``myProject.bin``。每个 CMakeLists 文件只能定义一个项目。


.. _optional_project_variable:

可选的项目变量
--------------

以下这些变量都有默认值，用户可以覆盖这些变量值以自定义构建行为。更多实现细节，请参阅 :idf_file:`/tools/cmake/project.cmake` 文件。

- ``COMPONENT_DIRS``：组件的搜索目录，默认为 ``IDF_PATH/components``、 ``PROJECT_DIR/components``、和 ``EXTRA_COMPONENT_DIRS``。如果你不想在这些位置搜索组件，请覆盖此变量。

- ``EXTRA_COMPONENT_DIRS``：用于搜索组件的其它可选目录列表。路径可以是相对于项目目录的相对路径，也可以是绝对路径。

- ``COMPONENTS``：要构建进项目中的组件名称列表，默认为 ``COMPONENT_DIRS`` 目录下检索到的所有组件。使用此变量可以“精简”项目以缩短构建时间。请注意，如果一个组件通过 ``COMPONENT_REQUIRES`` 指定了它依赖的另一个组件，则会自动将其添加到 ``COMPONENTS`` 中，所以 ``COMPONENTS`` 列表可能会非常短。

- ``BOOTLOADER_IGNORE_EXTRA_COMPONENT``：引导加载程序编译时应忽略的组件列表，位于 ``bootloader_components/`` 目录中。使用这一变量可以将一个组件有条件地包含在项目中。

以上变量中的路径可以是绝对路径，或者是相对于项目目录的相对路径。

请使用 `cmake 中的 set 命令 <cmake set_>`_ 来设置这些变量，如 ``set(VARIABLE "VALUE")``。请注意，``set()`` 命令需放在 ``include(...)`` 之前，``cmake_minimum(...)`` 之后。


.. _rename-main:

重命名 ``main`` 组件
--------------------

构建系统会对 ``main`` 组件进行特殊处理。假如 ``main`` 组件位于预期的位置（即 `${PROJECT_PATH}/main`），那么它会被自动添加到构建系统中。其他组件也会作为其依赖项被添加到构建系统中，这使用户免于处理依赖关系，并提供即时可用的构建功能。重命名 ``main`` 组件会减轻上述这些幕后工作量，但要求用户指定重命名后的组件位置，并手动为其添加依赖项。重命名 ``main`` 组件的步骤如下：

1. 重命名 ``main`` 目录。
2. 在项目 CMakeLists.txt 文件中设置 ``EXTRA_COMPONENT_DIRS``，并添加重命名后的 ``main`` 目录。
3. 在组件的 CMakeLists.txt 文件中设置 ``COMPONENT_REQUIRES`` 或 ``COMPONENT_PRIV_REQUIRES`` 以指定依赖项。


覆盖默认的构建规范
---------------------------------------

构建系统设置了一些全局的构建规范（编译标志、定义等），这些规范可用于编译来自所有组件的所有源文件。

.. highlight:: cmake

例如，其中一个默认的构建规范是编译选项 ``Wextra``。假设一个用户想用 ``Wno-extra`` 来覆盖这个选项，
应在 ``project()`` 之后进行::


    cmake_minimum_required(VERSION 3.16)
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    project(myProject)

    idf_build_set_property(COMPILE_OPTIONS "-Wno-error" APPEND)

这确保了用户设置的编译选项不会被默认的构建规范所覆盖，因为默认的构建规范是在 ``project()`` 内设置的。


.. _component-directories:

组件 CMakeLists 文件
====================

每个项目都包含一个或多个组件，这些组件可以是 ESP-IDF 的一部分，可以是项目自身组件目录的一部分，也可以从自定义组件目录添加（:ref:`见上文 <component-directories>`）。

组件是 ``COMPONENT_DIRS`` 列表中包含 ``CMakeLists.txt`` 文件的任何目录。


搜索组件
--------

搜索 ``COMPONENT_DIRS`` 中的目录列表以查找项目的组件，此列表中的目录可以是组件自身（即包含 `CMakeLists.txt` 文件的目录），也可以是子目录为组件的顶级目录。

当 CMake 运行项目配置时，它会记录本次构建包含的组件列表，它可用于调试某些组件的添加/排除。


.. _cmake-components-same-name:

同名组件
--------

ESP-IDF 在搜索所有待构建的组件时，会按照 ``COMPONENT_DIRS`` 指定的顺序依次进行，这意味着在默认情况下，首先搜索 ESP-IDF 内部组件（``IDF_PATH/components``），然后是 ``EXTRA_COMPONENT_DIRS`` 中的组件，最后是项目组件（``PROJECT_DIR/components``）。如果这些目录中的两个或者多个包含具有相同名字的组件，则使用搜索到的最后一个位置的组件。这就允许将组件复制到项目目录中再修改以覆盖 ESP-IDF 组件，如果使用这种方式，ESP-IDF 目录本身可以保持不变。

.. 注解::

  如果在现有项目中通过将组件移动到一个新位置来覆盖它，项目不会自动看到新组件的路径。请运行 ``idf.py reconfigure`` 命令后（或删除项目构建文件夹）再重新构建。


.. _minimum_cmakelists:

最小组件 CMakeLists 文件
--------------------------

.. highlight:: cmake

最小组件 ``CMakeLists.txt`` 文件通过使用 ``idf_component_register`` 将组件添加到构建系统中。

  idf_component_register(SRCS "foo.c" "bar.c"
                         INCLUDE_DIRS "include"
                         REQUIRES mbedtls)

- ``SRCS`` 是源文件列表（``*.c``、``*.cpp``、``*.cc``、``*.S``），里面所有的源文件都将会编译进组件库中。
- ``INCLUDE_DIRS`` 是目录列表，里面的路径会被添加到所有需要该组件的组件（包括 main 组件）全局 include 搜索路径中。
- ``REQUIRES`` 实际上并不是必需的，但通常需要它来声明该组件需要使用哪些其它组件，请参考 :ref:`组件依赖 <component-requirements>`。

上述命令会构建生成与组件同名的库，并最终被链接到应用程序中。

上述目录通常设置为相对于 ``CMakeLists.txt`` 文件的相对路径，当然也可以设置为绝对路径。

还有其它参数可以传递给 ``idf_component_register``，具体可参考 :ref:`here<cmake-component-register>`。

有关更完整的 ``CMakeLists.txt`` 示例，请参阅 `组件依赖示例`_ 和 `组件 CMakeLists 示例`_。


.. _preset_component_variables:

预设的组件变量
--------------

以下专用于组件的变量可以在组件 CMakeLists 中使用，但不建议修改：

- ``COMPONENT_DIR``：组件目录，即包含 ``CMakeLists.txt`` 文件的绝对路径，它与 ``CMAKE_CURRENT_SOURCE_DIR`` 变量一样，路径中不能包含空格。
- ``COMPONENT_NAME``：组件名，与组件目录名相同。
- ``COMPONENT_ALIAS``：库别名，由构建系统在内部为组件创建。
- ``COMPONENT_LIB``：库名，由构建系统在内部为组件创建。

以下变量在项目级别中被设置，但可在组件 CMakeLists 中使用：

- ``CONFIG_*``：项目配置中的每个值在 cmake 中都对应一个以 ``CONFIG_`` 开头的变量。更多详细信息请参阅 :doc:`Kconfig </api-reference/kconfig>`。
- ``ESP_PLATFORM``：ESP-IDF 构建系统处理 CMake 文件时，其值设为 1。


构建/项目变量
-----------------

以下是可作为构建属性的构建/项目变量，可通过组件 CMakeLists.txt 中的 ``idf_build_get_property`` 查询其变量值。

- ``PROJECT_NAME``：项目名，在项目 CMakeLists.txt 文件中设置。
- ``PROJECT_DIR``：项目目录（包含项目 CMakeLists 文件）的绝对路径，与 ``CMAKE_SOURCE_DIR`` 变量相同。
- ``COMPONENTS``：此次构建中包含的所有组件的名称，具体格式为用分号隔开的 CMake 列表。
- ``IDF_VER``：ESP-IDF 的 git 版本号，由 ``git describe`` 命令生成。
- ``IDF_VERSION_MAJOR``、 ``IDF_VERSION_MINOR``、 ``IDF_VERSION_PATCH``: ESP-IDF 的组件版本，可用于条件表达式。请注意这些信息的精确度不如 ``IDF_VER`` 变量，版本号 ``v4.0-dev-*``， ``v4.0-beta1``， ``v4.0-rc1`` 和 ``v4.0`` 对应的 ``IDF_VERSION_*`` 变量值是相同的，但是 ``IDF_VER`` 的值是不同的。
- ``IDF_TARGET``：项目的硬件目标名称。
- ``PROJECT_VER``：项目版本号。

  * 如果设置 :ref:`CONFIG_APP_PROJECT_VER_FROM_CONFIG` 选项，将会使用 :ref:`CONFIG_APP_PROJECT_VER` 的值。
  * 或者，如果在项目 CMakeLists.txt 文件中设置了 ``PROJECT_VER`` 变量，则该变量值可以使用。
  * 或者，如果 ``PROJECT_DIR/version.txt`` 文件存在，其内容会用作 ``PROJECT_VER`` 的值。
  * 或者，如果在 CMakeLists.txt 文件中将 ``VERSION`` 参数传递给 ``project()`` 调用，形式为 ``project(... VERSION x.y.z.w )``，那么 ``VERSION`` 参数将用作为 ``PROJECT_VER`` 的值。``VERSION`` 参数必须符合 `cmake 标准 <https://cmake.org/cmake/help/v3.16/command/project.html>`_。
  * 或者，如果项目位于某个 Git 仓库中，则使用 ``git describe`` 命令的输出作为 ``PROJECT_VER`` 的值。
  * 否则，``PROJECT_VER`` 的值为 1。
- ``EXTRA_PARTITION_SUBTYPES``：CMake 列表，用于创建额外的分区子类型。子类型的描述由字符串组成，以逗号为分隔，格式为 ``type_name, subtype_name, numeric_value``。组件可通过此列表，添加新的子类型。

其它与构建属性有关的信息请参考 :ref:`这里<cmake-build-properties>`。


.. _component_build_control:

组件编译控制
------------------

.. highlight:: cmake

在编译特定组件的源文件时，可以使用 `target_compile_options`_  函数来传递编译器选项::

  target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-variable)

如果给单个源文件指定编译器标志，可以使用 CMake 的 `set_source_files_properties`_ 命令::

    set_source_files_properties(mysrc.c
        PROPERTIES COMPILE_FLAGS
        -Wno-unused-variable
    )

如果上游代码在编译的时候发出了警告，那这么做可能会很有效。

.. note::

    如已借助 ``idf_component_register`` 中的 ``SRC_DIRS`` 变量填充源文件，CMake `set_source_files_properties`_ 命令将无法使用，详情请参考 :ref:`cmake-file-globbing`。

请注意，上述两条命令只能在组件 CMakeLists 文件的 ``idf_component_register`` 命令之后调用。


.. _component-configuration:

组件配置
========

每个组件都可以包含一个 ``Kconfig`` 文件，和 ``CMakeLists.txt`` 放在同一目录下。``Kconfig`` 文件中包含要添加到该组件配置菜单中的一些配置设置信息。

运行 menuconfig 时，可以在 ``Component Settings`` 菜单栏下找到这些设置。

创建一个组件的 Kconfig 文件，最简单的方法就是使用 ESP-IDF 中现有的 Kconfig 文件作为模板，在这基础上进行修改。

有关示例请参阅 :ref:`add_conditional_config`。


预处理器定义
============

ESP-IDF 构建系统会在命令行中添加以下 C 预处理器定义：

- ``ESP_PLATFORM``：可以用来检测在 ESP-IDF 内发生了构建行为。
- ``IDF_VER``：定义 git 版本字符串，例如：``v2.0`` 用于标记已发布的版本，``v1.0-275-g0efaa4f`` 则用于标记任意某次的提交记录。


.. _component-requirements:

组件依赖
========

编译各个组件时，ESP-IDF 系统会递归评估其依赖项。这意味着每个组件都需要声明它所依赖的组件，即 “requires”。


编写组件
--------

.. code-block:: cmake

   idf_component_register(...
                          REQUIRES mbedtls
                          PRIV_REQUIRES console spiffs)

- ``REQUIRES`` 需要包含所有在当前组件的 *公共* 头文件里 `#include` 的头文件所在的组件。

- ``PRIV_REQUIRES`` 需要包含被当前组件的源文件 `#include` 的头文件所在的组件（除非已经被设置在了 ``REQUIRES`` 中）。以及是当前组件正常工作必须要链接的组件。

- ``REQUIRES`` 和 ``PRIV_REQUIRES`` 的值不能依赖于任何配置选项（``CONFIG_xxx`` 宏）。这是因为在配置加载之前，依赖关系就已经被展开。其它组件变量（比如包含路径或源文件）可以依赖配置选择。

- 如果当前组件除了 `通用组件依赖项`_ 中设置的通用组件（比如 RTOS、libc 等）外，并不依赖其它组件，那么对于上述两个 ``REQUIRES`` 变量，可以选择其中一个或是两个都不设置。

如果组件仅支持某些硬件目标（``IDF_TARGET`` 的值），则可以在 ``idf_component_register`` 中指定 ``REQUIRED_IDF_TARGETS`` 来声明这个需求。在这种情况下，如果构建系统导入了不支持当前硬件目标的组件时就会报错。

.. 注解::

  在 CMake 中，``REQUIRES`` 和 ``PRIV_REQUIRES`` 是 CMake 函数 ``target_link_libraries(... PUBLIC ...)`` 和 ``target_link_libraries(... PRIVATE ...)`` 的近似包装。


.. _example component requirements:

组件依赖示例
--------------------

假设现在有一个 ``car`` 组件，它需要使用 ``engine`` 组件，而 ``engine`` 组件需要使用 ``spark_plug`` 组件：

.. code-block:: none

    - autoProject/
                 - CMakeLists.txt
                 - components/ - car/ - CMakeLists.txt
                                         - car.c
                                         - car.h
                               - engine/ - CMakeLists.txt
                                         - engine.c
                                         - include/ - engine.h
                               - spark_plug/  - CMakeLists.txt
                                              - spark_plug.c
                                              - spark_plug.h


Car 组件
^^^^^^^^^

.. highlight:: c

``car.h`` 头文件是 ``car`` 组件的公共接口。该头文件直接包含了 ``engine.h``，这是因为它需要使用 ``engine.h`` 中的一些声明::

  /* car.h */
  #include "engine.h"

  #ifdef ENGINE_IS_HYBRID
  #define CAR_MODEL "Hybrid"
  #endif

同时 car.c 也包含了 ``car.h``::

  /* car.c */
  #include "car.h"

这代表文件 ``car/CMakeLists.txt`` 需要声明 ``car`` 需要 ``engine``：

.. code-block:: cmake

  idf_component_register(SRCS "car.c"
                    INCLUDE_DIRS "."
                    REQUIRES engine)

- ``SRCS`` 提供 ``car`` 组件中源文件列表。
- ``INCLUDE_DIRS`` 提供该组件公共头文件目录列表，由于 ``car.h`` 是公共接口，所以这里列出了所有包含了 ``car.h`` 的目录。
- ``REQUIRES`` 给出该组件的公共接口所需的组件列表。由于 ``car.h`` 是一个公共头文件并且包含了来自 ``engine`` 的头文件，所以我们这里包含 ``engine``。这样可以确保任何包含 ``car.h`` 的其他组件也能递归地包含所需的 ``engine.h``。


Engine 组件
^^^^^^^^^^^^^^^^

.. highlight:: c

``engine`` 组件也有一个公共头文件 ``include/engine.h``，但这个头文件更为简单::

  /* engine.h */
  #define ENGINE_IS_HYBRID

  void engine_start(void);

在 ``engine.c`` 中执行::

  /* engine.c */
  #include "engine.h"
  #include "spark_plug.h"

  ...

在该组件中，``engine`` 依赖于 ``spark_plug``，但这是私有依赖关系。编译 ``engine.c`` 需要 ``spark_plug.h`` 但不需要包含 ``engine.h``。

这代表文件 ``engine/CMakeLists.txt`` 可以使用 ``PRIV_REQUIRES``：

.. code-block:: cmake

  idf_component_register(SRCS "engine.c"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES spark_plug)

因此，``car`` 组件中的源文件不需要在编译器搜索路径中添加 ``spark_plug`` include 目录。这可以加快编译速度，避免编译器命令行过于的冗长。


Spark Plug 组件
^^^^^^^^^^^^^^^^^^^^

``spark_plug`` 组件没有依赖项，它有一个公共头文件 ``spark_plug.h``，但不包含其他组件的头文件。

这代表 ``spark_plug/CMakeLists.txt`` 文件不需要任何 ``REQUIRES`` 或 ``PRIV_REQUIRES``：

.. code-block:: cmake

  idf_component_register(SRCS "spark_plug.c"
                    INCLUDE_DIRS ".")


源文件 Include 目录
---------------------

每个组件的源文件都是用这些 Include 路径目录编译的，这些路径在传递给 ``idf_component_register`` 的参数中指定：

.. code-block:: cmake

  idf_component_register(..
                         INCLUDE_DIRS "include"
                         PRIV_INCLUDE_DIRS "other")


- 当前组件的 ``INCLUDE_DIRS`` 和 ``PRIV_INCLUDE_DIRS``。
- ``REQUIRES`` 和 ``PRIV_REQUIRES`` 参数指定的所有其他组件（即当前组件的所有公共和私有依赖项）所设置的 ``INCLUDE_DIRS``。
- 递归列出所有组件 ``REQUIRES`` 列表中 ``INCLUDE_DIRS`` 目录（如递归展开这个组件的所有公共依赖项）。


主要组件依赖项
-----------------------

``main`` 组件比较特别，因为它在构建过程中自动依赖所有其他组件。所以不需要向这个组件传递 ``REQUIRES`` 或 ``PRIV_REQUIRES``。有关不再使用 ``main`` 组件时需要更改哪些内容，请参考 :ref:`重命名 main 组件<rename-main>`。


.. _component-common-requirements:

通用组件依赖项
--------------

为避免重复性工作，各组件都用自动依赖一些“通用”IDF 组件，即使它们没有被明确提及。这些组件的头文件会一直包含在构建系统中。

通用组件包括：cxx、newlib、freertos、esp_hw_support、heap、log、soc、hal、esp_rom、esp_common、esp_system。


在构建中导入组件
-----------------

- 默认情况下，每个组件都会包含在构建系统中。
- 如果将 ``COMPONENTS`` 变量设置为项目直接使用的最小组件列表，那么构建系统会扩展到包含所有组件。完整的组件列表为：

  * ``COMPONENTS`` 中明确提及的组件。
  * 这些组件的依赖项（以及递归运算后的组件）。
  * 每个组件都依赖的通用组件。

- 将 ``COMPONENTS`` 设置为所需组件的最小列表，可以显著减少项目的构建时间。


.. _component-circular-dependencies:

循环依赖
---------------------

一个项目中可能包含组件 A 和组件 B，而组件 A 依赖（``REQUIRES`` 或 ``PRIV_REQUIRES``）组件 B，组件 B 又依赖组件 A。这就是所谓的依赖循环或循环依赖。

CMake 通常会在链接器命令行上重复两次组件库名称来自动处理循环依赖。然而这种方法并不总是有效，还是可能构建失败并出现关于 “Undefined reference to ...” 的链接器错误，这通常是由于引用了循环依赖中某一组件中定义的符号。如果存在较大的循环依赖关系，即 A->B->C->D->A，这种情况极有可能发生。

最好的解决办法是重构组件以消除循环依赖关系。在大多数情况下，没有循环依赖的软件架构具有模块化和分层清晰的特性，并且从长远来看更容易维护。然而，移除循环依赖关系并不容易做到。

要绕过由循环依赖引起的链接器错误，最简单的解决方法是增加其中一个组件库的 CMake `LINK_INTERFACE_MULTIPLICITY`_ 属性。 这会让 CMake 在链接器命令行上对此库及其依赖项重复两次以上。

例如：

.. code-block:: cmake

    set_property(TARGET ${COMPONENT_LIB} APPEND PROPERTY LINK_INTERFACE_MULTIPLICITY 3)

- 这一行应该放在组件 CMakeLists.txt 文件 ``idf_component_register`` 之后。
- 可以的话，将此行放置在因依赖其他组件而造成循环依赖的组件中。实际上，该行可以放在循环内的任何一个组件中，但建议将其放置在拥有链接器错误提示信息中显示的源文件的组件中，或是放置在定义了链接器错误提示信息中所提到的符号的组件，先从这些组件开始是个不错的选择。
- 通常将值增加到 3（默认值是 2）就足够了，但如果不起作用，可以尝试逐步增加这个数字。
- 注意，增加这个选项会使链接器的命令行变长，链接阶段变慢。


高级解决方法：未定义符号
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

如果只有一两个符号导致循环依赖，而所有其他依赖都是线性的，那么有一种替代方法可以避免链接器错误：在链接时将“反向”依赖所需的特定符号指定为未定义符号。

例如，如果组件 A 依赖于组件 B，但组件 B 也需要引用组件 A 的 ``reverse_ops`` （但不依赖组件 A 中的其他内容），那么你可以在组件 B 的 CMakeLists.txt 中添加如下一行，以在链接时避免这出现循环。

.. code-block:: cmake

    # 该符号是由“组件 A”在链接时提供
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u reverse_ops")

- ``-u`` 参数意味着链接器将始终在链接中包含此符号，而不管依赖项顺序如何。
- 该行应该放在组件 CMakeLists.txt 文件中的 ``idf_component_register`` 之后。
- 如果“组件 B”不需要访问“组件 A”的任何头文件，只需链接几个符号，那么这一行可以用来代替 B 对 A 的任何 “REQUIRES”。这样则进一步简化了构建系统中的组件结构。

请参考 `target_link_libraries`_ 文档以了解更多关于此 CMake 函数的信息。


.. _component-requirements-implementation:

构建系统中依赖处理的实现细节
----------------------------

- 在 CMake 配置进程的早期阶段会运行 ``expand_requirements.cmake`` 脚本。该脚本会对所有组件的 CMakeLists.txt 文件进行局部的运算，得到一张组件依赖关系图（:ref:`此图可能会有闭环 <component-circular-dependencies>`）。此图用于在构建目录中生成 ``component_depends.cmake`` 文件。
- CMake 主进程会导入该文件，并以此来确定要包含到构建系统中的组件列表（内部使用的 ``BUILD_COMPONENTS`` 变量）。``BUILD_COMPONENTS`` 变量已排好序，依赖组件会排在前面。由于组件依赖关系图中可能存在闭环，因此不能保证每个组件都满足该排序规则。如果给定相同的组件集和依赖关系，那么最终的排序结果应该是确定的。
- CMake 会将 ``BUILD_COMPONENTS`` 的值以 “Component names:” 的形式打印出来。
- 然后执行构建系统中包含的每个组件的配置。
- 每个组件都被正常包含在构建系统中，然后再次执行 CMakeLists.txt 文件，将组件库加入构建系统。


组件依赖顺序
^^^^^^^^^^^^

``BUILD_COMPONENTS`` 变量中组件的顺序决定了构建过程中的其它顺序，包括：

- 项目导入 :ref:`project_include.cmake` 文件的顺序。
- 生成用于编译（通过 ``-I`` 参数）的头文件路径列表的顺序。请注意，对于给定组件的源文件，仅需将该组件的依赖组件的头文件路径告知编译器。


添加链接时依赖项
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. highlight:: cmake

ESP-IDF 的 CMake 辅助函数 ``idf_component_add_link_dependency`` 可以在组件之间添加仅作用于链接时的依赖关系。绝大多数情况下，我们都建议你使用 ``idf_component_register`` 中的 ``PRIV_REQUIRES`` 功能来构建依赖关系。然而在某些情况下，还是有必要添加另一个组件对当前组件的链接时依赖，即反转 ``PRIV_REQUIRES`` 中的依赖关系（参考示例：:doc:`/api-reference/peripherals/spi_flash/spi_flash_override_driver`）。

要使另一个组件在链接时依赖于这个组件::

  idf_component_add_link_dependency(FROM other_component)

请将上述行置于 ``idf_component_register`` 行之后。

也可以通过名称指定两个组件::

  idf_component_add_link_dependency(FROM other_component TO that_component)


.. _override_project_config:

覆盖项目的部分设置
=====================

.. _project_include.cmake:

project_include.cmake
---------------------

如果组件的某些构建行为需要在组件 CMakeLists 文件之前被执行，你可以在组件目录下创建名为 ``project_include.cmake`` 的文件，``project.cmake`` 在运行过程中会导入此 CMake 文件。

``project_include.cmake`` 文件在 ESP-IDF 内部使用，以定义项目范围内的构建功能，比如 ``esptool.py`` 的命令行参数和 ``bootloader`` 这个特殊的应用程序。

与组件 ``CMakeLists.txt`` 文件有所不同，在导入 ``project_include.cmake`` 文件的时候，当前源文件目录（即 ``CMAKE_CURRENT_SOURCE_DIR`` 和工作目录）为项目目录。如果想获得当前组件的绝对路径，可以使用 ``COMPONENT_PATH`` 变量。

请注意，``project_include.cmake`` 对于大多数常见的组件并不是必需的。例如给项目添加 include 搜索目录，给最终的链接步骤添加 ``LDFLAGS`` 选项等等都可以通过 ``CMakeLists.txt`` 文件来自定义。详细信息请参考 :ref:`optional_project_variable`。

``project_include.cmake`` 文件会按照 ``BUILD_COMPONENTS`` 变量中组件的顺序（由 CMake 记录）依次导入。即只有在当前组件所有依赖组件的 ``project_include.cmake`` 文件都被导入后，当前组件的 ``project_include.cmake`` 文件才会被导入，除非两个组件在同一个依赖闭环中。如果某个 ``project_include.cmake`` 文件依赖于另一组件设置的变量，则要特别注意上述情况。更多详情请参阅 :ref:`component-requirements-implementation`。

在 ``project_include.cmake`` 文件中设置变量或目标时要格外小心，这些值被包含在项目的顶层 CMake 文件中，因此他们会影响或破坏所有组件的功能。


KConfig.projbuild
-----------------

与 ``project_include.cmake`` 类似，也可以为组件定义一个 KConfig 文件以实现全局的 :ref:`component-configuration`。如果要在 menuconfig 的顶层添加配置选项，而不是在 “Component Configuration” 子菜单中，则可以在 ``CMakeLists.txt`` 文件所在目录的 KConfig.projbuild 文件中定义这些选项。

在此文件中添加配置时要小心，因为这些配置会包含在整个项目配置中。在可能的情况下，请为 :ref:`component-configuration` 创建 KConfig 文件。


通过封装对现有函数进行重新定义或扩展
-------------------------------------

链接器具有封装功能，可以重新定义或扩展现有 ESP-IDF 函数的行为。如需封装函数，你需要在项目的 ``CMakeLists.txt`` 文件中提供以下 CMake 声明：

.. code-block:: cmake

    target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=function_to_redefine")

其中，``function_to_redefine`` 为需要被重新定义或扩展的函数名称。启用此选项后，链接器将把二进制库中所有对 ``function_to_redefine`` 函数的调用改为对 ``__wrap_function_to_redefine`` 函数的调用。因此，你必须在应用程序中定义这一符号。

链接器会提供一个名为 ``__real_function_to_redefine`` 的新符号，指向将被重新定义的函数的原有实现。由此，可以从新的实现中调用该函数，从而对原有实现进行扩展。

请参考 :example:`build_system/wrappers` 示例，了解其详细原理。更多细节请参阅 :idf_file:`examples/build_system/wrappers/README.md`。


覆盖默认引导加载程序
--------------------------

由于 ESP-IDF 中存在可选目录 ``bootloader_components``，因此可以覆盖默认的 ESP-IDF 引导加载程序。覆盖前，应定义一个 ``bootloader_components/main`` 组件，使项目目录如下所示：

    - myProject/
                 - CMakeLists.txt
                 - sdkconfig
                 - bootloader_components/ - main/ - CMakeLists.txt
                                                  - Kconfig
                                                  - my_bootloader.c
                 - main/       - CMakeLists.txt
                               - app_main.c

                 - build/


此处的 ``my_bootloader.c`` 文件会成为新引导加载程序的源代码，这意味着它需要执行所有必要的操作来设置并从 flash 中加载 ``main`` 应用程序。

还可以根据特定的条件来替换引导加载程序，例如替换指定目标芯片的引导加载程序。这可以通过 ``BOOTLOADER_IGNORE_EXTRA_COMPONENT`` CMake 变量实现，该列表会让 ESP-IDF 引导加载项目忽略 ``bootloader_components`` 中的指定组件，不对其进行编译。例如，如果希望使用 ESP32 目标芯片的默认引导加载程序，``myProject/CMakeLists.txt`` 应如下所示::

    include($ENV{IDF_PATH}/tools/cmake/project.cmake)

    if(${IDF_TARGET} STREQUAL "esp32")
        set(BOOTLOADER_IGNORE_EXTRA_COMPONENT "main")
    endif()

    project(main)

值得注意的是，这还可以用于除 ``main`` 之外的其他引导加载程序组件。在任何情况下，都不能指定前缀 ``bootloader_component``。

请参考 :example:`custom_bootloader/bootloader_override` 查看覆盖默认引导加载程序的示例。


.. _config_only_component:

仅配置组件
===========

仅配置组件是一类不包含源文件的特殊组件，仅包含 ``Kconfig.projbuild``、``KConfig`` 和 ``CMakeLists.txt`` 文件，该 ``CMakeLists.txt`` 文件仅有一行代码，调用了 ``idf_component_register()`` 函数。此函数会将组件导入到项目构建中，但不会构建任何库，也不会将头文件添加到任何 include 搜索路径中。


CMake 调试
===========

请查看 `CMake v3.16 官方文档`_ 获取更多关于 CMake_ 和 CMake 命令的信息。

调试 ESP-IDF CMake 构建系统的一些技巧：

- CMake 运行时，会打印大量诊断信息，包括组件列表和组件路径。
- 运行 ``cmake -DDEBUG=1``，IDF 构建系统会生成更详细的诊断输出。
- 运行 ``cmake`` 时指定 ``--trace`` 或 ``--trace-expand`` 选项会提供大量有关控制流信息。详情请参考 `CMake 命令行文档`_。

当从项目 CMakeLists 文件导入时，``project.cmake`` 文件会定义工具模块和全局变量，并在系统环境中没有设置 ``IDF_PATH`` 时设置 ``IDF_PATH``。

同时还定义了一个自定义版本的内置 CMake_ ``project`` 函数， 这个函数被覆盖，以添加所有 ESP-IDF 特定的项目功能。


.. _warn-undefined-variables:

警告未定义的变量
------------------

默认情况下，警告未定义的变量这一功能是关闭的。

可通过将 ``--warn-uninitialized`` 标志传递给 CMake_ 或通过将 ``--cmake-warn-uninitialized`` 传递给 ``idf.py`` 来使能这一功能。这样，如果在构建的过程中引用了未定义的变量，CMake_ 会打印警告。这对查找有错误的 CMake 文件非常有用。

更多信息，请参考文件 :idf_file:`/tools/cmake/project.cmake` 以及 :idf:`/tools/cmake/` 中支持的函数。


.. _component_cmakelists_example:


组件 CMakeLists 示例
====================

因为构建环境试图设置大多数情况都能工作的合理默认值，所以组件 ``CMakeLists.txt`` 文件可能非常小，甚至是空的，请参考 :ref:`minimum_cmakelists`。但有些功能往往需要覆盖 :ref:`preset_component_variables` 才能实现。

以下是组件 CMakeLists 文件的更高级的示例。


.. _add_conditional_config:

添加条件配置
------------

配置系统可用于根据项目配置中选择的选项有条件地编译某些文件。

.. highlight:: none

``Kconfig``::

    config FOO_ENABLE_BAR
        bool "Enable the BAR feature."
        help
            This enables the BAR feature of the FOO component.

``CMakeLists.txt``::

    set(srcs "foo.c" "more_foo.c")

    if(CONFIG_FOO_ENABLE_BAR)
        list(APPEND srcs "bar.c")
    endif()

   idf_component_register(SRCS "${srcs}"
                        ...)

上述示例使用了 CMake 的 `if <cmake if_>`_ 函数和 `list APPEND <cmake list_>`_ 函数。

也可用于选择或删除某一实现，如下所示：

``Kconfig``::

    config ENABLE_LCD_OUTPUT
        bool "Enable LCD output."
        help
            Select this if your board has a LCD.

    config ENABLE_LCD_CONSOLE
        bool "Output console text to LCD"
        depends on ENABLE_LCD_OUTPUT
        help
            Select this to output debugging output to the lcd

    config ENABLE_LCD_PLOT
        bool "Output temperature plots to LCD"
        depends on ENABLE_LCD_OUTPUT
        help
            Select this to output temperature plots

.. highlight:: cmake

``CMakeLists.txt``::

    if(CONFIG_ENABLE_LCD_OUTPUT)
       set(srcs lcd-real.c lcd-spi.c)
    else()
       set(srcs lcd-dummy.c)
    endif()

    # 如果启用了控制台或绘图功能，则需要加入字体
    if(CONFIG_ENABLE_LCD_CONSOLE OR CONFIG_ENABLE_LCD_PLOT)
       list(APPEND srcs "font.c")
    endif()

    idf_component_register(SRCS "${srcs}"
                        ...)


硬件目标的条件判断
--------------------

CMake 文件可以使用 ``IDF_TARGET`` 变量来获取当前的硬件目标。

此外，如果当前的硬件目标是 ``xyz`` （即 ``IDF_TARGET=xyz``），那么 Kconfig 变量 ``CONFIG_IDF_TARGET_XYZ`` 同样也会被设置。

请注意，组件可以依赖 ``IDF_TARGET`` 变量，但不能依赖这个 Kconfig 变量。同样也不可在 CMake 文件的 ``include`` 语句中使用 Kconfig 变量，在这种上下文中可以使用 ``IDF_TARGET``。


生成源代码
----------

有些组件的源文件可能并不是由组件本身提供，而必须从另外的文件生成。假设组件需要一个头文件，该文件由 BMP 文件转换后（使用 bmp2h 工具）的二进制数据组成，然后将头文件包含在名为 graphics_lib.c 的文件中::

    add_custom_command(OUTPUT logo.h
         COMMAND bmp2h -i ${COMPONENT_DIR}/logo.bmp -o log.h
         DEPENDS ${COMPONENT_DIR}/logo.bmp
         VERBATIM)

    add_custom_target(logo DEPENDS logo.h)
    add_dependencies(${COMPONENT_LIB} logo)

    set_property(DIRECTORY "${COMPONENT_DIR}" APPEND PROPERTY
         ADDITIONAL_MAKE_CLEAN_FILES logo.h)

这个示例改编自 `CMake 的一则 FAQ <cmake faq generated files_>`_，其中还包含了一些同样适用于 ESP-IDF 构建系统的示例。

这个示例会在当前目录（构建目录）中生成 logo.h 文件，而 logo.bmp 会随组件一起提供在组件目录中。因为 logo.h 是一个新生成的文件，一旦项目需要清理，该文件也应该要被清除。因此，要将该文件添加到 `ADDITIONAL_MAKE_CLEAN_FILES`_ 属性中。

.. Note::

   如果需要生成文件作为项目 CMakeLists.txt 的一部分，而不是作为组件 CMakeLists.txt 的一部分，此时需要使用 ``${PROJECT_PATH}`` 替代 ``${COMPONENT_DIR}``，使用 ``${PROJECT_NAME}.elf`` 替代 ``${COMPONENT_LIB}``。

如果某个源文件是从其他组件中生成，且包含 ``logo.h`` 文件，则需要调用 ``add_dependencies``， 在这两个组件之间添加一个依赖项，以确保组件源文件按照正确顺序进行编译。


.. _cmake_embed_data:

嵌入二进制数据
---------------------

有时你的组件希望使用一个二进制文件或者文本文件，但是你又不希望将它们重新格式化为 C 源文件。

这时，你可以在组件注册中指定 ``EMBED_FILES`` 参数，用空格分隔要嵌入的文件名称::

  idf_component_register(...
                         EMBED_FILES server_root_cert.der)

或者，如果文件是字符串，则可以使用 ``EMBED_TXTFILES`` 变量，把文件的内容转成以 null 结尾的字符串嵌入::

  idf_component_register(...
                         EMBED_TXTFILES server_root_cert.pem)

.. highlight:: c

文件的内容会被添加到 flash 的 .rodata 段，用户可以通过符号名来访问，如下所示::

  extern const uint8_t server_root_cert_pem_start[] asm("_binary_server_root_cert_pem_start");
  extern const uint8_t server_root_cert_pem_end[]   asm("_binary_server_root_cert_pem_end");

符号名会根据文件全名生成，如 ``EMBED_FILES`` 中所示，字符 ``/``、``.`` 等都会被下划线替代。符号名称中的 _binary 前缀由 objcopy 命令添加，对文本文件和二进制文件都是如此。

.. highlight:: cmake

如果要将文件嵌入到项目中，而非组件中，可以调用 ``target_add_binary_data`` 函数::

    target_add_binary_data(myproject.elf "main/data.bin" TEXT)

并将这行代码放在项目 CMakeLists.txt 的 ``project()`` 命令之后，修改 ``myproject.elf`` 为你自己的项目名。如果最后一个参数是 ``TEXT``，那么构建系统会嵌入以 null 结尾的字符串，如果最后一个参数被设置为 ``BINARY``，则将文件内容按照原样嵌入。

有关使用此技术的示例，请查看 file_serving 示例 :example_file:`protocols/http_server/file_serving/main/CMakeLists.txt` 中的 main 组件，两个文件会在编译时加载并链接到固件中。

.. highlight:: cmake

也可以嵌入生成的文件::

  add_custom_command(OUTPUT my_processed_file.bin
                    COMMAND my_process_file_cmd my_unprocessed_file.bin)
  target_add_binary_data(my_target "my_processed_file.bin" BINARY)

上述示例中，``my_processed_file.bin`` 是通过命令 ``my_process_file_cmd`` 从文件 ``my_unprocessed_file.bin`` 中生成，然后嵌入到目标中。

使用 ``DEPENDS`` 参数来指明对目标的依赖性::

  add_custom_target(my_process COMMAND ...)
  target_add_binary_data(my_target "my_embed_file.bin" BINARY DEPENDS my_process)

``target_add_binary_data`` 的 ``DEPENDS`` 参数确保目标首先执行。


代码和数据的存放
----------------

ESP-IDF 还支持自动生成链接脚本，它允许组件通过链接片段文件定义其代码和数据在内存中的存放位置。构建系统会处理这些链接片段文件，并将处理后的结果扩充进链接脚本，从而指导应用程序二进制文件的链接过程。更多详细信息与快速上手指南，请参阅 :doc:`链接脚本生成机制 <linker-script-generation>`。


.. _component-build-full-override:

完全覆盖组件的构建过程
----------------------

.. highlight:: cmake

当然，在有些情况下，上面提到的方法不一定够用。如果组件封装了另一个第三方组件，而这个第三方组件并不能直接在 ESP-IDF 的构建系统中工作，在这种情况下，就需要放弃 ESP-IDF 的构建系统，改为使用 CMake 的 ExternalProject_ 功能。组件 CMakeLists 示例如下::

    # 用于 quirc 的外部构建过程，在源目录中运行
    # 并生成 libquirc.a
    externalproject_add(quirc_build
        PREFIX ${COMPONENT_DIR}
        SOURCE_DIR ${COMPONENT_DIR}/quirc
        CONFIGURE_COMMAND ""
        BUILD_IN_SOURCE 1
        BUILD_COMMAND make CC=${CMAKE_C_COMPILER} libquirc.a
        INSTALL_COMMAND ""
        )

    # 将 libquirc.a 添加到构建系统中
    add_library(quirc STATIC IMPORTED GLOBAL)
    add_dependencies(quirc quirc_build)

    set_target_properties(quirc PROPERTIES IMPORTED_LOCATION
        ${COMPONENT_DIR}/quirc/libquirc.a)
    set_target_properties(quirc PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
        ${COMPONENT_DIR}/quirc/lib)

    set_directory_properties( PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
        "${COMPONENT_DIR}/quirc/libquirc.a")

（上述 CMakeLists.txt 可用于创建名为 ``quirc`` 的组件，该组件使用自己的 Makefile 构建 quirc_ 项目。）

- ``externalproject_add`` 定义了一个外部构建系统。

  - 设置 ``SOURCE_DIR``、``CONFIGURE_COMMAND``、``BUILD_COMMAND`` 和 ``INSTALL_COMMAND``。如果外部构建系统没有配置这一步骤，可以将 ``CONFIGURE_COMMAND`` 设置为空字符串。在 ESP-IDF 的构建系统中，一般会将 ``INSTALL_COMMAND`` 变量设置为空。
  - 设置 ``BUILD_IN_SOURCE``，即构建目录与源目录相同。否则，你也可以设置 ``BUILD_DIR`` 变量。
  - 有关 ``externalproject_add()`` 命令的详细信息，请参阅 ExternalProject_。

- 第二组命令添加了一个目标库，指向外部构建系统生成的库文件。为了添加 include 目录，并告知 CMake 该文件的位置，需要再设置一些属性。
- 最后，生成的库被添加到 `ADDITIONAL_MAKE_CLEAN_FILES`_ 中。即执行 ``make clean`` 后会删除该库。请注意，构建系统中的其他目标文件不会被删除。

.. only:: esp32

    .. note:: 当外部构建系统使用 PSRAM 时，请记得将 ``-mfix-esp32-psram-cache-issue`` 添加到 C 编译器的参数中。关于该标志的更多详细信息，请参考 :ref:`CONFIG_SPIRAM_CACHE_WORKAROUND`。


.. _ADDITIONAL_MAKE_CLEAN_FILES_note:

ExternalProject 的依赖与构建清理
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

对于外部项目的构建，CMake 会有一些不同寻常的行为：

- `ADDITIONAL_MAKE_CLEAN_FILES`_ 仅在使用 Make 或 Ninja_ 构建系统时有效。如果使用 IDE 自带的构建系统，执行项目清理时，这些文件不会被删除。
- ExternalProject_ 会在 clean 运行后自动重新运行配置和构建命令。
- 可以采用以下两种方法来配置外部构建命令：

  1. 将外部 ``BUILD_COMMAND`` 命令设置为对所有源代码完整的重新编译。如果传递给 ``externalproject_add`` 命令的 ``DEPENDS`` 的依赖项发生了改变，或者当前执行的是项目清理操作（即运行了 ``idf.py clean``、``ninja clean`` 或者 ``make clean``），那么就会执行该命令。
  2. 将外部 ``BUILD_COMMAND`` 命令设置为增量式构建命令，并给 ``externalproject_add`` 传递 ``BUILD_ALWAYS 1`` 参数。即不管实际的依赖情况，每次构建时，都会构建外部项目。这种方式仅当外部构建系统具备增量式构建的能力，且运行时间不会很长时才推荐。

构建外部项目的最佳方法取决于项目本身、其构建系统，以及是否需要频繁重新编译项目。


.. _custom-sdkconfig-defaults:

自定义 sdkconfig 的默认值
=========================

对于示例工程或者其他你不想指定完整 sdkconfig 配置的项目，但是你确实希望覆盖 ESP-IDF 默认值中的某些键值，则可以在项目中创建 ``sdkconfig.defaults`` 文件。重新创建新配置时将会用到此文件，另外在 ``sdkconfig`` 没有设置新配置值时，上述文件也会被用到。

如若需要覆盖此文件的名称或指定多个文件，请设置 ``SDKCONFIG_DEFAULTS`` 环境变量或在顶层 CMakeLists.txt 文件中设置 ``SDKCONFIG_DEFAULTS``。非绝对路径的文件名将以当前项目的相对路径来解析。

在指定多个文件时，使用分号作为分隔符。先列出的文件将会先应用。如果某个键值在多个文件里定义，后面文件的定义会覆盖前面文件的定义。

一些 IDF 示例中包含了 ``sdkconfig.ci`` 文件。该文件是 CI（持续集成）测试框架的一部分，在正常构建过程中会被忽略。


依赖于硬件目标的 sdkconfig 默认值
---------------------------------

当且仅当 ``sdkconfig.defaults`` 文件存在时，构建系统还将尝试从 ``sdkconfig.defaults.TARGET_NAME`` 文件中加载默认值，其中 ``IDF_TARGET`` 的值为 ``TARGET_NAME``。例如，对于 ``esp32`` 这个目标芯片，sdkconfig 的默认值会首先从 ``sdkconfig.defaults`` 获取，然后再从 ``sdkconfig.defaults.esp32`` 获取。当没有通用的默认设置时，仍需创建一个空的 ``sdkconfig.defaults`` 文件，以便构建系统可以识别任何其他与目标芯片相关的 ``sdkconfig.defaults.TARGET_NAME`` 文件。

如果使用 ``SDKCONFIG_DEFAULTS`` 覆盖默认文件的名称，则硬件目标的默认文件名也会从 ``SDKCONFIG_DEFAULTS`` 值中派生。如果 ``SDKCONFIG_DEFAULTS`` 中有多个文件，硬件目标文件会在引入该硬件目标文件的文件之后应用， 而 ``SDKCONFIG_DEFAULTS`` 中所有其它后续文件则会在硬件目标文件之后应用 。

例如，如果 ``SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig_devkit1"``，并且在同一文件夹中有一个 ``sdkconfig.defaults.esp32`` 文件，那么这些文件将按以下顺序应用：（1) sdkconfig.defaults (2) sdkconfig.defaults.esp32 (3) sdkconfig_devkit1。


.. _flash_parameters:

flash 参数
==========

有些情况下，我们希望在没有 IDF 时也能烧写目标板，为此，我们希望可以保存已构建的二进制文件、esptool.py 和 esptool write_flash 命令的参数。可以通过编写一段简单的脚本来保存二进制文件和 esptool.py。

运行项目构建之后，构建目录将包含项目二进制输出文件（``.bin`` 文件），同时也包含以下烧录数据文件：

- ``flash_project_args`` 包含烧录整个项目的参数，包括应用程序 (app)、引导程序 (bootloader)、分区表，如果设置了 PHY 数据，也会包含此数据。
- ``flash_app_args`` 只包含烧录应用程序的参数。
- ``flash_bootloader_args`` 只包含烧录引导程序的参数。

.. highlight:: bash

你可以参照如下命令将任意烧录参数文件传递给 ``esptool.py``::

    python esptool.py --chip {IDF_TARGET_PATH_NAME} write_flash @build/flash_project_args

也可以手动复制参数文件中的数据到命令行中执行。

构建目录中还包含生成的 ``flasher_args.json`` 文件，此文件包含 JSON 格式的项目烧录信息，可用于 ``idf.py`` 和其它需要项目构建信息的工具。


构建 Bootloader
===============

引导程序是 :idf:`/components/bootloader/subproject` 内部独特的“子项目”，它有自己的项目 CMakeLists.txt 文件，能够构建独立于主项目的 ``.ELF`` 和 ``.BIN`` 文件，同时它又与主项目共享配置和构建目录。

子项目通过 :idf_file:`/components/bootloader/project_include.cmake` 文件作为外部项目插入到项目的顶层，主构建进程会运行子项目的 CMake，包括查找组件（主项目使用的组件的子集），生成引导程序专用的配置文件（从主 ``sdkconfig`` 文件中派生）。


.. _write-pure-component:

编写纯 CMake 组件
=================

ESP-IDF 构建系统用“组件”的概念“封装”了 CMake，并提供了很多帮助函数来自动将这些组件集成到项目构建当中。

然而，“组件”概念的背后是一个完整的 CMake 构建系统，因此可以制作纯 CMake 组件。

.. highlight:: cmake

下面是使用纯 CMake 语法为 ``json`` 组件编写的最小 CMakeLists 文件的示例::

  add_library(json STATIC
  cJSON/cJSON.c
  cJSON/cJSON_Utils.c)

  target_include_directories(json PUBLIC cJSON)

- 这实际上与 IDF 中的 :idf_file:`json 组件 </components/json/CMakeLists.txt>` 是等效的。
- 因为组件中的源文件不多，所以这个 CMakeLists 文件非常简单。对于具有大量源文件的组件而言，ESP-IDF 支持的组件通配符，可以简化组件 CMakeLists 的样式。
- 每当组件中新增一个与组件同名的库目标时，ESP-IDF 构建系统会自动将其添加到构建中，并公开公共的 include 目录。如果组件想要添加一个与组件不同名的库目标，就需要使用 CMake 命令手动添加依赖关系。


组件中使用第三方 CMake 项目
===========================

CMake 在许多开源的 C/C++ 项目中广泛使用，用户可以在自己的应用程序中使用开源代码。CMake 构建系统的一大好处就是可以导入这些第三方的项目，有时候甚至不用做任何改动。这就允许用户使用当前 ESP-IDF 组件尚未提供的功能，或者使用其它库来实现相同的功能。

.. highlight:: cmake

假设 ``main`` 组件需要导入一个假想库 ``foo``，相应的组件 CMakeLists 文件如下所示::

    # 注册组件
    idf_component_register(...)

    # 设置 `foo` 项目中的一些 CMake 变量，以控制 `foo` 的构建过程
    set(FOO_BUILD_STATIC OFF)
    set(FOO_BUILD_TESTS OFF)

    # 创建并导入第三方库目标
    add_subdirectory(foo)

    # 将 `foo` 目标公开链接至 `main` 组件
    target_link_libraries(main PUBLIC foo)

实际的案例请参考 :example:`build_system/cmake/import_lib`。请注意，导入第三方库所需要做的工作可能会因库的不同而有所差异。建议仔细阅读第三方库的文档，了解如何将其导入到其它项目中。阅读第三方库的 CMakeLists.txt 文件以及构建结构也会有所帮助。

用这种方式还可以将第三方库封装成 ESP-IDF 的组件。例如 :component:`mbedtls` 组件就是封装了 `mbedtls 项目 <https://github.com/Mbed-TLS/mbedtls>`_ 得到的。详情请参考 :component_file:`mbedtls 组件的 CMakeLists.txt 文件 <mbedtls/CMakeLists.txt>`。

每当使用 ESP-IDF 构建系统时，CMake 变量 ``ESP_PLATFORM`` 都会被设置为 1。如果要在通用的 CMake 代码加入 IDF 特定的代码时，可以采用 ``if (ESP_PLATFORM)`` 的形式加以分隔。


外部库中使用 ESP-IDF 组件
--------------------------

上述示例中假设的是外部库 ``foo`` （或 ``import_lib`` 示例中的 ``tinyxml`` 库）除了常见的 API 如 libc、libstdc++ 等外不需要使用其它 ESP-IDF API。如果外部库需要使用其它 ESP-IDF 组件提供的 API，则需要在外部 CMakeLists.txt 文件中通过添加对库目标 ``idf::<componentname>`` 的依赖关系。

例如，在 ``foo/CMakeLists.txt`` 文件::

  add_library(foo bar.c fizz.cpp buzz.cpp)

  if(ESP_PLATFORM)
    # 在 ESP-IDF 中、bar.c 需要包含 spi_flash 组件中的 esp_flash.h
    target_link_libraries(foo PRIVATE idf::spi_flash)
  endif()


组件中使用预建库
=================

.. highlight:: cmake

还有一种情况是你有一个由其它构建过程生成预建静态库（``.a`` 文件）。

ESP-IDF 构建系统为用户提供了一个实用函数 ``add_prebuilt_library``，能够轻松导入并使用预建库::

  add_prebuilt_library(target_name lib_path [REQUIRES req1 req2 ...] [PRIV_REQUIRES req1 req2 ...])

其中：

- ``target_name``- 用于引用导入库的名称，如链接到其它目标时
- ``lib_path``- 预建库的路径，可以是绝对路径或是相对于组件目录的相对路径

可选参数 ``REQUIRES`` 和 ``PRIV_REQUIRES`` 指定对其它组件的依赖性。这些参数与 ``idf_component_register`` 的参数的意义相同。

注意预建库的编译目标需与目前的项目相同。预建库的相关参数也要匹配。如果不特别注意，这两个因素可能会导致应用程序中出现 bug。

请查看示例 :example:`build_system/cmake/import_prebuilt`。


在自定义 CMake 项目中使用 ESP-IDF
=================================

ESP-IDF 提供了一个模板 CMake 项目，可以基于此轻松创建应用程序。然而在有些情况下，用户可能已有一个现成的 CMake 项目，或者想自己创建一个 CMake 项目，此时就希望将 IDF 中的组件以库的形式链接到用户目标（库/可执行文件）。

可以通过 :idf_file:`tools/cmake/idf.cmake` 提供的 :ref:`build system APIs <cmake_buildsystem_api>` 实现该目标。例如：

.. code-block:: cmake

  cmake_minimum_required(VERSION 3.16)
  project(my_custom_app C)

  # 导入提供 ESP-IDF CMake 构建系统 API 的 CMake 文件
  include($ENV{IDF_PATH}/tools/cmake/idf.cmake)

  # 在构建中导入 ESP-IDF 组件，可以视作等同 add_subdirectory()
  # 但为 ESP-IDF 构建增加额外的构建过程
  # 具体构建过程
  idf_build_process(esp32)

  # 创建项目可执行文件
  # 使用其别名 idf::newlib 将其链接到 newlib 组件
  add_executable(${CMAKE_PROJECT_NAME}.elf main.c)
  target_link_libraries(${CMAKE_PROJECT_NAME}.elf idf::newlib)

  # 让构建系统知道项目到可执行文件是什么，从而添加更多的目标以及依赖关系等
  idf_build_executable(${CMAKE_PROJECT_NAME}.elf)

:example:`build_system/cmake/idf_as_lib` 中的示例演示了如何在自定义的 CMake 项目创建一个类似于 :example:`Hello World <get-started/hello_world>` 的应用程序。

.. only:: esp32

   .. note:: IDF 构建系统只能为其构建的源文件设置编译器标志。当使用外部 CMakeLists.txt 文件并启用 PSRAM 时，记得在 C 编译器参数中添加 ``mfix-esp32-psram-cache-issue``。参见:ref:`CONFIG_SPIRAM_CACHE_WORKAROUND` 了解更多信息。


.. _cmake_buildsystem_api:

ESP-IDF CMake 构建系统 API
==============================

ESP-IDF 构建命令
------------------

.. code-block:: none

  idf_build_get_property(var property [GENERATOR_EXPRESSION])

检索一个 :ref:`构建属性 <cmake-build-properties>` *property*，并将其存储在当前作用域可访问的 var 中。特定 *GENERATOR_EXPRESSION* 将检索该属性的生成器表达式字符串（不是实际值），它可与支持生成器表达式的 CMake 命令一起使用。

.. code-block:: none

  idf_build_set_property(property val [APPEND])

设置 :ref:`构建属性 <cmake-build-properties>` *property* 的值为 *val*。特定 *APPEND* 将把指定的值附加到属性当前值之后。如果该属性之前不存在或当前为空，则指定的值将变为第一个元素/成员。

.. code-block:: none

  idf_build_component(component_dir)

向构建系统提交一个包含组件的 *component_dir* 目录。相对路径会被转换为相对于当前目录的绝对路径。
所有对该命令的调用必须在`idf_build_process`之前执行。

该命令并不保证组件在构建过程中会被处理（参见 `idf_build_process` 中 `COMPONENTS` 参数说明）

.. code-block:: none

  idf_build_process(target
                    [PROJECT_DIR project_dir]
                    [PROJECT_VER project_ver]
                    [PROJECT_NAME project_name]
                    [SDKCONFIG sdkconfig]
                    [SDKCONFIG_DEFAULTS sdkconfig_defaults]
                    [BUILD_DIR build_dir]
                    [COMPONENTS component1 component2 ...])

为导入 ESP-IDF 组件执行大量的幕后工作，包括组件配置、库创建、依赖性扩展和解析。在这些功能中，对于用户最重要的可能是通过调用每个组件的 ``idf_component_register`` 来创建库。该命令为每个组件创建库，这些库可以使用别名来访问，其形式为 idf::*component_name*。
这些别名可以用来将组件链接到用户自己的目标、库或可执行文件上。

该调用要求用 *target* 参数指定目标芯片。调用的可选参数包括：

- PROJECT_DIR - 项目目录，默认为 CMAKE_SOURCE_DIR。
- PROJECT_NAME - 项目名称，默认为 CMAKE_PROJECT_NAME。
- PROJECT_VER - 项目的版本/版本号，默认为 "1"。
- SDKCONFIG - 生成的 sdkconfig 文件的输出路径，根据是否设置 PROJECT_DIR，默认为 PROJECT_DIR/sdkconfig 或 CMAKE_SOURCE_DIR/sdkconfig。
- SDKCONFIG_DEFAULTS - 包含默认配置的文件列表（列表中必须包含完整的路径），默认为空；对于列表中的每个值 *filename*，如果存在的话，也会加载文件 *filename.target* 中的配置。对于列表中的 *filename* 的每一个值，也会加载文件 *filename.target* （如果存在的话）中的配置。
- BUILD_DIR - 用于放置 ESP-IDF 构建相关工具的目录，如生成的二进制文件、文本文件、组件；默认为 CMAKE_BINARY_DIR。
- COMPONENTS - 从构建系统已知的组件中选择要处理的组件（通过 ``idf_build_component`` 添加）。这个参数用于精简构建过程。
  如果在依赖链中需要其它组件，则会自动添加，即自动添加这个列表中组件的公共和私有依赖项，进而添加这些依赖项的公共和私有依赖，以此类推。如果不指定，则会处理构建系统已知的所有组件。

.. code-block:: none

  idf_build_executable(executable)

指定 ESP-IDF 构建的可执行文件 *executable*。这将添加额外的目标，如与 flash 相关的依赖关系，生成额外的二进制文件等。应在 ``idf_build_process`` 之后调用。

.. code-block:: none

  idf_build_get_config(var config [GENERATOR_EXPRESSION])

获取指定配置的值。就像构建属性一样，特定 *GENERATOR_EXPRESSION* 将检索该配置的生成器表达式字符串，而不是实际值，即可以与支持生成器表达式的 CMake 命令一起使用。然而，实际的配置值只有在调用 ``idf_build_process`` 后才能知道。


.. _cmake-build-properties:

ESP-IDF 构建属性
--------------------

可以通过使用构建命令 ``idf_build_get_property`` 来获取构建属性的值。例如，以下命令可以获取构建过程中使用的 Python 解释器的相关信息。

.. code-block:: none

  idf_build_get_property(python PYTHON)
  message(STATUS "The Python interpreter is: ${python}")

- BUILD_DIR - 构建目录；由 ``idf_build_process`` 的 BUILD_DIR 参数设置。
- BUILD_COMPONENTS - 包含在构建中的组件列表；由 ``idf_build_process`` 设置。
- BUILD_COMPONENT_ALIASES - 包含在构建中的组件的库别名列表；由 ``idf_build_process`` 设置。
- C_COMPILE_OPTIONS - 适用于所有组件的 C 源代码文件的编译选项。
- COMPILE_OPTIONS - 适用于所有组件的源文件（无论是 C 还是 C++）的编译选项。
- COMPILE_DEFINITIONS - 适用于所有组件源文件的编译定义。
- CXX_COMPILE_OPTIONS - 适用于所有组件的 C++ 源文件的编译选项。
- EXECUTABLE - 项目可执行文件；通过调用 ``idf_build_executable`` 设置。
- DEPENDENCIES_LOCK - 组件管理器使用的依赖关系锁定文件的路径。默认值为项目路径下的 `dependencies.lock`。
- EXECUTABLE_NAME - 不含扩展名的项目可执行文件的名称；通过调用 ``idf_build_executable`` 设置。
- EXECUTABLE_DIR - 输出的可执行文件的路径
- IDF_COMPONENT_MANAGER - 默认启用组件管理器，但如果设置这个属性为 ``0``，则会被 IDF_COMPONENT_MANAGER 环境变量禁用。
- IDF_PATH - ESP-IDF 路径；由 IDF_PATH 环境变量设置，或者从 ``idf.cmake`` 的位置推断。
- IDF_TARGET - 构建的目标芯片；由 ``idf_build_process`` 的目标参数设置。
- IDF_VER - ESP-IDF 版本；由版本文件或 IDF_PATH 仓库的 Git 版本设置。
- INCLUDE_DIRECTORIES - 包含所有组件源文件的目录。
- KCONFIGS - 构建过程中组件里的 Kconfig 文件的列表；由 ``idf_build_process`` 设置。
- KCONFIG_PROJBUILDS - 构建过程中组件中的 Kconfig.projbuild 文件的列表；由 ``idf_build_process`` 设置。
- PROJECT_NAME - 项目名称；由 ``idf_build_process`` 的 PROJECT_NAME 参数设置。
- PROJECT_DIR - 项目的目录；由 ``idf_build_process`` 的 PROJECT_DIR 参数设置。
- PROJECT_VER - 项目的版本；由 ``idf_build_process`` 的 PROJECT_VER 参数设置。
- PYTHON - 用于构建的 Python 解释器；如果有则从 PYTHON 环境变量中设置，如果没有，则使用 "python"。
- SDKCONFIG - 输出的配置文件的完整路径；由 ``idf_build_process`` SDKCONFIG 参数设置。
- SDKCONFIG_DEFAULTS - 包含默认配置的文件列表；由 ``idf_build_process`` SDKCONFIG_DEFAULTS 参数设置。
- SDKCONFIG_HEADER - 包含组件配置的 C/C++ 头文件的完整路径；由 ``idf_build_process`` 设置。
- SDKCONFIG_CMAKE - 包含组件配置的 CMake 文件的完整路径；由 ``idf_build_process`` 设置。
- SDKCONFIG_JSON - 包含组件配置的 JSON 文件的完整路径；由 ``idf_build_process`` 设置。
- SDKCONFIG_JSON_MENUS - 包含配置菜单的 JSON 文件的完整路径；由 ``idf_build_process`` 设置。


ESP-IDF 组件命令
----------------------

.. code-block:: none

  idf_component_get_property(var component property [GENERATOR_EXPRESSION])

检索一个指定的 *component* 的 :ref:`组件属性<cmake-component-properties>` *property*，并将其存储在当前作用域可访问的 *var* 中。指定 *GENERATOR_EXPRESSION* 将检索该属性的生成器表达式字符串（不是实际值），它可以在支持生成器表达式的 CMake 命令中使用。

.. code-block:: none

  idf_component_set_property(component property val [APPEND])

设置指定的 *component* 的 :ref:`组件属性<cmake-component-properties>`，*property* 的值为 *val*。特定 *APPEND* 将把指定的值追加到属性的当前值后。如果该属性之前不存在或当前为空，指定的值将成为第一个元素/成员。

.. _cmake-component-register:

.. code-block:: none

  idf_component_register([[SRCS src1 src2 ...] | [[SRC_DIRS dir1 dir2 ...] [EXCLUDE_SRCS src1 src2 ...]]
                         [INCLUDE_DIRS dir1 dir2 ...]
                         [PRIV_INCLUDE_DIRS dir1 dir2 ...]
                         [REQUIRES component1 component2 ...]
                         [PRIV_REQUIRES component1 component2 ...]
                         [LDFRAGMENTS ldfragment1 ldfragment2 ...]
                         [REQUIRED_IDF_TARGETS target1 target2 ...]
                         [EMBED_FILES file1 file2 ...]
                         [EMBED_TXTFILES file1 file2 ...]
                         [KCONFIG kconfig]
                         [KCONFIG_PROJBUILD kconfig_projbuild]
                         [WHOLE_ARCHIVE])

将一个组件注册到构建系统中。就像 ``project()`` CMake 命令一样，该命令应该直接从组件的 CMakeLists.txt 中调用（而不是通过函数或宏），且建议在其他命令之前调用该命令。下面是一些关于在 ``idf_component_register`` 之前 *不能* 调用哪些命令的指南：

  - 在 CMake 脚本模式下无效的命令。
  - 在 project_include.cmake 中定义的自定义命令。
  - 除了 ``idf_build_get_property`` 之外，构建系统的 API 命令；但要考虑该属性是否有被设置。

对变量进行设置和操作的命令，一般可在 ``idf_component_register`` 之前调用。

``idf_component_register`` 的参数包括：

  - SRCS - 组件的源文件，用于为组件创建静态库；如果没有指定，组件将被视为仅配置组件，从而创建接口库。
  - SRC_DIRS、EXCLUDE_SRCS - 用于通过指定目录来 glob 源文件 (.c、.cpp、.S)，而不是通过 SRCS 手动指定源文件。请注意，这受 :ref:`CMake 中通配符的限制<cmake-file-globbing>`。在 EXCLUDE_SRCS 中指定的源文件会从被 glob 的文件中移除。
  - INCLUDE_DIRS - 相对于组件目录的路径，该路径将被添加到需要当前组件的所有其他组件的 include 搜索路径中。
  - PRIV_INCLUDE_DIRS - 必须是相对于组件目录的目录路径，它仅被添加到这个组件源文件的 include 搜索路径中。
  - REQUIRES - 组件的公共组件依赖项。
  - PRIV_REQUIRES - 组件的私有组件依赖项；在仅用于配置的组件上会被忽略。
  - LDFRAGMENTS - 组件链接器片段文件。
  - REQUIRED_IDF_TARGETS - 指定该组件唯一支持的目标。
  - KCONFIG - 覆盖默认的 Kconfig 文件。
  - KCONFIG_PROJBUILD - 覆盖默认的 Kconfig.projbuild 文件。
  - WHOLE_ARCHIVE - 如果指定了此参数，链接时会在组件库的前后分别添加 ``-Wl,--whole-archive`` 和 ``-Wl,--no-whole-archive``。这与设置 ``WHOLE_ARCHIVE`` 组件属性的效果一致。

以下内容用于 :ref:`将数据嵌入到组件中<cmake_embed_data>`，并在确定组件是否仅用于配置时被视为源文件。这意味着，即使组件没有指定源文件，如果组件指定了以下其中之一，仍然会在内部为组件创建一个静态库。

  - EMBED_FILES - 嵌入组件的二进制文件
  - EMBED_TXTFILES - 嵌入组件的文本文件


.. _cmake-component-properties:

ESP-IDF 组件属性
------------------------

组件的属性值可以通过使用构建命令 ``idf_component_get_property`` 来获取。例如，以下命令可以获取 ``freertos`` 组件的目录。

.. code-block:: cmake

  idf_component_get_property(dir freertos COMPONENT_DIR)
  message(STATUS "The 'freertos' component directory is: ${dir}")

- COMPONENT_ALIAS - COMPONENT_LIB 的别名，用于将组件链接到外部目标；由 ``idf_build_component`` 设置，别名库本身由 ``idf_component_register`` 创建。
- COMPONENT_DIR - 组件目录；由 ``idf_build_component`` 设置。
- COMPONENT_OVERRIDEN_DIR - 如果 :ref:`这个组件覆盖了另一个组件<cmake-components-same-name>`，则包含原组件的目录。
- COMPONENT_LIB - 所创建的组件静态/接口库的名称；由 ``idf_build_component`` 设置，库本身由 ``idf_component_register`` 创建。
- COMPONENT_NAME - 组件的名称；由 ``idf_build_component`` 根据组件的目录名设置。
- COMPONENT_TYPE - 组件的类型（LIBRARY 或 CONFIG_ONLY）。如果一个组件指定了源文件或嵌入了一个文件，那么它的类型就是 LIBRARY。
- EMBED_FILES - 要嵌入组件的文件列表；由 ``idf_component_register`` EMBED_FILES 参数设置。
- EMBED_TXTFILES - 要嵌入组件的文本文件列表；由 ``idf_component_register`` EMBED_TXTFILES 参数设置。
- INCLUDE_DIRS - 组件 include 目录列表；由 ``idf_component_register`` INCLUDE_DIRS 参数设置。
- KCONFIG - 组件 Kconfig 文件；由 ``idf_build_component`` 设置。
- KCONFIG_PROJBUILD - 组件 Kconfig.projbuild；由 ``idf_build_component`` 设置。
- LDFRAGMENTS - 组件链接器片段文件列表；由 ``idf_component_register`` LDFRAGMENTS 参数设置。
- MANAGED_PRIV_REQUIRES - IDF 组件管理器从 ``idf_component.yml`` 清单文件中的依赖关系中添加的私有组件依赖关系列表。
- MANAGED_REQUIRES - IDF 组件管理器从 ``idf_component.yml`` 清单文件的依赖关系中添加的公共组件依赖关系列表。
- PRIV_INCLUDE_DIRS - 组件私有 include 目录列表；在 LIBRARY 类型的组件 ``idf_component_register`` PRIV_INCLUDE_DIRS 参数中设置。
- PRIV_REQUIRES - 私有组件依赖关系列表；根据 ``idf_component_register`` PRIV_REQUIRES 参数的值以及 ``idf_component.yml`` 清单文件中的依赖关系设置。
- REQUIRED_IDF_TARGETS - 组件支持的目标列表；由 ``idf_component_register`` REQUIRED_IDF_TARGETS 参数设置。
- REQUIRES - 公共组件依赖关系列表；根据 ``idf_component_register`` REQUIRES 参数的值以及 ``idf_component.yml`` 清单文件中的依赖关系设置。
- SRCS - 组件源文件列表；由 ``idf_component_register`` 的 SRCS 或 SRC_DIRS/EXCLUDE_SRCS 参数设置。
- WHOLE_ARCHIVE - 如果该属性被设置为 ``TRUE`` （或是其他 CMake 布尔“真”值：1、``ON``、``YES``、``Y`` 等），链接时会在组件库的前后分别添加 ``-Wl,--whole-archive`` 和 ``-Wl,--no-whole-archive`` 选项。这可以强制链接器将每个目标文件包含到可执行文件中，即使该目标文件没有解析来自应用程序其余部分的任何引用。当组件中包含依赖链接时注册的插件或模块时，通常会使用该方法。默认情况下，此属性为 ``FALSE``。可以从组件的 CMakeLists.txt 文件中将其设置为 ``TRUE``。


.. _cmake-file-globbing:

文件通配 & 增量构建
=====================

.. highlight:: cmake

在 ESP-IDF 组件中添加源文件的首选方法是在 ``COMPONENT_SRCS`` 中手动列出它们::

  idf_component_register(SRCS library/a.c library/b.c platform/platform.c
                         ...)

这是在 CMake 中手动列出源文件的 `最佳实践 <https://gist.github.com/mbinna/c61dbb39bca0e4fb7d1f73b0d66a4fd1/>`_。然而，当有许多源文件都需要添加到构建中时，这种方法就会很不方便。ESP-IDF 构建系统因此提供了另一种替代方法，即使用 ``SRC_DIRS`` 来指定源文件::

  idf_component_register(SRC_DIRS library platform
                         ...)

后台会使用通配符在指定的目录中查找源文件。但是请注意，在使用这种方法的时候，如果组件中添加了一个新的源文件，CMake 并不知道重新运行配置，最终该文件也没有被加入构建中。

如果是自己添加的源文件，这种折衷还是可以接受的，因为用户可以触发一次干净的构建，或者运行 ``idf.py reconfigure`` 来手动重启 CMake_。但是，如果你需要与其他使用 Git 等版本控制工具的开发人员共享项目时，问题就会变得更加困难，因为开发人员有可能会拉取新的版本。

ESP-IDF 中的组件使用了第三方的 Git CMake 集成模块（:idf_file:`/tools/cmake/third_party/GetGitRevisionDescription.cmake`），任何时候源码仓库的提交记录发生了改变，该模块就会自动重新运行 CMake。即只要拉取了新的 ESP-IDF 版本，CMake 就会重新运行。

对于不属于 ESP-IDF 的项目组件，有以下几个选项供参考：

- 如果项目文件保存在 Git 中，ESP-IDF 会自动跟踪 Git 修订版本，并在它发生变化时重新运行 CMake。
- 如果一些组件保存在第三方 Git 仓库中（不在项目仓库或 ESP-IDF 仓库），则可以在组件 CMakeLists 文件中调用 ``git_describe`` 函数，以便在 Git 修订版本发生变化时自动重启 CMake。
- 如果没有使用 Git，请记住在源文件发生变化时手动运行 ``idf.py reconfigure``。
- 使用 ``idf_component_register`` 的 ``SRCS`` 参数来列出项目组件中的所有源文件则可以完全避免这一问题。

具体选择哪一方式，就要取决于项目本身，以及项目用户。


.. _build_system_metadata:

构建系统的元数据
================

为了将 ESP-IDF 集成到 IDE 或者其它构建系统中，CMake 在构建的过程中会在 ``build/`` 目录下生成大量元数据文件。运行 ``cmake`` 或 ``idf.py reconfigure`` （或任何其它 ``idf.py`` 构建命令），可以重新生成这些元数据文件。

- ``compile_commands.json`` 是标准格式的 JSON 文件，它描述了在项目中参与编译的每个源文件。CMake 其中的一个功能就是生成此文件，许多 IDE 都知道如何解析此文件。
- ``project_description.json`` 包含有关 ESP-IDF 项目、已配置路径等的一些常规信息。
- ``flasher_args.json`` 包含 esptool.py 工具用于烧录项目二进制文件的参数，此外还有 ``flash_*_args`` 文件，可直接与 esptool.py 一起使用。更多详细信息请参阅 :ref:`flash_parameters`。
- ``CMakeCache.txt`` 是 CMake 的缓存文件，包含 CMake 进程、工具链等其它信息。
- ``config/sdkconfig.json`` 包含 JSON 格式的项目配置结果。
- ``config/kconfig_menus.json`` 是在 menuconfig 中显示菜单的 JSON 格式版本，用于外部 IDE 的 UI。


JSON 配置服务器
---------------

``kconfserver`` 工具可以帮助 IDE 轻松地与配置系统的逻辑进行集成，它运行在后台，通过使用 stdin 和 stdout 读写 JSON 文件的方式与调用进程交互。

你可以通过 ``idf.py confserver`` 或 ``ninja kconfserver`` 从项目中运行 ``kconfserver``，也可以使用不同的构建生成器来触发类似的目标。

有关 kconfserver 的更多信息，请参阅 `esp-idf-kconfig 文档 <https://github.com/espressif/esp-idf-kconfig/blob/master/docs/DOCUMENTATION.md>`_。


构建系统内部
=======================

构建脚本
-------------

ESP-IDF 构建系统的列表文件位于 :idf:`/tools/cmake` 中。实现构建系统核心功能的模块如下

    - build.cmake - 构建相关命令，即构建初始化、检索/设置构建属性、构建处理。
    - component.cmake - 组件相关的命令，如添加组件、检索/设置组件属性、注册组件。
    - kconfig.cmake - 从 Kconfig 文件中生成配置文件（sdkconfig、sdkconfig.h、sdkconfig.cmake 等）。
    - ldgen.cmake - 从链接器片段文件生成最终链接器脚本。
    - target.cmake - 设置构建目标和工具链文件。
    - utilities.cmake - 其它帮助命令。

 除了这些文件，还有两个重要的 CMake 脚本在 :idf:`/tools/cmake` 中：

    - idf.cmake - 设置构建参数并导入上面列出的核心模块。之所以包括在 CMake 项目中，是为了方便访问 ESP-IDF 构建系统功能。
    - project.cmake - 导入 ``idf.cmake``，并提供了一个自定义的 ``project()`` 命令，该命令负责处理建立可执行文件时所有的繁重工作。包含在标准 ESP-IDF 项目的顶层 CMakeLists.txt 中。

:idf:`/tools/cmake` 中的其它文件都是构建过程中的支持性文件或第三方脚本。


构建过程
-------------

本节介绍了标准的 ESP-IDF 应用构建过程。构建过程可以大致分为四个阶段：

.. blockdiag::
    :scale: 100%
    :caption: ESP-IDF Build System Process
    :align: center

    blockdiag idf-build-system-process {
        初始化 -> 枚举
        枚举 -> 处理
        处理 -> 完成
    }


初始化
^^^^^^^

 该阶段为构建设置必要的参数。

    - 在将 ``idf.cmake`` 导入 ``project.cmake`` 后，将执行以下步骤：
        - 在环境变量中设置 ``IDF_PATH`` 或从顶层 CMakeLists.txt 中包含的 ``project.cmake`` 路径推断相对路径。
        - 将 :idf:`/tools/cmake` 添加到 ``CMAKE_MODULE_PATH`` 中，并导入核心模块和各种辅助/第三方脚本。
        - 设置构建工具/可执行文件，如默认的 Python 解释器。
        - 获取 ESP-IDF git 修订版，并存储为 ``IDF_VER``。
        - 设置全局构建参数，即编译选项、编译定义、包括所有组件的 include 目录。
        - 将 :idf:`components` 中的组件添加到构建中。
    - 自定义 ``project()`` 命令的初始部分执行以下步骤：
        - 在环境变量或 CMake 缓存中设置 ``IDF_TARGET`` 以及设置相应要使用的 ``CMAKE_TOOLCHAIN_FILE``。
        - 添加 ``EXTRA_COMPONENT_DIRS`` 中的组件至构建中
        - 从 ``COMPONENTS``/ ``EXCLUDE_COMPONENTS``、``SDKCONFIG``、``SDKCONFIG_DEFAULTS`` 等变量中为调用命令 ``idf_build_process()`` 准备参数。

调用 ``idf_build_process()`` 命令标志着这个阶段的结束。


枚举
^^^^^^^^^^^
  这个阶段会建立一个需要在构建过程中处理的组件列表，该阶段在 ``idf_build_process()`` 的前半部分进行。

    - 检索每个组件的公共和私有依赖。创建一个子进程，以脚本模式执行每个组件的 CMakeLists.txt。``idf_component_register`` REQUIRES 和 PRIV_REQUIRES 参数的值会返回给父进程。这就是所谓的早期扩展。在这一步中定义变量 ``CMAKE_BUILD_EARLY_EXPANSION``。
    - 根据公共和私有的依赖关系，递归地导入各个组件。


处理
^^^^^^^

  该阶段处理构建中的组件，是 ``idf_build_process()`` 的后半部分。

  - 从 sdkconfig 文件中加载项目配置，并生成 sdkconfig.cmake 和 sdkconfig.h 头文件。这两个文件分别定义了可以从构建脚本和 C/C++ 源文件/头文件中访问的配置变量/宏。
  - 导入各组件的 ``project_include.cmake``。
  - 将每个组件添加为一个子目录，处理其 CMakeLists.txt。组件 CMakeLists.txt 调用注册命令 ``idf_component_register`` 添加源文件、导入目录、创建组件库、链接依赖关系等。


完成
^^^^^^^
  该阶段是 ``idf_build_process()`` 剩余的步骤。

  - 创建可执行文件并将其链接到组件库中。
  - 生成 project_description.json 等项目元数据文件并且显示所建项目等相关信息。

请参考 :idf_file:`/tools/cmake/project.cmake` 获取更多信息。


.. _migrating_from_make:

从 ESP-IDF GNU Make 构建系统迁移到 CMake 构建系统
=================================================

ESP-IDF CMake 构建系统与旧版的 GNU Make 构建系统在某些方面非常相似，开发者都需要提供 include 目录、源文件等。然而，有一个语法上的区别，即对于 ESP-IDF CMake 构建系统，开发者需要将这些作为参数传递给注册命令 ``idf_component_register``。


自动转换工具
-------------------------

在 ESP-IDF v4.x 版本中，`tools/cmake/convert_to_cmake.py` 提供了项目自动转换工具。由于该脚本依赖于 `make` 构建系统，所以 v5.0 版本中不包含该脚本。


CMake 中不可用的功能
--------------------

有些功能已从 CMake 构建系统中移除，或者已经发生很大改变。GNU Make 构建系统中的以下变量已从 CMake 构建系统中删除：

- ``COMPONENT_BUILD_DIR``：由 ``CMAKE_CURRENT_BINARY_DIR`` 替代。
- ``COMPONENT_LIBRARY``：默认为 ``$(COMPONENT_NAME).a`` 但是库名可以被组件覆盖。在 CMake 构建系统中，组件库名称不可再被组件覆盖。
- ``CC``、``LD``、``AR``、``OBJCOPY``：gcc xtensa 交叉工具链中每个工具的完整路径。CMake 使用 ``CMAKE_C_COMPILER``、``CMAKE_C_LINK_EXECUTABLE`` 和 ``CMAKE_OBJCOPY`` 进行替代。完整列表请参阅 `CMake 语言变量 <cmake language variables_>`_。
- ``HOSTCC``、``HOSTLD``、``HOSTAR``：宿主机本地工具链中每个工具的全名。CMake 系统不再提供此变量，外部项目需要手动检测所需的宿主机工具链。
- ``COMPONENT_ADD_LDFLAGS``：用于覆盖链接标志。CMake 中使用 `target_link_libraries`_ 命令替代。
- ``COMPONENT_ADD_LINKER_DEPS``：链接过程依赖的文件列表。`target_link_libraries`_ 通常会自动推断这些依赖。对于链接脚本，可以使用自定义的 CMake 函数 ``target_linker_scripts``。
- ``COMPONENT_SUBMODULES``：不再使用。CMake 会自动枚举 ESP-IDF 仓库中所有的子模块。
- ``COMPONENT_EXTRA_INCLUDES``：曾是 ``COMPONENT_PRIV_INCLUDEDIRS`` 变量的替代版本，仅支持绝对路径。CMake 系统中统一使用 ``COMPONENT_PRIV_INCLUDEDIRS`` （可以是相对路径，也可以是绝对路径）。
- ``COMPONENT_OBJS``：以前，可以以目标文件列表的方式指定组件源，现在，可以通过 ``COMPONENT_SRCS`` 以源文件列表的形式指定组件源。
- ``COMPONENT_OBJEXCLUDE``：已被 ``COMPONENT_SRCEXCLUDE`` 替换。用于指定源文件（绝对路径或组件目录的相对路径）。
- ``COMPONENT_EXTRA_CLEAN``：已被 ``ADDITIONAL_MAKE_CLEAN_FILES`` 属性取代，注意，:ref:`CMake 对此项功能有部分限制 <ADDITIONAL_MAKE_CLEAN_FILES_note>`。
- ``COMPONENT_OWNBUILDTARGET`` & ``COMPONENT_OWNCLEANTARGET``：已被 CMake `外部项目 <ExternalProject_>` 替代，详细内容请参阅 :ref:`component-build-full-override`。
- ``COMPONENT_CONFIG_ONLY``：已被 ``register_config_only_component()`` 函数替代，请参阅 :ref:`config_only_component`。
- ``CFLAGS``、``CPPFLAGS``、``CXXFLAGS``：已被相应的 CMake 命令替代，请参阅 :ref:`component_build_control`。


无默认值的变量
--------------

以下变量不再具有默认值：

- 源目录（Make 中的 ``COMPONENT_SRCDIRS`` 变量，CMake 中 ``idf_component_register`` 的 ``SRC_DIRS`` 参数）
- include 目录（Make 中的 ``COMPONENT_ADD_INCLUDEDIRS`` 变量，CMake 中 ``idf_component_register`` 的 ``INCLUDE_DIRS`` 参数）


不再需要的变量
--------------

在 CMake 构建系统中，如果设置了 ``COMPONENT_SRCS``，就不需要再设置 ``COMPONENT_SRCDIRS``。实际上，CMake 构建系统中如果设置了 ``COMPONENT_SRCDIRS``，那么 ``COMPONENT_SRCS`` 就会被忽略。


从 Make 中烧录
--------------

仍然可以使用 ``make flash`` 或者类似的目标来构建和烧录，但是项目 ``sdkconfig`` 不能再用来指定串口和波特率。可以使用环境变量来覆盖串口和波特率的设置，详情请参阅 :ref:`flash-with-ninja-or-make`。

.. _esp-idf-template: https://github.com/espressif/esp-idf-template
.. _Cmake: https://cmake.org
.. _ninja: https://ninja-build.org
.. _esptool.py: https://github.com/espressif/esptool/#readme
.. _CMake v3.16 官方文档: https://cmake.org/cmake/help/v3.16/index.html
.. _cmake 命令行文档: https://cmake.org/cmake/help/v3.16/manual/cmake.1.html#options
.. _cmake add_library: https://cmake.org/cmake/help/v3.16/command/add_library.html
.. _cmake if: https://cmake.org/cmake/help/v3.16/command/if.html
.. _cmake list: https://cmake.org/cmake/help/v3.16/command/list.html
.. _cmake project: https://cmake.org/cmake/help/v3.16/command/project.html
.. _cmake set: https://cmake.org/cmake/help/v3.16/command/set.html
.. _cmake string: https://cmake.org/cmake/help/v3.16/command/string.html
.. _cmake faq generated files: https://gitlab.kitware.com/cmake/community/-/wikis/FAQ#how-can-i-generate-a-source-file-during-the-build
.. _ADDITIONAL_MAKE_CLEAN_FILES: https://cmake.org/cmake/help/v3.16/prop_dir/ADDITIONAL_MAKE_CLEAN_FILES.html
.. _ExternalProject: https://cmake.org/cmake/help/v3.16/module/ExternalProject.html
.. _cmake language variables: https://cmake.org/cmake/help/v3.16/manual/cmake-variables.7.html#variables-for-languages
.. _set_source_files_properties: https://cmake.org/cmake/help/v3.16/command/set_source_files_properties.html
.. _target_compile_options: https://cmake.org/cmake/help/v3.16/command/target_compile_options.html
.. _target_link_libraries: https://cmake.org/cmake/help/v3.16/command/target_link_libraries.html#command:target_link_libraries
.. _cmake_toolchain_file: https://cmake.org/cmake/help/v3.16/variable/CMAKE_TOOLCHAIN_FILE.html
.. _LINK_INTERFACE_MULTIPLICITY: https://cmake.org/cmake/help/v3.16/prop_tgt/LINK_INTERFACE_MULTIPLICITY.html
.. _quirc: https://github.com/dlbeer/quirc
.. _pyenv: https://github.com/pyenv/pyenv#readme
.. _virtualenv: https://virtualenv.pypa.io/en/stable/
