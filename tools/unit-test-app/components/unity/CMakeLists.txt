set(COMPONENT_SRCDIRS .)
set(COMPONENT_ADD_INCLUDEDIRS include)

set(COMPONENT_REQUIRES spi_flash idf_test)

register_component()

if(GCC_NOT_5_2_0)
    component_compile_options(-Wno-unused-const-variable)
endif()