set(porting_srcs "" PARENT_SCOPE)
set(porting_include_dirs "" PARENT_SCOPE)
set(porting_priv_include_dirs "" PARENT_SCOPE)

if(NOT CONFIG_BT_CONTROLLER_ENABLED)
    return()
endif()

list(APPEND porting_priv_include_dirs
    ${CMAKE_CURRENT_SOURCE_DIR}/mem/
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

list(APPEND porting_srcs
    "${CMAKE_CURRENT_SOURCE_DIR}/mem/bt_osi_mem.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/mem/os_msys_init.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/mem/os_mempool.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/npl/freertos/src/npl_os_freertos.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/transport/src/hci_transport.c"
)

if(CONFIG_BT_LE_HCI_INTERFACE_USE_RAM)
    if(CONFIG_BT_NIMBLE_ENABLED)
        list(APPEND porting_srcs
            "${CMAKE_CURRENT_SOURCE_DIR}/transport/driver/vhci/hci_driver_nimble.c"
        )
    else()
        list(APPEND porting_srcs
            "${CMAKE_CURRENT_SOURCE_DIR}/transport/driver/vhci/hci_driver_standard.c"
        )
    endif()
elseif(CONFIG_BT_LE_HCI_INTERFACE_USE_UART)
    list(APPEND porting_srcs
        "${CMAKE_CURRENT_SOURCE_DIR}/transport/driver/common/hci_driver_util.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/transport/driver/common/hci_driver_h4.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/transport/driver/common/hci_driver_mem.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/transport/driver/uart/hci_driver_uart_config.c"
    )
    if(CONFIG_BT_LE_UART_HCI_DMA_MODE)
        list(APPEND porting_srcs
            "${CMAKE_CURRENT_SOURCE_DIR}/transport/driver/uart/hci_driver_uart_dma.c"
        )
    else()
        list(APPEND porting_srcs
            "${CMAKE_CURRENT_SOURCE_DIR}/transport/driver/uart/hci_driver_uart.c"
        )
    endif()
endif()

list(APPEND porting_include_dirs
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/npl/freertos/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/transport/include"
)

set(porting_srcs ${porting_srcs} PARENT_SCOPE)
set(porting_include_dirs ${porting_include_dirs} PARENT_SCOPE)
set(porting_priv_include_dirs ${porting_priv_include_dirs} PARENT_SCOPE)
