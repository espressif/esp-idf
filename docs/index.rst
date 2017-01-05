ESP32 Programming Guide
=======================

.. caution::

   Until ESP-IDF release 1.0, this documentation is a draft. It is incomplete and may have mistakes. Please mind your step!

Documentation adressed to developers of applications for `ESP32 <https://espressif.com/en/products/hardware/esp32/overview>`_ by `Espressif <https://espressif.com/>`_ using `esp-idf <https://github.com/espressif/esp-idf>`_.


Contents:

.. toctree::
   :caption: Setup Toolchain
   :maxdepth: 1

   Windows <windows-setup>
   Linux <linux-setup>
   Mac OS <macos-setup>

.. Connect - TBA

.. toctree::
   :caption: Build and Flash
   :maxdepth: 1

   Make <make-project>
   Eclipse IDE <eclipse-setup>

.. toctree::
   :caption: What Else?
   :maxdepth: 1
   
   General Notes <general-notes>
   partition-tables
   build_system
   openocd
   Flash encryption <security/flash-encryption>
   Secure Boot <security/secure-boot>
   ULP coprocessor <api/ulp.rst>

.. API Reference
   ..
   Table of Contents Outline
   ..
   1. System - TBA
     1.1. Fundamentals of multiprocessor programming with FreeRTOS - TBA
     1.2. Application startup flow - TBA
     1.3. Flash encryption and secure boot: how they work and APIs
     1.4. Lower Power Coprocessor - TBA
     1.5. Watchdogs <api/wdts>
     1.6. Memory allocation <api/mem_alloc>
     1.7. ...
   2. Memory - TBA
     2.1. Memory layout of the application (IRAM/IROM, limitations of each) - TBA
     2.2. Flash layout and partitions - TBA
     2.3. Flash access APIs - TBA
     2.4. Partition APIs - TBA
     2.5. OTA mechanism (app partitions, OTA partition) and APIs - TBA
     2.6. ... 
   3. Wi-Fi
   4. Bluetooth
     4.1. BT Classic - TBA
     4.2. BLE
   5. Ethernet - TBA
   6. Interfaces
     6.1. GPIO
     6.2. ADC - TBA
     6.3. DAC - TBA
     6.4. UART
     6.5. I2C - TBA
     6.6. I2S - TBA
     6.7. SPI - TBA
     6.8. CAN - TBA
     6.9. SD Controller - TBA
     6.10. Infrared - TBA
     6.11. Pulse Counter - TBA
     6.12. PWM - TBA
     6.13. LED PWM
     6.14. ...
   7. Sensors - TBA
     7.1. Hall Sensor - TBA
     7.2. Temperature Sensor - TBA
     7.3. Touch Sensor - TBA
   8. Protocols - TBA
   9. Components
     9.1. Logging
     9.2  Non-Volatile Storage
     9.3  Virtual Filesystem
     9.3. Http sever - TBA
   10. Applications - TBA
   ..
   API Dcoumentation Teamplate
   ..

.. toctree::
   :caption: API Reference
   :maxdepth: 1

   Wi-Fi <api/esp_wifi>
   Smart Config <api/esp_smartconfig>
   Bluetooth <api/bt>
   Watchdogs <api/wdts>
   OTA <api/ota>
   GPIO <api/gpio>
   UART <api/uart>
   LED Control <api/ledc>
   Remote Control <api/rmt>
   Timer <api/timer>
   I2C <api/i2c>
   Pulse Counter <api/pcnt>
   Sigma-delta Modulation <api/sigmadelta>
   SPI Flash and Partition APIs <api/spi_flash>
   Logging <api/log>
   Non-Volatile Storage <api/nvs_flash>
   Virtual Filesystem <api/vfs>
   Ethernet <api/esp_eth>
   Interrupt Allocation <api/intr_alloc>
   Memory Allocation <api/mem_alloc>
   Deep Sleep <api/deep_sleep>
   deep-sleep-stub

   Template <api/template>

.. toctree::
   :caption: Technical Reference

   Technical Reference <http://espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf>

.. Resources - TBA
   
.. toctree::
   :caption: Contribute
   :maxdepth: 1

   contributing
   Style Guide <style-guide>
   documenting-code
   contributor-agreement

.. toctree::
   :caption: Legal
   :maxdepth: 1

   COPYRIGHT

 
Indices
-------

* :ref:`genindex`
* :ref:`search`

