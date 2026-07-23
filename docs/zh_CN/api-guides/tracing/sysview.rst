.. _app_trace-system-behaviour-analysis-with-segger-systemview:

基于 SEGGER SystemView 的系统行为分析
=====================================

:link_to_translation:`en:[English]`

SEGGER SystemView 是一款实时记录和可视化工具，用于分析应用程序运行时的行为（任务调度、中断、系统事件）。在 :doc:`esp_trace <index>` 模型中，SystemView 以 **编码器** 的形式提供：它将 FreeRTOS 和应用程序事件格式化为 SystemView 协议，数据则由 :doc:`传输 <transports>` 层（通常为 JTAG 上的 apptrace，或用于实时查看的 UART）传送到主机。

工具详情请参阅 `SystemView <https://www.segger.com/products/development-tools/systemview/>`_。

启用 SystemView
---------------

SystemView 功能由托管组件 ``espressif/esp_sysview`` 提供。完成以下步骤后才会显示 SystemView 配置菜单：

1. 在 ``idf_component.yml`` 中添加组件依赖：

   .. code-block:: yaml

       dependencies:
         espressif/esp_sysview: ^1

2. 在 menuconfig 中选择外部库：``Component config`` > ``ESP Trace Configuration`` > ``Trace library`` > ``External library from component registry``。

之后，可通过 ``Component config`` > ``SEGGER SystemView Configuration`` 配置 SystemView。该菜单可用于选择时间戳源 (:ref:`CONFIG_ESP_TRACE_TIMESTAMP_SOURCE`)、单独启用或禁用 SystemView 事件收集 (``CONFIG_SEGGER_SYSVIEW_EVT_XXX``)，以及在使用 UART 目标时选择要跟踪的 CPU。

.. note::

    完整的最新配置选项和主机端设置，请参阅组件 README：`esp_sysview <https://components.espressif.com/components/espressif/esp_sysview>`_。

想要通过 UART 接口进行实时跟踪，请首先在 ``Component config`` > ``ESP Trace Configuration`` > ``Application Level Tracing`` 中选择 UART 作为目标传输方式。然后在 ``Component config`` > ``ESP Trace Configuration`` > ``SEGGER SystemView`` 中选择 Pro 或 App CPU。

OpenOCD SystemView 跟踪命令选项
-------------------------------

通过 JTAG 跟踪时，使用专用的 OpenOCD 命令收集数据。OpenOCD/JTAG 设置请参阅 :doc:`JTAG 调试 </api-guides/jtag-debugging/index>`。

命令用法：

``esp sysview [start <options>] | [stop] | [status]``

子命令：

``start``
    开启跟踪（连续流模式）。
``stop``
    停止跟踪。
``status``
    获取跟踪状态。

Start 子命令语法：

  ``start <outfile1> [outfile2] [poll_period [trace_size [stop_tmo]]]``

``outfile1``
    保存 PRO CPU 数据的文件路径。此参数需要具有如下格式：``file://path/to/file``。
``outfile2``
    保存 APP CPU 数据的文件路径。此参数需要具有如下格式：``file://path/to/file``。
``poll_period``
    跟踪数据的轮询周期（单位：毫秒）。如果该值大于 0，则命令以非阻塞的模式运行。默认为 1 毫秒。
``trace_size``
    最多要收集的数据量（单位：字节）。当收到指定数量的数据后，将停止跟踪。默认值是 -1（禁用跟踪大小停止触发器）。
``stop_tmo``
    空闲超时（单位：秒）。如果指定的时间内没有数据，将停止跟踪。默认值是 -1（禁用跟踪超时停止触发器）。

.. note::

    如果 ``poll_period`` 为 0，则在跟踪停止之前，OpenOCD 的 telnet 命令行将不可用。你需要复位板卡，或者在 OpenOCD 的窗口（非 telnet 会话窗口）输入 Ctrl+C 命令，手动停止跟踪。另一个办法是设置 ``trace_size``，等到收集满指定数量的数据后自动停止跟踪。

命令使用示例：

.. highlight:: none

::

    esp sysview start file://pro-cpu.SVDat file://app-cpu.SVDat

跟踪数据被检索并以非阻塞的方式保存。要停止此过程，需要在 OpenOCD 的 telnet 会话窗口输入 ``esp sysview stop`` 命令，也可以在 OpenOCD 窗口中按下快捷键 Ctrl+C。

多核 SystemView 跟踪命令
""""""""""""""""""""""""""

