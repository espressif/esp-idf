GCC
***

:link_to_translation:`zh_CN:[中文]`


GCC Version
===========

The previous GCC version was GCC 13.2.0. This has now been upgraded to GCC 14.2.0 on all targets. Users that need to port their code from GCC 13.2.0 to 14.2.0 should refer to the series of official GCC porting guides listed below:

* `Porting to GCC 14 <https://gcc.gnu.org/gcc-14/porting_to.html>`_


Warnings
========

The upgrade to GCC 14.2.0 has resulted in the addition of new warnings, or enhancements to existing warnings. The full details of all GCC warnings can be found in `GCC Warning Options <https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Warning-Options.html>`_. Users are advised to double-check their code, then fix the warnings if possible. Unfortunately, depending on the warning and the complexity of the user's code, some warnings will be false positives that require non-trivial fixes. In such cases, users can choose to suppress the warning in multiple ways. This section outlines some common warnings that users are likely to encounter and ways to fix them.

To suprress all new warnings enable ``CONFIG_COMPILER_DISABLE_GCC14_WARNINGS`` config option.

``-Wno-calloc-transposed-args``
-------------------------------

This is a coding style warning. The first argument to calloc() is documented to be number of elements in array, while the second argument is size of each element.

.. code-block:: c

    calloc (sizeof (int), n); // warning
    calloc (n, sizeof (int)); // ok
