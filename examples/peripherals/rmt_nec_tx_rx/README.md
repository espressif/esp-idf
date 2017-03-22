# Example: rmt_nec_tx_rx

This example uses the remote control (RMT) peripheral to transmit and receive codes for the NEC infrared remote protocol.

Configuration (pin numbers, etc.) can be modified in top of the main/infrared_nec_main.c file.

By default, this example runs a self test which assumes the TX and RX GPIO pins are connected together. To disable this, comment RMT_RX_SELF_TEST in infrared_nec_main.c.

