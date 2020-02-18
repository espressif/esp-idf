#pragma once

int test_tp_print_to_scope(float *data, unsigned char channel_num);
esp_err_t test_tp_scope_debug_init(uint8_t uart_num, int tx_io_num, int rx_io_num, int baud_rate);