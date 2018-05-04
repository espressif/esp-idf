TEST_NAME=test
FUZZ=afl-fuzz
COMPONENTS_DIR=../..
CFLAGS=-g -DMDNS_TEST_MODE -I. -I.. -I../include -I../private_include -I$(COMPONENTS_DIR)/tcpip_adapter/include -I$(COMPONENTS_DIR)/esp32/include -include esp32_compat.h
MDNS_C_DEPENDENCY_INJECTION=-include mdns_di.h
ifeq ($(INSTR),off)
    CC=gcc
    CFLAGS+=-DINSTR_IS_OFF
    TEST_NAME=test_sim
else
    CC=afl-clang-fast
endif
CPP=$(CC)
LD=$(CC)
OBJECTS=mdns.o esp32_mock.o test.o

OS := $(shell uname)
ifeq ($(OS),Darwin)
  LDLIBS=
else
   LDLIBS=-lbsd
   CFLAGS+=-DUSE_BSD_STRING
endif

all: $(TEST_NAME)

%.o: %.c
	@echo "[CC] $<"
	@$(CC) $(CFLAGS) -c $< -o $@

mdns.o: ../mdns.c
	@echo "[CC] $<"
	@$(CC) $(CFLAGS) $(MDNS_C_DEPENDENCY_INJECTION) -c $< -o $@

$(TEST_NAME): $(OBJECTS)
	@echo "[LD] $@"
	@$(LD)  $(OBJECTS) -o $@ $(LDLIBS)

fuzz: $(TEST_NAME)
	@$(FUZZ) -i "in" -o "out" -- ./$(TEST_NAME)

clean:
	@rm -rf *.o *.SYM $(TEST_NAME) out
