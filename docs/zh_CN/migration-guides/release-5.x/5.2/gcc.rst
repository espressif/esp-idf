GCC
***

:link_to_translation:`en:[English]`


GCC 版本
========

ESP-IDF 之前使用的 GCC 版本为 12.2.0，现已针对所有芯片目标升级至 GCC 13.2.0。若需要将代码从 GCC 12.2.0 迁移到 GCC 13.2.0，请参考以下 GCC 官方迁移指南。

* `迁移至 GCC 13 <https://gcc.gnu.org/gcc-13/porting_to.html>`_


常见迁移问题和解决方法
=================================

``stdio.h`` 不再包含 ``sys/types.h``
-------------------------------------------------

问题描述
^^^^^^^^^^^^

使用旧工具链的代码可能会出现编译错误，例如：

.. code-block:: c

    #include <stdio.h>
    clock_t var; // error: expected specifier-qualifier-list before 'clock_t'

解决方法
^^^^^^^^^^^^

使用正确的头文件可以解决这一问题。请按照以下方式重构代码：

.. code-block:: c

    #include <time.h>
    clock_t var;
