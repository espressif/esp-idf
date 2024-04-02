温度传感器
==================

:link_to_translation:`en:[English]`

简介
------------

{IDF_TARGET_NAME} 内置传感器，用于测量芯片内部的温度。该温度传感器模组包含一个 8 位 Sigma-Delta 模拟-数字转换器 (ADC) 和一个数字-模拟转换器 (DAC)，可以补偿测量结果，减少温度测量的误差。

由于硬件限制，温度传感器存在预定义的测量范围及其对应误差，详见下表：

.. list-table::
    :header-rows: 1
    :widths: 50 50
    :align: center

    * - 预定义测量范围 (°C)
      - 测量误差 (°C)
    * - 50 ~ 125
      - < 3
    * - 20 ~ 100
      - < 2
    * - -10 ~ 80
      - < 1
    * - -30 ~ 50
      - < 2
    * - -40 ~ 20
      - < 3

.. note::

    温度传感器主要用于测量芯片内部的温度变化。芯片内部温度通常高于环境温度，并且受到微控制器的时钟频率或 I/O 负载、外部散热环境等因素影响。

功能概述
-------------------

下文将分节概述温度传感器的功能：

.. list::

    - :ref:`temp-resource-allocation` - 介绍了部分参数，设置这些参数可以获取温度传感器句柄；还介绍了在温度传感器完成工作后如何回收资源。
    - :ref:`temp-enable-and-disable-temperature-sensor` - 介绍如何启用及禁用温度传感器。
    - :ref:`temp-get-temperature-value` - 介绍如何获取实时温度值。
    :SOC_TEMPERATURE_SENSOR_INTR_SUPPORT: - :ref:`temp-install-temperature-threshold-callback` - 介绍如何注册温度阈值回调函数。
    - :ref:`temp-power-management` - 介绍更改功耗模式（如 Light-sleep 模式）对温度传感器造成的影响。
    :SOC_TEMPERATURE_SENSOR_INTR_SUPPORT: - :ref:`temp-iram-safe` - 介绍在禁用 cache 时如何提高温度传感器的性能。
    - :ref:`temp-thread-safety` - 介绍如何使驱动程序具备线程安全性。
    :SOC_TEMPERATURE_SENSOR_SUPPORT_ETM: - :ref:`temperature-sensor-etm-event-and-task` - 介绍哪些事件和任务可以连接到 ETM 通道。

.. _temp-resource-allocation:

资源分配
^^^^^^^^^^^^^^^^^^^

{IDF_TARGET_NAME} 只有一个内置温度传感器硬件。:cpp:type:`temperature_sensor_handle_t` 表示温度传感器模块，该变量也是不同函数之间的纽带。通过使用相同的 :cpp:type:`temperature_sensor_handle_t` 变量，可以在不同的函数调用中访问和修改温度传感器属性，以控制和管理温度传感器。该变量会作为温度 API 的参数，携带有关硬件和配置的信息，你只需创建类型为 :cpp:type:`temperature_sensor_handle_t` 的指针，并将其传递给所需 API。

请在安装内置温度传感器模块前评估测量环境的温度范围。例如，如果在室内测量，环境温度可能在 10 °C ~ 30 °C；如果在灯泡中测量，环境温度则可能在 60 °C ~ 110 °C。在环境温度范围的基础上，请先根据以下值定义配置结构体 :cpp:type:`temperature_sensor_config_t`，再安装内置温度传感器：

- :cpp:member:`range_min`：所测量温度范围的最小值。
- :cpp:member:`range_max`：所测量温度范围的最大值。

设置好温度范围后，将配置结构体传递给 :cpp:func:`temperature_sensor_install`，该函数将创建温度传感器模块并返回句柄。

如前文所述，不同测量范围对应不同测量误差。然而你无需自行比对测量误差，乐鑫提供了一个内部机制，可以根据所给温度范围选择最小误差。

温度传感器使用完毕后，请调用 :cpp:func:`temperature_sensor_uninstall` 释放相应资源。

