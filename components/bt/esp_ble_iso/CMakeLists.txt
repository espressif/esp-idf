set(ble_iso_srcs "" PARENT_SCOPE)
set(ble_iso_include_dirs "" PARENT_SCOPE)

# API headers that are used in the docs are also compiled
# even if CONFIG_BT_ENABLED=n as long as CONFIG_IDF_DOC_BUILD=y
if(CONFIG_IDF_DOC_BUILD)
    set(ble_iso_include_dirs
        "${CMAKE_CURRENT_LIST_DIR}/api/include"
        PARENT_SCOPE
    )
    return()
endif()

if(CONFIG_BT_CONTROLLER_ONLY OR NOT CONFIG_BT_ISO)
    return()
endif()

list(APPEND ble_iso_srcs
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
    "${CMAKE_CURRENT_LIST_DIR}/api/esp_ble_iso_common_api.c"
)

# L2CAP host shim wiring is currently only consumed by the OTS service
# (which lives in esp_ble_audio). Keep the same gate so build output
# matches the pre-split behavior; the .c files belong to the host shim.
if(CONFIG_BT_OTS OR CONFIG_BT_OTS_CLIENT)
    list(APPEND ble_iso_srcs
        "${CMAKE_CURRENT_LIST_DIR}/host/common/l2cap.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/l2cap.c"
    )
endif()

list(APPEND ble_iso_include_dirs
    "${CMAKE_CURRENT_LIST_DIR}/api/include"
    "${CMAKE_CURRENT_LIST_DIR}/host/common/include"
    "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/include"
    "${CMAKE_CURRENT_LIST_DIR}/include/subsys/bluetooth"
    "${CMAKE_CURRENT_LIST_DIR}/include/subsys/bluetooth/host"
    "${CMAKE_CURRENT_LIST_DIR}/include"
)

set(ble_iso_srcs "${ble_iso_srcs}" PARENT_SCOPE)
set(ble_iso_include_dirs "${ble_iso_include_dirs}" PARENT_SCOPE)
