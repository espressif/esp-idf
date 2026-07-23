==================================
MCPWM 故障：将保护信号接入 MCPWM
==================================

.. contents::
    :local:
    :depth: 2

故障对象表示异常条件。GPIO 故障适合过流比较器等硬件信号；软件故障可让应用逻辑触发同一保护路径。随后使用操作器 :ref:`制动 <mcpwm-brake>` 定义输出响应。

故障模块存在的目的，是让保护路径尽可能不依赖软件轮询或任务调度。对于电机驱动和功率变换器，过流、互锁或急停通常要求在硬件路径里先把输出拉到安全状态，再由软件决定如何记录和恢复。

创建低电平有效 GPIO 故障源
==========================

故障源必须和将使用它的操作器位于同一组。引脚上拉/下拉属于独立 GPIO 配置；MCPWM 驱动不会为故障 GPIO 引脚自动使能内部上拉/下拉。如果故障信号在其他状态下并不驱动引脚电平，请自行用 :cpp:func:`gpio_set_pull_mode()` 配置上拉/下拉方向；启动功率级前应确保非故障电平明确。

.. code-block:: c

    mcpwm_fault_handle_t fault = NULL;
    mcpwm_gpio_fault_config_t fault_config = {
        .group_id = 0,
        .gpio_num = 4,
        .flags.active_level = 0,
    };
    ESP_ERROR_CHECK(mcpwm_new_gpio_fault(&fault_config, &fault));

GPIO 故障配置中有几个字段值得了解：

.. list::

    - :cpp:member:`group_id <mcpwm_gpio_fault_config_t::group_id>` — 故障所属的 MCPWM 组。必须与使用该故障的操作器所在组一致。
    - :cpp:member:`gpio_num <mcpwm_gpio_fault_config_t::gpio_num>` — 承载故障信号的 GPIO。
    - :cpp:member:`active_level <mcpwm_gpio_fault_config_t::flags::active_level>` — 何种电平视为有效。示例使用 ``0``，即故障为低有效；无故障时引脚上拉方向应使其保持非有效（高电平）。驱动不会改动引脚的上下拉配置，请按需调用 :cpp:func:`gpio_set_pull_mode()` 选择上拉或下拉。
    - :cpp:member:`intr_priority <mcpwm_gpio_fault_config_t::intr_priority>` — 故障事件回调使用的中断优先级。不设置（``0``）时由驱动选择较低优先级。

创建软件故障
============

对于应用检测到的条件，可创建 :cpp:func:`mcpwm_new_soft_fault()`，并在条件发生时调用 :cpp:func:`mcpwm_soft_fault_activate()`，而无需接入 GPIO 故障引脚。激活是一次性的故障事件，输出策略仍由操作器 :ref:`制动机制 <mcpwm-brake>` 决定。

.. note::

    激活前必须先用 :cpp:func:`mcpwm_operator_set_brake_on_fault()` 将软件故障绑定到某个操作器。驱动在 :cpp:func:`mcpwm_new_soft_fault()` 时并不会关联操作器；操作器关联及其制动模式都由该绑定调用设置，且一个软件故障只能绑定一个操作器。在绑定前调用 :cpp:func:`mcpwm_soft_fault_activate()` 属于未定义行为。

故障作为生成器动作的触发源
==========================

GPIO 故障也可通过 :cpp:func:`mcpwm_generator_set_action_on_fault_event()` 直接触发生成器动作。这是局部的边沿级响应——在故障边沿改变输出，但不会锁存安全状态。如需带恢复的持续制动，应使用操作器 :ref:`制动机制 <mcpwm-brake>`。

.. list-table::
    :header-rows: 1
    :widths: 18 34 28

    * - 机制
      - 行为
      - 适合场景
    * - 生成器故障动作
      - 单个输出在故障边沿立即改变
      - 某一路输出的局部快速响应
    * - 操作器制动
      - 为整个输出级定义安全状态、是否锁存和恢复策略
      - 主保护路径、半桥/全桥安全关断

故障事件回调
============

:cpp:member:`on_fault_enter <mcpwm_fault_event_callbacks_t::on_fault_enter>` 和 :cpp:member:`on_fault_exit <mcpwm_fault_event_callbacks_t::on_fault_exit>` 回调报告 GPIO 故障跳变，且仅 GPIO 故障可用——驱动会拒绝在软件故障上注册这些回调。软件故障会在硬件中立即触发制动，不会产生回调。这些回调在 ISR 上下文执行。应只进行时间戳记录或使用 ISR 安全的方式通知任务，在任务中做日志和恢复决策。

.. code-block:: c

    mcpwm_fault_event_callbacks_t cbs = {
        .on_fault_enter = my_fault_enter_cb,
        .on_fault_exit = my_fault_exit_cb,
    };
    ESP_ERROR_CHECK(mcpwm_fault_register_event_callbacks(fault, &cbs, NULL));

API 参考
========

MCPWM 故障驱动函数
------------------

.. include-build-file:: inc/mcpwm_fault.inc
