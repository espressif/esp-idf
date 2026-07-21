# Electronic Shelf Label (ESL) profile core for the NimBLE host.
#
# This directory is not a standalone IDF component: it is pulled in by
# components/bt/ble_profiles/nimble/CMakeLists.txt via add_subdirectory() and
# exports its sources and public include directory to the parent scope, which
# forwards them to the bt component.
#

set(ble_esl_srcs "" PARENT_SCOPE)
set(ble_esl_include_dirs "" PARENT_SCOPE)

if(NOT CONFIG_BLE_ESL_ENABLED)
    return()
endif()

set(_srcs "${CMAKE_CURRENT_LIST_DIR}/src/common/esl_common.c")

# The role-private headers (src/<role>/*_int.h) sit next to the sources that
# include them, so only the public include/ has to be exported.
set(_include_dirs "${CMAKE_CURRENT_LIST_DIR}/include")

if(CONFIG_BLE_ESL_TAG_ROLE)
    list(APPEND _srcs
        "${CMAKE_CURRENT_LIST_DIR}/src/esl/esl_state.c"
        "${CMAKE_CURRENT_LIST_DIR}/src/esl/esl_cmd_display.c"
        "${CMAKE_CURRENT_LIST_DIR}/src/esl/esl_cmd_led.c"
        "${CMAKE_CURRENT_LIST_DIR}/src/esl/esl_cmd_lifecycle.c"
        "${CMAKE_CURRENT_LIST_DIR}/src/esl/esl_pawr.c"
        "${CMAKE_CURRENT_LIST_DIR}/src/esl/esl_gatts.c"
    )
endif()

if(CONFIG_BLE_ESL_AP_ROLE)
    list(APPEND _srcs
        "${CMAKE_CURRENT_LIST_DIR}/src/ap/ap_connection.c"
        "${CMAKE_CURRENT_LIST_DIR}/src/ap/ap_pawr.c"
        "${CMAKE_CURRENT_LIST_DIR}/src/ap/ap_lifecycle.c"
        "${CMAKE_CURRENT_LIST_DIR}/src/ap/ap_command.c"
        "${CMAKE_CURRENT_LIST_DIR}/src/ap/ap_gattc.c"
    )
endif()

set(ble_esl_srcs "${_srcs}" PARENT_SCOPE)
set(ble_esl_include_dirs "${_include_dirs}" PARENT_SCOPE)
