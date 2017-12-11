#
#Component Makefile
#

LS_TESTDIR := ../libsodium/test/default

ifdef TESTS_ALL
$(info not linking libsodium tests, use 'TEST_COMPONENTS=libsodium' to test it)
else
COMPONENT_ADD_LDFLAGS = -Wl,--whole-archive -l$(COMPONENT_NAME) -Wl,--no-whole-archive
endif # TESTS_ALL

COMPONENT_SRCDIRS := . $(LS_TESTDIR)

COMPONENT_PRIV_INCLUDEDIRS := $(LS_TESTDIR)/../quirks

COMPONENT_OBJS := test_sodium.o

# The libsodium test suite is designed to be run each test case as an executable on a desktop computer and uses
# filesytem to write & then compare contents of each file.
#
# For now, use their "BROWSER_TEST" mode with these hacks so that
# multiple test cases can be combined into one ELF file.
#
# Run each test case from test_sodium.c as CASENAME_xmain().

define sodium_testcase
# This would generate 'warning "main" redefined' warnings at runtime, which are
# silenced here. Only other solution involves patching libsodium's cmptest.h.
$(LS_TESTDIR)/$(1).o: CFLAGS+=-Dxmain=$(1)_xmain -Dmain=$(1)_main
$(LS_TESTDIR)/$(1).o: CPPFLAGS+=-Wp,-w
ote:
COMPONENT_OBJS += $(LS_TESTDIR)/$(1).o
endef

TEST_CASES := chacha20 aead_chacha20poly1305 box box2 ed25519_convert sign hash

$(foreach case,$(TEST_CASES),$(eval $(call sodium_testcase,$(case))))

# this seems odd, but it prevents the libsodium test harness from
# trying to write to a file!
CFLAGS += -DBROWSER_TESTS


