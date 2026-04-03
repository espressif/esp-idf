# esp_wifi (remote)

The `esp_wifi_remote` component provides transparent WiFi connectivity for ESP chips through external WiFi hardware while keeping the public `esp_wifi` API compatible. It routes `esp_wifi` calls from a host device to a WiFi-capable slave via RPC, enabling WiFi on chips without native WiFi or adding a second WiFi interface alongside local `esp_wifi`.

Remote connectivity is provided by backend solutions such as `esp_hosted`, `wifi_remote_over_eppp`, and `wifi_remote_over_at`.

## Directory Structure and Auto-Generation

The `esp_wifi_remote` component relies heavily on auto-generated code to maintain synchronization with the native `esp_wifi` API. The generation is handled by `scripts/generate_and_check.py`, which parses the original `esp_wifi` headers and produces the necessary remote wrappers, mocked C implementations, and Kconfig files.

### Key Directories and Files

*   **`include/`**: Contains the generated header files defining the remote API (e.g., `esp_wifi_remote_api.h`, `esp_eap_client_remote_api.h`).
*   **`include/injected/`**: Contains modified copies of the native ESP-IDF headers (like `esp_wifi.h`, `esp_mesh.h`, `esp_now.h`). The generator script copies these from the native `esp_wifi/include` directory and "injects" changes into them—specifically replacing native Kconfig macros (e.g., `CONFIG_ESP_WIFI_`, `CONFIG_SOC_WIFI_`) with remote-specific macros (e.g., `CONFIG_WIFI_RMT_`, `CONFIG_SLAVE_SOC_WIFI_`). This allows the remote component to reuse the native data structures while relying on different Kconfig selections corresponding to the slave device.
*   **`scripts/generate_and_check.py`**: The core Python script responsible for generating the remote wrappers, injected headers, Kconfig mappings, and default implementations. This script is run as part of the CI pipeline (`check_wif_remote_api` job) to ensure the generated files are always up-to-date with any changes to the native WiFi API.
*   **`esp_wifi_remote_weak.c` & `esp_eap_client_remote_weak.c`**: Auto-generated C files containing weakly linked default implementations for the remote APIs. By default, these functions log a warning and return an unsupported error (`ESP_ERR_NOT_SUPPORTED`). Backend solutions (like `esp_hosted`) provide the actual strong implementations for these functions to route the calls over RPC.
*   **`Kconfig.*.in`**: Template Kconfig files used by the generator script. They bridge standard Wi-Fi configuration selections to the slave targets, enabling the host to configure the remote WiFi hardware seamlessly
