GPIO & RTC GPIO
===============

:link_to_translation:`en:[English]`

GPIO 汇总
---------

.. include:: gpio/{IDF_TARGET_PATH_NAME}.inc
    :start-after: gpio-summary
    :end-before: ---


.. only:: SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

    当 GPIO 连接到 "RTC" 低功耗和模拟子系统时，{IDF_TARGET_NAME} 芯片还单独支持 "RTC GPIO"。可在以下情况时使用这些管脚功能：

    .. list::

        - 处于 Deep-sleep 模式时
        :SOC_ULP_SUPPORTED: - :doc:`超低功耗协处理器 (ULP) <../../api-reference/system/ulp>` 运行时
        - 使用 ADC/DAC 等模拟功能时

应用示例
-------------------

* GPIO 输出和输入中断示例：:example:`peripherals/gpio/generic_gpio`。

API 参考 - 普通 GPIO
---------------------------

.. include-build-file:: inc/gpio.inc
.. include-build-file:: inc/gpio_types.inc


.. only:: SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

    API 参考 - RTC GPIO
    ------------------------

    .. include-build-file:: inc/rtc_io.inc
    .. include-build-file:: inc/rtc_io_types.inc
