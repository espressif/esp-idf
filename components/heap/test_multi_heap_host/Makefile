test: test_poisoning_disabled \
      test_poisoning_light \
      test_poisoning_comprehensive

test_poisoning_disabled:
	@echo ==== HEAP_POISONING_DISABLED ====
	CPPFLAGS= $(MAKE) -f Makefile.test clean test

test_poisoning_light:
	@echo ==== HEAP_POISONING_LIGHT ====
	CPPFLAGS=-DCONFIG_HEAP_POISONING_LIGHT $(MAKE) -f Makefile.test clean test

test_poisoning_comprehensive:
	@echo ==== HEAP_POISONING_COMPREHENSIVE ====
	CPPFLAGS=-DCONFIG_HEAP_POISONING_COMPREHENSIVE $(MAKE) -f Makefile.test clean test

%:
	CPPFLAGS=-DCONFIG_HEAP_POISONING_COMPREHENSIVE $(MAKE) -f Makefile.test $@
