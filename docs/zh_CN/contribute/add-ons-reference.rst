文档的附加工具和扩展功能指南
=============================

:link_to_translation:`en:[English]`

ESP-IDF 文档由 `Sphinx <http://www.sphinx-doc.org/>`_ 应用程序生成，使用 Sphinx 对 :idf:`docs` 目录中的 `reStructuredText <https://en.wikipedia.org/wiki/ReStructuredText>`_ (``.rst``) 格式文档进行渲染。关于渲染过程的详细信息，请参阅 :doc:`documenting-code`。

除 Sphinx 外，我们也使用了其它几种可为用户提供格式精美、便于查找的文档的应用程序。:ref:`setup-for-building-documentation` 中列出了这些应用程序，:idf_file:`docs/requirements.txt` 中列出了其相应的版本号。

ESP-IDF 中包含多种芯片的双语文档（英文，简体中文）。如运行 Sphinx，不需直接使用 ``sphinx``，可运行 Python 程序包 ``build_docs.py``。

在此基础上，我们也开发了一些自定义的附加工具和扩展功能，旨在帮助整合 `ESP-IDF`_ 目录下的各个文档以及更好地查找和维护文档内容。

本章节主要帮您快速了解这些附加工具和扩展功能。

文件夹结构
--------------

* ESP-IDF 根目录下包含一个专门放置文档的文件夹 :idf:`docs`。
* ``docs`` 目录下的 :idf:`docs/en` （英文）和 :idf:`docs/zh_CN` （简体中文）子文件夹中包含本地化文档。
* 图像文件和本地化文档通用的字体包位于 :idf:`docs/_static` 子文件夹中。
* ``docs`` 根目录下以及 ``docs/en`` 和 ``docs/zh_CN`` 中的其它文件则提供了自动生成文档过程中所使用的配置和脚本，其中就包括本章节提到的附加工具和扩展功能。
* ``extensions`` 和 ``idf_extensions`` 两目录中提供了 Sphinx 的扩展功能。·
* 使用 ``build_docs.py``，``docs`` 文件夹中将自动创建一个 ``_build`` 目录。这个目录不会被添加到 `ESP-IDF`_ 项目库中。


附加工具和扩展功能指南
--------------------------------

配置文件
^^^^^^^^^^^^

:idf_file:`docs/conf_common.py`
    该文件中包含每个本地化文档（包括英文文档、中文文档）所通用的配置信息。在文档每一次的构建过程中，该文件中的内容都将被导入至相应语言文件夹（包括，``docs/en``、``docs/zh_CN``）下的标准 Sphinx 配置文件 ``conf.py`` 中。

:idf_file:`docs/sphinx-known-warnings.txt`
    Sphinx 中存在一些伪错误警报，这些警报只能通过更新 Sphinx 源代码本身来解决。针对这一情况，我们将这些伪错误警报列在了 ``sphinx-known-warnings.txt`` 文件中，每一次生成文档时系统都将检测该文件并忽略这些伪错误警报。


脚本
^^^^^^^

:idf_file:`docs/build_docs.py`
    最高级可执行程序，负责运行 Sphinx 为单个或多个语言/目标生成文档。运行 ``build_docs.py --help`` 可查阅所有命令选项。

    当使用 ``build_docs.py`` 运行 Sphinx 时，系统将为 ``idf_target`` 配置变量，并设置一个与该配置变量相同名称的 Sphinx 标签，然后使用一些环境变量将路径发送至 :ref:`IDF-Specific Extensions`。

:idf_file:`docs/check_lang_folder_sync.sh`
    同时更新双语文档时，语言文件夹 ``docs/en`` 和 ``docs/zh_CN`` 下的文档结构和文件名应保持一致，以减少两文档间的不一致。每一次生成文档时都将运行 ``check_lang_folder_sync.sh`` 脚本，检测是否出现上述不一致的情况。

.. note::

        若一个新的章节为英语版本，且暂时还没有中文翻译，那么 ``zh_CN`` 文件夹中相应的中文文件内应写入 ``.. include::`` 指令，路径指向英文源文件。这样，中文读者将也可以看到英文版源文件。例如，如果 ``docs/zh_CN/contribute/documenting-code.rst`` 这一文件还没有中文翻译，则该文件中应写入 ``.. include:: ../../en/contribute/documenting-code.rst``。

非文档脚本
^^^^^^^^^^^^^^^^

以下脚本除了生成文档之外，也可以用于其它用途：

:idf_file:`tools/gen_esp_err_to_name.py`
    该脚本将检测整个 `ESP-IDF`_ 库，在源代码头文件中查找是否有错误代码和信息，然后在 :doc:`../api-reference/error-codes` 内生成一个 ``.inc`` 文件记录这些信息。

