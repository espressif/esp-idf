GCC
***

:link_to_translation:`en:[English]`

常见的移植问题及解决方法
=================================

``sys/dirent.h`` 不再包含一些函数原型
-------------------------------------------------------

问题
^^^^^^

使用旧工具链的代码可能会出现编译错误。例如：

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

解决方法
^^^^^^^^^

包含正确的头文件即可解决此问题。请将代码重构如下：

.. code-block:: c

    #include <dirent.h>
    /* .... */
    DIR* dir = opendir("test_dir");
