SoC 能力宏
==========

:link_to_translation:`en:[English]`

不同型号的 ESP 芯片内部会集成不同的硬件模块。哪怕是同一种模块，在不同的芯片上也可能具有细微的差异。ESP-IDF 中提供了一份小型的“数据库”来描述不同芯片之间的差异（注意，我们不描述共性，只描述差异）。这份“数据库“的内容以宏定义的形式定义在 **soc/soc_caps.h** 文件中，我们称之为 **SoC 能力宏**。用户可以通过在代码中使用条件编译指令（比如 ``#if``）来使用这些宏，从而控制实际需要编译的代码内容。

.. note::

    请注意， **soc/soc_caps.h** 中的内容目前还不稳定，未来可能会发生重大更改。

使用 SoC 能力宏
---------------

我们推荐通过下面的宏函数来间接地访问 SoC 能力宏：

.. list-table::
    :widths: 30 60 80
    :header-rows: 1

    * - 宏函数
      - 描述
      - 示例
    * - :c:macro:`SOC_IS`
      - 判断芯片型号
      - ``#if SOC_IS(ESP32)`` 判断是否为 ESP32 芯片
    * - :c:macro:`SOC_HAS`
      - 判断芯片是否具有某个硬件模块或功能
      - ``#if SOC_HAS(DAC)`` 判断是否具有 DAC 模块


API 参考
--------

.. include-build-file:: inc/soc_caps.inc
.. include-build-file:: inc/soc_caps_eval.inc
