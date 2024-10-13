GCC
***

:link_to_translation:`zh_CN:[中文]`

Common Porting Problems and Fixes
=================================

``sys/dirent.h`` No Longer Includes Function Prototypes
-------------------------------------------------------

Issue
^^^^^^

Compilation errors may occur in code that previously worked with the old toolchain. For example:

.. code-block:: c

    #include <sys/dirent.h>
    /* .... */
    DIR* dir = opendir("test_dir");
    /* .... */
    /**
     * Compile error:
     * test.c: In function 'test_opendir':
     * test.c:100:16: error: implicit declaration of function 'opendir' [-Werror=implicit-function-declaration]
     *   100 |     DIR* dir = opendir(path);
     *       |                ^~~~~~~
     */

Solution
^^^^^^^^^

To resolve this issue, the correct header must be included. Refactor the code like this:

.. code-block:: c

    #include <dirent.h>
    /* .... */
    DIR* dir = opendir("test_dir");
