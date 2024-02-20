+---------------+---------------------------------------+-------------------------------------+
| Max CPU       |            Lock Acquisition           | CPU and APB Frequencies             |
| Frequency Set |                                       |                                     |
+---------------+---------------------------------------+-------------------------------------+
|       96      | ``ESP_PM_CPU_FREQ_MAX`` acquired      | | CPU: 96 MHz                       |
|               |                                       | | APB: 32 MHz                       |
+               +---------------------------------------+-------------------------------------+
|               | ``ESP_PM_APB_FREQ_MAX`` acquired,     | | CPU: 32 MHz                       |
|               | ``ESP_PM_CPU_FREQ_MAX`` not acquired  | | APB: 32 MHz                       |
+               +---------------------------------------+-------------------------------------+
|               |                  None                 | Min values for both frequencies set |
|               |                                       | with :cpp:func:`esp_pm_configure`   |
+---------------+---------------------------------------+-------------------------------------+
|       64      | ``ESP_PM_CPU_FREQ_MAX`` acquired      | | CPU: 64 MHz                       |
|               |                                       | | APB: 32 MHz                       |
+               +---------------------------------------+-------------------------------------+
|               | ``ESP_PM_APB_FREQ_MAX`` acquired,     | | CPU: 32 MHz                       |
|               | ``ESP_PM_CPU_FREQ_MAX`` not acquired  | | APB: 32 MHz                       |
+               +---------------------------------------+-------------------------------------+
|               |                  None                 | Min values for both frequencies set |
|               |                                       | with :cpp:func:`esp_pm_configure`   |
+---------------+---------------------------------------+-------------------------------------+
|       48      | ``ESP_PM_CPU_FREQ_MAX`` acquired      | | CPU: 48 MHz                       |
|               |                                       | | APB: 32 MHz                       |
+               +---------------------------------------+-------------------------------------+
|               | ``ESP_PM_APB_FREQ_MAX`` acquired,     | | CPU: 32 MHz                       |
|               | ``ESP_PM_CPU_FREQ_MAX`` not acquired  | | APB: 32 MHz                       |
+               +---------------------------------------+-------------------------------------+
|               |                  None                 | Min values for both frequencies set |
|               |                                       | with :cpp:func:`esp_pm_configure`   |
+---------------+---------------------------------------+-------------------------------------+
