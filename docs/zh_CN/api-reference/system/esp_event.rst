事件循环库
==================

:link_to_translation:`en:[English]`

概述
--------

事件循环库使组件能够声明事件，允许其他组件注册处理程序（即在事件发生时执行的代码片段）。此时，无需直接涉及应用程序，松散耦合组件也能够在其他组件状态变化时附加所需的行为。此外，通过将代码执行序列化，在指定的任务中运行事件循环库，可以简化事件处理程序，实现更高效的事件处理。

.. only:: SOC_WIFI_SUPPORTED

    例如，当某个高级库要使用 Wi-Fi 库时，它可以直接订阅 :ref:`ESP32 Wi-Fi 编程模型 <wifi-programming-model>`，对有关事件做出相应。

.. only:: SOC_BT_SUPPORTED

    .. note::

        蓝牙栈各模块通过调用专用回调函数向应用程序传递事件，而非通过事件循环库传递。

调用 ``esp_event`` API
-----------------------------

使用事件循环库时应注意区分“事件”与“事件循环”。

事件表示重要的发生事件，如 Wi-Fi 成功连接到接入点。引用事件时应使用由两部分组成的标识符，详情请参阅 :ref:`事件定义与事件声明 <esp-event-declaring-defining-events>`。事件循环是连接事件和事件处理程序之间的桥梁，事件源通过使用事件循环库提供的 API 将事件发布到事件循环中，注册到事件循环中的事件处理程序会响应特定类型的事件。

以下为事件循环库的使用流程：

1. 定义一个函数，并在事件发布到事件循环中时运行该函数。此函数被称为事件处理程序，应具有与 :cpp:type:`esp_event_handler_t` 同类型的签名。
2. 调用 :cpp:func:`esp_event_loop_create` 创建事件循环，该函数输出类型为 :cpp:type:`esp_event_loop_handle_t` 的循环句柄，使用此 API 创建的事件循环称为用户事件循环。另有一种特殊事件循环，请参阅 :ref:`默认事件循环 <esp-event-default-loops>`。
3. 调用 :cpp:func:`esp_event_handler_register_with` 将事件处理程序注册到循环中。处理程序可以注册到多个循环中，请参阅 :ref:`注册处理程序注意事项 <esp-event-handler-registration>`。
4. 事件源调用 :cpp:func:`esp_event_post_to` 将事件发布到事件循环中。
5. 调用 :cpp:func:`esp_event_handler_unregister_with` ，组件可以在事件循环中取消注册事件处理程序。
6. 调用 :cpp:func:`esp_event_loop_delete` 删除不再需要的事件循环。

上述流程代码如下：

.. code-block:: c

    // 1. 定义事件处理程序
    void run_on_event(void* handler_arg, esp_event_base_t base, int32_t id, void* event_data)
    {
        // 事件处理程序逻辑
    }

    void app_main()
    {
        // 2. 用一个类型为 esp_event_loop_args_t 的配置结构体，指定所创建循环的属性。获取一个类型为 esp_event_loop_handle_t 的句柄，用于其他 API 引用循环、执行操作。
        esp_event_loop_args_t loop_args = {
            .queue_size = ...,
            .task_name = ...
            .task_priority = ...,
            .task_stack_size = ...,
            .task_core_id = ...
        };

        esp_event_loop_handle_t loop_handle;

        esp_event_loop_create(&loop_args, &loop_handle);

        // 3. 注册在 (1) 中定义的事件处理程序。MY_EVENT_BASE 和 MY_EVENT_ID 指定了一个假设事件：将处理程序 run_on_event 发布到循环中时，执行该处理程序。
        esp_event_handler_register_with(loop_handle, MY_EVENT_BASE, MY_EVENT_ID, run_on_event, ...);

        ...

        // 4. 将事件发布到循环中。此时，事件排入事件循环队列，在某个时刻，事件循环会执行已注册到发布事件的事件处理程序，例如此处的 run_on_event。为简化过程，此示例从 app_main 调用 esp_event_post_to，实际应用中可从任何其他任务中发布事件。
        esp_event_post_to(loop_handle, MY_EVENT_BASE, MY_EVENT_ID, ...);

        ...

        // 5. 注销无用的处理程序。
        esp_event_handler_unregister_with(loop_handle, MY_EVENT_BASE, MY_EVENT_ID, run_on_event);

        ...

        // 6. 删除无用的事件循环。
        esp_event_loop_delete(loop_handle);
    }

