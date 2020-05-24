LED PWM 控制器
==============

:link_to_translation:`en:[English]`

概述
------------

LED PWM 控制器主要用于控制 LED，也可产生 PWM 信号用于其他设备的控制。该控制器有 8 路高速通道和 8 路低速通道，可以产生独立的波形来驱动 RGB LED 设备等。

LED PWM 控制器的高速通道和低速通道均支持硬件渐变功能，可在无需 CPU 干预的情况下自动改变 PWM 信号的占空比，也可由软件改变 PWM 信号的占空比，实现亮度和颜色渐变。此外，低速通道在 Sleep 模式下仍可运行。


功能概览
----------------------

要让指定 LED PWM 控制器 :ref:`高速模式或低速模式 <ledc-api-high_low_speed_mode>` 通道运行，需进行如下配置：

1. :ref:`ledc-api-configure-timer` 指定 PWM 信号的频率和占空比分辨率。
2. :ref:`ledc-api-configure-channel` 绑定定时器和输出 PWM 信号的 GPIO。
3. :ref:`ledc-api-change-pwm-signal` 输出 PWM 信号来驱动 LED。可通过软件控制或使用硬件渐变功能来改变 LED 的亮度。


.. figure:: ../../../_static/ledc-api-settings.jpg
    :align: center
    :alt: Key Settings of LED PWM Controller's API
    :figclass: align-center

    配置 LED PWM 控制器的关键 API


.. _ledc-api-configure-timer:

配置定时器
^^^^^^^^^^^^^^^

要设置定时器，可调用函数 :cpp:func:`ledc_timer_config`，并将包括如下配置参数的数据结构 :cpp:type:`ledc_timer_config_t` 传递给该函数：

    - 定时器索引 :cpp:type:`ledc_timer_t`
    - 速度模式 :cpp:type:`ledc_mode_t`
    - PWM 信号频率
    - PWM 占空比分辨率

频率和占空比分辨率相互关联。PWM 频率越高，占空比分辨率越低，反之则越高。使用该 API 用于除改变 LED 亮度以外的其他目的时，这一点很重要。更多信息详见 :ref:`ledc-api-supported-range-frequency-duty-resolution` 一节。


.. _ledc-api-configure-channel:

配置通道
^^^^^^^^^^^^^^^^^

定时器设置好后，请配置选定的通道（:cpp:type:`ledc_channel_t` 之一）。配置通道需调用函数 :cpp:func:`ledc_channel_config`。

通道的配置与定时器设置类似，需向通道配置函数传递包括通道配置参数的结构 :cpp:type:`ledc_channel_config_t` 。

此时，通道会按照 :cpp:type:`ledc_channel_config_t` 的配置，在绑定的 GPIO 上输出具有指定频率和占空比的 PWM 信号。在通道工作过程中，可以随时通过调用函数 :cpp:func:`ledc_stop` 将其暂停。


.. _ledc-api-change-pwm-signal:

改变 PWM 信号
^^^^^^^^^^^^^^^^^

通道开始运行、生成具有恒定占空比和频率的 PWM 信号之后，有几种方式可以改变该信号。驱动 LED 时，主要通过改变占空比来变化光线亮度。

以下两节介绍了如何使用软件和硬件改变占空比。如有需要，PWM 信号的频率也可更改，详见 :ref:`ledc-api-change-pwm-frequency` 一节。


使用软件改变 PWM 占空比
""""""""""""""""""""""""""""""""""""

调用函数 :cpp:func:`ledc_set_duty` 可以设置新的占空比。之后，调用函数 :cpp:func:`ledc_update_duty` 使新配置生效。要查看当前的占空比，可使用 ``_get_`` 函数 :cpp:func:`ledc_get_duty`。

另外一种设置占空比和其他通道参数的方式是调用 :ref:`ledc-api-configure-channel` 一节提到的函数 :cpp:func:`ledc_channel_config`。

传递给函数的占空比数值范围取决于选定的 ``duty_resolution``，应为 ``0`` 至 ``(2 ** duty_resolution) - 1``。例如，如选定的占空比分辨率为 10，则占空比的数值范围为 0 至 1023。此时分辨率为 ~0.1%。


使用硬件渐变改变 PWM 占空比
""""""""""""""""""""""""""""""""""""

LED PWM 控制器硬件可逐渐改变占空比的数值。要使用此功能，需用函数 :cpp:func:`ledc_fade_func_install` 使能渐变，之后用下列可用渐变函数之一配置：

* :cpp:func:`ledc_set_fade_with_time`
* :cpp:func:`ledc_set_fade_with_step`
* :cpp:func:`ledc_set_fade`

