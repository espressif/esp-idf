#
# Main Makefile. This is basically the same as a component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

COMPONENT_ADD_LDFRAGMENTS += linker.lf
COMPONENT_ADD_INCLUDEDIRS := .