.. _esp-event-declaring-defining-events:

事件定义与事件声明
-----------------------------

如前所述，事件标识符由两部分组成：事件根基和事件 ID。事件根基标识独立的事件组；事件 ID 标识组中的特定事件。可以将事件根基和事件 ID 类比为人的姓和名，姓表示一个家族，名表示家族中的某个人。

事件循环库提供了宏以便声明和定义事件根基。

声明事件根基：

.. code-block:: c

    ESP_EVENT_DECLARE_BASE(EVENT_BASE);

定义事件根基：

.. code-block:: c

    ESP_EVENT_DEFINE_BASE(EVENT_BASE);

.. note::

    在 ESP-IDF 中，系统事件的根基标识符为大写字母，并以 ``_EVENT`` 结尾。例如，Wi-Fi 事件的根基声明为 ``WIFI_EVENT``，以太网的事件根基声明为 ``ETHERNET_EVENT`` 等。这样一来，事件根基与常量类似（尽管根据宏 ``ESP_EVENT_DECLARE_BASE`` 和 ``ESP_EVENT_DEFINE_BASE`` 的定义，它们属于全局变量）。

建议以枚举类型声明事件 ID，它们通常放在公共头文件中。

事件 ID：

.. code-block:: c

    enum {
        EVENT_ID_1,
        EVENT_ID_2,
        EVENT_ID_3,
        ...
    }

.. _esp-event-default-loops:

默认事件循环
------------------

默认事件循环是一种特殊循环，用于处理系统事件（如 Wi-Fi 事件）。用户无法使用该循环的句柄，创建、删除、注册/注销处理程序以及事件发布均通过用户事件循环 API 的变体完成，下表列出了这些变体及其对应用户事件循环。

.. list-table::
    :header-rows: 1
    :widths: 60 60
    :align: center

    * - 用户事件循环
      - 默认事件循环
    * - :cpp:func:`esp_event_loop_create`
      - :cpp:func:`esp_event_loop_create_default`
    * - :cpp:func:`esp_event_loop_delete`
      - :cpp:func:`esp_event_loop_delete_default`
    * - :cpp:func:`esp_event_handler_register_with`
      - :cpp:func:`esp_event_handler_register`
    * - :cpp:func:`esp_event_handler_unregister_with`
      - :cpp:func:`esp_event_handler_unregister`
    * - :cpp:func:`esp_event_post_to`
      - :cpp:func:`esp_event_post`

比较二者签名可知，它们大部分是相似的，唯一区别在于默认事件循环的 API 不需要指定循环句柄。

除了 API 的差异和用于系统事件的特殊分类外，默认事件循环和用户事件循环的行为并无差异。实际上，用户甚至可以将自己的事件发布到默认事件循环中，以节省内存而无需创建自己的循环。

.. _esp-event-handler-registration:

注册处理程序注意事项
----------------------------------

通过重复调用 :cpp:func:`esp_event_handler_register_with`，可以将单个处理程序独立注册到多个事件中，且每次调用均可指定处理程序应执行的具体事件根基和事件 ID。

然而，在某些情况下，你可能希望处理程序在以下情况时执行：

(1) 所有发布到循环的事件
(2) 特定基本标识符的所有事件

为此，可调用特殊的事件根基标识符 ``ESP_EVENT_ANY_BASE`` 和特殊的事件 ``ESP_EVENT_ANY_ID`` 实现，这些特殊标识符可以作为 :cpp:func:`esp_event_handler_register_with` 的事件根基和事件 ID 参数传递。

因此 :cpp:func:`esp_event_handler_register_with` 的有效参数为：

