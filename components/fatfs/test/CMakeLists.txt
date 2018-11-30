set(COMPONENT_SRCDIRS ".")
set(COMPONENT_ADD_INCLUDEDIRS ".")

set(COMPONENT_REQUIRES unity test_utils vfs fatfs)

set(COMPONENT_EMBED_TXTFILES fatfs.img)

register_component()