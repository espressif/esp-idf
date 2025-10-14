Analog Comparator
=================

:link_to_translation:`zh_CN:[中文]`

{IDF_TARGET_ANA_CMPR_SRC_CHAN0: default="NOT UPDATED", esp32h2="GPIO11", esp32p4="GPIO52", esp32c5="GPIO9", esp32c61="GPIO9"}
{IDF_TARGET_ANA_CMPR_EXT_REF_CHAN0: default="NOT UPDATED", esp32h2="GPIO10", esp32p4="GPIO51", esp32c5="GPIO8", esp32c61="GPIO8"}
{IDF_TARGET_ANA_CMPR_SRC_CHAN1: default="NOT UPDATED", esp32p4="GPIO54"}
{IDF_TARGET_ANA_CMPR_EXT_REF_CHAN1: default="NOT UPDATED", esp32p4="GPIO53"}

Introduction
------------

Analog comparator is a peripheral that can be used to compare a source signal with the internal reference voltage or an external reference signal.

Under the scenario of comparing the analog signals, the integrated analog comparator is a cost effective scheme to replace an operational amplifier. But unlike the continuous comparing of the operational amplifier, ESP analog comparator is driven by a source clock, which decides the sampling frequency.

Analog comparator on {IDF_TARGET_NAME} has {IDF_TARGET_SOC_ANA_CMPR_NUM} unit(s), the channels in the unit(s) are:

**UNIT0**

- Source Channel: {IDF_TARGET_ANA_CMPR_SRC_CHAN0}
- External Reference Channel: {IDF_TARGET_ANA_CMPR_EXT_REF_CHAN0}
- Internal Reference Channel: Range is 0% ~ 70% of the VDD, and the step is 10% of the VDD

.. only:: esp32p4

    **UNIT1**

    - Source Channel: {IDF_TARGET_ANA_CMPR_SRC_CHAN1}
    - External Reference Channel: {IDF_TARGET_ANA_CMPR_EXT_REF_CHAN1}
    - Internal Reference Channel: Range 0% ~ 70% of the VDD, the step is 10% of the VDD

Functional Overview
-------------------

The following sections of this document cover the typical steps to install and operate an analog comparator unit:

.. list::

    - :ref:`anacmpr-resource-allocation` - covers which parameters should be set up to get a unit handle and how to recycle the resources when it finishes working.
    - :ref:`anacmpr-further-configurations` - covers the other configurations that might need to specify and what they are used for.
    - :ref:`anacmpr-enable-and-disable-unit` - covers how to enable and disable the unit.
    - :ref:`anacmpr-power-management` - describes how different source clock selections can affect power consumption.
    - :ref:`anacmpr-iram-safe` - lists which functions are supposed to work even when the cache is disabled.
    - :ref:`anacmpr-thread-safety` - lists which APIs are guaranteed to be thread safe by the driver.
    - :ref:`anacmpr-kconfig-options` - lists the supported Kconfig options that can be used to make a different effect on driver behavior.
    :SOC_ANA_CMPR_SUPPORT_ETM: - :ref:`anacmpr-etm-events` - covers how to create an analog comparator cross event.

.. _anacmpr-resource-allocation:

Resource Allocation
^^^^^^^^^^^^^^^^^^^

An analog comparator unit channel is represented by :cpp:type:`ana_cmpr_handle_t`. Each unit can support either an internal or an external reference.

To allocate the resource of the analog comparator unit, :cpp:func:`ana_cmpr_new_unit` need to be called to get the handle of the unit. Configurations :cpp:type:`ana_cmpr_config_t` need to be specified while allocating the unit:

- :cpp:member:`ana_cmpr_config_t::unit` selects the analog comparator unit.
- :cpp:member:`ana_cmpr_config_t::clk_src` selects the source clock for analog comparator, and it can affect the sampling frequency. Note that the clock source of the analog comparator comes from the IO MUX. It is shared with GPIO extension peripherals like SDM (Sigma-Delta Modulation) and Glitch Filter. The configuration will fail if you specify different clock sources for multiple GPIO extension peripherals. The default clock sources of these peripherals are same, and typically, we select :cpp:enumerator:`soc_periph_ana_cmpr_clk_src_t::ANA_CMPR_CLK_SRC_DEFAULT` as the clock source.
- :cpp:member:`ana_cmpr_config_t::ref_src` selects the reference source from internal voltage or external signal.
- :cpp:member:`ana_cmpr_config_t::cross_type` selects which kind of cross type can trigger the interrupt.

