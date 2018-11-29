#
# Component Makefile
#

ifeq ($(GCC_NOT_5_2_0), 1)
unity/src/unity.o: CFLAGS += -Wno-unused-const-variable
endif

COMPONENT_ADD_INCLUDEDIRS = include unity/src
COMPONENT_SRCDIRS = unity/src .

ifdef CONFIG_UNITY_ENABLE_BACKTRACE_ON_FAIL
COMPONENT_PRIV_INCLUDEDIRS += include/priv
endif

ifndef CONFIG_UNITY_ENABLE_IDF_TEST_RUNNER
COMPONENT_OBJEXCLUDE += unity_runner.o
endif

ifdef CONFIG_UNITY_ENABLE_FIXTURE
COMPONENT_SRCDIRS += unity/extras/fixture/src
COMPONENT_ADD_INCLUDEDIRS += unity/extras/fixture/src
endif

COMPONENT_SUBMODULES := unity
