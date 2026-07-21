# NimBLE profile cores: the list of profiles built on top of the NimBLE host.
#
# Only reached when CONFIG_BT_NIMBLE_ENABLED is set (see ../CMakeLists.txt).
# Each profile owns its own file list and its own Kconfig switch, and returns
# early when that switch is off, so plugging a new one in is three lines here.
#

set(nimble_profiles_srcs "" PARENT_SCOPE)
set(nimble_profiles_include_dirs "" PARENT_SCOPE)

set(_srcs "")
set(_include_dirs "")

# Object Transfer Service
add_subdirectory(ble_ots)
list(APPEND _srcs ${ble_ots_srcs})
list(APPEND _include_dirs ${ble_ots_include_dirs})

set(nimble_profiles_srcs "${_srcs}" PARENT_SCOPE)
set(nimble_profiles_include_dirs "${_include_dirs}" PARENT_SCOPE)
