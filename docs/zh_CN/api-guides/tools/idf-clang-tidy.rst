******************
IDF clang-tidy
******************

:link_to_translation:`en:[English]`

IDF clang-tidy 是使用 `clang-tidy <https://clang.llvm.org/extra/clang-tidy/>`__ 对当前应用程序进行静态分析的工具。

.. warning::

   IDF clang-tidy 的功能及其依赖的工具链尚在开发中，最终版本发布前可能有重大变更。

   目前仅支持基于 clang 的工具链。在配置项目前，必须在环境变量或 CMake 缓存中设置 ``IDF_TOOLCHAIN=clang`` 进行激活。

.. only:: CONFIG_IDF_TARGET_ARCH_RISCV

   .. warning::

      当前工具尚不支持基于 RISC-V 的芯片。目前，乐鑫尚未针对 RISC-V 提供基于 clang 的工具链。

准备工作
=============

初次运行此工具时，请按照以下步骤准备该工具：

#. 运行 ``idf_tools.py install esp-clang`` 安装 clang-tidy 所需的二进制文件。

   .. note::

      该工具链尚在开发中，最终版本发布后，将无需手动安装工具链。

#. 再次运行导出脚本（如 ``export.sh``、``export.bat`` 等），刷新环境变量。

其他命令
==============

``clang-check``
---------------

运行 ``idf.py clang-check`` 可以重新生成编译数据库，并在当前项目文件夹下运行 ``clang-tidy``，所得输出写入 ``<project_dir>/warnings.txt``。

运行 ``idf.py clang-check --help`` 查看完整文档。

``clang-html-report``
---------------------

#. 运行 ``pip install codereport`` 安装附加依赖关系。
#. 运行 ``idf.py clang-html-report`` 会根据 ``warnings.txt`` 在 ``<project_dir>/html_report`` 文件夹内生成 HTML 报告。请在浏览器中打开 ``<project_dir>/html_report/index.html`` 查看报告。

错误报告
==========

此工具托管在 `espressif/clang-tidy-runner <https://github.com/espressif/clang-tidy-runner>`__。如遇到任何错误，或有任何功能请求，请通过 `Github issues <https://github.com/espressif/clang-tidy-runner/issues>`__ 提交报告。