The function :cpp:func:`ana_cmpr_new_unit` can fail due to various errors such as insufficient memory, invalid arguments, etc. If a previously created analog comparator unit is no longer required, you should recycle it by calling :cpp:func:`ana_cmpr_del_unit`. It allows the underlying HW channel to be used for other purposes. Before deleting an analog comparator unit handle, you should disable it by :cpp:func:`ana_cmpr_disable` in advance, or make sure it has not enabled yet by :cpp:func:`ana_cmpr_enable`.

.. code:: c

    #include "driver/ana_cmpr.h"

    ana_cmpr_handle_t cmpr = NULL;
    ana_cmpr_config_t config = {
        .unit = 0,
        .clk_src = ANA_CMPR_CLK_SRC_DEFAULT,
        .ref_src = ANA_CMPR_REF_SRC_INTERNAL,
        .cross_type = ANA_CMPR_CROSS_ANY,
    };
    ESP_ERROR_CHECK(ana_cmpr_new_unit(&config, &cmpr));
    // ...
    ESP_ERROR_CHECK(ana_cmpr_del_unit(cmpr));

.. _anacmpr-further-configurations:

Further Configurations
^^^^^^^^^^^^^^^^^^^^^^

- :cpp:func:`ana_cmpr_set_internal_reference` - Specify the internal reference voltage when :cpp:enumerator:`ana_cmpr_ref_source_t::ANA_CMPR_REF_SRC_INTERNAL` is selected as reference source.

It requires :cpp:member:`ana_cmpr_internal_ref_config_t::ref_volt` to specify the voltage. The voltage is related to the VDD power supply, which can only support a certain fixed percentage of VDD. Currently on {IDF_TARGET_NAME}, the internal reference voltage can be range to 0 ~ 70% VDD with a step 10%.

.. code:: c

    #include "driver/ana_cmpr.h"

    ana_cmpr_internal_ref_config_t ref_cfg = {
        .ref_volt = ANA_CMPR_REF_VOLT_50_PCT_VDD,
    };
    ESP_ERROR_CHECK(ana_cmpr_set_internal_reference(cmpr, &ref_cfg));

- :cpp:func:`ana_cmpr_set_debounce` - Set the debounce configuration.

It requires :cpp:member:`ana_cmpr_debounce_config_t::wait_us` to set the interrupt waiting time. The interrupt is disabled temporarily for :cpp:member:`ana_cmpr_debounce_config_t::wait_us` microseconds, so that the frequent triggering can be avoid while the source signal is crossing the reference signal. That is, the waiting time is supposed to be inverse ratio to the relative frequency between the source and reference. If the waiting time is set too short, it can not bypass the jitter totally, but if too long, the next crossing interrupt might be missed.

.. code:: c

    #include "driver/ana_cmpr.h"

    ana_cmpr_debounce_config_t dbc_cfg = {
        .wait_us = 1,
    };
    ESP_ERROR_CHECK(ana_cmpr_set_debounce(cmpr, &dbc_cfg));

- :cpp:func:`ana_cmpr_set_cross_type` - Set the source signal cross type.

The initial cross type is set in :cpp:func:`ana_cmpr_new_unit`. This function can update the cross type, even in ISR context.

.. code:: c

    #include "driver/ana_cmpr.h"

    ESP_ERROR_CHECK(ana_cmpr_set_cross_type(cmpr, ANA_CMPR_CROSS_POS));

- :cpp:func:`ana_cmpr_register_event_callbacks` - Register the callbacks.

Currently it supports :cpp:member:`ana_cmpr_event_callbacks_t::on_cross`, and it will be called when the crossing event (specified by :cpp:member:`ana_cmpr_config_t::cross_type`) occurs.

.. code:: c

    #include "driver/ana_cmpr.h"

    static bool IRAM_ATTR example_ana_cmpr_on_cross_callback(ana_cmpr_handle_t cmpr,
                                                         const ana_cmpr_cross_event_data_t *edata,
                                                         void *user_ctx)
    {
        // ...
        return false;
    }
    ana_cmpr_event_callbacks_t cbs = {
        .on_cross = example_ana_cmpr_on_cross_callback,
    };
    ESP_ERROR_CHECK(ana_cmpr_register_event_callbacks(cmpr, &cbs, NULL));

.. note::

    When :ref:`CONFIG_ANA_CMPR_ISR_CACHE_SAFE` is enabled, you should guarantee that the callback context and involved data are in internal RAM by adding the attribute ``IRAM_ATTR`` (See more in :ref:`anacmpr-iram-safe`).

.. _anacmpr-enable-and-disable-unit:

Enable and Disable Unit
^^^^^^^^^^^^^^^^^^^^^^^

- :cpp:func:`ana_cmpr_enable` - Enable the analog comparator unit.
- :cpp:func:`ana_cmpr_disable` - Disable the analog comparator unit.

