idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

if(CONFIG_IDF_TARGET_ESP32S3)
    set(TARGET_SRC_NAME "esp32c3")
elseif(CONFIG_IDF_TARGET_ESP32C61)
    set(TARGET_SRC_NAME "esp32c6")
elseif(CONFIG_IDF_TARGET_ESP32H21)
    set(TARGET_SRC_NAME "esp32h2")
else()
    set(TARGET_SRC_NAME "${idf_target}")
endif()

set(srcs "")
set(include_dirs "")
set(priv_include_dirs "")
set(ldscripts "linker_common.lf")

# API headers that are used in the docs are also compiled
# even if CONFIG_BT_ENABLED=n as long as CONFIG_IDF_DOC_BUILD=y
if(CONFIG_IDF_DOC_BUILD OR CONFIG_BT_ENABLED)
    add_subdirectory(common)
    list(APPEND srcs ${bt_common_srcs})
    list(APPEND include_dirs ${bt_common_include_dirs})
    list(APPEND priv_include_dirs ${bt_common_priv_include_dirs})

    # Controller
    add_subdirectory(controller)
    list(APPEND srcs ${bt_ctrl_srcs})
    list(APPEND include_dirs ${bt_ctrl_include_dirs})
    set(ldscripts ${bt_ctrl_ldscripts})

    # Controller porting or porting_btdm
    if(CONFIG_SOC_ESP_NIMBLE_CONTROLLER)
        if(CONFIG_BT_DUAL_MODE_ARCH)
            add_subdirectory(porting_btdm)
        else()
            add_subdirectory(porting)
        endif()
        list(APPEND srcs ${porting_srcs})
        list(APPEND include_dirs ${porting_include_dirs})
        list(APPEND priv_include_dirs ${porting_priv_include_dirs})
    endif()

    # Host Bluedroid
    add_subdirectory(host/bluedroid)
    list(APPEND srcs ${bluedroid_host_srcs})
    list(APPEND include_dirs ${bluedroid_host_include_dirs})
    list(APPEND priv_include_dirs ${bluedroid_host_priv_include_dirs})

    # NimBLE Host
    add_subdirectory(host/nimble)
    list(APPEND srcs ${nimble_host_srcs})
    list(APPEND include_dirs ${nimble_host_include_dirs})

    # BLE Mesh
    add_subdirectory(esp_ble_mesh)
    list(APPEND srcs ${ble_mesh_srcs})
    list(APPEND include_dirs ${ble_mesh_include_dirs})

    # BLE ISO (host shim + ISO transport, reusable by BLE Audio / HID-over-ISO / etc.)
    add_subdirectory(esp_ble_iso)
    list(APPEND srcs ${ble_iso_srcs})
    list(APPEND include_dirs ${ble_iso_include_dirs})

    # BLE Audio
    add_subdirectory(esp_ble_audio)
    list(APPEND srcs ${ble_audio_srcs})
    list(APPEND include_dirs ${ble_audio_include_dirs})

    # When log compression is enabled, selected logs are replaced
    # by auto-generated macros that emit pre-encoded data.
    # This eliminates the original format strings, reducing firmware size and
    # removing runtime formatting overhead, so logs are produced faster and
    # with less system impact.
    replace_log_compression_srcs()
endif()

set(bt_priv_requires
    nvs_flash
    soc
    esp_pm
    esp_phy
    esp_coex
    mbedtls
    esp_driver_uart
    vfs
    esp_ringbuf
    esp_gdbstub
    esp_security
)

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "${include_dirs}"
                       PRIV_INCLUDE_DIRS "${priv_include_dirs}"
                       REQUIRES esp_timer esp_wifi esp_driver_spi esp_driver_gpio
                       PRIV_REQUIRES "${bt_priv_requires}"
                       LDFRAGMENTS "${ldscripts}")

idf_component_get_property(bt_component_type bt COMPONENT_TYPE)
if(bt_component_type STREQUAL "LIBRARY")
    idf_component_optional_requires(PRIVATE ble_insights)
else()
    idf_component_optional_requires(INTERFACE ble_insights)
endif()

if(CONFIG_BT_ENABLED)
    target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-implicit-fallthrough -Wno-unused-const-variable)
    target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")

    register_bt_ctrl_libs()
    set_bluedroid_host_compile_flags()
    set_nimble_host_compile_flags()
    register_ble_mesh_libs()
    register_ble_audio_libs()

    register_log_compression_dependency()
    wrap_uart_log_tx_interface()
endif()
