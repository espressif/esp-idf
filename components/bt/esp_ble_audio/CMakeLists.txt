function(register_ble_audio_libs)
    if(NOT CONFIG_BT_AUDIO)
        return()
    endif()

    if(CONFIG_SOC_BLE_AUDIO_SUPPORTED)
        add_prebuilt_library(ble_audio "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/lib/lib/${idf_target}/libble_audio.a")
        target_link_libraries(${COMPONENT_LIB} PRIVATE ble_audio)
    endif()
endfunction()

set(ble_audio_srcs "" PARENT_SCOPE)
set(ble_audio_include_dirs "" PARENT_SCOPE)
set(ble_audio_priv_include_dirs "" PARENT_SCOPE)

# API headers that are used in the docs are also compiled
# even if CONFIG_BT_ENABLED=n as long as CONFIG_IDF_DOC_BUILD=y
if(CONFIG_IDF_DOC_BUILD)
    set(ble_audio_include_dirs
        "${CMAKE_CURRENT_LIST_DIR}/api/iso/include"
        "${CMAKE_CURRENT_LIST_DIR}/api/audio/include"
        PARENT_SCOPE
    )
    return()
endif()

if(CONFIG_BT_CONTROLLER_ONLY OR (NOT CONFIG_BT_ISO AND NOT CONFIG_BT_AUDIO))
    return()
endif()

if(CONFIG_BT_ISO)
    list(APPEND ble_audio_srcs
        "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/gatt/gatt.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/gatt/gatt.db.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/gatt/gatt.nrp.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/gap.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/iso.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/common/adv.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/common/conn.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/common/gatt.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/common/hci.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/common/host.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/common/iso.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/common/scan.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/common/task.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/common/app/gap.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/common/app/gatt.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/utils/crc/crc32_sw.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/utils/addr.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/utils/bt_str.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/utils/buf.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/utils/crypto.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/utils/timer.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/utils/utf8.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/utils/utils.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/utils/uuid.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/iso/iso.c"
        "${CMAKE_CURRENT_LIST_DIR}/api/iso/esp_ble_iso_common_api.c"
    )

    if(CONFIG_BT_OTS OR CONFIG_BT_OTS_CLIENT)
        list(APPEND ble_audio_srcs
            "${CMAKE_CURRENT_LIST_DIR}/host/common/l2cap.c"
            "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/l2cap.c"
        )
    endif()

    list(APPEND ble_audio_include_dirs
        "${CMAKE_CURRENT_LIST_DIR}/api/iso/include"
        "${CMAKE_CURRENT_LIST_DIR}/host/common/include"
        "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/include"
        "${CMAKE_CURRENT_LIST_DIR}/include/subsys/bluetooth"
        "${CMAKE_CURRENT_LIST_DIR}/include/subsys/bluetooth/host"
        "${CMAKE_CURRENT_LIST_DIR}/include"
    )
endif()

if(CONFIG_BT_AUDIO)
    list(APPEND ble_audio_include_dirs
        "${CMAKE_CURRENT_LIST_DIR}/api/audio/include"
        "${CMAKE_CURRENT_LIST_DIR}/include/subsys/bluetooth/host"
    )

    list(APPEND ble_audio_srcs
        "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/profiles/server.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/init.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/common/init.c"
        "${CMAKE_CURRENT_LIST_DIR}/api/audio/esp_ble_audio_aics_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/api/audio/esp_ble_audio_bap_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/api/audio/esp_ble_audio_cap_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/api/audio/esp_ble_audio_ccp_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/api/audio/esp_ble_audio_codec_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/api/audio/esp_ble_audio_common_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/api/audio/esp_ble_audio_csip_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/api/audio/esp_ble_audio_gmap_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/api/audio/esp_ble_audio_has_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/api/audio/esp_ble_audio_mcc_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/api/audio/esp_ble_audio_media_proxy_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/api/audio/esp_ble_audio_micp_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/api/audio/esp_ble_audio_pacs_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/api/audio/esp_ble_audio_pbp_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/api/audio/esp_ble_audio_tbs_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/api/audio/esp_ble_audio_tmap_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/api/audio/esp_ble_audio_vcp_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/api/audio/esp_ble_audio_vocs_api.c"
    )

    if(CONFIG_BT_ASCS)
        list(APPEND ble_audio_srcs
            "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/profiles/ascs.c"
        )
    endif()

    if(CONFIG_BT_PACS)
        list(APPEND ble_audio_srcs
            "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/profiles/pacs.c"
        )
    endif()

    if(CONFIG_BT_BAP_SCAN_DELEGATOR)
        list(APPEND ble_audio_srcs
            "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/profiles/bass.c"
        )
    endif()

    if(CONFIG_BT_CAP_ACCEPTOR)
        list(APPEND ble_audio_srcs
            "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/profiles/cas.c"
        )
    endif()

    if(CONFIG_BT_CSIP_SET_MEMBER)
        list(APPEND ble_audio_srcs
            "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/profiles/csis.c"
        )
    endif()

    if(CONFIG_BT_TBS)
        list(APPEND ble_audio_srcs
            "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/profiles/tbs.c"
        )
    endif()

    if(CONFIG_BT_TMAP)
        list(APPEND ble_audio_srcs
            "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/profiles/tmas.c"
        )
    endif()

    if(CONFIG_BT_VCP_VOL_REND)
        list(APPEND ble_audio_srcs
            "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/profiles/vcs.c"
        )
    endif()

    if(CONFIG_BT_MICP_MIC_DEV)
        list(APPEND ble_audio_srcs
            "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/profiles/mics.c"
        )
    endif()

    if(CONFIG_BT_MCS)
        list(APPEND ble_audio_srcs
            "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/profiles/mcs.c"
        )
    endif()

    if(CONFIG_BT_HAS)
        list(APPEND ble_audio_srcs
            "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/profiles/has.c"
        )
    endif()

    if(CONFIG_BT_OTS OR CONFIG_BT_OTS_CLIENT)
        list(APPEND ble_audio_include_dirs
            "${CMAKE_CURRENT_LIST_DIR}/host/services/ots"
        )

        list(APPEND ble_audio_srcs
            "${CMAKE_CURRENT_LIST_DIR}/host/services/ots/ots_l2cap.c"
        )

        if(CONFIG_BT_OTS)
            list(APPEND ble_audio_srcs
                "${CMAKE_CURRENT_LIST_DIR}/host/services/ots/ots_oacp.c"
                "${CMAKE_CURRENT_LIST_DIR}/host/services/ots/ots_obj_manager.c"
                "${CMAKE_CURRENT_LIST_DIR}/host/services/ots/ots_olcp.c"
                "${CMAKE_CURRENT_LIST_DIR}/host/services/ots/ots.c"
            )

            if(CONFIG_BT_OTS_DIR_LIST_OBJ)
                list(APPEND ble_audio_srcs
                    "${CMAKE_CURRENT_LIST_DIR}/host/services/ots/ots_dir_list.c"
                )
            endif()
        endif()

        if(CONFIG_BT_OTS_CLIENT)
            list(APPEND ble_audio_srcs
                "${CMAKE_CURRENT_LIST_DIR}/host/services/ots/ots_client.c"
            )
        endif()
    endif()
endif()

# Export the variables to the parent scope
set(ble_audio_srcs "${ble_audio_srcs}" PARENT_SCOPE)
set(ble_audio_include_dirs "${ble_audio_include_dirs}" PARENT_SCOPE)
set(ble_audio_priv_include_dirs "${ble_audio_priv_include_dirs}" PARENT_SCOPE)
