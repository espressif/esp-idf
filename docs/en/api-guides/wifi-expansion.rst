Wi-Fi Expansion
===============

:link_to_translation:`zh_CN:[中文]`

.. only:: not SOC_WIFI_SUPPORTED

  {IDF_TARGET_NAME} does not support Wi-Fi functionality natively, but it is possible to use the same Wi-Fi API and features using Wi-Fi expansion.

.. only:: SOC_WIFI_SUPPORTED

  {IDF_TARGET_NAME} does support Wi-Fi functionality natively, please refer to :doc:`wifi` documentation. Even though Wi-Fi is supported on {IDF_TARGET_NAME}, it is possible to expand it and use another instance of Wi-Fi expansion interfaces using `esp_wifi_remote <https://components.espressif.com/components/espressif/esp_wifi_remote>`_ component.


{IDF_TARGET_NAME} esp_wifi_remote
---------------------------------

The principle of Wi-Fi Expansion operation is to add another ESP32 series, Wi-Fi capable, target connected to the {IDF_TARGET_NAME} in a pre-defined way. Your project could then include the `esp_wifi_remote <https://components.espressif.com/components/espressif/esp_wifi_remote>`_ component using:

.. code:: bash

  idf.py add-dependency esp_wifi_remote


Please follow the instructions described in the component documentation which is linked in the above paragraph.

.. only:: not SOC_WIFI_SUPPORTED

  To explore the Wi-Fi Expansion functionality on {IDF_TARGET_NAME}, you can get started with this example: :idf_file:`examples/protocols/mqtt/tcp/README.md` and choose Wi-Fi connection in the project configuration menu.

.. only:: SOC_WIRELESS_HOST_SUPPORTED

  {IDF_TARGET_NAME} esp-extconn
  -----------------------------------------

  The principle of esp-extconn operation is to add another supported target series connected to the {IDF_TARGET_NAME} in a pre-defined way. Compared to the esp_wifi_remote approach, the target side can operate without flash, as the firmware is delivered by the hosted side. Your project could then include the `esp-extconn <https://components.espressif.com/components/espressif/esp-extconn>`_ component using:

  .. code:: bash

    idf.py add-dependency esp-extconn


  Please follow the instructions described in the `esp-extconn documentation <https://github.com/espressif/esp-extconn/blob/master/README.md>`_.

  To explore the esp-extconn functionality on {IDF_TARGET_NAME}, you can get started with this example: :idf_file:`examples/wifi/iperf/README.md` and choose Wi-Fi connection in the project configuration menu.
