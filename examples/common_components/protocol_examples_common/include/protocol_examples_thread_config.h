/*
 * Thread configurations for protocol examples
 *
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#include <sdkconfig.h>

#include <esp_openthread_types.h>

#ifdef CONFIG_OPENTHREAD_RADIO_NATIVE
#define ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG()                                  \
  {                                                                            \
      .radio_mode = RADIO_MODE_NATIVE,                                         \
  }

#elif defined(CONFIG_OPENTHREAD_RADIO_SPINEL_UART)
#define ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG()                                  \
  {                                                                            \
      .radio_mode = RADIO_MODE_UART_RCP,                                       \
      .radio_uart_config =                                                     \
          {                                                                    \
              .port = CONFIG_EXAMPLE_THREAD_UART_PORT,                         \
              .uart_config =                                                   \
                  {                                                            \
                      .baud_rate = CONFIG_EXAMPLE_THREAD_UART_BAUD,            \
                      .data_bits = UART_DATA_8_BITS,                           \
                      .parity = UART_PARITY_DISABLE,                           \
                      .stop_bits = UART_STOP_BITS_1,                           \
                      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,                   \
                      .rx_flow_ctrl_thresh = 0,                                \
                      .source_clk = UART_SCLK_DEFAULT,                         \
                  },                                                           \
              .rx_pin = CONFIG_EXAMPLE_THREAD_UART_RX_PIN,                     \
              .tx_pin = CONFIG_EXAMPLE_THREAD_UART_TX_PIN,                     \
          },                                                                   \
  }
#elif defined(CONFIG_OPENTHREAD_RADIO_SPINEL_SPI)
#define ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG()                                  \
  {                                                                            \
      .radio_mode = RADIO_MODE_SPI_RCP,                                        \
      .radio_spi_config =                                                      \
          {                                                                    \
              .host_device = SPI2_HOST,                                        \
              .dma_channel = 2,                                                \
              .spi_interface =                                                 \
                  {                                                            \
                      .mosi_io_num = CONFIG_EXAMPLE_THREAD_SPI_MOSI_PIN,       \
                      .miso_io_num = CONFIG_EXAMPLE_THREAD_SPI_MISO_PIN,       \
                      .sclk_io_num = CONFIG_EXAMPLE_THREAD_SPI_SCLK_PIN,       \
                      .quadwp_io_num = -1,                                     \
                      .quadhd_io_num = -1,                                     \
                  },                                                           \
              .spi_device =                                                    \
                  {                                                            \
                      .cs_ena_pretrans = 2,                                    \
                      .input_delay_ns = 100,                                   \
                      .mode = 0,                                               \
                      .clock_speed_hz = 2500 * 1000,                           \
                      .spics_io_num = CONFIG_EXAMPLE_THREAD_SPI_CS_PIN,        \
                      .queue_size = 5,                                         \
                  },                                                           \
              .intr_pin = CONFIG_EXAMPLE_THREAD_SPI_INTR_PIN,                  \
          },                                                                   \
  }
#else
#define ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG()                                  \
  {                                                                            \
      .radio_mode = RADIO_MODE_TREL,                                           \
  }
#endif

#if CONFIG_OPENTHREAD_CONSOLE_TYPE_UART
#define ESP_OPENTHREAD_DEFAULT_HOST_CONFIG()                                   \
  {                                                                            \
      .host_connection_mode = HOST_CONNECTION_MODE_CLI_UART,                   \
      .host_uart_config =                                                      \
          {                                                                    \
              .port = 0,                                                       \
              .uart_config =                                                   \
                  {                                                            \
                      .baud_rate = 115200,                                     \
                      .data_bits = UART_DATA_8_BITS,                           \
                      .parity = UART_PARITY_DISABLE,                           \
                      .stop_bits = UART_STOP_BITS_1,                           \
                      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,                   \
                      .rx_flow_ctrl_thresh = 0,                                \
                      .source_clk = UART_SCLK_DEFAULT,                         \
                  },                                                           \
              .rx_pin = UART_PIN_NO_CHANGE,                                    \
              .tx_pin = UART_PIN_NO_CHANGE,                                    \
          },                                                                   \
  }
#elif CONFIG_OPENTHREAD_CONSOLE_TYPE_USB_SERIAL_JTAG
#define ESP_OPENTHREAD_DEFAULT_HOST_CONFIG()                                   \
  {                                                                            \
      .host_connection_mode = HOST_CONNECTION_MODE_CLI_USB,                    \
      .host_usb_config = USB_SERIAL_JTAG_DRIVER_CONFIG_DEFAULT(),              \
  }
#else
#define ESP_OPENTHREAD_DEFAULT_HOST_CONFIG()                                   \
  {                                                                            \
      .host_connection_mode = HOST_CONNECTION_MODE_NONE,                       \
  }
#endif

#define ESP_OPENTHREAD_DEFAULT_PORT_CONFIG()                                   \
  {                                                                            \
      .storage_partition_name = "nvs",                                         \
      .netif_queue_size = 10,                                                  \
      .task_queue_size = 10,                                                   \
  }
