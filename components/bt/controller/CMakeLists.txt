# Function to register the libraries for the BT controller
function(register_bt_ctrl_libs)
    # TODO: The coex will use controller library even if CONFIG_BT_CONTROLLER_ENABLED=n which is not correct.
    if(CONFIG_IDF_TARGET_ESP32)
        add_prebuilt_library(bt_btdm_app "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/lib_esp32/esp32/libbtdm_app.a")
        target_link_libraries(${COMPONENT_LIB} PRIVATE bt_btdm_app)
        target_link_options(${COMPONENT_LIB} INTERFACE "SHELL:-u ld_include_hli_vectors_bt")
    endif()

    if(NOT CONFIG_BT_CONTROLLER_ENABLED)
        return()
    endif()

    if(CONFIG_IDF_TARGET_ESP32C3)
        if(CONFIG_BT_CTRL_RUN_IN_FLASH_ONLY)
            set(lib_name "btdm_app_flash")
        else()
            set(lib_name "btdm_app")
        endif()
        add_prebuilt_library(bt_btdm_app
                                "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/lib_esp32c3_family/esp32c3/lib${lib_name}.a")
        target_link_libraries(${COMPONENT_LIB} PRIVATE bt_btdm_app)
    elseif(CONFIG_IDF_TARGET_ESP32S3)
        if(CONFIG_BT_CTRL_RUN_IN_FLASH_ONLY)
            set(lib_name "btdm_app_flash")
        else()
            set(lib_name "btdm_app")
        endif()
        add_prebuilt_library(bt_btdm_app
                                "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/lib_esp32c3_family/esp32s3/lib${lib_name}.a")
        target_link_libraries(${COMPONENT_LIB} PRIVATE bt_btdm_app)
    elseif(NOT CONFIG_IDF_TARGET_ESP32)
        set(lib_path "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/lib_${TARGET_SRC_NAME}/${TARGET_SRC_NAME}-bt-lib")
        # BLE controller library
        if(NOT CONFIG_BT_DUAL_MODE_ARCH OR CONFIG_BT_CTRL_BLE_ENABLE)
            if(EXISTS "${lib_path}/libble_app.a")
                if(CONFIG_BT_CTRL_RUN_IN_FLASH_ONLY AND EXISTS "${lib_path}/libble_app_flash.a")
                    add_prebuilt_library(libble_app "${lib_path}/libble_app_flash.a" REQUIRES esp_phy bt)
                else()
                    add_prebuilt_library(libble_app "${lib_path}/libble_app.a" REQUIRES esp_phy bt)
                endif()
            else()
                if(CONFIG_BT_CTRL_RUN_IN_FLASH_ONLY AND EXISTS "${lib_path}/${idf_target}/libble_app_flash.a")
                    add_prebuilt_library(libble_app "${lib_path}/${idf_target}/libble_app_flash.a" REQUIRES esp_phy bt)
                else()
                    add_prebuilt_library(libble_app "${lib_path}/${idf_target}/libble_app.a" REQUIRES esp_phy bt)
                endif()
            endif()
            target_link_libraries(${COMPONENT_LIB} PRIVATE libble_app)
        endif()
        # BREDR controller library
        if(CONFIG_BT_CTRL_BREDR_ENABLE)
            if(EXISTS "${lib_path}/libbredr_app.a")
                add_prebuilt_library(libbredr_app "${lib_path}/libbredr_app.a" REQUIRES esp_phy bt)
            else()
                add_prebuilt_library(libbredr_app "${lib_path}/${idf_target}/libbredr_app.a" REQUIRES esp_phy bt)
            endif()
            target_link_libraries(${COMPONENT_LIB} PRIVATE libbredr_app)
        endif()
        # BTDM common library
        if(CONFIG_BT_DUAL_MODE_ARCH)
            if(EXISTS "${lib_path}/libbtdm_common.a")
                add_prebuilt_library(libbtdm_common "${lib_path}/libbtdm_common.a" esp_phy bt)
            else()
                add_prebuilt_library(libbtdm_common "${lib_path}/${idf_target}/libbtdm_common.a" esp_phy bt)
            endif()
            target_link_libraries(${COMPONENT_LIB} PRIVATE libbtdm_common)
        endif()
    endif()

    # Used to dump logs in the panic handler
    if(CONFIG_BT_LE_CONTROLLER_LOG_WRAP_PANIC_HANDLER_ENABLE)
        target_link_options(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=esp_panic_handler")
    endif()
endfunction()

set(bt_ctrl_srcs "" PARENT_SCOPE)
set(bt_ctrl_include_dirs "" PARENT_SCOPE)
set(bt_ctrl_ldscripts "${ldscripts}")
set(bt_ctrl_ldscripts "${bt_ctrl_ldscripts}" PARENT_SCOPE)

# API headers that are used in the docs are also compiled
# even if CONFIG_BT_ENABLED=n as long as CONFIG_IDF_DOC_BUILD=y
if(CONFIG_IDF_DOC_BUILD AND CONFIG_SOC_BT_SUPPORTED)
    get_filename_component(
        _bt_ctrl_absolute_include_dir
        "${CMAKE_CURRENT_LIST_DIR}/../include/${TARGET_SRC_NAME}/include"
        ABSOLUTE
    )
    set(bt_ctrl_include_dirs "${_bt_ctrl_absolute_include_dir}" PARENT_SCOPE)
    return()
endif()

if(NOT CONFIG_BT_CONTROLLER_ENABLED)
    return()
endif()

# Get all the .c files in the TARGET_SRC_NAME directory
file(GLOB_RECURSE _CTRL_SRCS "${TARGET_SRC_NAME}/*.c")
list(APPEND bt_ctrl_srcs ${_CTRL_SRCS})
list(APPEND bt_ctrl_include_dirs "${CMAKE_CURRENT_LIST_DIR}/../include/${TARGET_SRC_NAME}/include")

# TODO: Move this to a separate file for the controller and the host
if(CONFIG_SOC_ESP_NIMBLE_CONTROLLER AND NOT CONFIG_BT_NIMBLE_ENABLED)
    list(APPEND bt_ctrl_srcs
        "${CMAKE_CURRENT_LIST_DIR}/../host/nimble/port/src/esp_nimble_mem.c"
    )
    list(APPEND bt_ctrl_include_dirs
        "${CMAKE_CURRENT_LIST_DIR}/../host/nimble/port/include"
    )
endif()

# Add the linker scripts
if(CONFIG_IDF_TARGET_ESP32)
    list(APPEND bt_ctrl_srcs "${CMAKE_CURRENT_LIST_DIR}/esp32/hli_vectors.S")
    list(APPEND bt_ctrl_ldscripts "${CMAKE_CURRENT_LIST_DIR}/linker_rw_bt_controller.lf")
elseif(CONFIG_IDF_TARGET_ESP32C3)
    list(APPEND bt_ctrl_ldscripts "${CMAKE_CURRENT_LIST_DIR}/linker_rw_bt_controller.lf")
elseif(CONFIG_IDF_TARGET_ESP32S3)
    list(APPEND bt_ctrl_ldscripts "${CMAKE_CURRENT_LIST_DIR}/linker_rw_bt_controller.lf")
elseif(CONFIG_IDF_TARGET_ESP32C2)
    set(bt_ctrl_ldscripts "${CMAKE_CURRENT_LIST_DIR}/linker_esp32c2.lf")
elseif(CONFIG_BT_DUAL_MODE_ARCH)
    list(APPEND bt_ctrl_ldscripts "${CMAKE_CURRENT_LIST_DIR}/linker_esp_btdm_controller.lf")
else()
    list(APPEND bt_ctrl_ldscripts "${CMAKE_CURRENT_LIST_DIR}/linker_esp_ble_controller.lf")
endif()

# Export the variables to the parent scope
set(bt_ctrl_srcs "${bt_ctrl_srcs}" PARENT_SCOPE)
set(bt_ctrl_include_dirs "${bt_ctrl_include_dirs}" PARENT_SCOPE)
set(bt_ctrl_ldscripts "${bt_ctrl_ldscripts}" PARENT_SCOPE)
