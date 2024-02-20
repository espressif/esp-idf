API 文档模板
==========================

:link_to_translation:`en:[English]`

.. note::

    *说明*

    1. 使用此文件 (:idf_file:`docs/zh_CN/api-reference/template.rst`) 作为 API 参考文档模板。
    2. API 参考文档需和 API 的头文件名称保持一致。
    3. 使用 ``..include::`` 从 API 文件夹中添加相应的说明文件。

        * README.rst
        * example.rst
        * ...

    4. 可选择在此文件中直接提供描述。
    5. 完成后，删除所有的说明信息（类似本说明）和多余的头部信息。

概述
--------

.. note::

    *撰写说明*

    1. 提供概述，简要说明 API 的用途和使用方法。
    2. 必要时提供代码片段，以说明特定函数的功能。
    3. 用此 `文档 <https://www.sphinx-doc.org/en/master/usage/restructuredtext/basics.html#sections>`_ 中介绍的方式区分不同的章节标题：

        * ``#`` 用于设置各部分，标题上下同时标记
        * ``*`` 用于设置章标题，标题上下同时标记
        * ``=`` 用于设置节标题
        * ``-`` 用于设置小节标题
        * ``^`` 用于设置小小节标题
        * ``"`` 用于设置段落标题

应用示例
-------------------

.. note::

    *撰写说明*

    1. 准备一个或多个实际示例，展示此 API 的功能。
    2. 每个示例应遵循 ``esp-idf/examples/`` 文件夹中项目的模式。
    3. 将示例放在此文件夹中，添加 ``README.md`` 文件。
    4. 在 ``README.md`` 中对展示的功能进行概述。
    5. 良好的概述让读者能够充分理解示例，而无需参考源代码。
    6. 按照示例的复杂程度，将代码描述分解成几个部分，并对每部分的功能进行概述。
    7. 必要时，添加流程图和应用程序的输出截图。
    8. 最后为本章节的每个示例添加对应链接，示例文件夹应位于 ``esp-idf/examples/`` 中。

API 参考
-------------

.. highlight:: none

.. note::

    *撰写说明*

    1. ESP-IDF 仓库通过 :doc:`用 Doxygen 从头文件中检索代码标记 <../contribute/documenting-code>` 的方式自动更新 API 参考文档。

    2. 通过调用 Sphinx 扩展工具 ``esp_extensions/run_doxygen.py``，对 :idf_file:`docs/doxygen/Doxyfile` 中 ``INPUT`` 语句列出的所有头文件进行更新。

    3. ``INPUT`` 语句的每一行（以 ``##`` 开头的注释除外）都包含一个到头文件 ``*.h`` 的路径，用于生成相应的 ``*.inc`` 文件::

        ##
        ## Wi-Fi - API Reference
        ##
        ../components/esp32/include/esp_wifi.h \
        ../components/esp32/include/esp_smartconfig.h \

    4. 头文件被展开时， ``sdkconfig.h`` 中默认定义的宏以及在 SOC 特定 ``include/soc/*_caps.h`` 头文件中定义的宏都会被展开。这样，头文件就可以根据 ``IDF_TARGET`` 的值来添加或排除相关内容。

    5.  ``*.inc`` 文件中包含每次文档构建时自动生成的 API 成员格式化参考。所有 ``*.inc`` 文件都位于 Sphinx 的 ``_build`` 路径下。如需查看生成指令，以 ``esp_wifi.h`` 为例，运行 ``python gen-dxd.py esp32/include/esp_wifi.h``。

    6. 用以下语句在文档中显示 ``*.inc`` 文件的内容::

       .. include-build-file:: inc/esp_wifi.inc

       参考示例：:idf_file:`docs/en/api-reference/network/esp_wifi.rst`

    7. 你也可以不用 ``*.inc`` 文件，而使用自己的方式描述 API。参考示例：:idf_file:`docs/en/api-reference/storage/fatfs.rst`。

       以下为常见的 ``.. doxygen...::`` 指令：

        * 函数 - ``.. doxygenfunction:: name_of_function``
        * 联合体 - ``.. doxygenunion:: name_of_union``
        * 结构体 - ``.. doxygenstruct:: name_of_structure`` 和 ``:members:``
        * 宏 - ``.. doxygendefine:: name_of_define``
        * 类定义 - ``.. doxygentypedef:: name_of_type``
        * 枚举 - ``.. doxygenenum:: name_of_enumeration``

       如需更多信息，请参考 `Breathe 文档 <https://breathe.readthedocs.io/en/latest/directives.html>`_。

       使用 `link custom role` 指令添加指向头文件的链接，如下所示::

       * :component_file:`path_to/header_file.h`

    8. 在任何情况下，要生成 API 参考文档，应该更新文件 :idf_file:`docs/doxygen/Doxyfile` 并将其中的路径更新为正在添加文档的 ``*.h`` 头文件的路径。

    9. 更改提交并构建文档后，可以查看文档的渲染效果。如有需要，为相应的头文件 :doc:`纠正注释 <../contribute/documenting-code>`。
