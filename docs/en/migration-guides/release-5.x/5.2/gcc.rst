GCC
***

:link_to_translation:`zh_CN:[中文]`


GCC Version
===========

The previous GCC version was GCC 12.2.0. This has now been upgraded to GCC 13.2.0 on all targets. Users that need to port their code from GCC 12.2.0 to 13.2.0 should refer to the series of official GCC porting guides listed below:

* `Porting to GCC 13 <https://gcc.gnu.org/gcc-13/porting_to.html>`_


Common Porting Problems and Fixes
=================================

``stdio.h`` No Longer Includes ``sys/types.h``
-------------------------------------------------

Issue
^^^^^^

Compilation errors may occur in code that previously worked with the old toolchain. For example:

.. code-block:: c

    #include <stdio.h>
    clock_t var; // error: expected specifier-qualifier-list before 'clock_t'

Solution
^^^^^^^^^

To resolve this issue, the correct header must be included. Refactor the code like this:

.. code-block:: c

    #include <time.h>
    clock_t var;