:idf_file:`tools/kconfig_new/confgen.py`
    ESP-IDF :idf:`components` 的配置选项包含在每个部件目录下的 ``Kconfig`` 文件中，如 :idf_file:`components/bt/Kconfig`。该脚本将检测所有 ``component`` 目录并记录检测到的配置选项，然后在 :ref:`configuration-options-reference` 内生成一个 ``.inc`` 文件记录这些信息。

泛型扩展
^^^^^^^^^^^^^^^^^^

以下是专为 IDF 开发的 Sphinx 扩展，这些扩展不依赖于任何特定的 IDF 文档操作或配置：

:idf_file:`docs/extensions/toctree_filter.py`
    Sphinx 扩展功能，优先于 ``:toctree:`` 指令，允许系统根据是否有标签（如 ``:tagname: toctree_entry``）来过滤条目。完整描述请参考 Python 文件。

:idf_file:`docs/extensions/list_filter.py`
    Sphinx 扩展功能，提供一个 ``.. list::`` 指令，允许系统根据是否有标签（如 ``:tagname: - list content``）来过滤条目列表。完整描述请参考 Python 文件。

:idf_file:`docs/extensions/html_redirects.py`
    在文档的维护过程中，一些源文件可能会转移位置或被重命名。这个 Sphinx 扩展功能便添加了一个重新导向机制，通过在 Sphinx 输出中生成静态 HTML 重新导向页面来为 URL 地址已改变的文档重新导向。该脚本与重新导向列表 ``html_redirect_pages`` 一起使用。``conf_common.py`` 将负责从 :idf_file:`docs/page_redirects.txt` 中生成这个重新导向列表。


第三方扩展
^^^^^^^^^^^^^^^^^^^^^^

- ``sphinxcontrib`` 为 blockdiag、seqdiag、actdiag、nwdiag、rackdiag & packetdiag 等图表的扩展
- `Sphinx selective exclude`_ 为 ``eager_only`` 的扩展

.. _idf-specific extensions:

IDF 专属扩展
^^^^^^^^^^^^^^^^^^^^^^^

构建系统集成
###################

:idf:`docs/idf_extensions/build_system/`

Python 包实现了一个 Sphinx 扩展功能，即将 IDF 构建系统信息拉入文档构建中

* 创建一个 CMake IDF 项目模型，并运行 CMake 生成元数据。
* 注册一些新的配置变量并发出一个 Sphinx 新事件，这些信息都用于其它扩展功能中。

配置变量
@@@@@@@@@@@@@

* ``docs_root`` - $IDF_PATH/docs 目录的绝对路径
* ``idf_path`` - IDF_PATH 变量的值，未设置环境时为 IDF_PATH 的绝对路径
* ``build_dir`` - 运行 ``build_docs.py`` 时自动创建的文档生成目录，默认为 ``_build/<lang>/<target>``
* ``idf_target`` - IDF_TARGET 的值。``build_docs.py`` 应负责在 Sphinx 命令行中设置该值。

新事件
@@@@@@@@@

CMake 项目模型运行完成后，系统将在构建初期发出 ``idf-info`` 事件。

参数为 ``(app, project_description)``，其中 ``project_description`` 是一个字典，其中包含从 CMake 构建目录中的 ``project_description.json`` 内解析出的值。

其它 IDF 专属的扩展功能均订阅该事件，并使用该事件根据系统构建信息来设置一些文档参数。


其它扩展
#############

:idf_file:`docs/idf_extensions/include_build_file.py`
    ``include-build-file`` 指令相当于是内置的 ``include-file`` 指令，只是文件路径是相对于 ``build_dir`` 来评估的。

:idf_file:`docs/idf_extensions/kconfig_reference.py`
    订阅 ``idf-info`` 事件，并使用 confgen 从默认构建项目所使用的部件中生成 ``kconfig.inc`` 文件。之后，这个文件将被存储至 :doc:`/api-reference/kconfig` 中。

:idf_file:`docs/idf_extensions/link_roles.py`
    一个自定义的 `Sphinx 角色 <https://www.sphinx-doc.org/en/master/usage/restructuredtext/roles.html>`_ 的实现功能，帮助从文档链接到 `ESP-IDF`_ 项目中具体的文件和文件夹处。有关具体实现了哪些角色，请参阅 :ref:`link-custom-roles` 和 :ref:`link-language-versions`。

:idf_file:`docs/idf_extensions/esp_err_definitions.py`
    小扩展包，调用 ``gen_esp_err_to_name.py`` 并更新修改后的 .rst 文件。

