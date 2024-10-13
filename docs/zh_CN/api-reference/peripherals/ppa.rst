像素处理加速器 (PPA)
====================

简介
----

{IDF_TARGET_NAME} 含有像素处理加速器 (PPA) 模块，用于实现图像旋转、缩放、镜像和叠加等图像算法的硬件加速。

术语表
------

与 PPA 驱动程序相关的术语可参见下表及下图。

.. list-table::
    :widths: 25 75
    :header-rows: 1

    * - 术语
      - 定义
    * - 图片 (pic)
      - 存储在系统内存中的完整图像。
    * - 块
      - 从图片中裁剪出的一部分，其最大尺寸等于整个图片。
    * - 像素
      - 在 PPA 环境中使用的单位。
    * - PPA 操作
      - 图像算法加速的类型，包括缩放-旋转-镜像 (SRM)、叠加和填充。
    * - PPA 客户端
      - 执行 PPA 操作的主体。通常，每个 PPA 客户端由特定任务持有。
    * - PPA 事务
      - 来自 PPA 客户端的一次 PPA 操作请求即为一次 PPA 事务。

.. figure:: ../../../_static/diagrams/ppa/pic_blk_concept.png
    :align: center
    :alt: PPA 图片/块术语

    PPA 图片/块术语

功能概述
--------

以下部分详细介绍了 PPA 驱动程序的设计：

- :ref:`ppa-client-registration` - 涵盖如何注册 PPA 客户端来执行一切 PPA 操作。
- :ref:`ppa-register-callback` - 涵盖如何将用户特定代码挂接到 PPA 驱动程序事件回调函数。
- :ref:`ppa-perform-operation` - 涵盖如何执行 PPA 操作。
- :ref:`ppa-thread-safety`- 涵盖在线程安全方面使用 PPA 操作 API 的情况。
- :ref:`ppa-performance-overview` - 涵盖 PPA 操作的性能。

.. _ppa-client-registration:

注册 PPA 客户端
^^^^^^^^^^^^^^^

执行 PPA 操作的请求由 PPA 客户端发出。因此，在进行 PPA 操作之前应先注册 PPA 客户端。调用 :cpp:func:`ppa_register_client` 函数注册一个新客户端。使用 :cpp:type:`ppa_client_config_t` 结构体来指定客户端的属性。

- :cpp:member:`ppa_client_config_t::oper_type` - 每种 PPA 操作类型对应一个 PPA 客户端类型，已注册的 PPA 客户端只能请求一种特定类型的 PPA 操作。
- :cpp:member:`ppa_client_config_t::max_pending_trans_num` - 决定客户端可以持有的最大 PPA 事务挂起数量。

建议为每个任务注册一个独立的 PPA 客户端。例如，一个应用程序包含两个任务：任务 A 需要使用 PPA SRM 和 PPA 填充功能，则应在该任务中注册一个 PPA SRM 客户端和一个 PPA 填充客户端；而任务 B 也需要使用 PPA SRM 功能，那么在任务 B 中应再注册一个 PPA SRM 客户端。

如果任务不再需要执行 PPA 操作，可以调用 :cpp:func:`ppa_unregister_client` 函数注销相应的 PPA 客户端。

.. _ppa-register-callback:

注册 PPA 事件回调函数
^^^^^^^^^^^^^^^^^^^^^

当某个事件发生时（例如，完成一个 PPA 事务时），CPU 会通过中断接收到该事件的通知。如果需要在特定事件发生时调用某些特定函数，可以通过调用 :cpp:func:`ppa_client_register_event_callbacks` 为该事件注册回调函数。这在选择 ``PPA_TRANS_MODE_NON_BLOCKING`` 模式来执行 PPA 操作时特别有用。需要注意的是，虽然事件回调函数与 PPA 客户端绑定，但用户上下文是在调用 PPA 操作 API 时按事务提供的，这使事件回调函数能被更灵活得运用。

已注册的回调函数在中断上下文中被调用，因此，要遵循中断服务程序 (ISR) 的一般规则。

.. _ppa-perform-operation:

执行 PPA 操作
^^^^^^^^^^^^^

注册好 PPA 客户端，就可以用返回的 :cpp:type:`ppa_client_handle_t` 来请求执行 PPA 操作。

PPA 操作包括：

缩放、旋转、镜像 (SRM)
~~~~~~~~~~~~~~~~~~~~~~

调用 :cpp:func:`ppa_do_scale_rotate_mirror` 对图片内部的目标块执行缩放、旋转、镜像中的一个或多个操作。

注意以下几点事项，避免在配置 :cpp:type:`ppa_srm_oper_config_t` 时产生混淆：

