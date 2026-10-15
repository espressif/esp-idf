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

# Defined before the registration below: cmakev2 rejects a callback that is
# not yet a command when it is registered.
function(__bt_iso_addr_notice_post_build target)
    # On `app` where there is one, so it lands after the binary is generated.
    if(TARGET app)
        set(target app)
    endif()

    # An 80-column ASCII box, rows padded so the right side lines up. Green and
    # titled NOTE: CI that checks build logs (IDF's idf-build-apps included)
    # fails on any line saying "warning" or "error". Colour shows when idf.py
    # runs on a terminal (it sets CLICOLOR_FORCE); a redirected log stays plain.
    set(width 76)
    get_property(lines GLOBAL PROPERTY __BT_ISO_ADDR_NOTICE_LINES)
    list(POP_FRONT lines title)
    string(REPEAT "=" 78 rule)
    set(rule "+${rule}+")

    string(LENGTH "${title}" len)
    math(EXPR left "(${width} - ${len}) / 2")
    math(EXPR right "${width} - ${len} - ${left}")
    string(REPEAT " " ${left} left)
    string(REPEAT " " ${right} right)
    set(rows "${rule}" "| ${left}${title}${right} |" "${rule}")
    foreach(line IN LISTS lines)
        string(LENGTH "${line}" len)
        math(EXPR pad "${width} - ${len}")
        string(REPEAT " " ${pad} pad)
        list(APPEND rows "| ${line}${pad} |")
    endforeach()
    list(APPEND rows "${rule}")

    set(cmds COMMAND ${CMAKE_COMMAND} -E echo "")
    foreach(row IN LISTS rows)
        list(APPEND cmds COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --green --bold "${row}")
    endforeach()
    add_custom_command(TARGET ${target} POST_BUILD ${cmds} VERBATIM)
endfunction()

function(__bt_iso_addr_notice_deferred)
    if(TARGET app)
        __bt_iso_addr_notice_post_build(app)
    endif()
endfunction()

# Existing applications compile unchanged and fail only at run time, so this is
# the one place an application author is told. See CONFIG_BT_ISO_NOTICE_ADDR_LSB_FIRST.
if(CONFIG_BT_ISO_NOTICE_ADDR_LSB_FIRST)
    # Printed as the last thing a build does, the way esptool_py prints "App
    # built but not signed" - a configure-time message() scrolls past with the
    # rest of CMake's output. `app` only exists once every component is
    # processed, hence the deferral (cmakev1) or the POST_ELF callback
    # (cmakev2), as esp_blockdev does it.
    # The first line is the title; the box is drawn around them at build time.
    # A ';' inside a line would split it in two, as CMake lists are ';'-joined.
    set_property(GLOBAL PROPERTY __BT_ISO_ADDR_NOTICE_LINES
        "NOTE: BLE ISO / BLE Audio addresses are now LSB-first"
        "Now LSB-first on both hosts:"
        "  - addresses in esp_ble_iso / esp_ble_audio GAP events"
        "  - esp_ble_iso_ext_adv_info_t.addr"
        "Same order as the air, BASS and bt_addr_le_t."
        " "
        ">>> Bluedroid: esp_ble_gap_* / esp_ble_gattc_* still use MSB-first. Reverse"
        "    the bytes whenever an address passes between the two, as the examples"
        "    do with example_addr_le_to_host() in"
        "    examples/bluetooth/esp_ble_{audio,iso}/common_components/example_utils/."
        "    Unconverted code still compiles, then syncs, connects or pairs to the"
        "    wrong address at run time."
        ">>> NimBLE: no change needed."
        " "
        "Before the commit below, they were MSB-first when using Bluedroid:"
        "https://github.com/espressif/esp-idf/commit/47b94f0753e"
        " "
        "Turn off: BLE ISO Options > CONFIG_BT_ISO_NOTICE_ADDR_LSB_FIRST")

    if(COMMAND idf_component_register_build_event_callback)
        idf_component_register_build_event_callback(
            EVENT POST_ELF
            CALLBACK __bt_iso_addr_notice_post_build
        )
    else()
        cmake_language(DEFER DIRECTORY ${CMAKE_SOURCE_DIR} CALL __bt_iso_addr_notice_deferred)
    endif()
endif()

if(CONFIG_BT_NIMBLE_ENABLED)
    list(APPEND ble_iso_srcs
        "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/gatt/gatt.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/gatt/gatt.db.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/gatt/gatt.nrp.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/gap.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/iso.c"
    )
elseif(CONFIG_BT_BLUEDROID_ENABLED)
    list(APPEND ble_iso_srcs
        "${CMAKE_CURRENT_LIST_DIR}/host/adapter/bluedroid/hci.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/adapter/bluedroid/iso.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/adapter/bluedroid/gap.c"
        "${CMAKE_CURRENT_LIST_DIR}/host/adapter/bluedroid/gatt/gatt.c"
    )
endif()

list(APPEND ble_iso_srcs
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
    "${CMAKE_CURRENT_LIST_DIR}/host/utils/assert.c"
    "${CMAKE_CURRENT_LIST_DIR}/host/utils/addr.c"
    "${CMAKE_CURRENT_LIST_DIR}/host/utils/bt_str.c"
    "${CMAKE_CURRENT_LIST_DIR}/host/utils/buf.c"
    "${CMAKE_CURRENT_LIST_DIR}/host/utils/crypto.c"
    "${CMAKE_CURRENT_LIST_DIR}/host/utils/keys.c"
    "${CMAKE_CURRENT_LIST_DIR}/host/utils/mem.c"
    "${CMAKE_CURRENT_LIST_DIR}/host/utils/timer.c"
    "${CMAKE_CURRENT_LIST_DIR}/host/utils/utf8.c"
    "${CMAKE_CURRENT_LIST_DIR}/host/utils/uuid.c"
    "${CMAKE_CURRENT_LIST_DIR}/host/iso/iso.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/esp_ble_iso_common_api.c"
)

list(APPEND ble_iso_include_dirs
    "${CMAKE_CURRENT_LIST_DIR}/api/include"
    "${CMAKE_CURRENT_LIST_DIR}/host/common/include"
    "${CMAKE_CURRENT_LIST_DIR}/host/utils/include"
    "${CMAKE_CURRENT_LIST_DIR}/include/subsys/bluetooth"
    "${CMAKE_CURRENT_LIST_DIR}/include/subsys/bluetooth/host"
    "${CMAKE_CURRENT_LIST_DIR}/include"
)

if(CONFIG_BT_NIMBLE_ENABLED)
    list(APPEND ble_iso_include_dirs
        "${CMAKE_CURRENT_LIST_DIR}/host/adapter/nimble/include"
    )
elseif(CONFIG_BT_BLUEDROID_ENABLED)
    list(APPEND ble_iso_include_dirs
        "${CMAKE_CURRENT_LIST_DIR}/host/adapter/bluedroid/include"
    )
endif()

set(ble_iso_srcs "${ble_iso_srcs}" PARENT_SCOPE)
set(ble_iso_include_dirs "${ble_iso_include_dirs}" PARENT_SCOPE)
