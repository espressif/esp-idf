/*
 * Since at least FreeRTOS V7.5.3 uxTopUsedPriority is no longer
 * present in the kernel, so it has to be supplied by other means for
 * OpenOCD's threads awareness.
 *
 * Add this file to your project, and, if you're using --gc-sections,
 * ``--undefined=uxTopUsedPriority'' (or
 * ``-Wl,--undefined=uxTopUsedPriority'' when using gcc for final
 * linking) to your LDFLAGS; same with all the other symbols you need.
 */

#include "FreeRTOS.h"
#include "esp_attr.h"
#include "sdkconfig.h"

#ifdef __GNUC__
#define USED __attribute__((used))
#else
#define USED
#endif

#ifdef CONFIG_FREERTOS_DEBUG_OCDAWARE
const int USED DRAM_ATTR uxTopUsedPriority = configMAX_PRIORITIES - 1;
#endif
