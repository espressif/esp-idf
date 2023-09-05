触摸元件
=============

:link_to_translation:`en:[English]`

概述
--------

触摸元件库是基于触摸传感器驱动设计的高度抽象的元件库，该库提供了统一且友好的软件接口，可以快速构建电容式触摸传感器的应用。有关触摸传感器驱动 API 的更多信息，请参阅 :doc:`../peripherals/touch_pad`。

架构
^^^^^^^^^^^^

触摸元件库通过触摸传感器驱动程序配置触摸传感器外设。使用时，部分必要的硬件参数需要传递给函数 :cpp:func:`touch_element_install`，但只有在调用 :cpp:func:`touch_element_start` 函数后，才会自动配置这些参数。这些参数配置会对实时系统产生很大的影响，因此步骤顺序很重要，必须在调用启动函数之后进行配置，确保系统正常运行。

上述参数包括触摸通道阈值、防水屏蔽传感器驱动级别等。触摸元件库会设置触摸传感器中断和 esp_timer 例程，并在触摸传感器中断服务例程中获取触摸传感器的硬件信息（通道状态、通道编号）。当特定通道事件发生时，硬件信息将传递给 esp_timer 回调例程，esp_timer 回调例程将触摸传感器通道信息分配给触摸元件（例如按键、滑条等）。随后，触摸元件库运行特定算法，更新触摸元件状态或计算其位置，并将结果分派给相应的处理程序。

因此，在使用触摸元件库时，你无需关注触摸传感器外设的工作细节，该库会处理大部分硬件信息，并将更有意义的信息传递给事件处理程序。

下图展示了触摸元件库的工作流程。

.. figure:: /../_static/touch_element/te_architecture.svg
    :scale: 100 %
    :align: center

    触摸元件库架构


下表展示了 {IDF_TARGET_NAME} 中与触摸元件库有关的功能。

.. list-table::
    :header-rows: 1
    :widths: 70 30
    :align: center

    * - 功能
      - ESP32S2
    * - 防水
      - ✔
    * - 按键
      - ✔
    * - 触摸滑条
      - ✔
    * - 矩阵按键
      - ✔


外设
^^^^^^^^^^

{IDF_TARGET_NAME} 集成了一个触摸传感器外设，具有多个物理通道。

.. only:: esp32s2 or esp32s3

    - 14 个物理电容触摸通道
    - 定时器或软件 FSM 触发模式
    - 高达 5 种中断（高阈值和低阈值中断、测量单通道完成和测量所有通道完成中断、测量超时中断）
    - 睡眠模式唤醒源
    - 硬件内置降噪
    - 硬件滤波器
    - 硬件防水传感器
    - 硬件近场感应传感器

这些通道的具体位置如下：

.. list-table::
    :header-rows: 1
    :widths: 50 50
    :align: center

    * - 通道
      - {IDF_TARGET_NAME}
    * - **通道 0**
      - **GPIO 0（保留）**
    * - **通道 1**
      - **GPIO 1**
    * - **通道 2**
      - **GPIO 2**
    * - **通道 3**
      - **GPIO 3**
    * - **通道 4**
      - **GPIO 4**
    * - **通道 5**
      - **GPIO 5**
    * - **通道 6**
      - **GPIO 6**
    * - **通道 7**
      - **GPIO 7**
    * - **通道 8**
      - **GPIO 8**
    * - **通道 9**
      - **GPIO 9**
    * - **通道 10**
      - **GPIO 10**
    * - **通道 11**
      - **GPIO 11**
    * - **通道 12**
      - **GPIO 12**
    * - **通道 13**
      - **GPIO 13**
    * - **通道 14**
      - **GPIO 14**


.. only:: esp32

    - 10 个物理电容触摸通道
    - 定时器或软件 FSM 触发模式
    - 2 种中断（超过阈值和低于阈值中断）
    - 睡眠模式唤醒源

这些通道的具体位置如下：

.. list-table::
    :header-rows: 1
    :widths: 50 50
    :align: center

    * - 通道
      - {IDF_TARGET_NAME}
    * - **通道 0**
      - **GPIO 0（有效）**
    * - **通道 1**
      - **GPIO 1**
    * - **通道 2**
      - **GPIO 2**
    * - **通道 3**
      - **GPIO 3**
    * - **通道 4**
      - **GPIO 4**
    * - **通道 5**
      - **GPIO 5**
    * - **通道 6**
      - **GPIO 6**
    * - **通道 7**
      - **GPIO 7**
    * - **通道 8**
      - **GPIO 8**
    * - **通道 9**
      - **GPIO 9**


术语
-----------

触摸元件库的有关术语如下：

