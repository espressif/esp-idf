ESP32 Power Management Algorithm
--------------------------------

The table below shows how CPU and APB frequencies will be switched if dynamic frequency scaling is enabled. You can specify the maximum CPU frequency with either :cpp:func:`esp_pm_configure` or :ref:`CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ`.


+---------------+---------------------------------------+-------------------------------------+
| Max CPU       |            Lock Acquisition           | CPU and APB Frequncies              |
| Frequency Set |                                       |                                     |
+---------------+---------------------------------------+-------------------------------------+
|      240      | | Any of ``ESP_PM_CPU_FREQ_MAX``      |                                     |
|               | | or ``ESP_PM_APB_FREQ_MAX`` acquired | | CPU: 240 MHz                      |
|               |                                       | | APB: 80 MHz                       |
+               +---------------------------------------+-------------------------------------+
|               |                  None                 | Min values for both frequencies set |
|               |                                       | with :cpp:func:`esp_pm_configure`   |
+---------------+---------------------------------------+-------------------------------------+
|      160      | ``ESP_PM_CPU_FREQ_MAX`` acquired      | | CPU: 160 MHz                      |
|               |                                       | | APB: 80 MHz                       |
+               +---------------------------------------+-------------------------------------+
|               | ``ESP_PM_CPU_FREQ_MAX`` acquired,     | | CPU: 80 MHz                       |
|               | ``ESP_PM_APB_FREQ_MAX`` not acquired  | | APB: 80 MHz                       |
+               +---------------------------------------+-------------------------------------+
|               |                  None                 | Min values for both frequencies set |
|               |                                       | with :cpp:func:`esp_pm_configure`   |
+---------------+---------------------------------------+-------------------------------------+
|       80      | | Any of ``ESP_PM_CPU_FREQ_MAX``      | | CPU: 80 MHz                       |
|               | | or ``ESP_PM_APB_FREQ_MAX`` acquired | | APB: 80 MHz                       |
+               +---------------------------------------+-------------------------------------+
|               |                  None                 | Min values for both frequencies set |
|               |                                       | with :cpp:func:`esp_pm_configure`   |
+---------------+---------------------------------------+-------------------------------------+


If none of the locks are acquired, and light sleep is enabled in a call to :cpp:func:`esp_pm_configure`, the system will go into light sleep mode. The duration of light sleep will be determined by:

- FreeRTOS tasks blocked with finite timeouts
- Timers registered with :doc:`High resolution timer <esp_timer>` APIs

Light sleep duration will be chosen to wake up the chip before the nearest event (task being unblocked, or timer elapses).
