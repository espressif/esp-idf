中断分配
========

:link_to_translation:`en:[English]`

概述
----

.. only:: esp32 or esp32s3

  {IDF_TARGET_NAME} 有两个核，每个核有 32 个中断。每个中断都有一个确定的优先级别，大多数中断（但不是全部）都连接到中断矩阵。

.. only:: esp32s2

  {IDF_TARGET_NAME} 有一个核，32 个中断。每个中断都有一个确定的优先级别，大多数中断（但不是全部）都连接到中断矩阵。

.. only:: esp32c2 or esp32c3

  {IDF_TARGET_NAME} 有一个核，31 个中断。每个中断的优先级别都可独立地通过编程设置。

.. only:: esp32c6 or esp32h2

  {IDF_TARGET_NAME} 有一个核，28 个外部异步中断。每个中断的优先级别都可独立地通过编程设置。此外，还有 4 个核心本地中断源 (CLINT)。详细信息请参见 **{IDF_TARGET_NAME} 技术参考手册** [`PDF <{IDF_TARGET_TRM_CN_URL}#riscvcpu>`__]。

.. only:: esp32p4

  {IDF_TARGET_NAME} 有两个核，每个核有 32 个外部异步中断。每个中断的优先级别都可独立地通过编程设置。此外，每个核还有 3 个核心本地中断源 (CLINT)。详细信息请参见 **{IDF_TARGET_NAME} 技术参考手册** [`PDF <{IDF_TARGET_TRM_CN_URL}#riscvcpu>`__]。

由于中断源数量多于中断，有时多个驱动程序可以共用一个中断。:cpp:func:`esp_intr_alloc` 抽象隐藏了这些实现细节。

驱动程序可以通过调用 :cpp:func:`esp_intr_alloc`，或 :cpp:func:`esp_intr_alloc_intrstatus` 为某个外设分配中断。通过向此函数传递 flag，可以指定中断类型、优先级和触发方式。然后，中断分配代码会找到适用的中断，使用中断矩阵将其连接到外设，并为其安装给定的中断处理程序和 ISR。

中断分配器提供两种不同的中断类型：共享中断和非共享中断，这两种中断需要不同处理方式。非共享中断在每次调用 :cpp:func:`esp_intr_alloc` 时，都会分配一个单独的中断，该中断仅用于与其相连的外设，只调用一个 ISR。共享中断则可以由多个外设触发，当其中一个外设发出中断信号时，会调用多个 ISR。因此，针对共享中断的 ISR 应检查对应外设的中断状态，以确定是否需要采取任何操作。

非共享中断可由电平或边缘触发。共享中断只能由电平触发，因为使用边缘触发可能会错过中断。

要解释为什么共享中断只能由电平触发，以外设 A 和外设 B 共用一个边缘触发中断为例进行说明：当外设 B 触发中断时，会将其中断信号设置为高电平，产生一个从低到高的边缘，进而锁存 CPU 中断位，并触发 ISR。接着，ISR 开始执行，检查到此时外设 A 没有触发中断，于是继续处理外设 B 的中断信号，最后将外设 B 的中断状态清除。最后，CPU 会在 ISR 返回之前清除中断位锁存器。因此在整个中断处理过程中，如果外设 A 触发了中断，该中断会因 CPU 清除中断位锁存器而丢失。


.. only:: esp32 or esp32s3

    多核问题
    --------

    可以生成中断的外设包括以下两种类型：

      - 外部外设，属于 {IDF_TARGET_NAME}，但不属于 Xtensa 核。大多数 {IDF_TARGET_NAME} 外设都属于此类型。
      - 内部外设，是 Xtensa CPU 的一部分。

    这两种外设的中断处理略有不同。

    内部外设中断
    ^^^^^^^^^^^^^^^^^^^^

    每个 Xtensa CPU 核都有六个内部外设：

      - 三个定时器比较器
      - 一个性能监视器
      - 两个软件中断

    内部中断源在 ``esp_intr_alloc.h`` 中定义为 ``ETS_INTERNAL_*_INTR_SOURCE``。

    这些外设只能通过关联的内核进行配置。在生成中断时，它们生成的中断被硬连线到关联的内核上，例如，一个内核的内部定时器比较器不能生成另一个内核上的中断。因此，这些中断源只能通过在特定内核上运行任务来进行管理。内部中断源仍然可用 :cpp:func:`esp_intr_alloc` 正常分配，但不能共用，而且始终具有固定的中断级别（即与外设关联的硬件级别）。

    外部外设中断
    ^^^^^^^^^^^^^^^^^^^^

    其余中断源来自外部外设。

