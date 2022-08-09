| Supported Targets | ESP32-S3 |
| ----------------- | -------- |

# RTC power test

This test app is to enter 7 different sub power modes we have, so that the power consumption under different power modes can be measured.

Currently there are 6 sub power modes, 3 for deepsleep and 3 for lightsleep. Show as below (priority from high to low).

## Deepsleep
1. Mode for ADC/Temp Sensor in monitor mode (ULP). To enable this mode, call `esp_sleep_enable_adc_tsens_monitor`.
2. Default mode.
3. Ultra low power mode. To enable this mode, call `rtc_sleep_enable_ultra_low`. Note if mode 1 has higher priority than this.

## Lightsleep
1. Mode for using 40 MHz XTAL in lightsleep. To enable this mode, call `esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_ON)`.
2. Mode for using 8M clock by digital system (peripherals). To enable this mode, initialize LEDC with 8M clock source.
3. Mode for ADC/Temp Sensor in monitor mode (ULP). To enable this mdoe, call `esp_sleep_enable_adc_tsens_monitor`.
4. Default mode.
