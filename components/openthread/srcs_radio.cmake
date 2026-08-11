# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# Source files for RCP (Radio Co-Processor) mode.
# Uses explicit whitelist to avoid including non-RCP-compatible files.
# This prevents new OpenThread files from breaking RCP builds.

set(rcp_srcs
    # OpenThread core - RADIO_COMMON_SOURCES from openthread/src/core/CMakeLists.txt
    # No need for crypto/crypto_platform_mbedtls.cpp because we use esp-idf implementation
    openthread/src/core/api/diags_api.cpp
    openthread/src/core/api/error_api.cpp
    openthread/src/core/api/instance_api.cpp
    openthread/src/core/api/link_raw_api.cpp
    openthread/src/core/api/logging_api.cpp
    openthread/src/core/api/random_noncrypto_api.cpp
    openthread/src/core/api/tasklet_api.cpp
    openthread/src/core/common/binary_search.cpp
    openthread/src/core/common/error.cpp
    openthread/src/core/common/frame_builder.cpp
    openthread/src/core/common/log.cpp
    openthread/src/core/common/random.cpp
    openthread/src/core/common/string.cpp
    openthread/src/core/common/tasklet.cpp
    openthread/src/core/common/timer.cpp
    openthread/src/core/common/uptime.cpp
    openthread/src/core/crypto/aes_ccm.cpp
    openthread/src/core/crypto/aes_ecb.cpp
    openthread/src/core/crypto/crypto_platform_psa.cpp
    openthread/src/core/crypto/storage.cpp
    openthread/src/core/diags/factory_diags.cpp
    openthread/src/core/instance/instance.cpp
    openthread/src/core/mac/link_raw.cpp
    openthread/src/core/mac/mac_frame.cpp
    openthread/src/core/mac/mac_header_ie.cpp
    openthread/src/core/mac/mac_types.cpp
    openthread/src/core/mac/sub_mac.cpp
    openthread/src/core/mac/sub_mac_callbacks.cpp
    openthread/src/core/mac/sub_mac_csl_receiver.cpp
    openthread/src/core/mac/sub_mac_wed.cpp
    openthread/src/core/radio/radio.cpp
    openthread/src/core/radio/radio_callbacks.cpp
    openthread/src/core/radio/radio_platform.cpp
    openthread/src/core/radio/radio_types.cpp
    openthread/src/core/thread/link_quality.cpp
    openthread/src/core/utils/otns.cpp
    openthread/src/core/utils/parse_cmdline.cpp
    openthread/src/core/utils/power_calibration.cpp
    # Platform utils
    openthread/examples/platforms/utils/mac_frame.cpp
    # HDLC library
    openthread/src/lib/hdlc/hdlc.cpp
    # Spinel library
    openthread/src/lib/spinel/spinel.c
    openthread/src/lib/spinel/spinel_buffer.cpp
    openthread/src/lib/spinel/spinel_decoder.cpp
    openthread/src/lib/spinel/spinel_encoder.cpp
    # NCP
    openthread/src/ncp/changed_props_set.cpp
    openthread/src/ncp/ncp_base.cpp
    openthread/src/ncp/ncp_base_dispatcher.cpp
    openthread/src/ncp/ncp_base_radio.cpp
    openthread/examples/apps/ncp/ncp.c
    # ESP port (logging/misc/settings stay at the original path)
    src/port/esp_openthread_logging.c
    src/port/esp_openthread_misc.c
    src/port/esp_openthread_settings.c
    # ESP sources
    src/esp_openthread.cpp
    src/esp_openthread_lock.c
    src/esp_openthread_platform.cpp
    src/esp_openthread_task_queue.c
)

# Single-instance files stay at the original src/ paths. Multipan RCP swaps in
# radio/alarm/instance under src/multiple_instances/. NCP is shared in src/ncp/.
if(CONFIG_OPENTHREAD_MULTIPAN_RCP_ENABLE)
    set(OT_PORT_DIR "src/multiple_instances/port")
    set(OT_INSTANCE_SRC "src/multiple_instances/esp_openthread_instance.cpp")
else()
    set(OT_PORT_DIR "src/port")
    set(OT_INSTANCE_SRC "src/esp_openthread_instance.cpp")
endif()

list(APPEND rcp_srcs
    ${OT_PORT_DIR}/esp_openthread_alarm.c
    ${OT_PORT_DIR}/esp_openthread_radio.c
    ${OT_INSTANCE_SRC}
    src/ncp/esp_openthread_ncp.cpp
)

# Conditional sources based on host connection mode
if(CONFIG_OPENTHREAD_RCP_UART OR CONFIG_OPENTHREAD_RCP_USB_SERIAL_JTAG)
    list(APPEND rcp_srcs
        openthread/src/ncp/ncp_hdlc.cpp
        src/port/esp_openthread_uart.c)
    if(CONFIG_OPENTHREAD_NCP_VENDOR_HOOK)
        list(APPEND rcp_srcs src/ncp/esp_openthread_ncp_hdlc.cpp)
    endif()
elseif(CONFIG_OPENTHREAD_RCP_SPI)
    list(APPEND rcp_srcs
        openthread/src/ncp/ncp_spi.cpp
        src/port/esp_openthread_spi_slave.c)
    # Multipan SPI NCP is not supported; the compile-time check lives in
    # src/ncp/esp_openthread_ncp.cpp.
    if(CONFIG_OPENTHREAD_NCP_VENDOR_HOOK AND NOT CONFIG_OPENTHREAD_MULTIPAN_RCP_ENABLE)
        list(APPEND rcp_srcs src/ncp/esp_openthread_ncp_spi.cpp)
    endif()
elseif(CONFIG_OPENTHREAD_RCP_CUSTOM)
    list(APPEND rcp_srcs
        openthread/src/ncp/ncp_hdlc.cpp
        src/port/esp_openthread_transport_rcp.c)
    if(CONFIG_OPENTHREAD_NCP_VENDOR_HOOK)
        list(APPEND rcp_srcs src/ncp/esp_openthread_ncp_hdlc.cpp)
    endif()
endif()

if(CONFIG_OPENTHREAD_NCP_VENDOR_HOOK AND CONFIG_OPENTHREAD_RCP_SPINEL_CONSOLE)
    list(APPEND rcp_srcs src/ncp/esp_openthread_ncp_console.cpp)
endif()
