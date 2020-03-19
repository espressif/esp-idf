target_sources(${COMPONENT_LIB} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/dport_panic_highint_hdl.S"
                                        "${CMAKE_CURRENT_LIST_DIR}/reset_reason.c")