.. only:: esp32p4

    多核问题
    --------

    每个 {IDF_TARGET_NAME} 内核都同时提供内部中断和外部中断，内部中断由内核自身触发，外部中断由外设触发。但 ESP-IDF 仅使用 {IDF_TARGET_NAME} 上的外部中断。大多数 {IDF_TARGET_NAME} 中断源都属于外部中断。

    每个内核的各个外部中断槽都与中断矩阵相连。通过中断矩阵可将任何外部中断源连接到任何中断槽，也可将多个外部中断源映射到同一个中断槽。外部中断源在 ``soc/interrupts.h`` 中定义为 ``ETS_*_INTR_SOURCE``。

.. only:: SOC_HP_CPU_HAS_MULTIPLE_CORES

    - 外部中断会始终被分配到执行该分配的内核上。
    - 释放外部中断必须在分配该中断的内核上进行。
    - 允许从另一个内核禁用和启用外部中断。
    - 多个外部中断源可以通过向 :cpp:func:`esp_intr_alloc` 发送  ``ESP_INTR_FLAG_SHARED`` flag 来共享一个中断槽。

    须注意从未关联到内核的任务中调用 :cpp:func:`esp_intr_alloc` 的情况。在任务切换期间，这些任务可能在不同内核之间进行迁移，因此无法确定中断分配到了哪个 CPU，给释放中断句柄造成困难，也可能引起调试问题。建议使用特定 CoreID 参数的 :cpp:func:`xTaskCreatePinnedToCore` 来创建中断分配任务，这对于内部中断源而言是必要的。


IRAM-safe 中断处理程序
----------------------

``ESP_INTR_FLAG_IRAM`` flag 注册的中断处理程序始终在 IRAM（并从 DRAM 读取其所有数据）中运行，因此在擦除和写入 flash 时无需禁用。

这对于需要保证最小执行延迟的中断来说非常有用，因为 flash 写入和擦除操作可能很慢（擦除可能需要数十毫秒或数百毫秒才能完成）。

如果中断被频繁调用，可以将中断处理程序保留在 IRAM 中，避免 flash cache 丢失。

有关更多详细信息，请参阅 :ref:`SPI flash API 相关文档 <iram-safe-interrupt-handlers>`。

.. _intr-alloc-shared-interrupts:

多个处理程序共用一个中断源
--------------------------

如果用 ``ESP_INTR_FLAG_SHARED`` flag 分配所有处理程序，可能将多个处理程序分配给同一个源。这些程序会被分配给与源关联的中断，并在源可用时按顺序调用。处理程序可以单独禁用和释放。如果启用了一个或多个处理程序，则会将源关联到中断（启用），否则会取消关联。禁用的处理程序永远不会被调用，但是只要启用了源的任何一个处理程序，这个源仍然能被触发。

关联到非共享中断的源不支持此功能。

.. only:: not SOC_CPU_HAS_FLEXIBLE_INTC

    默认情况下，指定 ``ESP_INTR_FLAG_SHARED`` flag 时，中断分配器仅分配优先级为 1 的中断。可以使用 ``ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_LOWMED`` 允许分配优先级为 2 和 3 的共享中断。

尽管支持此功能，使用时也必须 **非常小心**。通常存在两种办法可以阻止中断触发： **禁用源** 或 **屏蔽外设中断状态**。ESP-IDF 仅处理源本身的启用和禁用，中断源的状态位和屏蔽位须由用户操作。

**状态位须在负责该位的处理程序禁用前屏蔽，也可以在另一个启用的中断中屏蔽和处理该状态位。**

.. note::

    如果不屏蔽状态位而让其处于未处理状态，同时禁用这些状态位的处理程序，就会导致无限次触发中断，引起系统崩溃。


排除中断分配故障
------------------

