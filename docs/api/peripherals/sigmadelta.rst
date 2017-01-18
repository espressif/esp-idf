Sigma-delta Modulation
======================

Overview
--------

ESP32 has a second-order sigma-delta modulation module. 
This driver configures the channels of the sigma-delta module.

Application Example
-------------------

Sigma-delta Modulation example: `examples/peripherals/sigmadelta <https://github.com/espressif/esp-idf/tree/master/examples/peripherals/sigmadelta>`_.

API Reference
-------------

Header Files
^^^^^^^^^^^^

  * `driver/sigmadelta.h <https://github.com/espressif/esp-idf/blob/master/components/driver/include/driver/sigmadelta.h>`_


Macros
^^^^^^


Type Definitions
^^^^^^^^^^^^^^^^


Enumerations
^^^^^^^^^^^^

.. doxygenenum:: sigmadelta_channel_t

Structures
^^^^^^^^^^

.. doxygenstruct:: sigmadelta_config_t
    :members:


Functions
^^^^^^^^^

.. doxygenfunction:: sigmadelta_config
.. doxygenfunction:: sigmadelta_set_duty
.. doxygenfunction:: sigmadelta_set_prescale
.. doxygenfunction:: sigmadelta_set_pin

