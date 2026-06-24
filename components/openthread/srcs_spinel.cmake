# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# Source files for SPINEL_ONLY mode.
# Minimal build for host-side spinel communication.
# Uses explicit whitelist for clarity.

set(private_include_dirs
    "private_include"
    "openthread/src"
    "openthread/src/core"
    "openthread/src/include"
    "openthread/src/lib"
    "openthread/src/lib/hdlc"
    "openthread/src/lib/spinel"
)

set(spinel_srcs
    # ESP spinel sources
    src/spinel/esp_radio_spinel.cpp
    src/spinel/esp_radio_spinel_uart_interface.cpp
    # ESP port (minimal)
    src/port/esp_openthread_alarm.c
    src/port/esp_openthread_logging.c
    # OpenThread spinel library
    openthread/src/lib/spinel/spinel.c
    openthread/src/lib/spinel/spinel_buffer.cpp
    openthread/src/lib/spinel/spinel_decoder.cpp
    openthread/src/lib/spinel/spinel_encoder.cpp
    openthread/src/lib/spinel/spinel_helper.cpp
    openthread/src/lib/spinel/radio_spinel.cpp
    openthread/src/lib/spinel/spinel_driver.cpp
    openthread/src/lib/spinel/logger.cpp
    # OpenThread HDLC library
    openthread/src/lib/hdlc/hdlc.cpp
    # OpenThread platform library
    openthread/src/lib/platform/exit_code.c
    # OpenThread core (minimal)
    openthread/src/core/api/error_api.cpp
    openthread/src/core/api/logging_api.cpp
    openthread/src/core/common/error.cpp
    openthread/src/core/common/log.cpp
    openthread/src/core/common/string.cpp
    openthread/src/core/mac/mac_frame.cpp
)
