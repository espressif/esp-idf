Thread
==========

Introduction
------------

`Thread <https://www.threadgroup.org>`_ is a IP-based mesh networking protocol. It's based on the 802.15.4 physical and MAC layer.

Application Examples
--------------------

The :example:`openthread` directory of ESP-IDF examples contains the following applications:

- The OpenThread interactive shell :example:`openthread/ot_cli`.
- The Thread border router :example:`openthread/ot_br`.
- The Thread radio co-processor :example:`openthread/ot_rcp`.


- :example:`openthread/ot_trel` demonstrates Thread Radio Encapsulation Link (TREL) function. This requires a board equipped with a Wi-Fi module.

- :example:`openthread/ot_sleepy_device/deep_sleep` demonstrates Thread Deep-sleep function.

- :example:`openthread/ot_sleepy_device/light_sleep` demonstrates Thread Light-sleep function.

API Reference
-------------

For manipulating the Thread network, the OpenThread api shall be used.
The OpenThread api docs can be found at the `OpenThread official website <https://openthread.io/reference>`_.

ESP-IDF provides extra apis for launching and managing the OpenThread stack, binding to network interfaces and border routing features.

.. include-build-file:: inc/esp_openthread.inc
.. include-build-file:: inc/esp_openthread_types.inc
.. include-build-file:: inc/esp_openthread_lock.inc
.. include-build-file:: inc/esp_openthread_netif_glue.inc
.. include-build-file:: inc/esp_openthread_border_router.inc
