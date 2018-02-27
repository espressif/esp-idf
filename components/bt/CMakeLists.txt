if(NOT CONFIG_BT_ENABLED)
    return()
endif()

set(COMPONENT_SRCDIRS .)
set(COMPONENT_ADD_INCLUDEDIRS include)

if(CONFIG_BLUEDROID_ENABLED)

    list(APPEND COMPONENT_ADD_INCLUDEDIRS
        bluedroid/bta/include
        bluedroid/bta/sys/include
        bluedroid/btcore/include
        bluedroid/device/include
        bluedroid/hci/include
        bluedroid/osi/include
        bluedroid/external/sbc/decoder/include
		bluedroid/external/sbc/encoder/include
        bluedroid/btc/profile/esp/blufi/include
        bluedroid/btc/profile/esp/include
        bluedroid/btc/profile/std/a2dp/include
        bluedroid/btc/profile/std/include
        bluedroid/btc/include
        bluedroid/stack/gap/include
        bluedroid/stack/gatt/include
        bluedroid/stack/l2cap/include
        bluedroid/stack/sdp/include
        bluedroid/stack/smp/include
        bluedroid/stack/avct/include
        bluedroid/stack/avrc/include
        bluedroid/stack/avdt/include
        bluedroid/stack/a2dp/include
        bluedroid/stack/rfcomm/include
        bluedroid/stack/include
        bluedroid/api/include
        bluedroid/include)

    list(APPEND COMPONENT_SRCDIRS
        bluedroid/bta/dm
        bluedroid/bta/gatt
        bluedroid/bta/hh
        bluedroid/bta/sdp
        bluedroid/bta/av
        bluedroid/bta/ar
        bluedroid/bta/sys
        bluedroid/bta/jv
        bluedroid/btcore
        bluedroid/btif
        bluedroid/device
        bluedroid/hci
        bluedroid/main
        bluedroid/osi
        bluedroid/external/sbc/decoder/srce
	    bluedroid/external/sbc/encoder/srce
        bluedroid/btc/core
        bluedroid/btc/profile/esp/blufi
        bluedroid/btc/profile/std/gap
        bluedroid/btc/profile/std/gatt
        bluedroid/btc/profile/std/a2dp
        bluedroid/btc/profile/std/avrc
        bluedroid/btc/profile/std/spp
        bluedroid/stack/btm
        bluedroid/stack/btu
        bluedroid/stack/gap
        bluedroid/stack/gatt
        bluedroid/stack/hcic
        bluedroid/stack/l2cap
        bluedroid/stack/sdp
        bluedroid/stack/smp
        bluedroid/stack/avct
        bluedroid/stack/avrc
        bluedroid/stack/avdt
        bluedroid/stack/a2dp
        bluedroid/stack/rfcomm
        bluedroid/api
        )

endif()

register_component()

target_link_libraries(bt "-L${CMAKE_CURRENT_LIST_DIR}/lib")
target_link_libraries(bt btdm_app)
