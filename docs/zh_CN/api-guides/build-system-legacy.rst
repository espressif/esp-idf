构建系统 (传统 GNU Make)
==========================
:link_to_translation:`en:[English]`

.. include:: ../gnu-make-legacy.rst

本文将介绍乐鑫物联网开发框架中的 ``构建系统`` 和 ``组件`` 的相关概念。

如果您想了解如何构建一个新的 ESP-IDF 项目，请阅读本文档。

我们建议您使用 `ESP-IDF 模板工程 <https://github.com/espressif/esp-idf-template>`_ 来开始您的新项目。

使用构建系统
------------

ESP-IDF 的 :idf_file:`README.md` 文件对如何使用构建系统来构建项目作了简要的说明。

概述
----

一个 ESP-IDF 项目可以看作是许多不同组件的集合，例如对于一个展示当前湿度的网站服务器来说，它可能会包含如下一些组件：

-  ESP32 基础库（libc，rom bindings 等）
-  WiFi 驱动库
-  TCP/IP 协议栈
-  FreeRTOS 操作系统
-  网站服务器
-  湿度传感器的驱动
-  将上述组件组织在一起的主代码

ESP-IDF 可以显式地指定和配置每个组件。在构建项目的时候，构建系统会查找 ESP-IDF 目录、项目目录和用户自定义目录（可选）中所有的组件，然后使用基于文本的菜单系统让用户配置 ESP-IDF 项目中需要的每个组件。在配置结束后，构建系统开始编译整个项目。

概念
~~~~

-  ``项目`` 特指一个目录，其中包含了构建可执行文件的所有源文件和配置，还有其他的支持型输出文件，比如分区表、数据/文件系统分区和引导程序。

-  ``项目配置`` 保存在项目根目录下名为 sdkconfig 的文件中，它可以通过 ``make menuconfig`` 进行修改，且一个项目只能包含一个项目配置。

-  ``应用程序`` 是由 ESP-IDF 构建得到的可执行文件。一个项目通常会构建两个应用程序：项目应用程序（主可执行文件，即用户自定义的固件）和引导程序（启动并初始化项目应用程序的引导程序）。

-  ``组件`` 是模块化的、独立的代码，它们被编译成静态库（.a 文件）后再链接成应用程序，有些组件是 ESP-IDF 官方提供的，有些则可能来自其它项目。

以下内容并不是项目的组成部分：

-  ``ESP-IDF`` 并不是项目的一部分，相反，它是独立的，并通过 IDF_PATH 环境变量链接到项目中，这样做就可以使 IDF 框架与您的项目分离，其中 IDF_PATH 变量保存了 ESP-IDF 目录的路径。

-  交叉编译工具链并不是项目的组成部分，它应该被安装在系统 PATH 环境变量中，或者可以在项目配置中显式指定工具链的前缀为本地的安装路径。

示例项目
~~~~~~~~

示例项目的目录树结构可能如下所示::

    - myProject/
                 - Makefile
                 - sdkconfig
                 - components/ - component1/ - component.mk
                                             - Kconfig
                                             - src1.c
                               - component2/ - component.mk
                                             - Kconfig
                                             - src1.c
                                             - include/ - component2.h
                 - main/       - src1.c
                               - src2.c
                               - component.mk

                 - build/

该示例项目 ``myProject`` 包含以下组成部分：

-  项目顶层 Makefile，该 Makefile 设置了 ``PROJECT_NAME`` 变量，还可以定义作用于整个项目的其它 make 变量（可选）。顶层 Makefile 会导入核心 Makefile 文件 ``$(IDF_PATH)/make/project.mk`` ，由它负责实现 ESP-IDF 构建系统的剩余部分。

-  项目配置文件 sdkconfig，执行 ``make menuconfig`` 后会创建或更新此文件，该文件中保存了项目中所有组件的配置信息（包括 ESP-IDF 本身）。``sdkconfig`` 文件可能会也可能不会被添加到项目的源代码管理系统中。

-  可选的组件目录中包含了属于项目一部分的自定义组件，不是每一个项目都需要它，但它有助于构建可重用代码或者导入第三方组件。

-  ``main`` 目录是一个特殊的 ``伪组件``，它包含项目本身的源代码。``main`` 是默认名称，Makefile 变量 ``COMPONENT_DIRS`` 默认会导入此组件，但您也可以修改此变量（或者设置 ``EXTRA_COMPONENT_DIRS`` ）以查找其他位置的组件。

-  ``build`` 目录在项目构建的时候创建或者更新，里面包含有构建生成的临时目标文件和库以及最终输出的二进制文件。此目录通常不会被添加到项目的源代码管理系统中，也不会随着项目源代码被发布。

组件目录中会包含组件自己的 Makefile 文件 ``component.mk`` ，里面会定义一些变量来控制该组件的构建过程，以及它与整个项目的集成。更多详细信息请参考 `组件 Makefiles <#component-makefiles>`_。

