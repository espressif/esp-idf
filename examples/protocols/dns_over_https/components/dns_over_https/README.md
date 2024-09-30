# DNS Over HTTPS (DoH) Component for ESP-IDF

## Overview
This component allows your ESP-IDF device to use DNS over HTTPS (DoH) for DNS resolution. By using DoH, your DNS queries are encrypted, which helps protect your privacy and prevents third parties from spying on or altering your DNS requests.

## Features
- **Secure DNS Queries:** Encrypts DNS requests using TLS for better privacy.
- **Runtime DNS Server Configuration Support:** Configure popular DNS providers like Google or Cloudflare, or set up a custom DNS server directly from your application.
- **Certificate Bundle Support (Default):** Uses an internal certificate bundle by default, simplifying the setup for common DoH servers.
- **Dynamic DNS Request Generation:** Creates DNS queries on-the-fly to handle different hostnames and types.
- **Note on Future Support for DNS over TLS (DoT):** While this component currently supports DNS over HTTPS (DoH), future updates may include support for DNS over TLS (DoT), enabling another secure and privacy-focused method for DNS resolution.

## How It Works
This component utilizes the `CONFIG_LWIP_HOOK_NETCONN_EXT_RESOLVE_CUSTOM` hook to override the core DNS functionality of LWIP and implement custom DNS over HTTPS resolution. To enable this, ensure that the configuration option `Component config → LWIP → Hooks → Netconn external resolve Hook` is set to `Custom implementation`.

Once you add this component to your project, it will replace the default LWIP DNS resolution automatically.

The component must be initialized using the `dns_over_https_init()` function. Security details necessary for server communication must be included in the configuration passed to this function.

**⚠️ Warning:** This component cannot work alongside the OpenThread component, as both components use the CONFIG_LWIP_HOOK_NETCONN_EXT_RESOLVE_CUSTOM hook.

## DNS Resolution Support
This component improves DNS resolution by combining the LWIP DNS module and DoH. 

It supports:
### Standard DNS Queries:
Uses the LWIP DNS module to resolve DNS server URLs, localhost, and IP addresses.

Note: Localhost and IP addresses are resolved locally and no actual queries are sent to the DNS server.

### DNS over HTTPS:
For any other DNS queries, it uses mbedTLS to securely send requests over HTTPS, ensuring your queries are encrypted for added privacy.

## Configuration
To set up the DNS over HTTPS component, follow these steps:

1. Go to your project directory.
2. Run `idf.py menuconfig`.
3. Look for the **Component config -> DNS-over-HTTPS Configuration** section.
4. Set the HTTP version for DNS-over-HTTPS (default: 1.1).

### Important Configuration Parameters
To enable custom DNS resolution, configure the `CONFIG_LWIP_HOOK_NETCONN_EXT_RESOLVE_CUSTOM` setting either through menuconfig by navigating to `Component config → LWIP → Hooks → Netconn external resolve Hook` and enabling it, or by adding `CONFIG_LWIP_HOOK_NETCONN_EXT_RESOLVE_CUSTOM=y` to your `sdkconfig.defaults` file to pre-set the configuration during the build process.

## Initialization Requirements
To initialize this module, make sure to call the `init_dns_over_https()` function after these initializations in your main application:

- `nvs_flash_init()`
- `esp_event_loop_create_default()`
- `esp_netif_init()`

## Example Usage
Here’s a quick example of how to use this component in your application:

```c
#include "dns_over_https.h" // Include the DoH component header

void app_main()
{
    ESP_ERROR_CHECK(esp_netif_init());  /* Initialize the network interface */
    ESP_ERROR_CHECK(esp_event_loop_create_default());   /* Create default event loop */
    esp_err_t ret = nvs_flash_init();   /* Initialize NVS */
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* Sets up periodic time updates */
    setup_periodic_time_updates();
    
    /* Initialize the DOH config */
    dns_over_https_config_t config = {
        .dns_server = "dns.google",
        .dns_service_path = "dns-query",
        /* Supply function to attach certificate bundle */
        .crt_bundle_attach = esp_crt_bundle_attach;
        /* or Supplying certificate */
        .cert_pem = server_root_cert_pem_start,
    };
    ESP_ERROR_CHECK(dns_over_https_init(&config));
    
    // Your application logic here...
}
```

**⚠️ Warning:** The default stack size may not be sufficient for the thread performing DNS resolution. To avoid potential issues, ensure that the stack size allocated to the task handling DNS resolution is increased.
