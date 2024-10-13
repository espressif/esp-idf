事件任务矩阵 (ETM)
=======================

:link_to_translation:`en:[English]`

简介
------------

如果外设 X 需要向外设 Y 发起事件通知，一般只能通过 CPU 中断实现。在此过程中，CPU 会代表外设 X，给外设 Y 发送通知。然而，在对时间敏感的应用程序中，CPU 中断引发的延迟不容忽视。

通过引入事件任务矩阵 (ETM) 模块，部分外设可以直接通过预先设置的连接关系，将事件通知发送给其他外设，无需 CPU 中断介入。由此，外设实现精确、低延迟同步，并减轻 CPU 负担。

.. blockdiag:: /../_static/diagrams/etm/etm_channel.diag
    :caption: ETM 通道概述
    :align: center

ETM 模块具有多个通道，这些通道支持用户根据需要进行配置，连接特定 **事件** 与 **任务**。激活某一事件时，ETM 将自动触发相应任务。

支持 ETM 功能的外设向 ETM 提供其事件和任务的连接关系。ETM 通道可以连接任意事件和任务，事件和任务甚至可以来自于同一个外设。然而，对于一个 ETM 通道，一次只能将一个事件与一个任务连接（即 1 对 1 关系）。如果要使用不同事件触发同一任务，则需申请多条 ETM 通道。

使用 ETM 通常可实现以下功能：

-  当定时器报警事件发生时，翻转 GPIO 电平
-  当在 GPIO 上监测到脉冲边沿时，启动 ADC 转换

功能概述
-------------------

下文将分节概述 ETM 的功能，并介绍配置和使用 ETM 模块的基本步骤：

- :ref:`etm-channel-allocation` - 介绍如何安装和卸载 ETM 通道。
- :ref:`etm-event` - 介绍如何分配新的 ETM 事件句柄，以及如何从不同外设获取现有句柄。
- :ref:`etm-task` - 介绍如何分配新的 ETM 任务句柄，以及如何从不同外设获取现有句柄。
- :ref:`etm-channel-control` - 介绍常见的 ETM 通道控制函数。
- :ref:`etm-thread-safety` - 列出了驱动程序中始终线程安全的 API。
- :ref:`etm-kconfig-options` - 列出了 ETM 支持的 Kconfig 选项，这些选项对驱动程序的行为会产生不同影响。

.. _etm-channel-allocation:

ETM 通道分配
^^^^^^^^^^^^^^^^^^^^^^

在 {IDF_TARGET_NAME} 中，存在许多相同的 ETM 通道 [1]_，各通道在软件中由 :cpp:type:`esp_etm_channel_handle_t` 表示。可用硬件资源汇集在资源池内，由 ETM 核心驱动程序管理，无需手动管理通道的分配和释放。ETM 核心驱动程序会在调用 :cpp:func:`esp_etm_new_channel` 时自动分配通道，在调用 :cpp:func:`esp_etm_del_channel` 时删除通道。分配通道的要求通过 :cpp:type:`esp_etm_channel_config_t` 配置。

在删除 ETM 通道前，请调用 :cpp:func:`esp_etm_channel_disable` 禁用要删除的通道，或确保该通道尚未由 :cpp:func:`esp_etm_channel_enable` 启用，再继续删除操作。

.. _etm-event:

ETM 事件
^^^^^^^^^

ETM 事件对其事件源进行了抽象，屏蔽了具体事件源的细节，并在软件中表示为 :cpp:type:`esp_etm_event_handle_t`，使应用程序可以更便捷地处理不同类型的事件。ETM 事件可以由各种外设产生，因此获取事件句柄的方法因外设而异。当不再需要某个事件时，请调用 :cpp:func:`esp_etm_channel_connect`，并传递一个 ``NULL`` 事件句柄，断开与事件的连接，随后调用 :cpp:func:`esp_etm_del_event`，释放事件资源。

GPIO 事件
~~~~~~~~~~~

GPIO **边沿** 事件是最常见的事件类型，任何 GPIO 管脚均可触发这类事件。要创建 GPIO 事件句柄，请调用 :cpp:func:`gpio_new_etm_event`，并使用 :cpp:type:`gpio_etm_event_config_t` 提供的配置信息：

- :cpp:member:`gpio_etm_event_config_t::edge` 或 :cpp:member:`gpio_etm_event_config_t::edges` 决定触发事件的边沿类型，支持的边沿类型已在 :cpp:type:`gpio_etm_event_edge_t` 中列出。