对于支持多核跟踪的 SystemView 3.60 及更高版本，请使用 ``esp sysview_mcore`` 命令。此命令与 ``esp sysview`` 相同，但使用官方 SEGGER SystemView 多核格式。所有核心的跟踪数据都保存在同一文件中，可在 SEGGER SystemView v3.60 或更高版本中打开。

命令使用示例：

::

    esp sysview_mcore start file://heap_log_mcore.SVDat

有关详细的命令语法和选项，请参考前文所述的 ``esp sysview`` 命令，因为 ``esp sysview_mcore`` 支持相同的参数。

数据可视化
----------

收集到跟踪数据后，可以使用特殊的工具对结果进行可视化并分析程序行为。

.. only:: SOC_HP_CPU_HAS_MULTIPLE_CORES

    **多核跟踪**

    SystemView 3.60 及更高版本支持多核心进行跟踪。对于多核跟踪，使用 ``esp sysview_mcore`` 命令可以生成与 SystemView 多核格式兼容的单个文件。此命令将创建一个单独的跟踪文件，可以直接加载到 SystemView 3.60+ 中进行多核可视化。

    **注意：** SystemView 3.60 之前的版本不支持多核跟踪。对于旧版本，当使用 JTAG 接口跟踪双核模式下的 {IDF_TARGET_NAME} 时会生成两个文件：一个用于 PRO CPU，另一个用于 APP CPU。用户可将每个文件载入不同的工具实例。使用 UART 进行跟踪时，在 menuconfig 中选择外部库后，用户可以选择 ``Component config`` > ``SEGGER SystemView Configuration`` 来指定需要跟踪的 CPU（Pro 或 App）。

    对于旧版本的 SystemView，在不同的实例中分别分析每个核的数据可能较为不便。另一个选择是使用名为 *Impulse* 的 Eclipse 插件，该插件可同时加载多个跟踪文件，实现在同一视图中检查来自两个核心的事件。与 SystemView 免费版相比，此插件还不受 100 万事件数量的限制。

关于如何安装、配置 Impulse 并使用它来可视化来自单个核心的跟踪数据，请参阅 `官方教程 <https://mcuoneclipse.com/2016/07/31/impulse-segger-systemview-in-eclipse/>`_。

.. note::

    ESP-IDF 使用自己的 SystemView FreeRTOS 事件 ID 映射，因此用户需要将 ``$SYSVIEW_INSTALL_DIR/Description/SYSVIEW_FreeRTOS.txt`` 替换成 ``$IDF_PATH/tools/esp_app_trace/SYSVIEW_FreeRTOS.txt``。在使用上述链接配置 SystemView 序列化程序时，也应该使用该特定文件的内容。

.. only:: SOC_HP_CPU_HAS_MULTIPLE_CORES

    配置 Impulse 实现双核跟踪
    """""""""""""""""""""""""

    在安装好 Impulse 插件并确保 Impulse 能够在单独的选项卡中成功加载每个核心的跟踪文件后，用户可以添加特殊的 Multi Adapter 端口并将这两个文件加载到一个视图中。为此，用户需要在 Eclipse 中执行以下操作：

    1. 打开 ``Signal Ports`` 视图，前往 ``Windows`` > ``Show View`` > ``Other`` 菜单，在 Impulse 文件夹中找到 ``Signal Ports`` 视图并双击。
    2. 在 ``Signal Ports`` 视图中，右键 ``Ports`` 并选择 ``Add``，然后选择 ``New Multi Adapter Port``。
    3. 在打开的对话框中按下 ``Add`` 按钮，选择 ``New Pipe/File``。
    4. 在打开的对话框中选择 ``SystemView Serializer`` 并设置 PRO CPU 跟踪文件的路径，按下 ``OK`` 保存设置。
    5. 对 APP CPU 的跟踪文件重复步骤 3 和 4。
    6. 双击创建的端口，会打开此端口的视图。
    7. 单击 ``Start/Stop Streaming`` 按钮，数据将会被加载。
    8. 使用 ``Zoom Out``，``Zoom In`` 和 ``Zoom Fit`` 按钮来查看数据。
    9. 有关设置测量光标和其他的功能，请参阅 `Impulse 官方文档 <https://toem.de/index.php/products/impulse>`_。

    .. note::

        如果你在可视化方面遇到了问题（未显示数据或者缩放操作异常），可以尝试删除当前的信号层次结构，再双击必要的文件或端口。Eclipse 会请求创建新的信号层次结构。

应用示例
--------

- :example:`system/sysview_tracing` 演示如何使用 SEGGER SystemView 记录 FreeRTOS 任务与系统事件。
- :example:`system/sysview_tracing_heap_log` 演示如何在记录 SystemView 事件的同时，对堆内存分配进行跟踪。
