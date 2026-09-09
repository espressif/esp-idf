| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- |

This project tests the multiple PHY init data bin support, i.e. `CONFIG_ESP_PHY_MULTIPLE_INIT_DATA_BIN`.

The tests cover:

- the layout, the checksums and the certified init data types of `phy_multiple_init_data.bin`
- loading the bin through `esp_phy_get_init_data()`
- switching the PHY init data type from a country code, both through `esp_phy_update_country_info()`
  and through `esp_wifi_set_country_code()`
- the fallback to the default init data for countries without a certified init data

Two configurations are tested:

| Config                             | Where the bin lives                                                      |
| ---------------------------------- | ------------------------------------------------------------------------ |
| `phy_multiple_init_data`           | flashed into the `phy` data partition                                    |
| `phy_multiple_init_data_embed`     | embedded into the application binary (`..._MULTIPLE_INIT_DATA_BIN_EMBED`) |

Which of the two paths was taken is only observable in the log, so `app_main` loads the bin
once before starting the unity runner.

Run the tests with:

```bash
idf.py set-target <target>
idf.py -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.phy_multiple_init_data" build flash monitor
```