接下来，请调用 :cpp:func:`gpio_etm_event_bind_gpio` 函数，连接 GPIO ETM 事件句柄与 GPIO 管脚。注意，要设置 GPIO 管脚，只能使用由 :cpp:func:`gpio_new_etm_event` 函数创建的 ETM 事件句柄。对于其他类型的 ETM 事件，调用此函数，将返回 :c:macro:`ESP_ERR_INVALID_ARG` 错误。该函数也无法完成 GPIO 的初始化，在使用 GPIO ETM 事件之前，仍需调用 :cpp:func:`gpio_config` 函数，设置 GPIO 管脚的属性，如方向、高/低电平模式等。

其他外设事件
~~~~~~~~~~~~~~~~~~~~~~~

.. list::

    :SOC_SYSTIMER_SUPPORT_ETM: - 调用 :cpp:func:`esp_systick_new_etm_alarm_event` 可以从 RTOS Systick 获取 ETM 事件句柄，每个 CPU 核心可以获取一个事件句柄。
    :SOC_SYSTIMER_SUPPORT_ETM: - 要了解如何从 esp_timer 获取 ETM 事件句柄，请参阅 :doc:`/api-reference/system/esp_timer`。
    :SOC_TIMER_SUPPORT_ETM: - 要了解如何从 GPTimer 获取 ETM 事件句柄，请参阅 :doc:`/api-reference/peripherals/gptimer`。
    :SOC_GDMA_SUPPORT_ETM: - 要了解如何从 async memcpy 获取 ETM 事件句柄，请参阅 :doc:`/api-reference/system/async_memcpy`。
    :SOC_MCPWM_SUPPORT_ETM: - 要了解如何从 MCPWM 中获取 ETM 事件句柄，请参阅 :doc:`/api-reference/peripherals/mcpwm`。
    :SOC_ANA_CMPR_SUPPORT_ETM: - 要了解如何从模拟比较器获取 ETM 事件句柄，请参阅 :doc:`/api-reference/peripherals/ana_cmpr`。
    :SOC_TEMPERATURE_SENSOR_SUPPORT_ETM: - 要了解如何从温度传感器获取 ETM 事件句柄，请参阅 :doc:`/api-reference/peripherals/temp_sensor`。
    :SOC_I2S_SUPPORTS_ETM:  - 要了解如何从 I2S 获取 ETM 事件句柄，请参阅 :doc:`/api-reference/peripherals/i2s`。

.. _etm-task:

ETM 任务
^^^^^^^^

ETM 任务对其操作进行了抽象，在软件中表示为 :cpp:type:`esp_etm_task_handle_t`，使任务得以用同一方式管理和表示。ETM 任务可以分配给不同外设，因此获取任务句柄的方式因外设而异。当不再需要某个任务时，请调用 :cpp:func:`esp_etm_channel_connect`，并传递一个 ``NULL`` 事件句柄，断开与任务的连接，随后调用 :cpp:func:`esp_etm_del_event`，释放任务资源。

GPIO 任务
~~~~~~~~~~

GPIO 任务是最常见的任务类型。一个 GPIO 可以采取一个或多个 GPIO 操作，而一个 GPIO 任务也可以同时管理多个 GPIO 管脚。当 ETM 通道激活任务时，任务可以同时设置管理的所有 GPIO 引脚，使其设置/清除/切换状态。要创建 GPIO 任务句柄，请调用 :cpp:func:`gpio_new_etm_task`，并使用 :cpp:type:`gpio_etm_task_config_t` 提供的配置信息：

- :cpp:member:`gpio_etm_task_config_t::action` 或 :cpp:member:`gpio_etm_task_config_t::actions` 决定 ETM 任务将采取的 GPIO 操作，支持的操作类型在 :cpp:type:`gpio_etm_task_action_t` 中列出。如果一个 GPIO 需要采取多个 GPIO 操作，这些操作任务的创建必须通过配置 :cpp:member:`gpio_etm_task_config_t::actions` 的数组并在一次 :cpp:func:`gpio_new_etm_task` 调用中一并完成。

接下来，需要连接 GPIO ETM 任务句柄与 GPIO 管脚。为此，请调用 :cpp:func:`gpio_etm_task_add_gpio` 函数。如果需要任务句柄管理更多的 GPIO 管脚，可以重复调用以上函数，注意，要设置 GPIO 管脚，只能使用由 :cpp:func:`gpio_new_etm_task` 函数创建的 ETM 任务句柄。对于其他类型的 ETM 任务，调用此函数，将返回 :c:macro:`ESP_ERR_INVALID_ARG` 错误。该函数也无法完成 GPIO 的初始化，在使用 GPIO ETM 任务之前，仍需调用 :cpp:func:`gpio_config` 函数，设置 GPIO 管脚的属性，如方向、高/低电平模式等。