After the analog comparator unit is enabled and the crossing event interrupt is enabled, a power management lock will be acquired if the power management is enabled (see :ref:`anacmpr-power-management`). Under the **enable** state, only :cpp:func:`ana_cmpr_set_internal_reference` and :cpp:func:`ana_cmpr_set_debounce` can be called, other functions can only be called after the unit is disabled.

Calling :cpp:func:`ana_cmpr_disable` does the opposite.

.. _anacmpr-power-management:

Power Management
^^^^^^^^^^^^^^^^

When power management is enabled (i.e., :ref:`CONFIG_PM_ENABLE` is on), the system will adjust the APB frequency before going into Light-sleep mode, thus potentially changing the resolution of the analog comparator.

However, the driver can prevent the system from changing APB frequency by acquiring a power management lock of type :cpp:enumerator:`ESP_PM_NO_LIGHT_SLEEP`. Whenever the driver creates an analog comparator unit instance that has selected the clock source like :cpp:enumerator:`ANA_CMPR_CLK_SRC_DEFAULT` or :cpp:enumerator:`ANA_CMPR_CLK_SRC_XTAL`, the driver guarantees that the power management lock is acquired when enable the channel by :cpp:func:`ana_cmpr_enable`. Likewise, the driver releases the lock when :cpp:func:`ana_cmpr_disable` is called for that channel.

.. _anacmpr-iram-safe:

IRAM Safe
^^^^^^^^^

By default, the analog comparator interrupt will be deferred when the cache is disabled for reasons like programming or erasing the flash. Thus the alarm interrupt will not get executed in time, which is not expected in a real-time application.

There is a Kconfig option :ref:`CONFIG_ANA_CMPR_ISR_CACHE_SAFE` that:

1. Enables the interrupt being serviced even when cache is disabled.
2. Places all functions that used by the ISR into IRAM. [1]_
3. Places driver object into DRAM (in case it is allocated on PSRAM).

This allows the interrupt to run while the cache is disabled but comes at the cost of increased IRAM consumption.

There is a Kconfig option :ref:`CONFIG_ANA_CMPR_CTRL_FUNC_IN_IRAM` that can put commonly used IO control functions into IRAM as well. So that these functions can also be executable when the cache is disabled. These IO control functions are listed as follows:

- :cpp:func:`ana_cmpr_set_internal_reference`
- :cpp:func:`ana_cmpr_set_debounce`
- :cpp:func:`ana_cmpr_set_cross_type`

.. _anacmpr-thread-safety:

Thread Safety
^^^^^^^^^^^^^

The factory function :cpp:func:`ana_cmpr_new_unit` is guaranteed to be thread safe by the driver, which means, it can be called from different RTOS tasks without protection by extra locks.

The following functions are allowed to run under ISR context. The driver uses a critical section to prevent them being called concurrently in both task and ISR:

- :cpp:func:`ana_cmpr_set_internal_reference`
- :cpp:func:`ana_cmpr_set_debounce`
- :cpp:func:`ana_cmpr_set_cross_type`

Other functions that take :cpp:type:`ana_cmpr_handle_t` as the first positional parameter, are not treated as thread safe. As a result, users should avoid calling them from multiple tasks.

.. _anacmpr-kconfig-options:

Kconfig Options
^^^^^^^^^^^^^^^

- :ref:`CONFIG_ANA_CMPR_ISR_CACHE_SAFE` controls whether the default ISR handler can work when cache is disabled. See :ref:`anacmpr-iram-safe` for more information.
- :ref:`CONFIG_ANA_CMPR_CTRL_FUNC_IN_IRAM` controls where to place the analog comparator control functions (IRAM or flash). See :ref:`anacmpr-iram-safe` for more information.
- :ref:`CONFIG_ANA_CMPR_ENABLE_DEBUG_LOG` is used to enable the debug log output. Enabling this option increases the firmware binary size.

.. only:: SOC_ANA_CMPR_SUPPORT_ETM

    .. _anacmpr-etm-events:

    ETM Events
    ^^^^^^^^^^

    To create an analog comparator cross event, you need to include ``driver/ana_cmpr_etm.h`` additionally, and allocate the event by :cpp:func:`ana_cmpr_new_etm_event`. You can refer to :doc:`ETM </api-reference/peripherals/etm>` for how to connect an event to a task.

Application Example
-------------------

* :example:`peripherals/analog_comparator` shows the basic usage of the analog comparator, and other potential usages like hysteresis comparator and SPWM generator.

API Reference
-------------

.. include-build-file:: inc/ana_cmpr.inc
.. include-build-file:: inc/ana_cmpr_types.inc

.. [1]
   :cpp:member:`ana_cmpr_event_callbacks_t::on_cross` callback and the functions invoked by it should also be placed in IRAM. Please take care of them.