创建温度传感器句柄
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* 第 1 步：评估测量范围。本示例的温度范围为 20 °C ~ 50 °C。
* 第 2 步：配置测量范围，获取温度传感器句柄。

.. code:: c

    temperature_sensor_handle_t temp_handle = NULL;
    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(20, 50);
    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor_config, &temp_handle));

.. _temp-enable-and-disable-temperature-sensor:

启用及禁用温度传感器
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. 调用 :cpp:func:`temperature_sensor_enable` 启用温度传感器。此时，内部温度传感器回路开始工作，驱动程序从初始化状态转为启用状态。
2. 调用 :cpp:func:`temperature_sensor_disable` 禁用温度传感器。

.. _temp-get-temperature-value:

获取测量的温度值
^^^^^^^^^^^^^^^^^^^^^

通过 :cpp:func:`temperature_sensor_enable` 启用温度传感器后，可以调用 :cpp:func:`temperature_sensor_get_celsius` 获取当前测量的温度值。

.. code:: c

    // 启用温度传感器
    ESP_ERROR_CHECK(temperature_sensor_enable(temp_handle));
    // 获取传输的传感器数据
    float tsens_out;
    ESP_ERROR_CHECK(temperature_sensor_get_celsius(temp_handle, &tsens_out));
    printf("Temperature in %f °C\n", tsens_out);
    // 温度传感器使用完毕后，禁用温度传感器，节约功耗
    ESP_ERROR_CHECK(temperature_sensor_disable(temp_handle));


.. only:: SOC_TEMPERATURE_SENSOR_INTR_SUPPORT

    .. _temp-install-temperature-threshold-callback:

    安装温度阈值回调函数
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    {IDF_TARGET_NAME} 支持自动触发温度传感器，持续监测内部温度，内部温度达到给定阈值时将触发中断。因此，可以安装中断回调函数执行所需操作，如报警、重启等。下文介绍了如何准备阈值回调函数。

    - 函数 :cpp:member:`temperature_sensor_event_callbacks_t::on_threshold` 在中断服务程序 (ISR) 的上下文中调用，请确保该函数不会涉及 block 操作。为此，可以检查调用 API 的后缀，确保仅从函数内调用具有 ``ISR`` 后缀的 FreeRTOS API 等。函数原型在 :cpp:type:`temperature_thres_cb_t` 中声明。

    通过参数 ``user_arg`` 可以将自定义上下文保存到 :cpp:func:`temperature_sensor_register_callbacks` 中，用户数据将直接传递给回调函数。

    .. code:: c

        IRAM_ATTR static bool temp_sensor_monitor_cbs(temperature_sensor_handle_t tsens, const temperature_sensor_threshold_event_data_t *edata, void *user_data)
        {
            ESP_DRAM_LOGI("tsens", "Temperature value is higher or lower than threshold, value is %d\n...\n\n", edata->celsius_value);
            return false;
        }

        // 配置回调函数
        temperature_sensor_abs_threshold_config_t threshold_cfg = {
            .high_threshold = 50,
            .low_threshold = -10,
        };
        // 设置监控阈值
        temperature_sensor_set_absolute_threshold(temp_sensor, &threshold_cfg);
        // 注册中断回调函数
        temperature_sensor_event_callbacks_t cbs = {
            .on_threshold = temp_sensor_monitor_cbs,
        };
        // 安装温度回调函数
        temperature_sensor_register_callbacks(temp_sensor, &cbs, NULL);

    .. _temp-power-management:

.. only:: not SOC_TEMPERATURE_SENSOR_INTR_SUPPORT

    .. _temp-power-management:

电源管理
^^^^^^^^^^^^^^^^

由于温度传感器不使用 APB 时钟，无论是否激活 ``CONFIG_PM_ENABLE`` 启用电源管理，温度传感器仍可以继续工作。