每个组件还可以包含一个 ``Kconfig`` 文件，它用于定义 ``menuconfig`` 时展示的组件配置信息的选项规则。某些组件还可能还会包含 ``Kconfig.projbuild`` 和 ``Makefile.projbuild`` 特殊文件，他们可以用来覆盖项目的部分配置。

项目 Makefiles
~~~~~~~~~~~~~~

每个项目都有一个 Makefile ，它包含整个项目的构建设置。默认情况下，项目 Makefile 可以非常小。

最小 Makefile 示例
^^^^^^^^^^^^^^^^^^

::

   PROJECT_NAME := myProject

   include $(IDF_PATH)/make/project.mk

必须设置的项目变量
^^^^^^^^^^^^^^^^^^

-  ``PROJECT_NAME``: 项目名称，最终输出的二进制文件也使用该名称，即 myProject.bin，myProject.elf 。

可选的项目变量
^^^^^^^^^^^^^^

以下这些变量都有默认值，用户可以重写这些变量以自定义构建行为。查看 ``make/project.mk`` 文件可以获得所有的实现细节。

-  ``PROJECT_PATH``：顶层项目目录，默认是包含 Makefile 文件的目录，许多其他的项目变量都基于此变量。注意，项目路径中不能包含有空格。
-  ``BUILD_DIR_BASE``：所有对象、库、二进制文件的输出目录，默认为 ``$(PROJECT_PATH)/build``。
-  ``COMPONENT_DIRS``：组件的搜索目录，默认为 ``$(IDF_PATH)/components``，``$(PROJECT_PATH)/components``，``$(PROJECT_PATH)/main`` 和 ``EXTRA_COMPONENT_DIRS`` 。如果您不希望从这些目录中搜索组件，请重写此变量。
-  ``EXTRA_COMPONENT_DIRS``：组件额外的搜索路径，可选。
-  ``COMPONENTS``： 要构建进项目中的组件列表，默认为 ``COMPONENT_DIRS`` 指定目录中所有的组件。
-  ``EXCLUDE_COMPONENTS``： 在构建的过程中需要剔除的组件列表，可选。请注意这只会减少构建的时间，并不会减少最终二进制文件的大小。
-  ``TEST_EXCLUDE_COMPONENTS``：在构建单元测试的过程中需要剔除的组件列表，可选。

以上这些 Makefile 变量中的任何路径都要使用绝对路径，您可以使用 ``$(PROJECT_PATH)/xxx``，``$(IDF_PATH)/xxx``，或者使用 Make 内置函数 ``$(abspath xxx)`` 将相对路径转换为绝对路径。

以上这些变量要在 Makefile 中 ``include $(IDF_PATH)/make/project.mk`` 的前面进行设置。

.. _component-makefiles:

组件 Makefiles
~~~~~~~~~~~~~~

每个项目都包含一个或者多个组件，这些组件可以是 ESP-IDF 的一部分，也可以从其他组件目录添加。

组件是包含 ``component.mk`` 文件的任何目录。

搜索组件
~~~~~~~~

搜索 ``COMPONENT_DIRS`` 中指定的目录以查找项目会使用的组件，目录可以是组件本身（即他们包含 ``component.mk`` 文件），也可以是包含组件的上层目录。

运行 ``make list-components`` 命令可以查询这些变量的值，这有助于调试组件的搜索路径是否正确。

同名组件
^^^^^^^^

ESP-IDF 搜索组件时，会按照 ``COMPONENT_DIRS`` 指定的顺序依次进行，这意味着在默认情况下，首先是 ESP-IDF 组件，然后是项目组件，最后是 ``EXTRA_COMPONENT_DIRS`` 中的组件。如果这些目录中的两个或者多个包含具有相同名字的组件，则使用搜索到的最后一个位置的组件。这就允许将组件复制到项目目录中再修改来覆盖 ESP-IDF 组件，如果使用这种方式，ESP-IDF 目录本身可以保持不变。

.. _minimal-component-makefile:

最小组件 Makefile
^^^^^^^^^^^^^^^^^

最简单的 ``component.mk`` 文件可以是一个空文件，如果文件为空，则组件的默认构建行为会被设置为：

-  makefile 所在目录中的所有源文件（``*.c``，``*.cpp``，``*.cc``，``*.S``）将会被编译进组件库中。
-  子目录 ``include`` 将被添加到其他组件的全局头文件搜索路径中。
-  组件库将会被链接到项目的应用程序中。

更完整的组件 makefile 可以查看 `组件 Makefile 示例 <#example-component-makefile>`_。

请注意，空的 ``component.mk`` 文件同没有 ``component.mk`` 文件之间存在本质差异，前者会调用默认的组件构建行为，后者不会发生默认的组件构建行为。一个组件中如果只包含影响项目配置或构建过程的文件，那么它可以没有 ``component.mk`` 文件。

