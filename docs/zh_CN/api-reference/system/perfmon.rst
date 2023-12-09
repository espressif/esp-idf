性能监视器
===================

:link_to_translation:`en:[English]`

性能监视器组件提供了多个 API，可以通过这些 API 来使用 {IDF_TARGET_NAME} 的内部性能计数器，分析函数和应用程序。

应用示例
-------------------

在目录 ``examples/system/perfmon`` 下提供了一个结合性能监视器的示例。此示例初始化并执行了性能监视器，同时打印了统计信息。

高级 API 参考
------------------------

头文件
^^^^^^^^^^^^

* :component_file:`perfmon/include/perfmon.h`

API 参考
-------------

.. include-build-file:: inc/xtensa_perfmon_access.inc
.. include-build-file:: inc/xtensa_perfmon_apis.inc
