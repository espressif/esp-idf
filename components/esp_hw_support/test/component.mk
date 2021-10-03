COMPONENT_SRCDIRS := .

COMPONENT_ADD_LDFLAGS = -Wl,--whole-archive -l$(COMPONENT_NAME) -Wl,--no-whole-archive -u ld_include_test_dport_xt_highint5
