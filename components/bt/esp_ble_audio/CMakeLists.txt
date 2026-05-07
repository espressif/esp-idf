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

# API headers that are used in the docs are also compiled
# even if CONFIG_BT_ENABLED=n as long as CONFIG_IDF_DOC_BUILD=y
if(CONFIG_IDF_DOC_BUILD)
    set(ble_audio_include_dirs
        "${CMAKE_CURRENT_LIST_DIR}/api/include"
        PARENT_SCOPE
    )
    return()
endif()

if(CONFIG_BT_CONTROLLER_ONLY OR NOT CONFIG_BT_AUDIO)
    return()
endif()

list(APPEND ble_audio_include_dirs
    "${CMAKE_CURRENT_LIST_DIR}/api/include"
    "${CMAKE_CURRENT_LIST_DIR}/host/common/include"
    "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/include"
    "${CMAKE_CURRENT_LIST_DIR}/include"
)

list(APPEND ble_audio_srcs
    "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/profiles/server.c"
    "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/init.c"
    "${CMAKE_CURRENT_LIST_DIR}/host/common/init.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/esp_ble_audio_aics_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/esp_ble_audio_bap_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/esp_ble_audio_cap_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/esp_ble_audio_ccp_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/esp_ble_audio_codec_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/esp_ble_audio_common_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/esp_ble_audio_csip_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/esp_ble_audio_gmap_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/esp_ble_audio_has_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/esp_ble_audio_mcc_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/esp_ble_audio_media_proxy_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/esp_ble_audio_micp_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/esp_ble_audio_pacs_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/esp_ble_audio_pbp_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/esp_ble_audio_tbs_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/esp_ble_audio_tmap_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/esp_ble_audio_vcp_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/esp_ble_audio_vocs_api.c"
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

# Export the variables to the parent scope
set(ble_audio_srcs "${ble_audio_srcs}" PARENT_SCOPE)
set(ble_audio_include_dirs "${ble_audio_include_dirs}" PARENT_SCOPE)
