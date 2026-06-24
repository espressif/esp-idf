GCC
***

:link_to_translation:`en:[English]`


GCC 版本
========

ESP-IDF 之前使用的 GCC 版本为 13.2.0，现已针对所有芯片目标升级至 GCC 14.2.0。若需要将代码从 GCC 13.2.0 迁移到 GCC 14.2.0，请参考以下 GCC 官方迁移指南。

* `迁移至 GCC 14 <https://gcc.gnu.org/gcc-14/porting_to.html>`_


警告
====

升级至 GCC 14.2.0 后会触发新警告，或是导致原有警告内容发生变化。了解所有 GCC 警告的详细内容，请参考 `GCC 警告选项 <https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Warning-Options.html>`_。建议用户仔细检查代码，并尽量解决这些警告。但由于某些警告的特殊性及用户代码的复杂性，有些警告可能为误报，需要进行关键修复。在这种情况下，用户可以采取多种方式来抑制警告。本节介绍了用户可能遇到的常见警告及如何修复这些警告。

要抑制所有新的警告，请启用 CONFIG_COMPILER_DISABLE_GCC14_WARNINGS 配置选项。

``-Wno-calloc-transposed-args``
-------------------------------

这是一个编码风格警告。根据文档说明，calloc() 的第一个参数应为数组元素的数量，第二个参数为每个元素的大小。

.. code-block:: c

    calloc (sizeof (int), n); // 警告
    calloc (n, sizeof (int)); // 好的
