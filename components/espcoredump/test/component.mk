ifeq ($(TESTS_ALL),1)
    $(info not linking coredump tests from CI)
else
    COMPONENT_ADD_LDFLAGS = -Wl,--whole-archive -l$(COMPONENT_NAME) -Wl,--no-whole-archive
    COMPONENT_SRCDIRS := .
    COMPONENT_ADD_INCLUDEDIRS := .
    COMPONENT_PRIV_INCLUDEDIRS := .
    COMPONENT_REQUIRES := unity nvs_flash
endif # TESTS_ALL