.. _preset-component-variables:

预设的组件变量
^^^^^^^^^^^^^^

以下特定于组件的变量可以在 ``component.mk`` 中使用，但不应该被修改。

-  ``COMPONENT_PATH``： 组件的目录，即包含 ``component.mk`` 文件的绝对路径，路径中不能包含空格。
-  ``COMPONENT_NAME``： 组件的名字，默认为组件目录的名称。
-  ``COMPONENT_BUILD_DIR``： 组件的构建目录，即存放组件构建输出的绝对路径，它是 `$(BUILD_DIR_BASE)` 的子目录。该变量也是构建组件时的当前工作目录，所以 make 中的相对路径都以此目录为基础。
-  ``COMPONENT_LIBRARY``： 组件构建后的静态库文件（相对于组件的构建目录）的名字，默认为 ``$(COMPONENT_NAME).a``。

以下变量在项目顶层中设置，并被导出到组件中构建时使用：

-  ``PROJECT_NAME``： 项目名称，在项目的 Makefile 中设置。
-  ``PROJECT_PATH``： 包含项目 Makefile 的目录的绝对路径。
-  ``COMPONENTS``： 此次构建中包含的所有组件的名字。
-  ``CONFIG_*``： 项目配置中的每个值在 make 中都对应一个以 ``CONFIG_`` 开头的变量。
-  ``CC``，``LD``，``AR``，``OBJCOPY``： gcc xtensa 交叉编译工具链中每个工具的完整路径。
-  ``HOSTCC``，``HOSTLD``，``HOSTAR``： 主机本地工具链中每个工具的全名。
-  ``IDF_VER``： ESP-IDF 的版本号，可以通过检索 ``$(IDF_PATH)/version.txt`` 文件（假如存在的话）或者使用 git 命令 ``git describe`` 来获取。这里推荐的格式是在一行中指定主 IDF 的发布版本号，例如标记为 ``v2.0`` 的发布版本或者是标记任意一次提交记录的 ``v2.0-275-g0efaa4f``。应用程序可以通过调用 :cpp:func:`esp_get_idf_version` 函数来使用该变量。
-  ``IDF_VERSION_MAJOR``, ``IDF_VERSION_MINOR``, ``IDF_VERSION_PATCH``: ESP-IDF 的组件版本，可用于条件表达式。请注意这些信息的精确度不如 ``IDF_VER`` 变量，版本号 ``v4.0-dev-*``， ``v4.0-beta1``， ``v4.0-rc1`` 和 ``v4.0`` 对应的 ``IDF_VERSION_*`` 变量值是相同的，但是 ``IDF_VER`` 的值是不同的。
- ``PROJECT_VER``：项目版本。

  * 如果设置了 :ref:`CONFIG_APP_PROJECT_VER_FROM_CONFIG` 选项，则将使用 :ref:`CONFIG_APP_PROJECT_VER` 的值作为项目版本。
  * 如果在项目 Makefile 文件中设置了 ``PROJECT_VER`` 变量，则使用该值作为项目版本。
  * 如果存在 ``$PROJECT_PATH/version.txt``，则用其内容为 ``PROJECT_VER``。
  * 如果项目位于 Git 仓库中，将使用 git 描述部分的内容作为项目版本。
  * 如果以上均不存在，则 ``PROJECT_VER`` 为 "1"。

如果您在 ``component.mk`` 文件中修改这些变量，这并不会影响其它组件的构建，但可能会使您的组件变得难以构建或调试。

.. _optional-project-wide-component-variables:

可选的项目通用组件变量
^^^^^^^^^^^^^^^^^^^^^^

可以在 ``component.mk`` 中设置以下变量来控制整个项目的构建行为：

-  ``COMPONENT_ADD_INCLUDEDIRS``： 相对于组件目录的路径，将被添加到项目中所有组件的头文件搜索路径中。如果该变量未被覆盖，则默认为 ``include`` 目录。如果一个头文件路径仅仅为当前组件所用，那么应该将该路径添加到 ``COMPONENT_PRIV_INCLUDEDIRS`` 中。
-  ``COMPONENT_ADD_LDFLAGS``： 添加链接参数到全局 ``LDFLAGS`` 中用以指导链接最终的可执行文件，默认为 ``-l$(COMPONENT_NAME)``。如果将预编译好的库添加到此目录，请使用它们为绝对路径，即 ``$(COMPONENT_PATH)/libwhatever.a``。
-  ``COMPONENT_DEPENDS``： 需要在当前组件之前构建的组件列表，这对于处理链接时的依赖不是必需的，因为所有组件的头文件目录始终可用。如果一个组件会生成一个头文件，然后另外一个组件需要使用它，此时该变量就有必要进行设置。大多数的组件不需要设置此变量。
-  ``COMPONENT_ADD_LINKER_DEPS``： 保存一些文件的路径，当这些文件发生改变时，会触发 ELF 文件重新链接。该变量通常用于链接脚本文件和二进制文件，大多数的组件不需要设置此变量。