.. list-table::
    :header-rows: 1
    :widths: 50 50
    :align: center

    * - 术语
      - 定义
    * - **触摸传感器 (touch sensor)**
      - 芯片内部的触摸传感器外设
    * - **触摸通道 (touch channel)**
      - 触摸传感器外设内的触摸通道
    * - **触摸焊盘 (touch pad)**
      - 外部物理触摸焊盘，通常位于 PCB 内部
    * - **降噪通道 (de-noise channel)**
      - 内部降噪通道，始终为通道 0 且已预留
    * - **屏蔽传感器 (shield sensor)**
      - 防水传感器之一，用于小面积的水滴检测，并补偿水滴对读数的影响
    * - **防护传感器 (guard sensor)**
      - 防水传感器之一，用于大面积的涉水检测，并临时禁用触摸传感器
    * - **屏蔽通道 (shield channel)**
      - 防水屏蔽传感器连接的通道，始终为通道 14
    * - **防护通道 (guard channel)**
      - 防水防护传感器连接的通道
    * - **屏蔽焊盘 (shield pad)**
      - 外部物理屏蔽焊盘，通常是网格状，与防水传感器相连
    * - **防护焊盘 (guard pad)**
      - 外部物理防护焊盘，通常是环状，与防护传感器相连

.. figure:: /../_static/touch_element/te_component.svg
    :scale: 100 %
    :align: center

    触摸传感器应用程序系统组件


触摸传感器信号
^^^^^^^^^^^^^^^^^^^

触摸传感器可提供以下信号：

- 原始信号：从触摸传感器获取、未经滤波的信号。
- 平滑信号：原始信号通过内部硬件滤波器滤波后的信号。
- 基准信号：经过滤波的信号，已过滤极低频噪声。

以上信号均可通过触摸传感器驱动程序 API 获取。

.. figure:: /../_static/touch_element/te_signal.png
    :scale: 40 %
    :align: center

    触摸传感器信号


触摸传感器信号阈值
^^^^^^^^^^^^^^^^^^^^^^

触摸传感器阈值支持重新配置，可用于确定触摸传感器状态。当平滑信号和基准信号间的差值大于阈值，即 ``（平滑信号 - 基准信号）> 信号阈值`` 时，触摸通道状态改变，并触发触摸中断。

.. figure:: /../_static/touch_element/te_threshold.svg
    :scale: 40 %
    :align: center

    触摸传感器信号阈值


灵敏度
^^^^^^^^^^^^^^^^

触摸传感器的一个重要性能参数，该值越大，表明触摸传感器越灵敏。可以通过以下公式计算：

.. math::

    Sensitivity = \frac{Signal_{press} - Signal_{release}}{Signal_{release}} = \frac{Signal_{delta}}{Signal_{benchmark}}


防水性能
^^^^^^^^^^^^^^^^^^

防水性能是触摸传感器的硬件功能，包括防护传感器和屏蔽传感器（始终连接到通道 14），可以抵御一定程度的水滴影响，并检测水流。


触摸按键
^^^^^^^^^^^^^^^^^^

触摸按键占用触摸传感器的一个通道，外观如下图所示：


.. figure:: /../_static/touch_element/te_button.svg
    :scale: 100 %
    :align: center

    触摸按键


触摸滑条
^^^^^^^^^^^^^^^^^^

触摸滑条占用触摸传感器的多个通道（至少三个），占用的通道越多，滑条的采样分辨率和准确度越高。触摸滑条外观如下图所示：

.. figure:: /../_static/touch_element/te_slider.svg
    :scale: 100 %
    :align: center

    触摸滑条


触摸矩阵
^^^^^^^^^^^^^^^^^^

触摸矩阵按键占用触摸传感器的多个通道（至少 2 + 2 = 4 个通道），它支持通过较少通道获取更多按键。{IDF_TARGET_NAME} 最多支持 49 个按键。触摸矩阵按键外观如下图所示：

.. figure:: /../_static/touch_element/te_matrix.svg
    :scale: 100 %
    :align: center

    触摸矩阵


使用触摸元件库
--------------

使用触摸元件库时，请遵循以下初始化流程：

1. 调用 :cpp:func:`touch_element_install`，初始化触摸元件库。
2. 调用 :cpp:func:`touch_xxxx_install`，初始化触摸元件（按键、滑条等）。
3. 调用 :cpp:func:`touch_xxxx_create`，创建新元件实例。
4. 调用 :cpp:func:`touch_xxxx_subscribe_event`，订阅事件通知。
5. 调用 :cpp:func:`touch_xxxx_set_dispatch_method`，选择事件通知的传递方式。
6. 选择使用回调函数传递事件通知时，调用 :cpp:func:`touch_xxxx_set_callback`，设置事件处理函数。
7. 调用 :cpp:func:`touch_element_start`，启用触摸元件库。
8. 选择使用回调函数传递事件通知时，若事件发生，驱动核心会调用回调函数，你无需做任何处理；选择使用事件任务传递事件通知时，你需要创建一个事件任务，并调用 :cpp:func:`touch_element_message_receive`，循环获取信息。
9. （可选）如果要暂停触摸元件的实时系统，或因某种原因无法获取触摸元件信息，应调用 :cpp:func:`touch_element_stop`，暂停触摸元件系统，然后再次调用 :cpp:func:`touch_element_start` 恢复系统。

