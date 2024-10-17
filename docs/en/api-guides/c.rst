C Support
===========

:link_to_translation:`zh_CN:[中文]`

ESP-IDF is primarily written in C and provides C APIs. ESP-IDF uses `Newlib <https://sourceware.org/newlib/>`_ as its C Standard Library implementation (the Newlib version is specified in :component_file:`newlib/sbom.yml`). In general, all C features that are supported by the compiler (currently GCC) can be used in ESP-IDF, unless specified in :ref:`unsupported_c_features` below.

.. _c_version:

C Version
---------

**GNU dialect of ISO C17** (``--std=gnu17``) is the current default C version in ESP-IDF.

To compile the source code of a certain component using a different language standard, set the desired compiler flag in the component's ``CMakeLists.txt`` file:

.. code-block:: cmake

    idf_component_register( ... )
    target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu11)

If the public header files of the component also need to be compiled with the same language standard, replace the flag ``PRIVATE`` with ``PUBLIC``.

.. _unsupported_c_features:

Unsupported C Features
----------------------

The following features are not supported in ESP-IDF.

Nested Function Pointers
^^^^^^^^^^^^^^^^^^^^^^^^

The **GNU dialect of ISO C17** supports `nested functions <https://gcc.gnu.org/onlinedocs/gcc/Nested-Functions.html>`_. However, ESP-IDF does not support referencing nested functions as pointers. This is due to the fact that the GCC compiler generates a `trampoline <https://gcc.gnu.org/onlinedocs/gccint/Trampolines.html>`_ (i.e., small piece of executable code) on the stack when a pointer to a nested function is referenced. ESP-IDF does not permit executing code from a stack, thus use of pointers to nested functions is not supported.