最后用 :cpp:func:`ledc_fade_start` 开启渐变。

如不需要渐变和渐变中断，可用函数 :cpp:func:`ledc_fade_func_uninstall` 关闭。


.. _ledc-api-change-pwm-frequency:

改变 PWM 频率
""""""""""""""""""""

LED PWM 控制器 API 有多种方式即时改变 PWM 频率：

    * 通过调用函数 :cpp:func:`ledc_set_freq` 设置频率。可用函数 :cpp:func:`ledc_get_freq` 查看当前频率。
    * 通过调用函数 :cpp:func:`ledc_bind_channel_timer` 改变频率和占空比分辨率，将其他定时器与通道相连。
    * 通过调用函数 :cpp:func:`ledc_channel_config` 改变通道的定时器。


控制 PWM 的更多方式
"""""""""""""""""""""

要改变 PWM 设置，可使用低电平定时器的特定功能：

* :cpp:func:`ledc_timer_set`
* :cpp:func:`ledc_timer_rst`
* :cpp:func:`ledc_timer_pause`
* :cpp:func:`ledc_timer_resume`

前两个功能可通过函数 :cpp:func:`ledc_channel_config` 在后台运行，在定时器配置后启动。


使用中断
^^^^^^^^^^^^^^

配置 LED PWM 控制器通道时，可在 :cpp:type:`ledc_channel_config_t` 中选取参数 :cpp:type:`ledc_intr_type_t` ，在渐变完成时触发中断。

要注册处理程序来处理中断，可调用函数 :cpp:func:`ledc_isr_register`。


.. _ledc-api-high_low_speed_mode:

LED PWM 控制器高速和低速模式
----------------------------

LED PWM 控制器有 8 个可用定时器和 16 路通道，其中有一半专以高速模式运行，另一半则以低速模式运行。要选取高速或低速定时器或通道，可借助所调用函数中的参数 :cpp:type:`ledc_mode_t` 。

高速模式的优点是可平稳地改变定时器设置。也就是说，高速模式下如定时器设置改变，此变更会自动应用于定时器的下一次溢出中断。而更新低速定时器时，设置变更应由软件显式触发。LED PWM 驱动器在后台更改设置，比如在调用函数 :cpp:func:`ledc_timer_config` 或 :cpp:func:`ledc_timer_set` 时。

更多关于速度模式的详细信息请参阅 `ESP32 技术参考手册 <https://espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_cn.pdf>`_ (PDF)。注意，该手册中提到的支持 ``SLOW_CLOCK`` 暂不适用于 LED PWM 驱动器。


.. _ledc-api-supported-range-frequency-duty-resolution:

频率和占空比分辨率支持范围
-------------------------------------------------

LED PWM 控制器主要用于驱动 LED。该控制器 PWM 占空比设置的分辨率范围较广。比如，PWM 频率为 5 kHz 时，占空比分辨率最大可为 13 位。这意味着占空比可为 0 至 100% 之间的任意值，分辨率为 ~0.012%（2 ** 13 = 8192 LED 亮度的离散电平）。

LED PWM 控制器可用于生成频率较高的信号，足以为数码相机模组等其他设备计时。此时，最大频率可为 40 MHz，占空比分辨率为 1 位。也就是说，占空比固定为 50%，无法调整。

LED PWM 控制器 API 可在设定的频率和占空比分辨率超过 LED PWM 控制器硬件范围时报错。例如，试图将频率设置为 20 MHz、占空比分辨率设置为 3 位时，串行端口监视器上会报告如下错误：

.. highlight:: none

::

    E (196) ledc: requested frequency and duty resolution cannot be achieved, try reducing freq_hz or duty_resolution. div_param=128

此时，占空比分辨率或频率必须降低。比如，将占空比分辨率设置为 2 会解决这一问题，让占空比设置为 25% 的倍数，即 25%、50% 或 75%。

如设置的频率和占空比分辨率低于所支持的最低值，LED PWM 驱动器也会反映并报告，如： 

::

    E (196) ledc: requested frequency and duty resolution cannot be achieved, try increasing freq_hz or duty_resolution. div_param=128000000

占空比分辨率通常用 :cpp:type:`ledc_timer_bit_t` 设置，范围是 10 至 15位。如需较低的占空比分辨率（上至 10，下至 1），可直接输入相应数值。


应用实例
-------------------

LED PWM 改变占空比和渐变控制的实例请参照 :example:`peripherals/ledc`。


API 参考
-------------

.. include-build-file:: inc/ledc.inc
.. include-build-file:: inc/ledc_types.inc

