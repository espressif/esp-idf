| Supported Targets | ESP32 | ESP32-P4 | ESP32-S31 |
| ----------------- | ----- | -------- | --------- |

# Internal EMAC Test

Tests for the internal ESP Ethernet MAC (DMA, address filter, RMII clock out, Light-sleep retention, Wake-on-LAN).

This app does **not** use `ethernet_init` or `eth_test_app`. It initializes the internal EMAC and a generic IEEE 802.3 PHY locally so EMAC features (including Light-sleep) can be tested without waiting for those components to expose the same options.

Generic Ethernet driver tests live in `../test_app_eth`.

## Local configuration

Board and Light-sleep options are in `main/Kconfig.projbuild`:

- `EMAC_TEST_PHY_RST_GPIO` / `EMAC_TEST_PHY_HOLD_RESET_GPIO`
- `EMAC_TEST_SLEEP_MODE_*` (no sleep, Wake-on-LAN, allow power down)
- `EMAC_TEST_RMII_CLK_OUTPUT` and related GPIOs (for CLK_OUT tests)

`ETH_ESP32_EMAC_DEFAULT_CONFIG()` supplies the default SMI/data-plane GPIOs per target.
