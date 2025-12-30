# ESP Hardware Abstraction Layer for RTC Timer Peripherals

> [!NOTE]
> This component is currently in beta. Its API, behavior, and compatibility may change at any time and without notice; backward compatibility is not guaranteed. Use caution when integrating into production systems.

## Overview

The `esp_hal_rtc_timer` component provides a **unified Hardware Abstraction Layer** for RTC Timer peripherals across all ESP-IDF supported targets. This HAL abstracts hardware differences between legacy RTC timer implementations (in RTC_CNTL peripheral) and modern low-power timer implementations, providing a consistent API for wakeup timer configuration and RTC time reading across different ESP chip families.

The component consolidates timer functionality from two different hardware implementations:
- **SOC_RTC_TIMER_V1_SUPPORTED**: Legacy RTC timer functionality in RTC_CNTL peripheral (ESP32, ESP32C2, ESP32C3, ESP32S2, ESP32S3)
- **SOC_RTC_TIMER_V2_SUPPORTED**: Newer LP timer peripheral (ESP32C5, ESP32C6, ESP32C61, ESP32H2, ESP32H21, ESP32H4, ESP32P4)

### Unified HAL API (All Chips)
- Wakeup timer configuration via `rtc_timer_hal_set_wakeup_time()`
- RTC time reading via `rtc_timer_hal_get_cycle_count()`

### Hardware-Specific LL API
- **V1 Chips**: Direct register access in RTC_CNTL peripheral
- **V2 Chips**: LP Timer peripheral with alarm target configuration and counter snapshot operations

Please note that the interfaces provided by this component are internal to ESP-IDF and are subject to change.
