Interrupt allocation
====================

Overview
--------

The ESP32 has two cores, with 32 interrupts each. Each interrupt has a certain priority level, most (but not all) interrupts are connected
to the interrupt mux. Because there are more interrupt sources than interrupts, sometimes it makes sense to share an interrupt in
multiple drivers. The esp_intr_alloc abstraction exists to hide all these implementation details.

A driver can allocate an interrupt for a certain peripheral by calling esp_intr_alloc (or esp_intr_alloc_sintrstatus). It can use
the flags passed to this function to set the type of interrupt allocated, specifying a specific level or trigger method. The
interrupt allocation code will then find an applicable interrupt, use the interrupt mux to hook it up to the peripheral, and
install the given interrupt handler and ISR to it.

This code has two different types of interrupts it handles differently: Shared interrupts and non-shared interrupts. The simplest
of the two are non-shared interrupts: a separate interrupt is allocated per esp_intr_alloc call and this interrupt is solely used for
the peripheral attached to it, with only one ISR that will get called. Non-shared interrupts can have multiple peripherals triggering 
it, with multiple ISRs being called when one of the peripherals attached signals an interrupt. Thus, ISRs that are intended for shared
interrupts should check the interrupt status of the peripheral they service in order to see if any action is required.

Non-shared interrupts can be either level- or edge-triggered. Shared interrupts can
only be level interrupts (because of the chance of missed interrupts when edge interrupts are
used.)
(The logic behind this: DevA and DevB share an int. DevB signals an int. Int line goes high. ISR handler
calls code for DevA -> does nothing. ISR handler calls code for DevB, but while doing that,
DevA signals an int. ISR DevB is done, clears int for DevB, exits interrupt code. Now an 
interrupt for DevA is still pending, but because the int line never went low (DevA kept it high
even when the int for DevB was cleared) the interrupt is never serviced.)




Application Example
-------------------

API Reference
-------------

Header Files
^^^^^^^^^^^^

  * `esp_intr_alloc.h <https://github.com/espressif/esp-idf/blob/master/components/esp32/include/esp_intr_alloc.h>`_


Macros
^^^^^^

.. doxygendefine:: ESP_INTR_FLAG_LEVEL1
.. doxygendefine:: ESP_INTR_FLAG_LEVEL2
.. doxygendefine:: ESP_INTR_FLAG_LEVEL3
.. doxygendefine:: ESP_INTR_FLAG_LEVEL4
.. doxygendefine:: ESP_INTR_FLAG_LEVEL5
.. doxygendefine:: ESP_INTR_FLAG_LEVEL6
.. doxygendefine:: ESP_INTR_FLAG_NMI
.. doxygendefine:: ESP_INTR_FLAG_LOWMED
.. doxygendefine:: ESP_INTR_FLAG_HIGH
.. doxygendefine:: ESP_INTR_FLAG_SHARED
.. doxygendefine:: ESP_INTR_FLAG_EDGE
.. doxygendefine:: ESP_INTR_FLAG_IRAM

Type Definitions
^^^^^^^^^^^^^^^^

Enumerations
^^^^^^^^^^^^

Structures
^^^^^^^^^^

Functions
^^^^^^^^^

.. doxygenfunction:: esp_intr_mark_shared
.. doxygenfunction:: esp_intr_reserve
.. doxygenfunction:: esp_intr_alloc
.. doxygenfunction:: esp_intr_alloc_intrstatus
.. doxygenfunction:: esp_intr_free
.. doxygenfunction:: esp_intr_get_cpu
.. doxygenfunction:: esp_intr_get_intno
.. doxygenfunction:: esp_intr_disable
.. doxygenfunction:: esp_intr_enable
.. doxygenfunction:: esp_intr_noniram_disable
.. doxygenfunction:: esp_intr_noniram_enable
