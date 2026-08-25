function(replace_log_compression_srcs)
    if(NOT CONFIG_BLE_COMPRESSED_LOG_ENABLE)
        return()
    endif()

    get_filename_component(CODE_BASE_PATH "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/.." ABSOLUTE)
    # When log compression is enabled, selected logs are replaced
    # by auto-generated macros that emit pre-encoded data.
    # This eliminates the original format strings, reducing firmware size and
    # removing runtime formatting overhead, so logs are produced faster and
    # with less system impact.
    add_subdirectory(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/ble_log/extension/log_compression)
    set(LOG_COMPRESSION_TARGET "${LOG_COMPRESSION_TARGET}" PARENT_SCOPE)

    if(LOG_COMPRESSION_TARGET)
        # Replace the caller's srcs/include_dirs with the compressed-log ones
        set(srcs "${LOG_COMPRESS_SRCS}" PARENT_SCOPE)
        set(include_dirs "${LOG_COMPRESS_INCLUDE_DIRS}" PARENT_SCOPE)
        # LOG_COMPRESS_FILES_WITH_FLAGS format: "file_path|compile_flags"
        if(LOG_COMPRESS_FILES_WITH_FLAGS)
            foreach(file_entry ${LOG_COMPRESS_FILES_WITH_FLAGS})
                # Split by '|' to get file path and flags
                string(REPLACE "|" ";" file_parts "${file_entry}")
                list(GET file_parts 0 file_path)
                list(GET file_parts 1 compile_flags)
                set_source_files_properties(${file_path}
                    PROPERTIES COMPILE_FLAGS "${compile_flags}")
            endforeach()
        endif()
    else()
        # Only append extra include dirs, keep caller's srcs/include_dirs intact
        set(_merged_include_dirs ${include_dirs} ${LOG_COMPRESS_INCLUDE_DIRS})
        set(include_dirs "${_merged_include_dirs}" PARENT_SCOPE)
    endif()
endfunction()

# Make the bt component library depend on the log-compression custom target
# so that the Python-driven code-generation step (log index headers and
# replacement sources referenced by LOG_COMPRESS_SRCS) finishes before the
# component is compiled. Does nothing if compressed log is disabled or no
# module actually participates in log compression.
function(register_log_compression_dependency)
    if(NOT CONFIG_BLE_COMPRESSED_LOG_ENABLE)
        return()
    endif()
    if(NOT LOG_COMPRESSION_TARGET)
        return()
    endif()

    add_dependencies(${COMPONENT_LIB} ${LOG_COMPRESSION_TARGET})
endfunction()

function(wrap_uart_log_tx_interface)
    if(NOT CONFIG_BT_ENABLED)
        return()
    endif()

    # UART redir wrap flags — needed whenever BLE Log uses UART DMA on port 0,
    # regardless of whether BLE controller is enabled.
    if(DEFINED CONFIG_BLE_LOG_PRPH_UART_DMA_PORT)
        if(CONFIG_BLE_LOG_PRPH_UART_DMA_PORT EQUAL 0)
            target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=uart_tx_chars")
            target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=uart_write_bytes")
            target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=uart_write_bytes_with_break")
        endif()
    endif()
endfunction()

# Inject the ESP-IDF build commit into the bt component as a private compile
# definition (BLE_LOG_IDF_COMMIT), reported later in the BLE Log version info
# frame. Does nothing when the commit cannot be determined.
function(register_ble_log_idf_commit)
    idf_build_get_property(idf_path IDF_PATH)
    # "git rev-parse" walks up parent dirs; only trust it when the IDF tree
    # itself is a repo/worktree. Release trees have no .git and must leave
    # BLE_LOG_IDF_COMMIT undefined instead of picking up an enclosing repo.
    if(NOT EXISTS "${idf_path}/.git")
        return()
    endif()
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD
        WORKING_DIRECTORY "${idf_path}"
        RESULT_VARIABLE ble_log_git_result
        OUTPUT_VARIABLE ble_log_idf_commit
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    string(LENGTH "${ble_log_idf_commit}" ble_log_idf_commit_len)
    # Take the first 12 chars of the full hash; --short=N may return a longer
    # ambiguous abbreviation, so never rely on abbreviated output
    if(ble_log_git_result EQUAL 0 AND ble_log_idf_commit_len EQUAL 40)
        string(SUBSTRING "${ble_log_idf_commit}" 0 12 ble_log_idf_commit)
        target_compile_definitions(${COMPONENT_LIB} PRIVATE
            "BLE_LOG_IDF_COMMIT=\"${ble_log_idf_commit}\"")
    endif()
endfunction()

set(bt_common_srcs "" PARENT_SCOPE)
set(bt_common_include_dirs "" PARENT_SCOPE)
set(bt_common_priv_include_dirs "" PARENT_SCOPE)

