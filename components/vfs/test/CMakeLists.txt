set(COMPONENT_SRCDIRS ".")
set(COMPONENT_ADD_INCLUDEDIRS ".")

set(COMPONENT_REQUIRES unity test_utils vfs fatfs spiffs)

set(COMPONENT_ADD_LDFRAGMENTS linker.lf)

register_component()