1. <event base>, <event ID> - 根基为 <event base> 且 ID 为 <event ID> 的事件发布到循环中时，执行处理程序。
2. <event base>, ESP_EVENT_ANY_ID - 任何根基为 <event base> 的事件发布到循环中时，执行处理程序。
3. ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID - 任何事件发布到循环中时，执行处理程序。

例如，如果注册了以下处理程序：

.. code-block:: c

    esp_event_handler_register_with(loop_handle, MY_EVENT_BASE, MY_EVENT_ID, run_on_event_1, ...);
    esp_event_handler_register_with(loop_handle, MY_EVENT_BASE, ESP_EVENT_ANY_ID, run_on_event_2, ...);
    esp_event_handler_register_with(loop_handle, ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID, run_on_event_3, ...);

如果假设事件由 ``MY_EVENT_BASE`` 和 ``MY_EVENT_ID`` 组成，则三个处理程序 ``run_on_event_1``、 ``run_on_event_2`` 和 ``run_on_event_3`` 都会执行。

如果假设事件由 ``MY_EVENT_BASE`` 和 ``MY_OTHER_EVENT_ID`` 组成，则仅执行处理程序 ``run_on_event_2`` 和 ``run_on_event_3``。

如果假设事件由 ``MY_OTHER_EVENT_BASE`` 和 ``MY_OTHER_EVENT_ID`` 组成，则仅执行处理程序 ``run_on_event_3``。

处理程序自行注销
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

通常情况下，由事件循环运行的事件处理程序 **不允许在该事件循环上执行任何注册/注销活动**，但允许处理程序自行注销。例如，可以执行以下操作：

.. code-block:: c

    void run_on_event(void* handler_arg, esp_event_base_t base, int32_t id, void* event_data)
    {
        esp_event_loop_handle_t *loop_handle = (esp_event_loop_handle_t*) handler_arg;
        esp_event_handler_unregister_with(*loop_handle, MY_EVENT_BASE, MY_EVENT_ID, run_on_event);
    }

    void app_main(void)
    {
        esp_event_loop_handle_t loop_handle;
        esp_event_loop_create(&loop_args, &loop_handle);
        esp_event_handler_register_with(loop_handle, MY_EVENT_BASE, MY_EVENT_ID, run_on_event, &loop_handle);
        // ... 发布事件 MY_EVENT_BASE 和 MY_EVENT_ID，并在某些时候运行循环
    }


注册处理程序及处理程序调度顺序
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

一般而言，对于在调度期间与某个已发布事件匹配的处理程序，先注册的也会先执行。在所有注册均使用单个任务执行的情况下，可以通过在其他处理程序注册前注册目标处理程序，控制处理程序的执行顺序。如果计划利用这一规则，在有多个任务注册处理程序的情况下要多加小心。此时，虽然“先注册，先执行”的规则仍然成立，但率先执行的任务也会率先注册其处理程序，而由单个任务连续注册的处理函数仍然按相对顺序调度。但如果该任务在注册期间被另一个任务抢占，而该任务还注册了处理程序，则在调度期间，那些处理程序也将在处理其他任务时执行。


事件循环性能分析
--------------------

要启动数据收集，统计所有已创建事件循环的数据，请激活配置选项 :ref:`CONFIG_ESP_EVENT_LOOP_PROFILING`，函数 :cpp:func:`esp_event_dump` 可将收集的统计数据输出到文件流中。有关转储信息的更多详情，请参阅 :cpp:func:`esp_event_dump` API 参考。

应用示例
-------------------

使用 ``esp_event`` 库的示例存放在 :example:`system/esp_event` 中，涵盖事件声明、循环创建、处理程序注册和注销以及事件发布。

其他使用 ``esp_event`` 库的示例:

    * :example:`NMEA Parser <peripherals/uart/nmea0183_parser>`，该示例将解码从 GPS 接收到的语句。

API 参考
-------------

.. include-build-file:: inc/esp_event.inc
.. include-build-file:: inc/esp_event_base.inc

相关文档
-----------------

.. toctree::
    :maxdepth: 1
