#
# Component Makefile
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

# esp_app_desc structure is added as an undefined symbol because otherwise the 
# linker will ignore this structure as it has no other files depending on it.
COMPONENT_ADD_LDFLAGS += -u esp_app_desc

ifndef IS_BOOTLOADER_BUILD
GET_PROJECT_VER ?=
ifeq ("${PROJECT_VER}", "")
ifeq ("$(wildcard ${PROJECT_PATH}/version.txt)","")

GET_PROJECT_VER := $(shell cd ${PROJECT_PATH} && git describe --always --tags --dirty 2> /dev/null)
ifeq ("${GET_PROJECT_VER}", "")
GET_PROJECT_VER := "1"
$(info Project is not inside a git repository, will not use 'git describe' to determine PROJECT_VER.)
endif

else
# read from version.txt
GET_PROJECT_VER := $(shell cat ${PROJECT_PATH}/version.txt)
endif
endif
# If ``PROJECT_VER`` variable set in project Makefile file, its value will be used.
# Else, if the ``$PROJECT_PATH/version.txt`` exists, its contents will be used as ``PROJECT_VER``.
# Else, if the project is located inside a Git repository, the output of git describe will be used.
# Otherwise, ``PROJECT_VER`` will be "1".

ifeq ("${PROJECT_VER}", "")
PROJECT_VER:= $(GET_PROJECT_VER)
else
PROJECT_VER:= $(PROJECT_VER)
endif

# cut PROJECT_VER and PROJECT_NAME to required 32 characters.
PROJECT_VER_CUT  := $(shell echo "$(PROJECT_VER)"  | cut -c 1-31)
PROJECT_NAME_CUT := $(shell echo "$(PROJECT_NAME)" | cut -c 1-31)

$(info App "$(PROJECT_NAME_CUT)" version: $(PROJECT_VER_CUT))

NEW_DEFINES:= "$(PROJECT_VER_CUT) $(PROJECT_NAME_CUT) $(IDF_VER)"
ifeq ("$(wildcard ${TMP_DEFINES})","")
OLD_DEFINES:= ""
else
OLD_DEFINES:= "$(shell cat $(TMP_DEFINES))"
endif

# If NEW_DEFINES (PROJECT_VER, PROJECT_NAME) were changed then rebuild only esp_app_desc.
ifneq (${NEW_DEFINES}, ${OLD_DEFINES})
$(shell echo $(NEW_DEFINES) > $(TMP_DEFINES); rm -f esp_app_desc.o;)
endif

esp_app_desc.o: CPPFLAGS += -D PROJECT_VER=\""$(PROJECT_VER_CUT)"\" -D PROJECT_NAME=\""$(PROJECT_NAME_CUT)"\"
endif
