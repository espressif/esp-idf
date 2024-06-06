Static Analyzer
===============

:link_to_translation:`zh_CN:[中文]`

A static analyzer is a tool that checks source code for errors and vulnerabilities without running it. It helps developers find issues early, improving code quality.

GNU Static Analyzer
-------------------

The GNU Static Analyzer is distributed with GCC (refer to `GCC documentation <https://gcc.gnu.org/onlinedocs/gcc/Static-Analyzer-Options.html>`_). It can be enabled with :ref:`CONFIG_COMPILER_STATIC_ANALYZER` to perform code checks during application builds.

Suppressing Warnings
^^^^^^^^^^^^^^^^^^^^

GNU Static Analyzer is still under development and may give some false-positive warnings. Here is an example of how to suppress unwanted warnings using IDF:

.. code-block:: c

    #include "esp_compiler.h"
    /* .... */
      ESP_COMPILER_DIAGNOSTIC_PUSH_IGNORE("-Wanalyzer-null-dereference")
      *((volatile int *) 0) = 0;
      ESP_COMPILER_DIAGNOSTIC_POP("-Wanalyzer-null-dereference")
    /* .... */


Clang Static Analyzer
---------------------

See :doc:`IDF Clang-Tidy <../../api-guides/tools/idf-clang-tidy>`