以下变量仅适用于属于 ESP-IDF 的组件：

-  ``COMPONENT_SUBMODULES``： 组件使用的 git 子模块的路径列表（相对于 ``COMPONENT_PATH``）。这些路径会在构建的过程中被检查（并在必要的时候初始化）。如果组件位于 ``IDF_PATH`` 目录之外，则忽略此变量。


可选的组件特定变量
^^^^^^^^^^^^^^^^^^

以下变量可以在 ``component.mk`` 中进行设置，用以控制该组件的构建行为：

-  ``COMPONENT_PRIV_INCLUDEDIRS``： 相对于组件目录的目录路径，该目录仅会被添加到该组件源文件的头文件搜索路径中。
-  ``COMPONENT_EXTRA_INCLUDES``： 编译组件的源文件时需要指定的额外的头文件搜索路径，这些路径将以 ``-l`` 为前缀传递给编译器。这和  ``COMPONENT_PRIV_INCLUDEDIRS`` 变量的功能有些类似，但是这些路径不会相对于组件目录进行扩展。
-  ``COMPONENT_SRCDIRS``： 相对于组件目录的目录路径，这些路径用于搜索源文件（``*.cpp``，``*.c``，``*.S``），默认为 ``.``，即组件目录本身。重写该变量可以指定包含源文件的不同目录列表。
-  ``COMPONENT_OBJS``： 要编译生成的目标文件，默认是 ``COMPONENT_SRCDIRS`` 中每个源文件的 .o 文件。重写该变量将允许您剔除 ``COMPONENT_SRCDIRS`` 中的某些源文件，否则他们将会被编译。相关示例请参阅 `指定需要编译的组件源文件 <#specify-source-files>`_。
-  ``COMPONENT_EXTRA_CLEAN``： 相对于组件构建目录的路径，指向 ``component.mk`` 文件中自定义 make 规则生成的任何文件，它们也是 ``make clean`` 命令需要删除的文件。相关示例请参阅 `源代码生成 <#source-code-generation>`_。
-  ``COMPONENT_OWNBUILDTARGET`` & ``COMPONENT_OWNCLEANTARGET``： 这些目标允许您完全覆盖组件的默认编译行为。有关详细信息，请参阅 `完全覆盖组件的 Makefile <#fully-overriding-component-makefile-legacy>`_。
-  ``COMPONENT_CONFIG_ONLY``： 如果设置了此标志，则表示组件根本不会产生构建输出（即不会构建得到 ``COMPONENT_LIBRARY``），并且会忽略大多数其它组件变量。此标志用于 IDF 内部组件，其中仅包含 ``KConfig.projbuild`` 和/或 ``Makefile.projbuild`` 文件来配置项目，但是没有源文件。
-  ``CFLAGS``： 传递给 C 编译器的标志。根据项目设置已经定义一组默认的 ``CFLAGS``，可以通过 ``CFLAGS +=`` 来为组件添加特定的标志，也可以完全重写该变量（尽管不推荐这么做）。
-  ``CPPFLAGS``： 传递给 C 预处理器的标志（用于 ``.c``，``.cpp`` 和 ``.S`` 文件）。根据项目设置已经定义一组默认的 ``CPPFLAGS`` ，可以通过 ``CPPFLAGS +=`` 来为组件添加特定的标志，也可以完全重写该变量（尽管不推荐这么做）。
-  ``CXXFLAGS``： 传递给 C++ 编译器的标志。根据项目设置已经定义一组默认的 ``CXXFLAGS`` ，可以通过 ``CXXFLAGS +=`` 来为组件添加特定的标志，也可以完全重写该变量（尽管不推荐这么做）。
- ``COMPONENT_ADD_LDFRAGMENTS``：链接器脚本生成功能的链接器片段文件的路径，请查看 :doc:`链接器脚本生成 <linker-script-generation>`.

如果要将编译标志应用于单个源文件，您可以将该源文件的目标规则覆盖，例如::

  apps/dhcpserver.o: CFLAGS += -Wno-unused-variable

如果上游代码在编译的时候发出了警告，那这么做可能会很有效。

配置组件
~~~~~~~~

每个组件都可以包含一个 Kconfig 文件，和 ``component.mk`` 放在同一个目录下。Kconfig 中包含此组件在 ``make menuconfig`` 时要展示的配置规则的设置。

运行 menuconfig 时，可以在 ``Component Settings`` 菜单栏下找到这些设置。

创建一个组件的 Kconfig 文件，最简单的方法就是使用 ESP-IDF 中现有的 Kconfig 文件作为模板，在这基础上进行修改。

