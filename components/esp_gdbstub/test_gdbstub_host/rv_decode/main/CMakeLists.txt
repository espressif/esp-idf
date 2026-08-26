idf_component_register(SRCS
                            "test_rv_decode.cpp"
                            "../../../src/port/riscv/rv_decode.c"
                       INCLUDE_DIRS
                            "../../../private_include"
                            "../../../include"
                            "../../../src/port/riscv/include"
                            "../../../../soc/esp32c3/include"
                            "../../../../esp_common/include"
                            "../../../../riscv/include"
                       PRIV_REQUIRES esp_hw_support
                       WHOLE_ARCHIVE)

# Currently 'main' for IDF_TARGET=linux is defined in freertos component.
# Since we are using a freertos mock here, need to let Catch2 provide 'main'.
target_link_libraries(${COMPONENT_LIB} PRIVATE Catch2WithMain)
