# BLE profiles: host-agnostic profile cores and the shared profile
# infrastructure they build on (e.g. the shared profile task in common/).
#

set(ble_profiles_srcs "" PARENT_SCOPE)
set(ble_profiles_include_dirs "" PARENT_SCOPE)

if(NOT CONFIG_BT_ENABLED)
    return()
endif()

set(_srcs "")
# The header is always visible so profiles can include it unconditionally; the
# implementation is compiled only when the shared task is enabled.
set(_include_dirs "${CMAKE_CURRENT_LIST_DIR}/common/include")

if(CONFIG_BT_PRF_TASK_ENABLED)
    list(APPEND _srcs "${CMAKE_CURRENT_LIST_DIR}/common/src/bt_prf_task.c")
endif()

# NimBLE profile cores, mirroring the "if BT_NIMBLE_ENABLED" guard in Kconfig.in.
if(CONFIG_BT_NIMBLE_ENABLED)
    add_subdirectory(nimble)
    list(APPEND _srcs ${nimble_profiles_srcs})
    list(APPEND _include_dirs ${nimble_profiles_include_dirs})
endif()

set(ble_profiles_srcs "${_srcs}" PARENT_SCOPE)
set(ble_profiles_include_dirs "${_include_dirs}" PARENT_SCOPE)
