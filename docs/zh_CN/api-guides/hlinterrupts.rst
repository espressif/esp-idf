高优先级中断处理程序
=====================

:link_to_translation:`en:[English]`

.. toctree::
   :maxdepth: 1

Xtensa 架构支持 32 个中断处理程序，这些中断分为从 1 到 7 的 7 个优先级，其中优先级 7 是非可屏蔽中断 (NMI)。此外，该架构也支持处理其他异常情况。在 {IDF_TARGET_NAME} 上，:doc:`../api-reference/system/intr_alloc` 可以通过中断复用器，将大多数中断源路由到上述中断上。通常中断由 C 语言编写，但 ESP-IDF 支持使用汇编语言编写高优先级中断，从而尽可能消除中断延迟。

中断处理程序优先级
--------------------

.. only:: esp32

  .. list-table::
      :header-rows: 1
      :widths: 20 30 50
      :align: center

      * - 优先级
        - 符号
        - 备注
      * - 1
        - N/A
        - 异常和低优先级中断，由 ESP-IDF 处理。
      * - 2-3
        - N/A
        - 中等优先级中断，由 ESP-IDF 处理。
      * - 4
        - xt_highint4
        - 高优先级中断，可供自由使用。 [1]_
      * - 5
        - xt_highint5
        - 通常由 ESP-IDF 的调试逻辑使用。 [1]_
      * - NMI
        - xt_nmi
        - 非可屏蔽中断，可供自由使用。
      * - dbg
        - xt_debugexception
        - 调试异常情况，例如在执行 BREAK 指令时调用。 [2]_

  .. [1] 在 :ref:`CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL` 中配置 ESP-IDF 的调试逻辑，可以使其在 ``xt_highint4`` 或 ``xt_highint5`` 上运行。启用 :ref:`CONFIG_BTDM_CTRL_HLI`，可以将蓝牙中断配置为优先级 4。如果启用了 :ref:`CONFIG_BTDM_CTRL_HLI`，则 ESP-IDF 的调试逻辑必须运行在优先级 5 的中断上。
  .. [2] 如果启用了 :ref:`CONFIG_BTDM_CTRL_HLI`，则可以用 ``xt_debugexception`` 修复 ESP32 ECO3 中的 `活锁问题 <https://www.espressif.com/sites/default/files/documentation/eco_and_workarounds_for_bugs_in_esp32_en.pdf>`_。

.. only:: not esp32

  .. list-table::
      :header-rows: 1
      :widths: 20 30 50
      :align: center

      * - 优先级
        - 符号
        - 备注
      * - 1
        - N/A
        - 异常和低优先级中断，由 ESP-IDF 处理。
      * - 2-3
        - N/A
        - 中等优先级中断，由 ESP-IDF 处理。
      * - 4
        - xt_highint4
        - 通常由 ESP-IDF 的调试逻辑使用。
      * - 5
        - xt_highint5
        - 高优先级中断，可供自由使用。
      * - NMI
        - xt_nmi
        - 非可屏蔽中断，可供自由使用。
      * - dbg
        - xt_debugexception
        - 调试异常情况，例如在执行 BREAK 指令时调用。

要使用这些符号，需要创建一个后缀为 ``.S`` 的汇编文件，并定义命名的符号，如下所示：

.. code-block:: none

        .section .iram1,"ax"
        .global     xt_highint5
        .type       xt_highint5,@function
        .align      4
    xt_highint5:
        ... your code here
        rsr     a0, EXCSAVE_5
        rfi     5

实际应用示例请参阅 :component_file:`esp_system/port/soc/{IDF_TARGET_PATH_NAME}/highint_hdl.S`，该示例使用了紧急处理程序。


注意事项
--------

- 请勿从高级中断中调用 C 代码，因为这些中断在临界区域运行，从高级中断调用 C 代码可能会导致目标系统崩溃。注意，尽管紧急处理程序会调用常见的 C 代码，但由于该处理程序不会返回，即在紧急处理程序之后，应用程序不会继续运行，因此中断 C 代码的执行流程不会造成问题。

.. only:: esp32

  由于存在额外保护措施，启用 :ref:`CONFIG_BTDM_CTRL_HLI` 时，可以放心从高级中断中调用 C 代码。

- 请确保所用汇编代码成功链接。由于可自由使用的符号声明为弱符号，链接器可能会丢弃包含此类符号的文件。如果用户文件中定义或使用的唯一符号是可自由使用的符号 ``xt_*``，则会发生上述情况。为了避免这种情况，应在包含 ``xt_*`` 符号的汇编文件中定义另一个符号，例如：

.. code-block:: none

  .global ld_include_my_isr_file
  ld_include_my_isr_file:

此处符号名称为 ``ld_include_my_isr_file``，但只要该符号未在项目的其他位置定义，也可使用任意名称。

随后，在组件的 ``CMakeLists.txt`` 文件中，将该名称作为未解析符号，添加到 ld 命令行参数中：

.. code-block:: none

  target_link_libraries(${COMPONENT_TARGET} "-u ld_include_my_isr_file")

这能够确保链接器始终包含定义 ``ld_include_my_isr_file`` 的文件，从而保持 ISR 与项目的链接。

- 使用 :cpp:func:`esp_intr_alloc` 和相关函数可以路由和处理高级中断，但传递给 :cpp:func:`esp_intr_alloc` 的处理程序和处理程序参数必须为 NULL。

- 中等优先级的中断理论上也可以通过上述方式处理，但 ESP-IDF 尚不支持此功能。

- 要检查 Xtensa 指令集架构 (ISA)，请参阅 `Xtensa ISA 摘要 <https://www.cadence.com/content/dam/cadence-www/global/en_US/documents/tools/ip/tensilica-ip/isa-summary.pdf>`_。
