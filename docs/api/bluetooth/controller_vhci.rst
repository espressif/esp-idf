Controller && VHCI
==================

Overview
--------

`Instructions`_

.. _Instructions: ../template.html

Application Example
-------------------

Check :example:`bluetooth` folder in ESP-IDF examples, which contains the following example:

:example:`bluetooth/ble_adv`

  This is a BLE advertising demo with virtual HCI interface. Send Reset/ADV_PARAM/ADV_DATA/ADV_ENABLE HCI command for BLE advertising.

API Reference
-------------

Header Files
^^^^^^^^^^^^

  * :component_file:`bt/include/bt.h`

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

