# Object Transfer Service (OTS) profile core for the NimBLE host.
#
# This directory is not a standalone IDF component: it is pulled in by
# components/bt/ble_profiles/CMakeLists.txt via add_subdirectory() and exports
# its sources and public include directory to the parent scope, which forwards
# them to the bt component.
#

set(ble_ots_srcs "" PARENT_SCOPE)
set(ble_ots_include_dirs "" PARENT_SCOPE)

if(NOT CONFIG_BLE_OTS_ENABLED)
    return()
endif()

set(_srcs "${CMAKE_CURRENT_LIST_DIR}/src/ble_ots_common.c")

# The role-private headers (src/<role>/*_int.h) sit next to the sources that
# include them, so only the public include/ has to be exported.
set(_include_dirs "${CMAKE_CURRENT_LIST_DIR}/include")

if(CONFIG_BLE_OTS_CLIENT_ENABLED)
    list(APPEND _srcs
        "${CMAKE_CURRENT_LIST_DIR}/src/client/ble_ots_client_discovery.c"
        "${CMAKE_CURRENT_LIST_DIR}/src/client/ble_ots_client_metadata.c"
        "${CMAKE_CURRENT_LIST_DIR}/src/client/ble_ots_client_object_nav.c"
        "${CMAKE_CURRENT_LIST_DIR}/src/client/ble_ots_client_transfer.c"
    )
endif()

if(CONFIG_BLE_OTS_SERVER_ENABLED)
    list(APPEND _srcs
        "${CMAKE_CURRENT_LIST_DIR}/src/server/ots_server_init.c"
        "${CMAKE_CURRENT_LIST_DIR}/src/server/ots_server_metadata.c"
        "${CMAKE_CURRENT_LIST_DIR}/src/server/ots_server_oacp_ops.c"
        "${CMAKE_CURRENT_LIST_DIR}/src/server/ots_server_oacp_transfer.c"
        "${CMAKE_CURRENT_LIST_DIR}/src/server/ots_server_olcp.c"
        "${CMAKE_CURRENT_LIST_DIR}/src/server/ots_server_filter_changed.c"
    )
endif()

set(ble_ots_srcs "${_srcs}" PARENT_SCOPE)
set(ble_ots_include_dirs "${_include_dirs}" PARENT_SCOPE)
