==================================================
MCPWM Fault: Bring a Protection Signal into MCPWM
==================================================

.. contents::
    :local:
    :depth: 2

A fault object represents an abnormal condition. A GPIO fault is for a hardware signal such as an over-current comparator; a software fault lets application logic trigger the same protection route. Use the object with the operator :ref:`brake <mcpwm-brake>` to define the output response.

The purpose of the fault path is to make protection depend as little as possible on software polling or task scheduling. In motor drives and power converters, over-current, interlock, or emergency-stop conditions usually need hardware to force a safe output state first, then let software decide how to log and recover.

Create an active-low GPIO fault
===============================

Create the fault in the same group as the operator that will consume it. The pin's electrical pull configuration is separate GPIO setup, so make the inactive level unambiguous before starting the power stage. The MCPWM driver does not enable internal pull resistors for a GPIO fault pin; if the fault signal does not drive the pin in the inactive state, configure the pull direction yourself with :cpp:func:`gpio_set_pull_mode()`.

.. code-block:: c

    mcpwm_fault_handle_t fault = NULL;
    mcpwm_gpio_fault_config_t fault_config = {
        .group_id = 0,
        .gpio_num = 4,
        .flags.active_level = 0,
    };
    ESP_ERROR_CHECK(mcpwm_new_gpio_fault(&fault_config, &fault));

The GPIO fault configuration has a few fields to consider:

.. list::

    - :cpp:member:`group_id <mcpwm_gpio_fault_config_t::group_id>` — the MCPWM group the fault belongs to. It must match the group of the operator consuming the fault.
    - :cpp:member:`gpio_num <mcpwm_gpio_fault_config_t::gpio_num>` — the GPIO carrying the fault signal.
    - :cpp:member:`active_level <mcpwm_gpio_fault_config_t::flags::active_level>` — the level treated as active. The example uses ``0``, so the fault is active low; the pin's pull direction must keep it inactive (high) when nothing asserts the fault. The driver leaves the pad's pull configuration untouched, so call :cpp:func:`gpio_set_pull_mode()` to select the pull-up/pull-down as appropriate.
    - :cpp:member:`intr_priority <mcpwm_gpio_fault_config_t::intr_priority>` — the interrupt priority used by the fault event callbacks. Not setting it (``0``) lets the driver choose a low priority.

Create a software fault
=======================

For an application-detected condition, create :cpp:func:`mcpwm_new_soft_fault()` and invoke :cpp:func:`mcpwm_soft_fault_activate()` when the condition occurs, instead of wiring a GPIO fault pin. The activation is a one-time fault event; its output policy is still configured by the operator :ref:`brake <mcpwm-brake>` mechanism.

.. note::

    Bind the soft fault to an operator with :cpp:func:`mcpwm_operator_set_brake_on_fault()` before activating it. The driver does not attach the soft fault to an operator at :cpp:func:`mcpwm_new_soft_fault()` time; the operator association and its brake mode are set by the bind call, and a soft fault can be bound to only one operator. Calling :cpp:func:`mcpwm_soft_fault_activate()` before binding is undefined behavior.

Fault as a trigger for generator actions
=========================================

A GPIO fault can also directly trigger a generator action via :cpp:func:`mcpwm_generator_set_action_on_fault_event()`. This is a local edge-level response — it changes the output at the fault edge but does not latch a safe state. For persistent braking with recovery, use the operator :ref:`brake mechanism <mcpwm-brake>`.

.. list-table::
    :header-rows: 1
    :widths: 22 34 28

    * - Mechanism
      - Behavior
      - Best fit
    * - Generator fault action
      - Immediately changes one output at the fault edge
      - Local fast reaction on a single output
    * - Operator brake
      - Defines safe state, latching, and recovery policy
      - Primary protection path for a power stage

Fault event callbacks
=====================

The :cpp:member:`on_fault_enter <mcpwm_fault_event_callbacks_t::on_fault_enter>` and :cpp:member:`on_fault_exit <mcpwm_fault_event_callbacks_t::on_fault_exit>` callbacks report GPIO fault transitions and are only available for GPIO faults — the driver rejects registering them on a soft fault. Soft faults trigger the brake immediately in hardware, with no callback. The callbacks run in ISR context. Timestamp the event or notify a task with an ISR-safe primitive, then make logging and recovery decisions in the task.

.. code-block:: c

    mcpwm_fault_event_callbacks_t cbs = {
        .on_fault_enter = my_fault_enter_cb,
        .on_fault_exit = my_fault_exit_cb,
    };
    ESP_ERROR_CHECK(mcpwm_fault_register_event_callbacks(fault, &cbs, NULL));

API Reference
=============

MCPWM Fault Driver Functions
----------------------------

.. include-build-file:: inc/mcpwm_fault.inc
