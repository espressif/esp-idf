idf_component_register(SRCS "test_app_main.c"
                            "test_privilege.c"
                            "privilege_test_fixture.c"
                            "privilege_code.c"
                       PRIV_REQUIRES esp_privilege unity
                       WHOLE_ARCHIVE)

# The code running in U-mode must not use the HWLP/PIE extensions, their state CSRs are only accessible
# from M-mode. Table jumps (Zcmt) are not used either, since they rely on the jvt CSR.
set_source_files_properties("privilege_code.c" PROPERTIES
                            COMPILE_OPTIONS "-march=rv32imafcb_zicsr_zifencei_zcb_zcmp")
