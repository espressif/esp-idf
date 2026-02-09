/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "bootloader_console.h"
#include "soc/soc_caps.h"
#include "hal/uart_periph.h"
#include "soc/uart_pins.h"
#include "soc/io_mux_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/rtc.h"
#include "hal/gpio_ll.h"
#include "hal/uart_ll.h"
#if CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/usb/cdc_acm.h"
#include "esp32s2/rom/usb/usb_common.h"
#endif
#if CONFIG_ESP_CONSOLE_USB_CDC
#include "hal/usb_wrap_ll.h"
#endif
#include "esp_rom_gpio.h"
#include "esp_rom_serial_output.h"
#include "esp_rom_sys.h"
#include "esp_rom_caps.h"

static void __attribute__((unused)) release_default_console_io(void)
{
    // Default console is UART0 with TX and RX on their IOMUX pins
    gpio_ll_output_disable(&GPIO, U0TXD_GPIO_NUM);
    gpio_ll_func_sel(&GPIO, U0TXD_GPIO_NUM, PIN_FUNC_GPIO); // Set TX pin to GPIO function to truly disable output
    esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT, UART_PERIPH_SIGNAL(UART_NUM_0, SOC_UART_PERIPH_SIGNAL_RX), 0);
}

#ifdef CONFIG_ESP_CONSOLE_NONE
void bootloader_console_init(void)
{
    // Wait for UART FIFO to be empty.
    esp_rom_output_tx_wait_idle(0);
    release_default_console_io();

    esp_rom_install_channel_putc(1, NULL);
    esp_rom_install_channel_putc(2, NULL);
}
#endif // CONFIG_ESP_CONSOLE_NONE

#ifdef CONFIG_ESP_CONSOLE_UART
void bootloader_console_init(void)
{
    const int uart_num = CONFIG_ESP_CONSOLE_UART_NUM;
    int __DECLARE_RCC_ATOMIC_ENV __attribute__ ((unused)); // To avoid build errors/warnings about __DECLARE_RCC_ATOMIC_ENV

    // Install rom uart printf as console.
    esp_rom_install_uart_printf();

    // Wait for UART FIFO to be empty.
    esp_rom_output_tx_wait_idle(0);

#if CONFIG_ESP_CONSOLE_UART_CUSTOM
    // Some constants to make the following code less upper-case
    const int uart_tx_gpio = (CONFIG_ESP_CONSOLE_UART_TX_GPIO >= 0) ? CONFIG_ESP_CONSOLE_UART_TX_GPIO : U0TXD_GPIO_NUM;
    const int uart_rx_gpio = (CONFIG_ESP_CONSOLE_UART_RX_GPIO >= 0) ? CONFIG_ESP_CONSOLE_UART_RX_GPIO : U0RXD_GPIO_NUM;

    // Switch to the new UART (this just changes UART number used for esp_rom_printf in ROM code).
    esp_rom_output_set_as_console(uart_num);

    // If console is attached to UART1 or if non-default pins are used,
    // need to reconfigure pins using GPIO matrix
    if (uart_num != 0 ||
            uart_tx_gpio != U0TXD_GPIO_NUM ||
            uart_rx_gpio != U0RXD_GPIO_NUM) {
        release_default_console_io();
        // Route GPIO signals to/from pins
        const uint32_t tx_idx = UART_PERIPH_SIGNAL(uart_num, SOC_UART_PERIPH_SIGNAL_TX);
        const uint32_t rx_idx = UART_PERIPH_SIGNAL(uart_num, SOC_UART_PERIPH_SIGNAL_RX);
        gpio_ll_func_sel(&GPIO, uart_rx_gpio, PIN_FUNC_GPIO);
        gpio_ll_input_enable(&GPIO, uart_rx_gpio);
        esp_rom_gpio_pad_pullup_only(uart_rx_gpio);
        esp_rom_gpio_connect_out_signal(uart_tx_gpio, tx_idx, 0, 0);
        esp_rom_gpio_connect_in_signal(uart_rx_gpio, rx_idx, 0);
        gpio_ll_func_sel(&GPIO, uart_tx_gpio, PIN_FUNC_GPIO);
        // Enable the peripheral
        uart_ll_enable_bus_clock(uart_num, true);
        uart_ll_reset_register(uart_num);
        // Set clock source
#if SOC_UART_SUPPORT_XTAL_CLK
        uart_ll_set_sclk(UART_LL_GET_HW(uart_num), (soc_module_clk_t)UART_SCLK_XTAL);
#else
        uart_ll_set_sclk(UART_LL_GET_HW(uart_num), (soc_module_clk_t)UART_SCLK_APB);
#endif
        // Reset TX and RX FIFOs
        uart_ll_txfifo_rst(UART_LL_GET_HW(uart_num));
        uart_ll_rxfifo_rst(UART_LL_GET_HW(uart_num));
    }
#endif // CONFIG_ESP_CONSOLE_UART_CUSTOM

    // Set configured UART console baud rate
    uint32_t clock_hz = rtc_clk_apb_freq_get();
#if ESP_ROM_UART_CLK_IS_XTAL
    clock_hz = (uint32_t)rtc_clk_xtal_freq_get() * MHZ; // From esp32-s3 on, UART clk source is selected to XTAL in ROM
#endif
    _uart_ll_set_baudrate(UART_LL_GET_HW(uart_num), CONFIG_ESP_CONSOLE_UART_BAUDRATE, clock_hz);
}
#endif // CONFIG_ESP_CONSOLE_UART

#ifdef CONFIG_ESP_CONSOLE_USB_CDC
/* Buffer for CDC data structures. No RX buffer allocated. */
static char s_usb_cdc_buf[ESP_ROM_CDC_ACM_WORK_BUF_MIN];

void bootloader_console_init(void)
{
    // Wait for UART FIFO to be empty.
    esp_rom_output_tx_wait_idle(0);
    release_default_console_io();

#ifdef CONFIG_IDF_TARGET_ESP32S2
    /* ESP32-S2 specific patch to set the correct serial number in the descriptor.
     * Later chips don't need this.
     */
    rom_usb_cdc_set_descriptor_patch();
#endif

    esp_rom_output_usb_acm_init(s_usb_cdc_buf, sizeof(s_usb_cdc_buf));
    esp_rom_output_set_as_console(ESP_ROM_USB_OTG_NUM);
    esp_rom_install_channel_putc(1, bootloader_console_write_char_usb);
    // Ensure that the USB FSLS PHY is mapped to the USB WRAP
    usb_wrap_ll_phy_enable_pad(&USB_WRAP, true);
    usb_wrap_ll_phy_enable_external(&USB_WRAP, false);
}
#endif //CONFIG_ESP_CONSOLE_USB_CDC

#ifdef CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
void bootloader_console_init(void)
{
    // Wait for UART FIFO to be empty.
    esp_rom_output_tx_wait_idle(0);
    release_default_console_io();

    esp_rom_output_switch_buffer(ESP_ROM_USB_SERIAL_DEVICE_NUM);

    /* Switch console channel to avoid output on UART and allow  */
    esp_rom_output_set_as_console(ESP_ROM_USB_SERIAL_DEVICE_NUM);

    /* ROM printf by default also prints to USB-Serial-JTAG on channel 2
       need to disable to not print twice */
    esp_rom_install_channel_putc(2, NULL);
}
#endif //CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