CPU 中断在大多数 Espressif SoC 上都是有限的资源。因此，一个运行的程序有可能耗尽 CPU 中断，例如初始化多个外设驱动程序的情况。这通常导致驱动程序的初始化函数返回 ``ESP_ERR_NOT_FOUND`` 错误。

这种情况下，可使用 :cpp:func:`esp_intr_dump` 函数打印中断列表及其状态。此函数输出通常如下：

.. code-block::

    CPU 0 interrupt status:
    Int  Level  Type   Status
    0     1    Level  Reserved
    1     1    Level  Reserved
    2     1    Level  Used: RTC_CORE
    3     1    Level  Used: TG0_LACT_LEVEL
    ...

输出列含义如下：

.. list::

    - ``Int``：CPU 中断输入编号。通常不直接在软件中使用，仅作为参考。
    :not SOC_CPU_HAS_FLEXIBLE_INTC: - ``Level``：CPU 中断的优先级（1-7）。此优先级固定在硬件上，无法更改。
    :SOC_CPU_HAS_FLEXIBLE_INTC: - ``Level``：已分配中断的优先级级别。空闲的中断具有标记 ``*``。
    :not SOC_CPU_HAS_FLEXIBLE_INTC: - ``Type``：CPU 中断的中断类型（电平或边缘中断）。此类型在硬件上固定，无法更改。
    :SOC_CPU_HAS_FLEXIBLE_INTC: - ``Type``：已分配中断的类型（电平或边缘中断）。空闲的中断具有标记 ``*``。
    - ``Status``：中断的可能状态：
        - ``Reserved``：中断在硬件层面保留，或由 ESP-IDF 的某些部分保留。不能使用 :cpp:func:`esp_intr_alloc` 分配。
        - ``Used: <source>``：中断已分配并连接到单个外设。
        - ``Shared: <source1> <source2> ...``：中断已分配并连接到多个外设。参见本文档 :ref:`intr-alloc-shared-interrupts` 章节。
        - ``Free``：中断未分配，可以由 :cpp:func:`esp_intr_alloc` 使用。
        :not SOC_CPU_HAS_FLEXIBLE_INTC: - ``Free (not general-use)``：中断未分配，但它是高优先级中断（级别 4-7）或边缘触发中断。高优先级中断可以使用 :cpp:func:`esp_intr_alloc` 分配，但要求处理程序必须用汇编语言编写，参见 :doc:`../../api-guides/hlinterrupts`。低优先级和中优先级的边缘触发中断也可以用 :cpp:func:`esp_intr_alloc` 分配，但很少使用，因为大多数外设中断是电平触发的。

如果已确认应用程序的确用完了中断，可组合采用下列建议解决问题：

.. list::

    :SOC_HP_CPU_HAS_MULTIPLE_CORES: - 在多核目标上，尝试通过固定在第二个核的任务来初始化某些外设驱动程序。中断通常分配在运行外设驱动程序初始化函数的同一个内核上，因此，通过在第二个内核上运行初始化函数，就可以使用更多的中断输入。
    - 找到可接受更高延迟的中断，并用 ``ESP_INTR_FLAG_SHARED`` flag （或与 ``ESP_INTR_FLAG_LOWMED`` 进行 OR 运算）分配这些中断。对两个或更多外设使用此 flag 能让它们使用单个中断输入，从而为其他外设节约中断输入。参见 :ref:`intr-alloc-shared-interrupts`。
    :not SOC_CPU_HAS_FLEXIBLE_INTC: - 一些外设驱动程序可能默认使用 ``ESP_INTR_FLAG_LEVEL1`` flag 来分配中断，因此默认情况下不会使用优先级为 2 或 3 的中断。如果 :cpp:func:`esp_intr_dump` 显示某些优先级为 2 或 3 的中断可用，尝试在初始化驱动程序时将中断分配 flag 改为 ``ESP_INTR_FLAG_LEVEL2`` 或 ``ESP_INTR_FLAG_LEVEL3``。
    - 检查是否有些外设驱动程序不需要一直启用，并按需将其初始化或取消初始化。这样可以减少同时分配的中断数量。


API 参考
--------

.. include-build-file:: inc/esp_intr_types.inc
.. include-build-file:: inc/esp_intr_alloc.inc
