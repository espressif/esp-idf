.. _get-started-customized-setup-cmake:

*********************************************************
工具链自定义设置 (CMake)
*********************************************************

:link_to_translation:`en:[英文]`

除了从乐鑫官网（请见 :ref:`get-started-setup-toolchain-cmake`）下载二进制工具链外，你还可以自行编制工具链。

如果没有特别需求，建议直接使用我们提供的预编制二进制工具链。不过，你也可能也会由于以下原因，编制你自己的工具链：

- 需要定制工具链编制配置
- 使用其他 GCC 版本（如 4.8.5）
- 需要破解 gcc、newlib 或 libstdc++
- 有相关兴趣或时间充裕
- 不信任从网站下载的 bin 文件

无论如何，如果你希望自行编制工具链，请查看以下文档：

.. toctree::
    :maxdepth: 1

    windows-setup-scratch
    linux-setup-scratch
    macos-setup-scratch

