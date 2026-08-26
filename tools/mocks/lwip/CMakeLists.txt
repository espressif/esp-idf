# NOTE: This kind of mocking currently works on Linux targets only.
#       On Espressif chips, too many dependencies are missing at the moment.
message(STATUS "building LWIP MOCKS (only netdb)")

idf_component_get_property(original_lwip_dir lwip COMPONENT_OVERRIDEN_DIR)

idf_component_mock(INCLUDE_DIRS "${original_lwip_dir}/include"
                                "${original_lwip_dir}/include/lwip"
                                "${original_lwip_dir}/lwip/src/include"
                                "${original_lwip_dir}/lwip/src/include/compat/posix"
                                "${original_lwip_dir}/port/include"
                                "${original_lwip_dir}/port/linux/include"
                                "${original_lwip_dir}/port/freertos/include"
                                "${original_lwip_dir}/port/linux/include/arch"
                   MOCK_HEADER_FILES ${original_lwip_dir}/lwip/src/include/lwip/netdb.h)
