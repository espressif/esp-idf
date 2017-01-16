Controller && VHCI
==================

Overview
--------

`Instructions`_

Application Example
-------------------

Check `/examples <https://github.com/espressif/esp-idf/tree/master/examples>`_ folder of `espressif/esp-idf <https://github.com/espressif/esp-idf/>`_ repository, that contains the following example:

`05_ble_adv <https://github.com/espressif/esp-idf/blob/master/examples/05_ble_adv/main/app_bt.c>`_ 

  This is a BLE advertising demo with virtual HCI interface. Send Reset/ADV_PARAM/ADV_DATA/ADV_ENABLE HCI command for BLE advertising.

`Instructions`_

.. _Instructions: template.html

API Reference
-------------

Header Files
^^^^^^^^^^^^

  * `bt/include/bt.h <https://github.com/espressif/esp-idf/blob/master/components/bt/include/bt.h>`_

Type Definitions
^^^^^^^^^^^^^^^^

.. doxygentypedef:: esp_vhci_host_callback_t

Enumerations
^^^^^^^^^^^^


Structures
^^^^^^^^^^

.. doxygenstruct:: esp_vhci_host_callback
    :members:


Functions
^^^^^^^^^

.. doxygenfunction:: esp_bt_controller_init
.. doxygenfunction:: esp_vhci_host_check_send_available
.. doxygenfunction:: esp_vhci_host_send_packet
.. doxygenfunction:: esp_vhci_host_register_callback

