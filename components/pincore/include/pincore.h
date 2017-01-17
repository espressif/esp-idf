#ifndef __PINCORE_H_
#define __PINCORE_H_

#include "esp_log.h"

#ifdef CONFIG_PINCORE_MODULE_ENABLED

#define PINCORE_SELECT_CURRENT_CORE 0x00000100

#define PINCORE_START_CORE_PINNED_SECTION() \
	do { \
		int __pincore_save = 0x00000100; \
		ESP_LOGV("PINCORE", "[A] Pinning task to current core"); \
		pincore_alter_current_freertos_task(&__pincore_save); \
		ESP_LOGV("PINCORE", "[B] Task was pinned to %x",__pincore_save) 

#define PINCORE_STOP_CORE_PINNED_SECTION() \
		ESP_LOGV("PINCORE", "[C] Pinnig task back to %x",__pincore_save); \
		pincore_alter_current_freertos_task(&__pincore_save); \
		ESP_LOGV("PINCORE", "[D] Task was pinned to %x",__pincore_save); \
	} while(0)

#define PINCORE_RETURN_FROM_SECTION(x) \
	pincore_alter_current_freertos_task(&__pincore_save); \
	return x

#define PINCORE_EXIT_FROM_SECTION(x) \
	pincore_alter_current_freertos_task(&__pincore_save); \
	return

void pincore_alter_current_freertos_task(int * coreaff);

#else

#define PINCORE_START_CORE_PINNED_SECTION() \
	do {

#define PINCORE_STOP_CORE_PINNED_SECTION() \
	} while(0)

#define PINCORE_RETURN_FROM_SECTION(x) \
	return x

#define PINCORE_EXIT_FROM_SECTION() \
	return

#endif

#endif