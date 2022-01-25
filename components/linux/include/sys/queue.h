#if __has_include(<bsd/sys/queue.h>)
/* On Linux, try using sys/queue.h provided by libbsd-dev */
#include <bsd/sys/queue.h>
#else
/* Fall back to sys/queue.h which may exist e.g. on macOS */
#include_next <sys/queue.h>
#endif