有关示例请参阅 `添加条件配置 <#add-conditional-configuration>`_。

预处理器定义
~~~~~~~~~~~~

ESP-IDF 构建系统会在命令行中添加以下 C 预处理定义：

-  ``ESP_PLATFORM`` — 可以用来检测在 ESP-IDF 内发生的构建行为。
-  ``IDF_VER`` — ESP-IDF 的版本，请参阅 `预设的组件变量 <#preset-component-variables>`_。

构建的内部过程
~~~~~~~~~~~~~~

顶层：项目 Makefile
^^^^^^^^^^^^^^^^^^^

-  ``make`` 始终从项目目录处运行，并且项目的 makefile 名字通常为 Makefile 。
-  项目的 makefile 文件会设置 ``PROJECT_NAME`` ，并且可以自定义其他可选的项目变量。
-  项目 makefile 文件会导入 ``$(IDF_PATH)/make/project.mk`` ，该文件中会导入项目级的 Make 逻辑。
-  ``project.mk`` 填写默认的项目级 make 变量，并导入项目配置中的 make 变量。如果生成的包含项目配置的 makefile 文件已经过期，那么它将会被重新生成（通过 ``project_config.mk`` 中的目标规则），然后 make 进程从顶层重新开始。
-  ``project.mk`` 根据默认组件目录或者可选项目变量中设置的自定义组件列表来编译组件。
-  每个组件都可以设置一些 `可选的项目通用组件变量 <#optional-project-wide-component-variables>`_ ，他们会通过 ``component_project_vars.mk`` 被导入 ``project.mk`` 文件中。如果这些文件有缺失或者过期，他们会被重新生成（通过对组件 makefile 的递归调用），然后 make 进程从顶层重新开始。
-  组件中的 Makefile.projbuild 文件被包含在了 make 的进程中，以添加额外的目标或者配置。
-  默认情况下，项目 makefile 还为每个组件生成顶层的编译和清理目标，并设置 app 和 clean 目标来调用所有这些子目标。
-  为了编译每个组件，对组件 makefile 执行递归构建。

为了更好地理解项目的构建过程，请通读 ``project.mk`` 文件。

第二层：组件 Makefile 文件
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

-  每次调用组件 makefile 文件都是通过 ``$(IDF_PATH)/make/component_wrapper.mk`` 这个包装器进行的。
-  此组件包装器包含了所有组件的 ``Makefile.componentbuild`` 文件，使这些文件中的任何配置，变量都可用于每个组件。
-  调用 ``component_wrapper.mk`` 时将当前目录设置为组件构建目录，并将 ``COMPONENT_MAKEFILE`` 变量设置为 ``component.mk`` 的绝对路径。
-  ``component_wrapper.mk`` 为所有组件变量设置默认值，然后导入 ``component.mk`` 文件来覆盖或修改这些变量。
-  如果未定义 ``COMPONENT_OWNBUILDTARGET`` 和 ``COMPONENT_OWNCLEANTARGET`` 文件，则会为组件的源文件和必备组件 ``COMPONENT_LIBRARY`` 静态库文件创建默认构建和清理目标。
-  ``component_project_vars.mk`` 文件在 ``component_wrapper.mk`` 中有自己的目标，如果由于组件的 makefile 或者项目配置的更改而需要重建此文件，则从 ``project.mk`` 文件中进行评估。

为了更好地理解组件制作过程，请阅读 ``component_wrapper.mk`` 文件和 ESP-IDF 中的 ``component.mk`` 文件。

以非交互的方式运行 Make
~~~~~~~~~~~~~~~~~~~~~~~

如果在运行 ``make`` 的时候不希望出现交互式提示（例如：在 IDE 或自动构建系统中），可以将 ``BATCH_BUILD=1`` 添加到 make 的参数中（或者将其设置为环境变量）。

设置 ``BATCH_BUILD`` 意味着：

-  详细输出（与 ``V=1`` 相同，见下文），如果不需要详细输出，就设置 ``V=0`` 。
-  如果项目配置缺少新配置项（来自新组件或者 ESP-IDF 更新），则项目使用默认值，而不是提示用户输入每个项目。
-  如果构建系统需要调用 ``menuconfig`` ，则会打印错误并且构建失败。

.. _make-size:

构建目标的进阶用法
~~~~~~~~~~~~~~~~~~

- ``make app``，``make bootloader``，``make partition table`` 可以根据需要为项目单独构建生成应用程序文件、启动引导文件和分区表文件。
- ``make erase_flash`` 和 ``make erase_otadata`` 会调用 esptool.py 脚本分别擦除整块闪存芯片或者其中 OTA 分区的内容。
- ``make size`` 会打印应用程序的大小信息。``make size-components`` 和 ``make size-files`` 两者功能相似，分别打印每个组件或者每个源文件大小的详细信息。

调试 Make 的过程
~~~~~~~~~~~~~~~~

