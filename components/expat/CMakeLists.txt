set(COMPONENT_ADD_INCLUDEDIRS expat/expat/lib port/include)
set(COMPONENT_SRCDIRS expat/expat/lib)

set(COMPONENT_REQUIRES)

register_component()

component_compile_definitions(HAVE_EXPAT_CONFIG_H)
component_compile_definitions(HAVE_GETRANDOM)

# Temporary suppress "fallthrough" warnings until they are fixed in expat repo
component_compile_options(-Wno-implicit-fallthrough)
