set(porting_btdm_srcs "")
set(porting_btdm_include_dirs "")

# btdm_common
file(GLOB_RECURSE BTDM_COMMON_SRCS "controller/btdm_common/src/*.c")
list(APPEND porting_btdm_srcs ${BTDM_COMMON_SRCS})
list(APPEND porting_btdm_include_dirs "${CMAKE_CURRENT_SOURCE_DIR}/controller/btdm_common/include")

# ble
if(CONFIG_BTDM_CTRL_MODE_BLE_ONLY OR CONFIG_BTDM_CTRL_MODE_BTDM)
    file(GLOB_RECURSE BLE_SRCS "controller/ble/src/*.c")
    list(APPEND porting_btdm_srcs ${BLE_SRCS})
    list(APPEND porting_btdm_include_dirs "${CMAKE_CURRENT_SOURCE_DIR}/controller/ble/include")
endif()

# bredr
if(CONFIG_BTDM_CTRL_MODE_BR_EDR_ONLY OR CONFIG_BTDM_CTRL_MODE_BTDM)
    file(GLOB_RECURSE BREDR_SRCS "controller/bredr/src/*.c")
    list(APPEND porting_btdm_srcs ${BREDR_SRCS})
    list(APPEND porting_btdm_include_dirs "${CMAKE_CURRENT_SOURCE_DIR}/controller/bredr/include")
endif()

# transport
file(GLOB_RECURSE TRANSPORT_SRCS "transport/src/*.c")
list(APPEND porting_btdm_srcs ${TRANSPORT_SRCS})
list(APPEND porting_btdm_include_dirs "${CMAKE_CURRENT_SOURCE_DIR}/transport/include")
if(CONFIG_BT_CTRL_HCI_INTERFACE_USE_RAM)
    if(CONFIG_BT_NIMBLE_ENABLED)
        list(APPEND porting_btdm_srcs
            "${CMAKE_CURRENT_SOURCE_DIR}/transport/driver/vhci/hci_driver_nimble.c"
        )
    else()
        list(APPEND porting_btdm_srcs
            "${CMAKE_CURRENT_SOURCE_DIR}/transport/driver/vhci/hci_driver_standard.c"
        )
    endif()
elseif(CONFIG_BT_CTRL_HCI_INTERFACE_USE_UART)
    list(APPEND porting_btdm_srcs
        "${CMAKE_CURRENT_SOURCE_DIR}/transport/driver/common/hci_driver_util.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/transport/driver/common/hci_driver_h4.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/transport/driver/common/hci_driver_mem.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/transport/driver/uart/hci_driver_uart_config.c"
    )
    if(CONFIG_BT_CTRL_UART_HCI_NO_DMA_MODE)
        list(APPEND porting_btdm_srcs
            "${CMAKE_CURRENT_SOURCE_DIR}/transport/driver/uart/hci_driver_uart.c"
        )
    else()
        list(APPEND porting_btdm_srcs
            "${CMAKE_CURRENT_SOURCE_DIR}/transport/driver/uart/hci_driver_uart_dma.c"
        )
    endif()
endif()

set(porting_btdm_srcs ${porting_btdm_srcs} PARENT_SCOPE)
set(porting_btdm_include_dirs ${porting_btdm_include_dirs} PARENT_SCOPE)
