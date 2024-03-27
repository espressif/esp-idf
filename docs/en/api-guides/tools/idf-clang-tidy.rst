******************
IDF Clang-Tidy
******************

:link_to_translation:`zh_CN:[中文]`

The IDF Clang Tidy is a tool that uses `clang-tidy <https://clang.llvm.org/extra/clang-tidy/>`__ to run static analysis on your current app.

.. warning::

   This functionality and the toolchain it relies on are still under development. There may be breaking changes before a final release.

   Only clang based toolchain is currently supported. It has to be activated by setting ``IDF_TOOLCHAIN=clang`` in the environment or in CMake cache before configuring the project.

.. only:: CONFIG_IDF_TARGET_ARCH_RISCV

   .. warning::

      This tool does not support RISC-V based chips yet. For now, we do not provide clang based toolchain for RISC-V.

Prerequisites
=============

If you have never run this tool before, take the following steps to get this tool prepared.

#. Run ``idf_tools.py install esp-clang`` to install the clang-tidy required binaries

   .. note::

      This toolchain is still under development. After the final release, you do not have to install them manually.

#. Run the export scripts (``export.sh`` / ``export.bat`` / ... ) again to refresh the environment variables.

Extra Commands
==============

``clang-check``
---------------

Run ``idf.py clang-check`` to re-generate the compilation database and run ``clang-tidy`` under your current project folder. The output would be written to ``<project_dir>/warnings.txt``.

Run ``idf.py clang-check --help`` to see the full documentation.

``clang-html-report``
---------------------

#. Run ``pip install codereport`` to install the additional dependency.
#. Run ``idf.py clang-html-report`` to generate an HTML report in folder ``<project_dir>/html_report`` according to the ``warnings.txt``. Please open the ``<project_dir>/html_report/index.html`` in your browser to check the report.

Bug Report
==========

This tool is hosted in `espressif/clang-tidy-runner <https://github.com/espressif/clang-tidy-runner>`__. If you were to face any bugs or have any feature request, please report them via `Github issues <https://github.com/espressif/clang-tidy-runner/issues>`__