# API headers that are used in the docs are also compiled
# even if CONFIG_BT_ENABLED=n as long as CONFIG_IDF_DOC_BUILD=y
if(CONFIG_IDF_DOC_BUILD)
    set(bt_common_include_dirs
        "${CMAKE_CURRENT_LIST_DIR}/api/include/api"
        "${CMAKE_CURRENT_LIST_DIR}/btc/profile/esp/blufi/include"
        "${CMAKE_CURRENT_LIST_DIR}/btc/profile/esp/include"
        "${CMAKE_CURRENT_LIST_DIR}/hci_log/include"
        "${CMAKE_CURRENT_LIST_DIR}/ble_log/include"
        "${CMAKE_CURRENT_LIST_DIR}/ble_log/deprecated/include"
        PARENT_SCOPE
    )
    return()
endif()

if(NOT CONFIG_BT_ENABLED)
    return()
endif()

list(APPEND bt_common_srcs
    "${CMAKE_CURRENT_LIST_DIR}/btc/core/btc_alarm.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/esp_blufi_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/hci_log/bt_hci_log.c"
    "${CMAKE_CURRENT_LIST_DIR}/btc/core/btc_manage.c"
    "${CMAKE_CURRENT_LIST_DIR}/hci_log/bt_hci_log_insights.c"
    "${CMAKE_CURRENT_LIST_DIR}/btc/core/btc_task.c"
    "${CMAKE_CURRENT_LIST_DIR}/btc/profile/esp/blufi/blufi_prf.c"
    "${CMAKE_CURRENT_LIST_DIR}/btc/profile/esp/blufi/blufi_protocol.c"
    "${CMAKE_CURRENT_LIST_DIR}/osi/alarm.c"
    "${CMAKE_CURRENT_LIST_DIR}/osi/allocator.c"
    "${CMAKE_CURRENT_LIST_DIR}/osi/buffer.c"
    "${CMAKE_CURRENT_LIST_DIR}/osi/config.c"
    "${CMAKE_CURRENT_LIST_DIR}/osi/fixed_queue.c"
    "${CMAKE_CURRENT_LIST_DIR}/osi/pkt_queue.c"
    "${CMAKE_CURRENT_LIST_DIR}/osi/fixed_pkt_queue.c"
    "${CMAKE_CURRENT_LIST_DIR}/osi/future.c"
    "${CMAKE_CURRENT_LIST_DIR}/osi/hash_functions.c"
    "${CMAKE_CURRENT_LIST_DIR}/osi/hash_map.c"
    "${CMAKE_CURRENT_LIST_DIR}/osi/list.c"
    "${CMAKE_CURRENT_LIST_DIR}/osi/mutex.c"
    "${CMAKE_CURRENT_LIST_DIR}/osi/thread.c"
    "${CMAKE_CURRENT_LIST_DIR}/osi/osi.c"
    "${CMAKE_CURRENT_LIST_DIR}/osi/semaphore.c"
    "${CMAKE_CURRENT_LIST_DIR}/ble_log/deprecated/ble_log_spi_out.c"
    "${CMAKE_CURRENT_LIST_DIR}/osal/src/bt_osal_freertos.c"
)

list(APPEND bt_common_include_dirs
    "${CMAKE_CURRENT_LIST_DIR}/osi/include"
    "${CMAKE_CURRENT_LIST_DIR}/api/include/api"
    "${CMAKE_CURRENT_LIST_DIR}/btc/profile/esp/blufi/include"
    "${CMAKE_CURRENT_LIST_DIR}/btc/profile/esp/include"
    "${CMAKE_CURRENT_LIST_DIR}/hci_log/include"
    "${CMAKE_CURRENT_LIST_DIR}/ble_log/include"
    "${CMAKE_CURRENT_LIST_DIR}/ble_log/deprecated/include"
    "${CMAKE_CURRENT_LIST_DIR}/osal/include"
)

list(APPEND bt_common_priv_include_dirs
    "${CMAKE_CURRENT_LIST_DIR}/btc/include"
    "${CMAKE_CURRENT_LIST_DIR}/include"
)