:idf_file:`docs/idf_extensions/gen_toolchain_links.py`
    文档内许多地方提供了下载工具链的链接。为了整合这些链接，减少需要分别手动更新这些链接的时间，该脚本会根据 :idf_file:`tools/toolchain_versions.mk` 内的信息生成工具链下载链接和工具链解压代码片段。

:idf_file:`docs/idf_extensions/gen_version_specific_includes.py`
    也是一个自动生成 reStructuredText 文本 ``.inc`` 的扩展功能，其中内容是基于当前 ESP-IDF 版本所写。

:idf_file:`docs/idf_extensions/util.py`
    提供一系列实用功能，主要提高本地化生成文档（请参见 :ref:`setup-for-building-documentation`）的效率，节省后续再次生成文本所需时间。

:idf_file:`docs/idf_extensions/format_idf_target.py`
    通过将 idf_target 发送至 Sphinx 命令行替换 target 相关名称的扩展功能。例如：

     This is a {\IDF_TARGET_NAME}, with /{\IDF_TARGET_PATH_NAME}/soc.c, compiled with `{\IDF_TARGET_TOOLCHAIN_PREFIX}-gcc` with `CONFIG_{\IDF_TARGET_CFG_PREFIX}_MULTI_DOC`

    删掉退格键后，将被渲染为

     This is a {IDF_TARGET_NAME}, with /{IDF_TARGET_PATH_NAME}/soc.c, compiled with `{IDF_TARGET_TOOLCHAIN_PREFIX}-gcc` with `CONFIG_{IDF_TARGET_CFG_PREFIX}_MULTI_DOC`.

    同时，也支持使用以下语法标记本地（单个 rst 文件）替代文件的定义：
    {\IDF_TARGET_TX_PIN:default="IO3",esp32="IO4",esp32s2="IO5"}

    这样将在当前的 rst 文件中定义标签 {\IDF_TARGET_TX_PIN} 的替换名称。

    为了使用相同的格式规则规范文档内容，该扩展功能优先于默认的 ``.. include::`` 指令。

    在依赖于字符排列方式的格式内无法使用这一替换方式，例如，表格内。

:idf_file:`docs/idf_extensions/latex_builder.py`
    一个在 latex 生成器内添加 ESP-IDF 专属功能的扩展，优先于默认的 Sphinx latex 生成器。

    在输出目录内创建并添加 espidf.sty latex 包，其中包含一些运行时所需变量的宏包，如 IDF-Target。

:idf_file:`docs/idf_extensions/gen_defines.py`
    Sphinx 扩展，将 IDF 中的定义整合入 Sphinx 构建过程中，在 IDF 项目模型创建完成后开始运行。

    解析这些定义值，并将其添加为 Sphinx 标签。

    发出新的 'idf-defines-generated' 事件，其中有一个包含所有原始定义值的字典，其它扩展功能可以使用这些原始值生成相关数据。

:idf_file:`docs/idf_extensions/exclude_docs.py`
    Sphinx 扩展，根据 conditional_include_dict {tag:documents} 标签更新已被排除的文档。如果文档设置有这个标签，则其将被添加至文档列表内。

    同时也负责在使用 config 值 ``docs_to_build`` 生成文档时，排除不相关文档。此时，未在 ``docs_to_build`` 列表内的文档都将被排除。

    订阅 ``idf-defines-generated`` 事件，因为该扩展功能需要根据 Sphinx 标签来决定需排除哪些文档。

:idf_file:`docs/idf_extensions/run_doxygen.py`
    订阅 ``idf-defines-generated`` 事件，运行 Doxygen (:idf_file:`docs/doxygen/Doxyfile_common`) 生成描述密钥头文件的 XML 文件，然后运行 Breathe 将这些文件转换为可直接被添加至 API 参考页面的 ``.inc`` 文件。

    将一些特定目标的自定义环境变量推入 Doxygen 中，包括项目的默认 ``sdkconfig.h`` 文件内定义的所有宏包以及 ``soc`` 部件 ``xxx_caps.h`` 的头文件中定义的所有宏包。这意味着，公共 API 头文件可以依赖于特定目标的配置选项或者 ``soc`` 功能头文件选项，如头文件中 ``#ifdef`` & ``#if`` 预处理器选项。

    也就是说，我们可以根据生成文档的目标来生成不同的 Doxygen 文件。

    有关这一流程的更多信息，请参考 :doc:`documenting-code` 和 :doc:`../api-reference/template` 中的 **API 参考** 章节。

相关文档
-----------------

* :doc:`documenting-code`


.. _ESP-IDF: https://github.com/espressif/esp-idf/
.. _Sphinx selective exclude: https://github.com/pfalcon/sphinx_selective_exclude