调试 ESP-IDF 构建系统的一些技巧：

-  将 ``V=1`` 添加到 make 的参数中（或将其设置为环境变量）将使 make 回显所有已经执行的命令，以及为子 make 输入的每个目录。
-  运行 ``make -w`` 将导致 make 在为子 make 输入时回显每个目录——与 ``V=1`` 相同但不回显所有命令。
-  运行 ``make --trace`` （可能除了上述参数之一）将打印出构建时的每个目标，以及导致它构建的依赖项）。
-  运行 ``make -p`` 会打印每个 makefile 中每个生成的目标的（非常详细的）摘要。

更多调试技巧和通用的构建信息，请参阅 `GNU 构建手册 <http://www.gnu.org/software/make/manual/make.html>`_。

.. _warn-undefined-variables-legacy:

警告未定义的变量
^^^^^^^^^^^^^^^^

默认情况下，如果引用了未定义的变量（如 ``$(DOES_NOT_EXIST)`` ，构建过程将会打印警告，这对于查找变量名称中的错误非常有用。

如果不想要此行为，可以在 menuconfig 顶层菜单下的 `SDK tool configuration` 中禁用它。

请注意，如果在 Makefile 中使用 ``ifdef`` 或者 ``ifndef`` ，则此选项不会出发警告。

覆盖项目的部分内容
~~~~~~~~~~~~~~~~~~

Makefile.projbuild
^^^^^^^^^^^^^^^^^^

如果一个组件含有必须要在项目构建过程的顶层进行计算的变量，则可以在组件目录下创建名为 ``Makefile.projbuild`` 的文件，项目在执行 ``project.mk`` 的时候会导入此 makefile 。

例如，如果您的组件需要为整个项目添加 CFLAGS（不仅仅是为自身的源文件），那么可以在 ``Makefile.projbuild`` 中设置 ``CFLAGS +=`` 。

``Makefile.projbuild`` 文件在 ESP-IDF 中大量使用，用于定义项目范围的构建功能，例如 ``esptool.py`` 命令行参数和 ``bootloader`` 这个特殊的程序。

请注意， ``Makefile.projbuild`` 对于最常见的组件不是必需的 - 例如向项目中添加 include 目录，或者将 LDFLAGS 添加到最终链接步骤，同样可以通过 ``component.mk`` 文件来自定义这些值。有关详细信息，请参阅 `可选的项目通用组件变量 <#optional-project-wide-component-variables>`_ 。

在此文件中设置变量或者目标时要小心，由于这些值包含在项目的顶层 makefile 中，因此他们可以影响或者破坏所有组件的功能！

KConfig.projbuild
^^^^^^^^^^^^^^^^^

这相当于 ``Makefile.projbuild`` 的组件配置 KConfig 文件，如果要在 menuconfig 的顶层添加配置选项，而不是在 ``组件配置`` 子菜单中，则可以在 ``component.mk`` 文件所在目录中的 KConfig.projbuild 文件中定义这些选项。

在此文件中添加配置时要小心，因为他们将包含在整个项目配置中，在可能的情况下，通常最好为组件创建和配置 KConfig 文件。

Makefile.componentbuild
^^^^^^^^^^^^^^^^^^^^^^^

对于一些特殊的组件，比如它们会使用工具从其余文件中生成源文件，这时就有必要将配置、宏或者变量的定义添加到每个组件的构建过程中。这是通过在组件目录中包含 ``Makefile.componentbuild`` 文件来实现的。此文件在 ``component.mk`` 文件之前被导入 ``component_wrapper.mk`` 中。同 ``Makefile.projbuild`` 文件一样，请留意这些文件，因为他们包含在每个组件的构建中，所有只有在编译完全不同的组件时才会出现 ``Makefile.componentbuild`` 错误。

仅配置的组件
^^^^^^^^^^^^

仅配置的组件是一类不包含源文件的特殊组件，只有 ``Kconfig.projbuild`` 和 ``Makefile.projbuild`` 文件，可以在 ``conponent.mk`` 文件中设置标志 ``COMPONENT_CONFIG_ONLY``。如果设置了此标志，则忽略大多数其他组件变量，并且不会为组件执行构建操作。

.. _example-component-makefile-legacy:

组件 Makefile 示例
~~~~~~~~~~~~~~~~~~

因为构建环境试图设置大多数情况都能工作的合理默认值，所以 ``component.mk`` 可能非常小，甚至是空的，请参考 `最小组件 Makefile <#minimal-component-makefile>`_。但是某些功能通常需要覆盖组件的变量。

以下是 ``component.mk`` 的一些更高级的示例：

增加源文件目录
^^^^^^^^^^^^^^

默认情况下，将忽略子目录。如果您的项目在子目录中而不是在组件的根目录中有源文件，那么您可以通过设置 ``COMPONENT_SRCDIRS`` 将其告知构建系统：

