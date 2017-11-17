
#ifndef sodium_version_H
#define sodium_version_H

#include <sodium/export.h>

/* IMPORTANT: As we don't use autotools, these version are not automatically
   updated if we change submodules. They need to be changed manually.
*/

#define SODIUM_VERSION_STRING "1.0.12-idf"

/* Note: these are not the same as the overall version, see
   configure.ac for the relevant macros */
#define SODIUM_LIBRARY_VERSION_MAJOR 9
#define SODIUM_LIBRARY_VERSION_MINOR 4

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
const char *sodium_version_string(void);

SODIUM_EXPORT
int         sodium_library_version_major(void);

SODIUM_EXPORT
int         sodium_library_version_minor(void);

#ifdef __cplusplus
}
#endif

#endif
