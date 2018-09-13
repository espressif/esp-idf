set(COMPONENT_ADD_INCLUDEDIRS port/include include/expat)
set(COMPONENT_SRCS "library/xmlparse.c"
                   "library/xmlrole.c"
                   "library/xmltok.c"
                   "library/xmltok_impl.c"
                   "library/xmltok_ns.c"
                   "port/chardata.c"
                   "port/expat_element.c"
                   "port/minicheck.c")

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
