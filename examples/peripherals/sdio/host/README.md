| Supported Targets | ESP32 | ESP32-S3 |
| ----------------- | ----- | -------- |


## Troubleshooting

### Getting the following error

> `E (2562) sdmmc_io: sdmmc_io_read_byte: sdmmc_io_rw_direct (read 0x2) returned 0x107`

Kindly activate the debug log from `idf.py menuconfig -> Component config -> Log output -> Default log verbosity` option and verify the card's IO type status. If the card does not possess IO capabilities, it will encounter issues while attempting to read IO buffers.

To identify the card type, please inspect the following lines in the debug log.

```
D (659) sdmmc_io: sdmmc_init_io: io_send_op_cond (1) returned 0x106; not IO card
D (729) sdmmc_init: sdmmc_card_init: card type is SD
```

See README.md in the parent folder
