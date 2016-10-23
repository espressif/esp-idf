.. Read the Docs Template documentation master file

Welcome to ESP32 Programming Guide
==================================


Example C functions
-------------------

.. c:function:: esp_err_t esp_wifi_get_ap_list (uint16_t *number, wifi_ap_list_t *ap_list)

.. c:function:: esp_err_t esp_wifi_set_protocol (wifi_interface_t ifx, uint8_t protocol_bitmap)


Example C function integration
------------------------------

.. doxygenfunction:: esp_wifi_init
.. doxygenfunction:: esp_wifi_set_config

.. doxygenfunction:: gpio_isr_register
.. doxygenfunction:: ledc_timer_set 	


Example C enum integration
--------------------------

.. doxygenenum:: wifi_auth_mode_t


Example C struct integration
----------------------------

.. doxygenstruct:: wifi_scan_config_t
   :members:


Contents:

.. About - TBA

.. toctree::
   :caption: Toolchain Setup
   :maxdepth: 1

   windows-setup
   linux-setup
   macos-setup
   eclipse-setup

.. API Reference - TBA

.. Technical Reference - TBA

.. toctree::
   :caption: Debugging
   :maxdepth: 1

   openocd

.. Resources - TBA
   
.. toctree::
   :caption: Contribute
   :maxdepth: 1

   contributing
   contributor-agreement

.. toctree::
   :caption: Copyrights and Licenses
   :maxdepth: 1

   COPYRIGHT

.. toctree::
   :caption: Flapping Documents
   :maxdepth: 1
   
   partition-tables
   build_system

   
Indices and tables
==================

* :ref:`genindex`
* :ref:`search`
