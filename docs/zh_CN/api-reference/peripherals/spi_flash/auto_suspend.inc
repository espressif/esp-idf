.. _auto-suspend:

flash 自动暂停功能
--------------------------------------

.. important::

    1. 应使用支持暂停/恢复功能的 flash。

    2. 应使用支持自动暂停功能的 MSPI 硬件，即硬件支持自动发送暂停命令。

    如果在不支持自动暂停的 flash 上使用该功能，可能会造成严重的程序崩溃。因此，强烈建议提前阅读规格书，确保 flash 至少满足如下条件：

    1. 基于当前软件的实现限制，flash 状态寄存器中的 SUS 位应当位于 SR2 bit7 或 SR bit15。

    2. 基于当前软件的实现限制，flash 暂停命令应为 75H，恢复命令为 7AH。

    3. flash 成功暂停后，除已擦除的段或块外，flash 地址中的任何内容都支持读取。也可以在此种状态下立即下达恢复命令。

    4. flash 从暂停模式恢复后，支持立即下达另一个暂停命令。

启用 :ref:`CONFIG_SPI_FLASH_AUTO_SUSPEND` 后，缓存将保持启用状态，禁用该选项即可禁用缓存。SPI0 和 SPI1 之间的仲裁由硬件决定。当 SPI1 进行读取等耗时较短的操作时，CPU 和缓存将等待至 SPI1 操作完成。然而，在擦除、页面写入或状态寄存器写入等过程中，如 ``SE``、``PP`` 和 ``WRSR``，自动暂停功能将中断正在进行的 flash 操作，使 CPU 得以在有限的时间内读取缓存及 flash 中的数据。

基于此功能，部分的代码及变量现可存放在 flash/PSRAM 中，同时仍能保证在 flash 擦除期间的正常执行，减少了 IRAM/DRAM 的消耗。

需注意，使用此功能暂停/恢复 flash 时，会导致额外开销。在 flash 擦除期间，频繁触发中断将显著延长擦除时间。为避免这种情况，建议调整 FreeRTOS 任务优先级，仅将实时关键的任务排在擦除操作前，从而确保在合理时间内完成 flash 擦除操作。

代码可以分为以下三类：

1. 关键代码：存放在 IRAM/DRAM 中。这类代码通常有较高的性能要求，与 cache/flash/PSRAM 相关，或者被频繁调用。

2. 缓存访问的代码：存放在 flash/PSRAM 中。这类代码的性能要求较低，或者较少被调用。flash 擦除时将执行这类代码，导致一定的开销。

3. 低优先级代码：存放在 flash/PSRAM 中，且在 flash 擦除期间禁止运行。这类代码的任务优先级应低于擦除操作，避免影响 flash 擦除的速度。

关于 flash 自动暂停功能的用法和对应的响应时间延迟，请参考 :example:`system/flash_suspend` 示例。

.. note::

    由于 flash 自动暂停功能极度依赖时序，其多用作为一种优化方案，而非万能解法。例如 LCD 刷新、蓝牙、Wi-Fi 等场景对实时系统要求较高、或需要频繁触发中断，所以此方案并不适用。建议参照以下步骤，选择搭配 flash 自动暂停功能使用的 ISR 类型：

    .. wavedrom:: /../_static/diagrams/spi_flash/flash_auto_suspend_timing.json

    需注意图中的两个关键值：

    1. ISR 时间 (ISR time)：ISR 时间不能过长，需小于 ``IWDT`` 中设置的值。ISR 时间会显著增加擦除/写入操作的完成时间。

    2. ISR 间隔时间 (ISR interval)：由于不能频繁触发 ISR，需格外注意 **ISR 间隔时间减去 ISR 时间后的剩余时间** （图中 b 点至 c 点的距离）。在此期间，SPI1 会发送恢复命令重新启动操作，所需准备时间 ``tsus`` 的典型值约为 **40 us**。如果在 SPI1 完成恢复操作前接收到了新的暂停命令，可能导致 CPU 饥饿，触发 ``TWDT``。

    对于第 2 点中所提到的 ``tsus`` 时间可以通过翻阅 flash datasheets 查找，通常在 AC CHARACTERISTICS 章节中。用户需要保证从 datasheets 获得的 ``tsus`` 值不大于  :ref:`CONFIG_SPI_FLASH_SUSPEND_TSUS_VAL_US` 值。

    此外，flash 暂停可能延迟。CPU 和缓存通过 SPI0 频繁访问 flash，且 SPI1 频繁发送暂停命令时，会导致 MSPI 数据传输效率下降。可以通过在内部使用 **锁** 来避免此种情况。当 SPI1 发送暂停命令时，SPI0 将接管内存 SPI 总线并启用锁。SPI0 完成数据传输后，在锁延迟时间结束前，都将保有对内存 SPI 总线的控制权。在此锁延迟期间，如果接收到其他 SPI0 事务，则该 SPI0 事务将正常进行，并开启新一轮锁延迟周期。如无其他 SPI0 事务，则 SPI0 释放内存总线并启动 SPI0/1 仲裁。

suspend-resume 进阶用法
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

在默认配置下，flash 暂停由硬件自动发出，恢复也由硬件在合适的时机自动完成，并采用基于固定时间的延迟来等待暂停命令生效。这种实现已经可以满足绝大多数场景。然而在某些对实时性、性能或暂停粒度有更高要求的场景下，ESP-IDF 还提供了进阶配置项，用于精细化控制 suspend-resume 行为。

1. 通过 flash 寄存器自动判断暂停是否生效

通常情况下，硬件在发出暂停命令后，会按 ``tsus`` 设定的时间延迟一段时间，再开放内存总线给 SPI0/CPU 使用。该延迟必须按 datasheet 中的最坏情况进行设置，因此在大多数情况下都偏保守。

通过启用 :ref:`CONFIG_SPI_FLASH_AUTO_CHECK_SUSPEND_STATUS` 后，硬件将通过读取 flash 状态寄存器中的 ``WIP`` 位，来判断暂停命令是否真正生效，而不再依据 :ref:`CONFIG_SPI_FLASH_SUSPEND_TSUS_VAL_US` 给出的固定时间进行等待。由于实际的暂停建立时间通常远小于 datasheet 给出的最大值，所以这种方式可以显著降低暂停过程中的开销，提升整体性能。

.. important::

    使用前需确认所用 flash 在暂停后 ``WIP`` 位的行为符合预期。绝大多数支持 suspend/resume 的 flash 都满足该条件，但仍建议查阅相应 datasheet 进行确认。

2. 软件控制的 flash resume

通常情况下，硬件在 flash 被暂停后，会自动安排合适的时机发送 resume 命令，让 flash 继续完成原有的擦除/写入操作。这种实现对软件透明，但存在一个副作用：在高优先级任务/中断仍在运行时，硬件可能再次发起 suspend/resume 流程，从而打断这些任务，影响其执行的连续性与时序。

通过启用 :ref:`CONFIG_SPI_FLASH_SOFTWARE_RESUME` 后，硬件自动 resume 功能将被关闭，flash 的恢复操作改由软件在合适的时机主动发出。此时，flash 在被暂停后将一直保持暂停状态，直到软件显式恢复。在 SPI1 的等待空闲流程中，软件会主动检查 flash 的 suspend 状态，若发现处于暂停状态则调用驱动的 resume 接口让 flash 重新开始原有操作。这意味着只有当高优先级任务或中断真正完成、软件回到 SPI1 操作上下文时，才会发出 resume，从而避免 suspend-resume 行为在高优先级路径上反复抢占总线。

由于该机制依赖软件层在确定的执行点完成 resume，且当前实现没有针对多核做相应保护，因此该选项有以下限制：

- 仅支持单核场景，需要使能 :ref:`CONFIG_FREERTOS_UNICORE`。
- 属于实验性功能，需要使能 :ref:`CONFIG_IDF_EXPERIMENTAL_FEATURES` 后才能可见。
- 该功能会提升中断响应的连续性，提升应用的性能。但同时，单次操作 flash 的耗时会上升。