上述流程代码如下所示：

.. code-block:: c

    static touch_xxx_handle_t element_handle; //声明一个触摸元件句柄

    //定义订阅的事件处理函数
    void event_handler(touch_xxx_handle_t out_handle, touch_xxx_message_t out_message, void *arg)
    {
        //事件处理逻辑
    }

    void app_main()
    {
        //使用默认初始化器配置触摸元件库
        touch_elem_global_config_t global_config = TOUCH_ELEM_GLOBAL_DEFAULT_CONFIG();
        touch_element_install(&global_config);

        //使用默认初始化器配置触摸元件
        touch_xxx_global_config_t elem_global_config = TOUCH_XXXX_GLOBAL_DEFAULT_CONFIG();
        touch_xxx_install(&elem_global_config);

        //创建新的实例
        touch_xxx_config_t element_config = {
            ...
            ...
        };
        touch_xxx_create(&element_config, &element_handle);

        //通过事件掩码订阅特定事件
        touch_xxx_subscribe_event(element_handle, TOUCH_ELEM_EVENT_ON_PRESS | TOUCH_ELEM_EVENT_ON_RELEASE, NULL);

        //选择使用回调函数传递事件通知
        touch_xxx_set_dispatch_method(element_handle, TOUCH_ELEM_DISP_CALLBACK);

        //注册回调函数
        touch_xxx_set_callback(element_handle, event_handler);

        //启用触摸元件库处理
        touch_element_start();
    }


初始化
^^^^^^^^^^^^^^

1. 要初始化触摸元件库，请调用 :cpp:func:`touch_element_install` 函数，并传递一个 :cpp:type:`touch_elem_global_config_t` 类型的实例作为参数，以配置触摸传感器外设和触摸元件库。默认初始化器位于 :cpp:func:`TOUCH_ELEM_GLOBAL_DEFAULT_CONFIG` 中，此默认配置适用于多数常见应用场景。建议在充分了解触摸传感器外设前，不要更改默认配置，以免影响系统。

2. 要初始化特定的触摸元件，需要调用其构造函数 :cpp:func:`touch_xxxx_install`。在调用此构造函数前，所有触摸元件都不会工作，以节省内存。因此，若要设置所需元件，需要为每个要使用的触摸元件分别调用构造函数。


启动触摸元件实例
^^^^^^^^^^^^^^^^

1. 要创建新的触摸元件实例，请调用 :cpp:func:`touch_xxxx_create`，选择一个通道，并将其 `灵敏度`_ 传递给新的元件实例。

2. 要订阅事件通知，请调用 :cpp:func:`touch_xxxx_subscribe_event`。触摸元件库提供了多个事件，事件掩码存放在 :idf_file:`components/touch_element/include/touch_element/touch_element.h` 中。通过使用这些事件掩码，可以订阅单个特定事件，或将单个事件组合在一起，订阅多个事件。

3. 要配置传递事件通知的方式，请调用 :cpp:func:`touch_xxxx_subscribe_event`。触摸元件库提供了两种方式: :cpp:enumerator:`TOUCH_ELEM_DISP_EVENT` 和 :cpp:enumerator:`TOUCH_ELEM_DISP_CALLBACK`，支持以不同方式获取并处理触摸元件信息。

事件处理
^^^^^^^^^^^^^^^^^

如果配置的是 :cpp:enumerator:`TOUCH_ELEM_DISP_EVENT`，需要启用一个事件处理任务获取触摸元件信息。调用 :cpp:func:`touch_element_message_receive` 可以获取所有元件的原始信息，随后通过调用相应的信息解码器 :cpp:func:`touch_xxxx_get_message`，提取特定类型元件的信息，获取有关触摸操作的详细数据。如果配置的是 :cpp:enumerator:`TOUCH_ELEM_DISP_CALLBACK`，在触摸元件开始工作之前，需要调用 :cpp:func:`touch_xxxx_set_callback`，传递一个事件处理函数，有关触摸操作的详细数据都会传递到该事件处理函数。

.. warning::

    由于事件处理函数在元件库的核心运行，即在 esp-timer 回调中运行，请避免执行可能导致阻塞或延迟的操作，如调用 :cpp:func:`vTaskDelay`。


事件处理过程代码如下所示：