.. list::

    - 执行 SRM 操作时，:cpp:member:`ppa_in_pic_blk_config_t::buffer` 和 :cpp:member:`ppa_out_pic_blk_config_t::buffer` 必须是指向不同图片 buffer 的指针。
    - :cpp:member:`ppa_srm_oper_config_t::scale_x` 和 :cpp:member:`ppa_srm_oper_config_t::scale_y` 的精度将被截断为 1/16 的步长。
    - 输出块的宽度/高度完全由输入块的宽度/高度、缩放因子和旋转角度决定，因此无需配置输出块的宽度/高度。但请确保输出块可以适应输出图片中的偏移位置。
    - 如果输入或输出图片的色彩模式为 ``PPA_SRM_COLOR_MODE_YUV420``，那么其 ``pic_w``、``pic_h``、``block_w``、``block_h``、``block_offset_x`` 以及 ``block_offset_y`` 字段必须为偶数。

叠加
~~~~

调用 :cpp:func:`ppa_do_blend` 将前景 (FG) 和背景 (BG) 图片的两个目标块进行叠加。

叠加遵循一般的 Alpha Blending 公式：

:math:`A_{out} = A_b + A_f - A_b \times A_f`

:math:`C_{out} = (C_b \times A_b \times (1 - A_f) + C_f \times A_f) / (A_b + A_f - A_b \times A_f)`

其中 :math:`A_b` 是背景层的 Alpha 通道，:math:`A_f` 是前景层的 Alpha 通道，:math:`C_b` 对应背景层的 R、G、B 分量，:math:`C_f` 对应前景层的 R、G、B 分量。

注意，此公式对 FG 和 BG 的处理是不对称的。当 :math:`A_f = 1` 时， :math:`C_{out} = C_f`，:math:`A_{out} = 1`，这意味着如果 FG 图片的色彩模式为 ``PPA_BLEND_COLOR_MODE_RGB565`` 或 ``PPA_BLEND_COLOR_MODE_RGB888``，PPA 硬件会填充 Alpha 值为 255（即 :math:`A_f = 1`），叠加结果将与 FG 块相同。

如果将 :cpp:member:`ppa_blend_oper_config_t::bg_ck_en` 或 :cpp:member:`ppa_blend_oper_config_t::fg_ck_en` 设置为 ``true``，则色键（color-key，也叫 Chroma-key）范围内的像素不会按照正常 Alpha Blending 流程输出。请查看 **{IDF_TARGET_NAME} 技术参考手册** > **像素处理加速器 (PPA)** > **功能描述** > **图层叠加 (BLEND)** [`PDF <{IDF_TARGET_TRM_EN_URL}#ppa>`__] 了解详细规则。

注意以下几点事项，避免在配置 :cpp:type:`ppa_blend_oper_config_t` 时产生混淆：

.. list::

    - 执行叠加操作时，:cpp:member:`ppa_out_pic_blk_config_t::buffer` 与输入的 :cpp:member:`ppa_in_pic_blk_config_t::buffer` 之一可以是指向相同图片 buffer 的指针。
    - FG 和 BG 的块宽度/高度应相同，输出块的宽度/高度值应与之一致。
    - 如果输入图片的色彩模式为 ``PPA_BLEND_COLOR_MODE_A4``，那么其 ``block_w`` 和 ``block_offset_x`` 字段必须为偶数。

填充
~~~~

调用 :cpp:func:`ppa_do_fill` 填充图片内部的目标块。

:cpp:type:`ppa_trans_mode_t` 为可配置字段，适用于所有 PPA 操作 API。可以配置该字段，在调用 PPA 操作 API 时等待操作完成后再返回，或者在事务推送到内部队列后立即返回。

.. _ppa-thread-safety:

线程安全
^^^^^^^^^

在以下情境中调用 PPA 操作 API，PPA 驱动程序可确保线程安全：

.. list::

    - 同一任务中不同类型的客户端之间
    - 不同任务中同一类型的客户端之间
    - 不同任务中不同类型的客户端之间

.. _ppa-performance-overview:

性能概述
^^^^^^^^^

PPA 操作作用于输入图片的目标块。因此，完成一次 PPA 事务所需的时间与块中的数据量成正比。整个图片的大小对性能没有影响。更重要的是，如果图片位于 PSRAM，则 PPA 的性能高度依赖于 PSRAM 的带宽。若有多个外设同时读写 PSRAM，则 PPA 操作的性能将大大降低。

应用示例
^^^^^^^^^

* :example:`peripherals/ppa/ppa_dsi` - 使用 DSI 显示屏的 PPA 示例。首先，该示例所使用的图像会被放大、逆时针旋转后复原、镜像后复原、缩小。其次，该图像将与一个透明度较低的全红图像叠加，`ESP32` 字样将被色键移除。最后，会在 `ESP32` 周围填充一个框。

API 参考
--------

.. include-build-file:: inc/ppa.inc
.. include-build-file:: inc/ppa_types.inc