.. only:: SOC_TEMPERATURE_SENSOR_INTR_SUPPORT

    .. _temp-iram-safe:

    IRAM 安全
    ^^^^^^^^^

    默认情况下，禁用 cache 时，写入/擦除 flash 等原因将导致温度传感器中断延迟，事件回调函数也将延迟执行。在实时应用程序中，应避免此类情况。

    因此，可以启用 Kconfig 选项 :ref:`CONFIG_TEMP_SENSOR_ISR_IRAM_SAFE`，该选项：

    1. 支持在禁用 cache 时启用所需中断
    2. 支持将 ISR 使用的所有函数存放在 IRAM 中

    启用该选项可以保证 cache 禁用时的中断运行，但会占用更多的 IRAM。

    .. _temp-thread-safety:

.. only:: not SOC_TEMPERATURE_SENSOR_INTR_SUPPORT

    .. _temp-thread-safety:

线程安全
^^^^^^^^^^^^^

温度传感器中并未添加任何确保线程安全的额外保护，因为温度传感器通常只在一个任务中调用。如果要在不同任务中使用该驱动程序，请设置额外的锁进行保护。

.. only:: SOC_TEMPERATURE_SENSOR_SUPPORT_ETM

    .. _temperature-sensor-etm-event-and-task:

    ETM 事件和任务
    ^^^^^^^^^^^^^^^^^^

    温度传感器能够生成事件，这些事件可以与 :doc:ETM </api-reference/peripherals/etm> 模块进行交互。:cpp:type:temperature_sensor_etm_event_type_t 中列出了所有支持的事件。可以调用 :cpp:func:temperature_sensor_new_etm_event 来获取相应的 ETM 事件句柄。:cpp:type:temperature_sensor_etm_task_type_t 中列出了所有支持的任务。可以调用 :cpp:func:temperature_sensor_new_etm_task 来获取相应的 ETM 任务句柄。

    .. note::

        - 对于 :cpp:member:`temperature_sensor_etm_event_type_t::event_type` 的 :cpp:enumerator:`TEMPERATURE_SENSOR_EVENT_OVER_LIMIT` 取决于首先设置的阈值类型。如果是通过 :cpp:func:`temperature_sensor_set_absolute_threshold` 设置了绝对阈值，那么 :cpp:enumerator:`TEMPERATURE_SENSOR_EVENT_OVER_LIMIT` 将指代绝对阈值。同样，如果是通过 :cpp:func:`temperature_sensor_set_delta_threshold` 设置了增量阈值，那么 :cpp:enumerator:`TEMPERATURE_SENSOR_EVENT_OVER_LIMIT` 将指代增量阈值。

    有关如何将事件和任务连接到 ETM 通道的详细信息，请参阅 :doc:ETM </api-reference/peripherals/etm> 文档。

意外情况
--------------------

1. 从芯片获取的温度值通常与环境温度不同，因为温度传感器内置于芯片，从某种程度来说，温度传感器测量的是芯片内的温度。

2. 安装温度传感器失败时，如果驱动程序打印的错误信息为 ``the boundary you gave cannot meet the range of internal temperature sensor``，说明内置温度传感器温度测量范围的限制影响了安装过程，该错误通常由以下几种不正确的 :cpp:type:`temperature_sensor_config_t` 配置造成：

    (1) 超出温度测量范围，如 200 °C ~ 300 °C。
    (2) 超过了预定义测量范围的界限，如 40 °C ~ 110 °C。

应用示例
-------------------

.. list::

    * 读取温度传感器测量值：:example:`peripherals/temperature_sensor/temp_sensor`。
    :SOC_TEMPERATURE_SENSOR_INTR_SUPPORT: * 监测温度传感器测量值：:example:`peripherals/temperature_sensor/temp_sensor_monitor`。

API 参考
----------------------------------

.. include-build-file:: inc/temperature_sensor.inc
.. include-build-file:: inc/temperature_sensor_types.inc

.. only:: SOC_TEMPERATURE_SENSOR_SUPPORT_ETM

    .. include-build-file:: inc/temperature_sensor_etm.inc