.. code-block:: c

    /* ---------------------------------------------- TOUCH_ELEM_DISP_EVENT ----------------------------------------------- */
    void element_handler_task(void *arg)
    {
        touch_elem_message_t element_message;
        while(1) {
            if (touch_element_message_receive(&element_message, Timeout) == ESP_OK) {
                const touch_xxxx_message_t *extracted_message = touch_xxxx_get_message(&element_message); //信息解码
                ... //事件处理逻辑
            }
        }
    }
    void app_main()
    {
        ...

        touch_xxxx_set_dispatch_method(element_handle, TOUCH_ELEM_DISP_EVENT);  //设置以 TOUCH_ELEM_DISP_EVENT 传递事件通知
        xTaskCreate(&element_handler_task, "element_handler_task", 2048, NULL, 5, NULL);  //创建一个事件处理任务

        ...
    }
    /* -------------------------------------------------------------------------------------------------------------- */

    ...
    /* ---------------------------------------------- TOUCH_ELEM_DISP_CALLBACK ----------------------------------------------- */
    void element_handler(touch_xxxx_handle_t out_handle, touch_xxxx_message_t out_message, void *arg)
    {
        //事件处理逻辑
    }

    void app_main()
    {
        ...

        touch_xxxx_set_dispatch_method(element_handle, TOUCH_ELEM_DISP_CALLBACK);  //设置以 ``TOUCH_ELEM_DISP_CALLBACK`` 传递事件通知
        touch_xxxx_set_callback(element_handle, element_handler);  //注册事件处理函数

        ...
    }
    /* -------------------------------------------------------------------------------------------------------------- */


使用防水功能
^^^^^^^^^^^^^^^^

1. 一旦初始化触摸元件的防水功能，防水屏蔽传感器会始终处于开启状态。防水屏蔽传感器为可选项，如果不需要，可以通过配置结构体，将 ``TOUCH_WATERPROOF_GUARD_NOUSE`` 传递给 :cpp:func:`touch_element_waterproof_install`。

2. 要关联触摸元件与防护传感器，请调用 :cpp:func:`touch_element_waterproof_add`，将触摸元件句柄添加到触摸元件防水功能的掩码列表中。触摸元件与防护传感器关联后，水流触发防护传感器时会关闭触摸元件，为其提供保护。

查看使用触摸元件防水功能的示例代码，请前往 ESP-IDF 示例的 :example:`peripherals/touch_sensor/touch_element/touch_element_waterproof` 目录。

配置防水功能的代码如下所示：

.. code-block:: c

    void app_main()
    {
        ...

        touch_xxxx_install();                 //初始化实例（按键、滑条等）
        touch_xxxx_create(&element_handle);  //创建新的触摸元件

        ...

        touch_element_waterproof_install();              //初始化触摸元件防水功能
        touch_element_waterproof_add(element_handle);   //关联触摸元件与防护传感器

        ...
    }

从 Light/Deep-sleep 模式唤醒
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

仅触摸按键可配置为唤醒源。

使用触摸传感器，可以唤醒从 Light-sleep 或 Deep-sleep 模式中唤醒芯片。在 Light-sleep 模式下，任何已安装的触摸按键都可以唤醒芯片。但在 Deep-sleep 模式下，只有睡眠按键可以唤醒芯片，触摸传感器还会立即进行校准。如果手指没有及时离开，可能导致校准参考值出错。尽管在手指离开后，校准参考值会自行恢复，不会影响驱动逻辑，但如果你不想在从 Deep-sleep 模式唤醒时看到错误的校准参考值，可以调用 :cpp:func:`touch_element_sleep_enable_wakeup_calibration`，禁用唤醒校准功能。

查看使用触摸元件唤醒芯片的示例代码，请前往 ESP-IDF 示例的 :example:`system/light_sleep` 目录。

.. code-block:: c

    void app_main()
    {
        ...
        touch_element_install();
        touch_button_install();                 //初始化触摸按键
        touch_button_create(&element_handle);  //创建新的触摸元件

        ...

        // ESP_ERROR_CHECK(touch_element_enable_light_sleep(&sleep_config));
        ESP_ERROR_CHECK(touch_element_enable_deep_sleep(button_handle[0], &sleep_config));
        // ESP_ERROR_CHECK(touch_element_sleep_enable_wakeup_calibration(button_handle[0], false)); //（可选）禁用唤醒校准，防止基准值更新为错误值

        touch_element_start();

        ...
    }

应用示例
-------------------

查看使用触摸元件库的示例代码，请前往 ESP-IDF 示例的 :example:`peripherals/touch_sensor/touch_element` 目录。


API 参考 - 触摸元件核心
----------------------------------

.. include-build-file:: inc/touch_element.inc


API 参考 - 触摸按键
----------------------------------

.. include-build-file:: inc/touch_button.inc


API 参考 - 触摸滑条
----------------------------------

.. include-build-file:: inc/touch_slider.inc


API 参考 - 触摸矩阵
----------------------------------

.. include-build-file:: inc/touch_matrix.inc