if(CONFIG_BLE_LOG_ENABLED)
    list(APPEND bt_common_srcs
        "${CMAKE_CURRENT_LIST_DIR}/ble_log/src/ble_log.c"
        "${CMAKE_CURRENT_LIST_DIR}/ble_log/src/ble_log_lbm.c"
        "${CMAKE_CURRENT_LIST_DIR}/ble_log/src/ble_log_rt.c"
        "${CMAKE_CURRENT_LIST_DIR}/ble_log/src/ble_log_util.c"
    )

    list(APPEND bt_common_include_dirs
        "${CMAKE_CURRENT_LIST_DIR}/ble_log/include"
    )

    list(APPEND bt_common_priv_include_dirs
        "${CMAKE_CURRENT_LIST_DIR}/ble_log/src/internal_include"
        "${CMAKE_CURRENT_LIST_DIR}/ble_log/src/internal_include/prph"
    )

    # Timestamp synchronization extension
    if(CONFIG_BLE_LOG_TS_ENABLED)
        list(APPEND bt_common_srcs "${CMAKE_CURRENT_LIST_DIR}/ble_log/src/ble_log_ts.c")
    endif()

    # Peripheral interface implementation
    if(CONFIG_BLE_LOG_PRPH_DUMMY)
        list(APPEND bt_common_srcs
            "${CMAKE_CURRENT_LIST_DIR}/ble_log/src/prph/ble_log_prph_dummy.c"
        )
    elseif(CONFIG_BLE_LOG_PRPH_TEST)
        list(APPEND bt_common_srcs
            "${CMAKE_CURRENT_LIST_DIR}/ble_log/src/prph/ble_log_prph_test.c"
        )
    elseif(CONFIG_BLE_LOG_PRPH_SPI_MASTER_DMA)
        list(APPEND bt_common_srcs
            "${CMAKE_CURRENT_LIST_DIR}/ble_log/src/prph/ble_log_prph_spi_master_dma.c"
        )
    elseif(CONFIG_BLE_LOG_PRPH_SPI_MASTER_HD)
        list(APPEND bt_common_srcs
            "${CMAKE_CURRENT_LIST_DIR}/ble_log/src/prph/ble_log_prph_spi_master_hd.c"
        )
    elseif(CONFIG_BLE_LOG_PRPH_UART_DMA)
        list(APPEND bt_common_srcs
            "${CMAKE_CURRENT_LIST_DIR}/ble_log/src/prph/ble_log_prph_uart_dma.c"
        )
    endif()
endif()

# Compile TinyCrypt if:
# 1. Controller uses TinyCrypt (not mbedTLS), OR
# 2. NimBLE uses TinyCrypt (not mbedTLS), OR
# 3. Bluedroid Host SMP uses TinyCrypt
if(CONFIG_BT_SMP_CRYPTO_STACK_TINYCRYPT OR NOT CONFIG_BT_NIMBLE_CRYPTO_STACK_MBEDTLS)
    list(APPEND bt_common_include_dirs
        "${CMAKE_CURRENT_LIST_DIR}/tinycrypt/include"
        "${CMAKE_CURRENT_LIST_DIR}/tinycrypt/port"
    )
    list(APPEND bt_common_srcs
        "${CMAKE_CURRENT_LIST_DIR}/tinycrypt/src/utils.c"
        "${CMAKE_CURRENT_LIST_DIR}/tinycrypt/src/sha256.c"
        "${CMAKE_CURRENT_LIST_DIR}/tinycrypt/src/ecc.c"
        "${CMAKE_CURRENT_LIST_DIR}/tinycrypt/src/ctr_prng.c"
        "${CMAKE_CURRENT_LIST_DIR}/tinycrypt/src/ctr_mode.c"
        "${CMAKE_CURRENT_LIST_DIR}/tinycrypt/src/aes_decrypt.c"
        "${CMAKE_CURRENT_LIST_DIR}/tinycrypt/src/aes_encrypt.c"
        "${CMAKE_CURRENT_LIST_DIR}/tinycrypt/src/ccm_mode.c"
        "${CMAKE_CURRENT_LIST_DIR}/tinycrypt/src/ecc_dsa.c"
        "${CMAKE_CURRENT_LIST_DIR}/tinycrypt/src/cmac_mode.c"
        "${CMAKE_CURRENT_LIST_DIR}/tinycrypt/src/ecc_dh.c"
        "${CMAKE_CURRENT_LIST_DIR}/tinycrypt/src/hmac_prng.c"
        "${CMAKE_CURRENT_LIST_DIR}/tinycrypt/src/ecc_platform_specific.c"
        "${CMAKE_CURRENT_LIST_DIR}/tinycrypt/src/hmac.c"
        "${CMAKE_CURRENT_LIST_DIR}/tinycrypt/src/cbc_mode.c"
        "${CMAKE_CURRENT_LIST_DIR}/tinycrypt/port/esp_tinycrypt_port.c"
    )
endif()

# Export the variables to the parent scope
set(bt_common_srcs "${bt_common_srcs}" PARENT_SCOPE)
set(bt_common_include_dirs "${bt_common_include_dirs}" PARENT_SCOPE)
set(bt_common_priv_include_dirs "${bt_common_priv_include_dirs}" PARENT_SCOPE)
