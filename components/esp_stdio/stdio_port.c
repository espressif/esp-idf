/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_stdio.h"

#if CONFIG_IDF_TARGET_LINUX
#include "esp_stdio_linux.h"
#elif CONFIG_VFS_SUPPORT_IO  // VFS required for all device drivers below

#include "esp_vfs_common.h"

#if CONFIG_ESP_CONSOLE_USB_CDC
#include "esp_vfs_cdcacm.h"
#include "esp_private/esp_vfs_cdcacm.h"
#endif // CONFIG_ESP_CONSOLE_USB_CDC

#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG_ENABLED
#include "driver/esp_private/usb_serial_jtag_vfs.h"
#include "driver/usb_serial_jtag_vfs.h"
#endif // #if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG_ENABLED

#if CONFIG_ESP_CONSOLE_UART
#include "driver/esp_private/uart_vfs.h"
#include "driver/uart_vfs.h"
#endif // CONFIG_ESP_CONSOLE_UART

#endif // CONFIG_VFS_SUPPORT_IO

esp_err_t esp_stdio_install_io_driver(void)
{
    esp_err_t ret = ESP_FAIL;

#if CONFIG_IDF_TARGET_LINUX
    linux_port_config_t config = ESP_CONSOLE_DEV_LINUX_CONFIG_DEFAULT();
    ret = linux_vfs_dev_port_init(&config);
#elif CONFIG_VFS_SUPPORT_IO

    /* - set rx_mode to ESP_LINE_ENDINGS_CRLF as minicom, screen, idf_monitor
     * send CR when ENTER key is pressed.
     * - set tx_mode to move the caret to the beginning of the next line on '\n' */

#if CONFIG_ESP_CONSOLE_UART
    esp_console_dev_uart_config_t config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ret = uart_vfs_dev_port_init(&config, ESP_LINE_ENDINGS_CR, ESP_LINE_ENDINGS_CRLF);
#elif CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    esp_console_dev_usb_serial_jtag_config_t config = ESP_CONSOLE_DEV_USB_SERIAL_JTAG_CONFIG_DEFAULT();
    ret = usb_serial_jtag_vfs_dev_port_init(&config, ESP_LINE_ENDINGS_CR, ESP_LINE_ENDINGS_CRLF);
#elif CONFIG_ESP_CONSOLE_USB_CDC
    esp_console_dev_usb_cdc_config_t config = ESP_CONSOLE_DEV_CDC_CONFIG_DEFAULT();
    ret = cdcacm_vfs_dev_port_init(&config, ESP_LINE_ENDINGS_CR, ESP_LINE_ENDINGS_CRLF);
#endif
#else
    ret = ESP_ERR_NOT_SUPPORTED;
#endif
    return ret;
}

void esp_stdio_uninstall_io_driver(void)
{
#if CONFIG_IDF_TARGET_LINUX
    linux_port_config_t config = ESP_CONSOLE_DEV_LINUX_CONFIG_DEFAULT();
    linux_vfs_dev_port_deinit(&config);
#elif CONFIG_VFS_SUPPORT_IO
#if CONFIG_ESP_CONSOLE_UART
    esp_console_dev_uart_config_t config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    uart_vfs_dev_port_deinit(&config);
#elif CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    esp_console_dev_usb_serial_jtag_config_t config = ESP_CONSOLE_DEV_USB_SERIAL_JTAG_CONFIG_DEFAULT();
    usb_serial_jtag_vfs_dev_port_deinit(&config);
#elif CONFIG_ESP_CONSOLE_USB_CDC
    esp_console_dev_usb_cdc_config_t config = ESP_CONSOLE_DEV_CDC_CONFIG_DEFAULT();
    cdcacm_vfs_dev_port_deinit(&config);
#endif
#endif
}
