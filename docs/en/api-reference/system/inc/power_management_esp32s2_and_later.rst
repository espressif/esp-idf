{IDF_TARGET_APB_FREQ: default="80 Mhz", esp32c2="40 MHz"}

+---------------+---------------------------------------+-------------------------------------+
| Max CPU       |            Lock Acquisition           | CPU and APB Frequencies             |
| Frequency Set |                                       |                                     |
+---------------+---------------------------------------+-------------------------------------+
|      240      | ``ESP_PM_CPU_FREQ_MAX`` acquired      | | CPU: 240 MHz                      |
|               |                                       | | APB: {IDF_TARGET_APB_FREQ}                       |
+               +---------------------------------------+-------------------------------------+
|               | ``ESP_PM_APB_FREQ_MAX`` acquired,     | | CPU: 80 MHz                       |
|               | ``ESP_PM_CPU_FREQ_MAX`` not acquired  | | APB: {IDF_TARGET_APB_FREQ}                       |
+               +---------------------------------------+-------------------------------------+
|               |                  None                 | Min values for both frequencies set |
|               |                                       | with :cpp:func:`esp_pm_configure`   |
+---------------+---------------------------------------+-------------------------------------+
|      160      | ``ESP_PM_CPU_FREQ_MAX`` acquired      | | CPU: 160 MHz                      |
|               |                                       | | APB: {IDF_TARGET_APB_FREQ}                       |
+               +---------------------------------------+-------------------------------------+
|               | ``ESP_PM_APB_FREQ_MAX`` acquired,     | | CPU: 80 MHz                       |
|               | ``ESP_PM_CPU_FREQ_MAX`` not acquired  | | APB: {IDF_TARGET_APB_FREQ}                       |
+               +---------------------------------------+-------------------------------------+
|               |                  None                 | Min values for both frequencies set |
|               |                                       | with :cpp:func:`esp_pm_configure`   |
+---------------+---------------------------------------+-------------------------------------+
|       80      | | Any of ``ESP_PM_CPU_FREQ_MAX``      | | CPU: 80 MHz                       |
|               | | or ``ESP_PM_APB_FREQ_MAX`` acquired | | APB: {IDF_TARGET_APB_FREQ}                       |
+               +---------------------------------------+-------------------------------------+
|               |                  None                 | Min values for both frequencies set |
|               |                                       | with :cpp:func:`esp_pm_configure`   |
+---------------+---------------------------------------+-------------------------------------+
