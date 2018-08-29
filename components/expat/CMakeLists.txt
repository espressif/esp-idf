set(COMPONENT_ADD_INCLUDEDIRS port/include include/expat)
set(COMPONENT_SRCDIRS library port)

set(COMPONENT_REQUIRES)

register_component()

component_compile_definitions(HAVE_EXPAT_CONFIG_H)

# patch around warnings in third-party files
set_source_files_properties(
    library/xmlparse.c
    PROPERTIES COMPILE_FLAGS
    -Wno-unused-function
    )

# Temporary suppress "fallthrough" warnings until they are fixed in expat repo
component_compile_options(-Wno-implicit-fallthrough)
