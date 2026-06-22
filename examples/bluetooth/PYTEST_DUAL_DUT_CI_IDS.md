# Dual-DUT Pytest CI Example IDs

Dual-DUT CI tests pair devices by advertising name. When CI config is enabled, the device name format is:

```
BE<EXAMPLE_ID>_<PIPELINE_ID>_<CHIP_ID>
```

- **Example ID** — `CONFIG_EXAMPLE_CI_ID`; distinguishes test scenarios so unrelated examples on the same pipeline do not connect by mistake.
- **Pipeline ID** — `CONFIG_EXAMPLE_CI_PIPELINE_ID` (in CI: `${CI_PIPELINE_ID}`).
- **Chip ID** — `CONFIG_IDF_FIRMWARE_CHIP_ID`; the suffix keeps the same Example ID distinct across chips.

CI device naming is disabled when `CONFIG_EXAMPLE_CI_PIPELINE_ID=0` (Kconfig default).

---

## Why register IDs

pytest **randomly picks two boards** from the test environment to flash and run. MAC addresses are unknown beforehand, so dual-DUT tests can only find each other by **device name**. Names must be unique, or boards may pair with the wrong peer.

`CONFIG_EXAMPLE_CI_ID` and `CONFIG_EXAMPLE_CI_PIPELINE_ID` (plus chip ID) form `BE<EXAMPLE_ID>_<PIPELINE_ID>_<CHIP_ID>`, so different pipelines, chips, and examples all use distinct names during testing.

CI runners often run many dual-DUT tests in parallel. Registering allocated Example IDs in this file also avoids:

- Reusing an existing ID for a new test and pairing with the wrong peer;
- ID conflicts when several developers add tests at the same time.

Before adding a new dual-DUT pytest, take **max allocated ID + 1** from the table below and append a row before merging.

---

## Allocated IDs

Paths are relative to `examples/bluetooth/`. Bluedroid uses `esp_bluedroid_get_example_name()`; NimBLE uses `esp_ble_cte_get_example_name()` and similar helpers.

| ID | Example pair (server/adv → client/sync) | Pytest |
|----|----------------------------------------|--------|
| 1 | `bluedroid/ble/ble_throughput/throughput_server` / `throughput_client` (`notify`) | `ble/ble_throughput/pytest_ble_throughput_test.py` |
| 2 | same pair (`write`) | same |
| 3 | `bluedroid/ble/gatt_server` / `gatt_client` (`name`) | `bluedroid/ble/pytest_ble_test.py` |
| 4 | same pair (`cfg_test`) | same |
| 5 | `bluedroid/ble/gatt_security_server` / `gatt_security_client` (`name`) | same |
| 6 | `bluedroid/ble_50/ble50_security_server` / `ble50_security_client` (`name`) | `bluedroid/ble_50/pytest_ble50_test.py` |
| 7 | same pair (`cfg_test`) | same |
| 8 | `bluedroid/ble_50/periodic_adv` / `periodic_sync` | same |
| 9 | `bluedroid/ble/gatt_server` / `gatt_client` (`smp_off`) | `bluedroid/ble/pytest_ble_test.py` |
| 10 | same pair (`min_bin`) | same |
| 11 | same pair (`log_off`) | same |
| 12 | `bluedroid/ble/gatt_security_server` / `gatt_security_client` (`min_bin`) | same |
| 13 | same pair (`log_off`) | same |
| 14 | `bluedroid/ble_50/ble50_security_server` / `ble50_security_client` (`min_bin`) | — (sdkconfig only) |
| 15 | same pair (`log_off`) | — (sdkconfig only) |
| 16 | `bluedroid/ble/ble_spp_server` / `ble_spp_client` | `bluedroid/ble/pytest_ble_test.py` |
| 17 | `bluedroid/ble_50/ble_power_control_peripheral` / `ble_power_control_central` | `bluedroid/ble_50/pytest_ble50_test.py` |
| 18 | `bluedroid/ble_50/ble_conn_subrating_peripheral` / `ble_conn_subrating_central` | same |
| 19 | `bluedroid/ble_50/ble_periodic_adv_with_cte` / `ble_periodic_sync_with_cte` | same |
| 20 | `bluedroid/ble_50/ble_connection_peripheral_with_cte` / `ble_connection_central_with_cte` | same |
| 21 | `bluedroid/ble_50/ble_pawr_advertiser` / `ble_pawr_synchronizer` (`name`) | same |
| 22 | `bluedroid/ble_50/ble_pawr_advertiser_conn` / `ble_pawr_synchronizer` (`conn_name`) | same |
| 23 | `bluedroid/ble_50/ble50_throughput/throughput_server` / `throughput_client` (`notify`) | `bluedroid/ble_50/ble50_throughput/pytest_ble50_throughput_test.py` |
| 24 | same pair (`write`) | same |
| 25 | `nimble/ble_cte/ble_periodic_adv_with_cte` / `ble_periodic_sync_with_cte` | `nimble/pytest_nimble_test.py` |

**Next available Example ID: 26**

---

## Notes

- Both boards in a dual-DUT test must use the **same** `CONFIG_EXAMPLE_CI_ID` and the same `sdkconfig.ci.<config>` basename.
- Set `CONFIG_EXAMPLE_CI_PIPELINE_ID=${CI_PIPELINE_ID}` in every CI sdkconfig; run `export CI_PIPELINE_ID=...` locally before `idf-ci build` and pytest, using the **same** value for both commands.
- Different `sdkconfig.ci.*` files for the **same** example must use **different Example IDs**, e.g. `gatt_server` / `gatt_client`: `name` (ID 3), `min_bin` (ID 10), `log_off` (ID 11), `smp_off` (ID 9)—do not reuse the default `name` ID.
- Some examples may pair with **more than one peer example**; use separate `sdkconfig.ci.*` configs and a distinct Example ID for each pairing so device names stay unique. For example, `ble_pawr_synchronizer` uses `name` (ID 21) with `ble_pawr_advertiser` and `conn_name` (ID 22) with `ble_pawr_advertiser_conn`.
- **Single-board tests** do not need CI device names: no `CONFIG_EXAMPLE_CI_ID` / `CONFIG_EXAMPLE_CI_PIPELINE_ID` and no row in this table. For example, `bluedroid/ble_50/multi-adv` is a single-board smoke test with no dual-DUT pairing.