要删除 GPIO ETM 任务，请调用 :cpp:func:`esp_etm_del_task`。在此之前，请确保已经调用过 :cpp:func:`gpio_etm_task_rm_gpio`，删除了所有先前添加的 GPIO 管脚。

其他外设任务
~~~~~~~~~~~~~~~~~~~~~~

.. list::

    :SOC_TIMER_SUPPORT_ETM: - 要了解如何从 GPTimer 获取 ETM 任务句柄，请参阅 :doc:`/api-reference/peripherals/gptimer`。
    :SOC_TEMPERATURE_SENSOR_SUPPORT_ETM: - 要了解如何从温度传感器获取 ETM 任务句柄，请参阅 :doc:`/api-reference/peripherals/temp_sensor`。
    :SOC_I2S_SUPPORTS_ETM:  - 要了解如何从 I2S 获取 ETM 任务句柄，请参阅 :doc:`/api-reference/peripherals/i2s`。

.. _etm-channel-control:

ETM 通道控制
^^^^^^^^^^^^^^^^^^^

映射事件与任务
~~~~~~~~~~~~~~~~~~~~~~

在调用 :cpp:func:`esp_etm_channel_connect` 将它们连接到同一个 ETM 通道之前，ETM 事件与 ETM 任务之间没有任何映射关系。注意，使用 ``NULL`` 任务/事件句柄调用该函数时，会将通道与任何任务或事件解除映射。此函数可以在通道启用之前或之后调用，但在运行时调用此函数更改映射关系存在一定风险，因为此时通道可能正处于周期的中间阶段，新的映射可能无法立即生效。

启用及禁用通道
~~~~~~~~~~~~~~~~~~~~~~~~~~

调用 :cpp:func:`esp_etm_channel_enable` 启用 ETM 通道，调用 :cpp:func:`esp_etm_channel_disable` 禁用 ETM 通道。

ETM 通道分析
~~~~~~~~~~~~~~~~~~~~~

要检查是否为 ETM 通道设置了正确的事件和任务，可以调用 :cpp:func:`esp_etm_dump`，输出所有工作中的 ETM 通道及其关联的事件和任务。输出格式如下：

::

    ===========ETM Dump Start==========
    channel 0: event 48 ==> task 17
    channel 1: event 48 ==> task 90
    channel 2: event 48 ==> task 94
    ===========ETM Dump End============

以上输出信息打印的数字 ID 在 ``soc/soc_etm_source.h`` 文件中定义。

.. _etm-thread-safety:

线程安全
^^^^^^^^^^^^^

ETM 驱动程序会确保工厂函数 :cpp:func:`esp_etm_new_channel` 和 :cpp:func:`gpio_new_etm_task` 的线程安全。使用时，可以直接从不同的 RTOS 任务中调用此类函数，无需额外锁保护。

在 ISR 环境中，不支持运行任何函数。

其他以 :cpp:type:`esp_etm_channel_handle_t`、:cpp:type:`esp_etm_task_handle_t` 和 :cpp:type:`esp_etm_event_handle_t` 作为首个位置参数的函数，则非线程安全，应避免从不同任务中调用此类函数。

.. _etm-kconfig-options:

Kconfig 选项
^^^^^^^^^^^^^^^

- :ref:`CONFIG_ETM_ENABLE_DEBUG_LOG` 用于启用调试日志输出，启用此选项将增加固件的二进制文件大小。

API 参考
-------------

.. include-build-file:: inc/esp_etm.inc
.. include-build-file:: inc/gpio_etm.inc
.. include-build-file:: inc/esp_systick_etm.inc

.. [1]
   不同 ESP 芯片系列的 ETM 通道数量可能不同。要了解更多详情，请参阅 **{IDF_TARGET_NAME} 技术参考手册** > **事件任务矩阵 (ETM)** [`PDF <{IDF_TARGET_TRM_EN_URL}#evntaskmatrix>`__]。驱动程序对通道申请数量不做限制，但当硬件资源用尽时，驱动程序将返回错误。因此，每次进行通道分配（即调用 :cpp:func:`esp_etm_new_channel`）时，请注意检查返回值。