.. code:: 

   COMPONENT_SRCDIRS := src1 src2

构建系统将会编译 src1/ 和 src2/ 子目录中的所有源文件。

.. _specify-source-files-legacy:

指定源文件
^^^^^^^^^^

标准 ``component.mk`` 逻辑将源目录中的所有 .S 和 .c 文件添加为无条件编译的源。通过将 ``COMPONENT_OBJS`` 变量手动设置为需要生成的对象的名称，可以绕过该逻辑并对要编译的对象进行硬编码::

    COMPONENT_OBJS := file1.o file2.o thing/filea.o thing/fileb.o anotherthing/main.o
    COMPONENT_SRCDIRS := . thing anotherthing

请注意，还需要另外设置 ``COMPONENT_SRCDIRS`` 。

.. _add-conditional-configuration-legacy:

添加条件配置
^^^^^^^^^^^^

配置系统可用于有条件地编译某些文件，具体取决于 ``make menuconfig`` 中选择的选项。为此，ESP-IDF 具有 ``compile_only_if`` 和 ``compile_only_if_not`` 的宏：

``Kconfig``::

    config FOO_ENABLE_BAR
        bool "Enable the BAR feature."
        help
            This enables the BAR feature of the FOO component.

``component.mk``::

    $(call compile_only_if,$(CONFIG_FOO_ENABLE_BAR),bar.o)

从示例中可以看出， ``compile_only_if`` 宏将条件和目标文件列表作为参数。如果条件为真（在这种情况下：如果在 menuconfig 中启用了 BAR 功能），将始终编译目标文件（在本例中为 bar.o）。相反的情况也是如此，如果条件不成立，bar.o 将永远不会被编译。 ``compile_only_if_not`` 执行相反的操作，如果条件为 false 则编译，如果条件为 true 则不编译。

这也可以用于选择或者删除实现，如下所示：

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


``component.mk``::

    # If LCD is enabled, compile interface to it, otherwise compile dummy interface
    $(call compile_only_if,$(CONFIG_ENABLE_LCD_OUTPUT),lcd-real.o lcd-spi.o)
    $(call compile_only_if_not,$(CONFIG_ENABLE_LCD_OUTPUT),lcd-dummy.o)

    #We need font if either console or plot is enabled
    $(call compile_only_if,$(or $(CONFIG_ENABLE_LCD_CONSOLE),$(CONFIG_ENABLE_LCD_PLOT)), font.o)

请注意使用 Make 或者函数来包含字体文件。其他的替换函数，比如 ``and`` 和 ``if`` 也适用于此处。也可以使用不在 menuconfig 中定义的变量，ESP-IDF 使用默认的 Make 策略，判断一个空的或只包含空格的变量视为 false，而其中任何非空格的变量都为 true 。

（注意：本文档的历史版本建议将目标文件添加到 ``COMPONENT_OBJS`` 中，虽然这仍然可行，但是只有当组件中的所有目标文件都明确命名时才会起作用，并且在 ``make clean`` 后并不会清除 make 中取消选择的目标文件）。

.. _source-code-generation-legacy:

源代码生成
^^^^^^^^^^

某些组件会出现源文件未随组件本身提供，而必须从另外一个文件生成的情况。假设我们的组件有一个头文件，该文件由 BMP 文件转换后的二进制数据组成，假设使用 bmp2h 的工具进行转换，然后将头文件包含在名为 graphics_lib.c 的文件中::

    COMPONENT_EXTRA_CLEAN := logo.h

    graphics_lib.o: logo.h

    logo.h: $(COMPONENT_PATH)/logo.bmp
        bmp2h -i $^ -o $@

这个示例会在当前目录（构建目录）中生成 graphics_lib.o 和 logo.h 文件，而 logo.bmp 随组件一起提供并位于组件路径下。因为 logo.h 是一个生成的文件，所以当调用 ``make clean`` 时需要清理它，这就是为什么要将它添加到 ``COMPONENT_EXTRA_CLEAN`` 变量中。

润色与改进
^^^^^^^^^^

将 logo.h 添加作为 ``graphics_lib.o`` 的依赖项会导致在编译 ``graphics_lib.c`` 之前先生成它。

如果另一个组件中的源文件需要使用 logo.h，则必须将此组件的名称添加到另一个组件的 ``COMPONENT_DEPENDS`` 列表中，以确保组件按顺序编译。

嵌入二进制数据
^^^^^^^^^^^^^^

有时您的组件希望使用一个二进制文件或者文本文件，但是您又不希望将它重新格式化为 C 源文件。

这时，您可以在 ``component.mk`` 文件中设置变量 ``COMPONENT_EMBED_FILES``，以这种方式指定要嵌入的文件的名称::

  COMPONENT_EMBED_FILES := server_root_cert.der

