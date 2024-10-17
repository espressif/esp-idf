
.. flat-table::
    :widths: 1 3 3

    * - CPU 最高频率
      - 电源管理锁获取情况
      - APB 频率和 CPU 频率
    * - :rspan:`2` 160
      - 获取 ``ESP_PM_CPU_FREQ_MAX``
      -
        - CPU: 160 MHz
        - APB: 80 MHz
    * - 获取 ``ESP_PM_APB_FREQ_MAX``，未获得 ``ESP_PM_CPU_FREQ_MAX``
      -
        - CPU: 80 MHz
        - APB: 80 MHz
    * - 无
      - 使用 :cpp:func:`esp_pm_configure` 为二者设置最小值
    * - :rspan:`1` 80
      - 获取 ``ESP_PM_CPU_FREQ_MAX`` 或 ``ESP_PM_APB_FREQ_MAX``
      -
        - CPU: 80 MHz
        - APB: 80 MHz
    * - 无
      - 使用 :cpp:func:`esp_pm_configure` 为二者设置最小值
