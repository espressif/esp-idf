创建示例项目
============

:link_to_translation:`en:[English]`

每个 ESP-IDF 的示例都是一个完整的项目，其他人可以将示例复制至本地，并根据实际情况进行一定修改。请注意，示例项目主要是为了展示 ESP-IDF 的功能。

示例项目结构
------------

-  ``main`` 目录需要包含一个名为 ``(something)_example_main.c`` 的源文件，里面包含示例项目的主要功能。
-  如果该示例项目的子任务比较多，请根据逻辑将其拆分为 ``main`` 目录下的多个 C 或者 C++ 源文件，并将对应的头文件也放在同一目录下。
-  如果该示例项目具有多种功能，可以考虑在项目中增加一个 ``components`` 子目录，通过库功能，将示例项目的不同功能划分为不同的组件。注意，如果该组件提供的功能相对完整，且具有一定的通用性，则应该将它们添加到 ESP-IDF 的 ``components`` 目录中，使其成为 ESP-IDF 的一部分。
-  示例项目需要包含一个 ``README.md`` 文件，建议使用 :idf_file:`示例项目 README 模板 <docs/TEMPLATE_EXAMPLE_README.md>` ，并根据项目实际情况进行修改。
-  示例项目需要包含一个 ``example_test.py`` 文件，用于进行自动化测试。如果在 GitHub 上初次提交 Pull Request 时，可以先不包含这个脚本文件。具体细节，请见有关 `Pull Request <https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/proposing-changes-to-your-work-with-pull-requests/creating-a-pull-request>`_ 的相关内容。

一般准则
--------

示例代码需要遵循 :doc:`《乐鑫物联网开发框架风格指南》 <style-guide>`。

检查清单
--------

提交一个新的示例项目之前，需要检查以下内容：

-  每个示例项目只能有一个主要功能。如果某个示例项目有多个主要功能，请将其拆分为两个或更多示例项目。
-  每个示例项目应包含一个 ``README.md`` 文件，建议使用 :idf_file:`示例项目 README 模板 <docs/TEMPLATE_EXAMPLE_README.md>`。
-  示例项目中的函数和变量的命令要遵循 :ref:`命名规范 <style-guide-naming>` 中的要求。对于仅在示例项目源文件中使用的非静态变量/函数，请使用 ``example`` 或其他类似的前缀。
-  示例项目中的所有代码结构良好，关键代码要有详细注释。
-  示例项目中所有不必要的代码（旧的调试日志，注释掉的代码等）都必须清除掉。
-  示例项目中使用的选项（比如网络名称，地址等）不得直接硬编码，应尽可能地使用配置项，或者定义为宏或常量。
-  配置项可见 ``KConfig.projbuild`` 文件，该文件中包含一个名为 “Example Configuration” 的菜单。具体情况，请查看现有示例项目。
-  所有的源代码都需要在文件开头指定许可信息（表示该代码是 ``in the public domain CC0``）和免责声明。或者，源代码也可以应用 ``Apache License 2.0`` 许可条款。请查看现有示例项目的许可信息和免责声明，并根据实际情况进行修改。
-  任何第三方代码（无论是直接使用，还是进行了一些改进）均应保留原始代码中的许可信息，且这些代码的许可必须兼容 ``Apache License 2.0`` 协议。
