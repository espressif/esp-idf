# Extra make rules for components containing ULP coprocessor code.
#
# ULP program(s) gets built and linked into the application.
# Steps taken here are explained in docs/ulp.rst

# Define names for files generated at different stages
ULP_ELF := $(ULP_APP_NAME).elf
ULP_MAP := $(ULP_ELF:.elf=.map)
ULP_SYM := $(ULP_ELF:.elf=.sym)
ULP_BIN := $(ULP_ELF:.elf=.bin)
ULP_EXPORTS_LD := $(ULP_ELF:.elf=.ld)
ULP_EXPORTS_HEADER := $(ULP_ELF:.elf=.h)
ULP_LD_SCRIPT := $(ULP_ELF:.elf=.common.ld)

ULP_OBJECTS := $(notdir $(ULP_S_SOURCES:.S=.ulp.o))
ULP_DEP := $(notdir $(ULP_S_SOURCES:.S=.ulp.d)) $(ULP_LD_SCRIPT:.ld=.d)
ULP_PREPROCESSED := $(notdir $(ULP_S_SOURCES:.S=.ulp.pS))
ULP_LISTINGS := $(notdir $(ULP_S_SOURCES:.S=.ulp.lst))

ULP_PREPROCESSOR_ARGS := \
	$(addprefix -I ,$(COMPONENT_INCLUDES)) \
	$(addprefix -I ,$(COMPONENT_EXTRA_INCLUDES)) \
	-I$(COMPONENT_PATH) -D__ASSEMBLER__

-include $(ULP_DEP)

# Preprocess LD script used to link ULP program
$(ULP_LD_SCRIPT): $(ULP_LD_TEMPLATE)
	$(summary) CPP $(notdir $@)
	$(CC) $(CPPFLAGS) -MT $(ULP_LD_SCRIPT) -E -P -xc -o $@ $(ULP_PREPROCESSOR_ARGS) $<

# Generate preprocessed assembly files.
# To inspect these preprocessed files, add a ".PRECIOUS: %.ulp.pS" rule. 
%.ulp.pS: $(COMPONENT_PATH)/ulp/%.S
	$(summary) CPP $(notdir $<)
	$(CC) $(CPPFLAGS) -MT $(patsubst %.ulp.pS,%.ulp.o,$@) -E -P -xc -o $@ $(ULP_PREPROCESSOR_ARGS) $<

# Compiled preprocessed files into object files.
%.ulp.o: %.ulp.pS
	$(summary) ULP_AS $(notdir $@)
	$(ULP_AS) -al=$(patsubst %.ulp.o,%.ulp.lst,$@) -o $@ $<

# Link object files and generate map file
$(ULP_ELF): $(ULP_OBJECTS) $(ULP_LD_SCRIPT)
	$(summary) ULP_LD $(notdir $@)
	$(ULP_LD) -o $@ -A elf32-esp32ulp -Map=$(ULP_MAP) -T $(ULP_LD_SCRIPT) $<

# Dump the list of global symbols in a convenient format.
$(ULP_SYM): $(ULP_ELF)
	$(ULP_NM) -g -f posix $< > $@

# Dump the binary for inclusion into the project 
$(COMPONENT_BUILD_DIR)/$(ULP_BIN): $(ULP_ELF)
	$(summary) ULP_BIN $(notdir $@)
	$(ULP_OBJCOPY) -O binary $< $@

# Left and right side of the rule are the same, but the right side
# is given as an absolute path.  
# (Make can not resolve such things automatically)
$(ULP_EXPORTS_HEADER): $(COMPONENT_BUILD_DIR)/$(ULP_EXPORTS_HEADER)

# Artificial intermediate target to trigger generation of .h and .ld files.
.INTERMEDIATE: $(COMPONENT_NAME)_ulp_mapgen_intermediate

$(COMPONENT_BUILD_DIR)/$(ULP_EXPORTS_HEADER)\
$(COMPONENT_BUILD_DIR)/$(ULP_EXPORTS_LD): $(COMPONENT_NAME)_ulp_mapgen_intermediate

# Convert the symbols list into a header file and linker export script.
$(COMPONENT_NAME)_ulp_mapgen_intermediate: $(ULP_SYM)
	$(summary) ULP_MAPGEN $(notdir $<)
	$(ULP_MAP_GEN) -s $(ULP_SYM) -o $(ULP_EXPORTS_LD:.ld=)

# Building the component separately from the project should result in
# ULP files being built.  
build: $(COMPONENT_BUILD_DIR)/$(ULP_EXPORTS_HEADER) \
	$(COMPONENT_BUILD_DIR)/$(ULP_EXPORTS_LD) \
	$(COMPONENT_BUILD_DIR)/$(ULP_BIN)

# Objects listed as being dependent on $(ULP_EXPORTS_HEADER) must also
# depend on $(ULP_SYM), to order build steps correctly. 
$(ULP_EXP_DEP_OBJECTS) : $(ULP_EXPORTS_HEADER) $(ULP_SYM)

# Finally, set all the variables processed by the build system. 
COMPONENT_EXTRA_CLEAN := $(ULP_OBJECTS) \
			$(ULP_LD_SCRIPT) \
			$(ULP_PREPROCESSED) \
			$(ULP_ELF) $(ULP_BIN) \
			$(ULP_MAP) $(ULP_SYM) \
			$(ULP_EXPORTS_LD) \
			$(ULP_EXPORTS_HEADER) \
			$(ULP_DEP) \
			$(ULP_LISTINGS)

COMPONENT_EMBED_FILES := $(COMPONENT_BUILD_DIR)/$(ULP_BIN)
COMPONENT_ADD_LDFLAGS := -l$(COMPONENT_NAME) -T $(COMPONENT_BUILD_DIR)/$(ULP_EXPORTS_LD)
COMPONENT_EXTRA_INCLUDES := $(COMPONENT_BUILD_DIR)
