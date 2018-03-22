set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_PRIV_INCLUDEDIRS include/freertos)
set(COMPONENT_SRCDIRS ".")
set(COMPONENT_REQUIRES)

register_component()

target_link_libraries(freertos "-Wl,--undefined=uxTopUsedPriority")
