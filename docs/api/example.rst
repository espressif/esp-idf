Example Visualizations
======================

Function prototpe
-----------------

.. c:function:: esp_err_t esp_wifi_get_ap_list (uint16_t *number, wifi_ap_list_t *ap_list)
.. c:function:: esp_err_t esp_wifi_set_protocol (wifi_interface_t ifx, uint8_t protocol_bitmap)


Function definition
-------------------

Wi-Fi
^^^^^
.. doxygenfunction:: esp_wifi_init
.. doxygenfunction:: esp_wifi_set_config

GPIO
^^^^
.. doxygenfunction:: gpio_isr_register

Led Control
^^^^^^^^^^^

.. doxygenfunction:: ledc_timer_set 	


Enum definition
---------------

.. doxygenenum:: wifi_auth_mode_t


Struct definition
-----------------

.. doxygenstruct:: wifi_scan_config_t
   :members:


