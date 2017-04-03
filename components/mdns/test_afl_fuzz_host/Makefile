TEST_NAME=test
FUZZ=afl-fuzz
CC=afl-clang-fast
CPP=$(CC)
LD=$(CC)
OBJECTS=mdns.o test.o
CFLAGS=-DMDNS_TEST_MODE -I. -I../include

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
	@$(CC) $(CFLAGS) -c $< -o $@

$(TEST_NAME): $(OBJECTS)
	@echo "[LD] $@"
	@$(LD) $(LDLIBS) $(OBJECTS) -o $@

fuzz: $(TEST_NAME)
	@$(FUZZ) -i "in" -o "out" -- ./$(TEST_NAME)

clean:
	@rm -rf *.o *.SYM $(TEST_NAME) out