或者，如果文件是字符串，则可以使用变量 ``COMPONENT_EMBED_TXTFILES``，这将把文本文件的内容当成以 null 结尾的字符串嵌入::

  COMPONENT_EMBED_TXTFILES := server_root_cert.pem

文件的内容会被编译进 flash 中的 .rodata 段，并通过符号名称来访问，如下所示::

  extern const uint8_t server_root_cert_pem_start[] asm("_binary_server_root_cert_pem_start");
  extern const uint8_t server_root_cert_pem_end[]   asm("_binary_server_root_cert_pem_end");

符号名称是根据文件的全名生成的，如 ``COMPONENT_EMBED_FILES`` 中的所示，字符 / ， . ， 等都将会被下划线替代。符号名称中的 ``_binary`` 前缀由 ``objcopy`` 添加，对于文本和二进制文件都是相同的。

有关使用此技术的示例，请查看 file_serving 示例 :example_file:`protocols/http_server/file_serving/main/CMakeLists.txt` 中的 main 组件，两个文件会在编译时加载并链接到固件中。

代码和数据的存放
----------------

ESP-IDF 还支持自动生成链接脚本，它允许组件通过链接片段文件定义其代码和数据在内存中的存放位置。构建系统会处理这些链接片段文件，并将处理后的结果扩充进链接脚本，从而指导应用程序二进制文件的链接过程。更多详细信息与快速上手指南，请参阅 :doc:`链接脚本生成机制 <linker-script-generation>`。

.. _fully-overriding-component-makefile:

完全覆盖组件的 Makefile
~~~~~~~~~~~~~~~~~~~~~~~

显然，在某些情况下，所有这些配置都不足以满足某个组件，例如，当组件基本上是另一个第三方组件的包装器时，该第三方组件最初不打算在 ESP-IDF 构建系统下工作，在这种情况下，可以通过设置 ``COMPONENT_OWNBUILDTARGET`` 和可能的 ``COMPONENT_OWNCLEANTARGET``，并在 ``component.mk`` 中定义名为 ``build`` 和 ``clean`` 的目标。构建目标可以执行任何操作，只要它为项目生成了 ``$(COMPONENT_LIBRARY)`` ，并最终被链接到应用程序二进制文件中即可。

（实际上，这并不是必须的 - 如果 ``COMPONENT_ADD_LDFLAGS`` 变量被覆盖，那么组件可以指示链接器链接其他二进制文件。）

.. note:: 当外部构建系统使用 PSRAM 时，请记得将 ``-mfix-esp32-psram-cache-issue`` 添加到 C 编译器的参数中。关于该标志的更多详细信息，请参考 :ref:`CONFIG_SPIRAM_CACHE_WORKAROUND`。

.. _esp-idf-template: https://github.com/espressif/esp-idf-template
.. _GNU Make Manual: https://www.gnu.org/software/make/manual/make.html


.. _custom-sdkconfig-defaults-legacy:

自定义 sdkconfig 的默认值
~~~~~~~~~~~~~~~~~~~~~~~~~

对于示例工程或者其他您不想指定完整 sdkconfig 配置的项目，但是您确实希望覆盖 ESP-IDF 默认值中的某些键值，则可以在项目中创建文件 ``sdkconfig.defaults``,运行 ``make defconfig`` 或从头创建新配置时将会使用此文件。

要想覆盖此文件的名称，请设置 ``SDKCONFIG_DEFAULTS`` 环境变量。

保存 flash 参数
~~~~~~~~~~~~~~~

在某些情况下，我们希望在没有 IDF 的情况下烧写目标板，对于这种情况，我们希望保存构建的二进制文件、esptool.py 和 esptool write_flash 命令的参数。可以简单编写一段脚本来保存二进制文件和 esptool.py，并且使用命令 ``make print_flash_cmd`` 来查看烧写 flash 时的参数::

    --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 bootloader/bootloader.bin 0x10000 example_app.bin 0x8000 partition_table_unit_test_app.bin

然后使用这段 flash 参数作为 esptool write_flash 命令的参数::

    python esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 bootloader/bootloader.bin 0x10000 example_app.bin 0x8000 partition_table_unit_test_app.bin

构建 Bootloader
---------------

引导程序默认作为 ``make all`` 的一部分被构建，或者也可以通过 ``make bootloader-clean`` 来单独构建，此外还可以通过 ``make bootloader-list-components`` 来查看构建引导程序时包含的组件。

引导程序是一个特殊的组件，因为主项目中的二级引导程序拥有单独的 .EFL 和 .BIN 文件。但是它与主项目共享配置和构建目录。

这是通过在 components/bootloader/subproject 下添加子项目来完成的。这个子项目有自己的 Makefile，但它希望通过 components/bootloader/Makefile.projectbuild 文件中的一些配置使自己从主项目的 Makefile 被调用。有关详细信息，请参阅这些文件。
