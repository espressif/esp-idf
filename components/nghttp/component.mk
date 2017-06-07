#
# Component Makefile
#

COMPONENT_ADD_INCLUDEDIRS := port/include nghttp2/lib/includes

COMPONENT_SRCDIRS := nghttp2/lib

# nghttp2_session.c uses assert(0) in place of abort() in some functions,
# that miss a return statement if assertions are disabled.
# So it always needs assertions to be enabled
nghttp2/lib/nghttp2_session.o: CPPFLAGS := $(filter-out -DNDEBUG,$(CPPFLAGS))
