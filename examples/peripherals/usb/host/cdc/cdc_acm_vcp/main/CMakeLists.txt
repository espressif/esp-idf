idf_component_register(
    SRCS "cdc_acm_vcp_example_main.cpp"
    INCLUDE_DIRS "."
    )

# We cannot set property CXX_STANDARD to '17'
# because CMake 3.5 (shipped with IDF 4.4) does not know this standard
# -fconcepts flag should be propagated from usb_host_vcp component...
target_compile_options(${COMPONENT_LIB} PRIVATE -fconcepts -std=gnu++17)
