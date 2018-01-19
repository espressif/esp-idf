set(COMPONENT_ADD_INCLUDEDIRS port/include include/expat)
set(COMPONENT_SRCDIRS library port)

register_component()

target_compile_definitions(expat PRIVATE HAVE_EXPAT_CONFIG_H)

# patch around warnings in third-party files
set_source_files_properties(
  library/xmlparse.c
  PROPERTIES COMPILE_FLAGS
  -Wno-unused-function
  )
