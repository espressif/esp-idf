# Component lists shared by the cmakev1 (g1_components) and cmakev2
# (g1_components_cmakev2) variants of this test app. Must be included before the
# project is configured, since cmakev1 uses COMPONENTS to restrict the build.

set(g0_components soc hal esp_common esp_rom)  # also <arch>, i.e. xtensa or riscv, will be added below
set(g1_components freertos log heap esp_libc esp_system esp_hw_support esp_mm esp_stdio esp_mspi)
# The HAL components that required by G1 components
# Most of these HAL components are required by `clk.c` in `esp_system` for peripherals clock initialization.
set(esp_hal_components
    esp_hal_ana_conv
    esp_hal_cache
    esp_hal_cam
    esp_hal_dma
    esp_hal_gpio
    esp_hal_i2s
    esp_hal_mspi
    esp_hal_gpspi
    esp_hal_timg
    esp_hal_touch_sens
    esp_hal_uart
    esp_hal_usb
    esp_hal_wdt
    esp_hal_pmu
    esp_hal_rtc_timer
    esp_hal_clock
    esp_hal_security
    esp_hal_systimer
    esp_hal_regi2c
    esp_hal_debug_assist
)
set(COMPONENTS ${g0_components} ${g1_components} ${esp_hal_components} main)
