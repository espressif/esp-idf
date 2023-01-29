**************
IDF Clang Tidy
**************

The IDF Clang Tidy is a tool that uses `clang-tidy <https://clang.llvm.org/extra/clang-tidy/>`_ to run static analysis on your current app.

.. warning::

   This functionality and the toolchain it relies on are still under development. There may be breaking changes before a final release.

.. only:: CONFIG_IDF_TARGET_ARCH_RISCV

   .. warning::

      This tool does not support RISC-V based chips yet. For now, we don't provide clang based toolchain for RISC-V.

Prerequisites
=============

If you have never run this tool before, take the following steps to get this tool prepared.

#. Run the export scripts (``export.sh`` / ``export.bat`` / ... ) to set up the environment variables.
#. Run ``pip install --upgrade pyclang`` to install this plugin. The extra commands would be activated in ``idf.py`` automatically.
#. Run ``idf_tools.py install esp-clang`` to install the clang-tidy required binaries

   .. note::

      This toolchain is still under development. After the final release, you don't have to install them manually.

#. Get file from the `llvm repository <https://github.com/llvm/llvm-project/blob/main/clang-tools-extra/clang-tidy/tool/run-clang-tidy.py>`_ and add the folder of this script to the ``$PATH``. Or you could pass an optional argument ``--run-clang-tidy-py`` later when you call ``idf.py clang-check``. Please don't forget to make the script executable.

   .. note::

      This file would be bundled in future toolchain releases. This is a temporary workaround.

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

This tool is hosted in `espressif/clang-tidy-runner <https://github.com/espressif/clang-tidy-runner>`_. If you faced any bugs or have any feature request, please report them via `github issues <https://github.com/espressif/clang-tidy-runner/issues>`_.
