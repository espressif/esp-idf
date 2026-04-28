# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# Source files for FTD (Full Thread Device) and MTD (Minimal Thread Device) modes.
# Uses directory-based collection with exclusions.

set(src_dirs
    "src"
    "src/port"
    "src/spinel"
    "openthread/examples/platforms/utils"
    "openthread/src/core/api"
    "openthread/src/core/common"
    "openthread/src/core/crypto"
    "openthread/src/core/diags"
    "openthread/src/core/instance"
    "openthread/src/core/mac"
    "openthread/src/core/radio"
    "openthread/src/core/thread"
    "openthread/src/core/utils"
    "openthread/src/lib/hdlc"
    "openthread/src/lib/spinel"
    "openthread/src/core/backbone_router"
    "openthread/src/core/coap"
    "openthread/src/core/meshcop"
    "openthread/src/core/net"
    "openthread/src/lib/platform"
)

set(exclude_srcs
    "openthread/examples/platforms/utils/logging_rtt.c"
    "openthread/examples/platforms/utils/soft_source_match_table.c"
    "openthread/src/core/instance/extension_example.cpp"
    "openthread/src/core/crypto/crypto_platform_mbedtls.cpp"
    "openthread/src/core/api/random_crypto_api.cpp"
)

# CLI sources
if(CONFIG_OPENTHREAD_CLI)
    list(APPEND src_dirs
        "openthread/examples/apps/cli"
        "openthread/src/cli")
    list(APPEND exclude_srcs
        "openthread/examples/apps/cli/main.c")
endif()

# Radio interface exclusions
if(CONFIG_OPENTHREAD_RADIO_NATIVE)
    list(APPEND exclude_srcs
        "src/port/esp_openthread_radio_spinel.cpp"
        "src/port/esp_spi_spinel_interface.cpp"
        "src/spinel/esp_radio_spinel.cpp"
        "src/spinel/esp_radio_spinel_uart_interface.cpp")
elseif(CONFIG_OPENTHREAD_RADIO_SPINEL_UART OR CONFIG_OPENTHREAD_RADIO_SPINEL_SPI)
    list(APPEND exclude_srcs
        "src/port/esp_openthread_radio.c"
        "src/port/esp_openthread_sleep.c"
        "src/spinel/esp_radio_spinel.cpp")
    if(CONFIG_OPENTHREAD_RADIO_SPINEL_SPI)
        list(APPEND exclude_srcs "src/spinel/esp_radio_spinel_uart_interface.cpp")
    endif()
elseif(CONFIG_OPENTHREAD_RADIO_154_NONE)
    list(APPEND exclude_srcs
        "src/port/esp_openthread_radio_spinel.cpp"
        "src/port/esp_spi_spinel_interface.cpp"
        "src/spinel/esp_radio_spinel.cpp"
        "src/spinel/esp_radio_spinel_uart_interface.cpp"
        "src/port/esp_openthread_radio.c"
        "src/port/esp_openthread_sleep.c")
endif()

# TREL
if(NOT CONFIG_OPENTHREAD_RADIO_TREL)
    list(APPEND exclude_srcs
        "src/port/esp_openthread_trel.c")
endif()

# Border router
if(CONFIG_OPENTHREAD_BORDER_ROUTER)
    list(APPEND src_dirs
        "openthread/src/core/border_router")
endif()

# FTD-specific
if(CONFIG_OPENTHREAD_FTD)
    set_source_files_properties("openthread/src/core/net/srp_server.cpp"
        PROPERTIES COMPILE_FLAGS
        -Wno-maybe-uninitialized)
endif()

# NCP vendor hook
if(CONFIG_OPENTHREAD_NCP_VENDOR_HOOK)
    list(APPEND src_dirs
        "src/ncp")
    if(CONFIG_OPENTHREAD_RCP_UART OR CONFIG_OPENTHREAD_RCP_USB_SERIAL_JTAG)
        list(APPEND exclude_srcs
            "src/ncp/esp_openthread_ncp_spi.cpp")
    elseif(CONFIG_OPENTHREAD_RCP_SPI)
        list(APPEND exclude_srcs
            "src/ncp/esp_openthread_ncp_hdlc.cpp")
    endif()
endif()

# Optional features
if(NOT CONFIG_OPENTHREAD_DNS64_CLIENT)
    list(APPEND exclude_srcs
        "src/esp_openthread_dns64.c")
endif()

if(NOT CONFIG_OPENTHREAD_PLATFORM_NETIF)
    list(APPEND exclude_srcs
        "src/esp_openthread_lwip_netif.c"
        "src/esp_openthread_netif_glue.c")
endif()

if(NOT CONFIG_FREERTOS_USE_TICKLESS_IDLE)
    list(APPEND exclude_srcs
        "src/port/esp_openthread_sleep.c")
endif()

if(NOT CONFIG_OPENTHREAD_PLATFORM_MSGPOOL_MANAGEMENT)
    list(APPEND exclude_srcs
        "src/port/esp_openthread_messagepool.c")
endif()

if(NOT CONFIG_OPENTHREAD_RCP_SPINEL_CONSOLE)
    list(APPEND exclude_srcs
        "src/ncp/esp_openthread_ncp_console.cpp")
endif()
