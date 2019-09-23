# This is the minimal test component makefile.
#
# The following line is needed to force the linker to include all the object
# files into the application, even if the functions in these object files
# are not referenced from outside (which is usually the case for unit tests).
# 
COMPONENT_ADD_LDFLAGS = -Wl,--whole-archive -l$(COMPONENT_NAME) -Wl,--no-whole-archive
