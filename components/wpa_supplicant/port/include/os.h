/*
 * OS specific functions
 * Copyright (c) 2005-2009, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#ifndef OS_H
#define OS_H
#include "esp_types.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "esp_err.h"
#include "supplicant_opt.h"
#include "esp_wifi.h"

typedef time_t os_time_t;

/**
 * os_sleep - Sleep (sec, usec)
 * @sec: Number of seconds to sleep
 * @usec: Number of microseconds to sleep
 */
void os_sleep(os_time_t sec, os_time_t usec);

struct os_time {
	os_time_t sec;
	suseconds_t usec;
};

#define os_reltime os_time

struct os_tm {
    int sec; /* 0..59 or 60 for leap seconds */
    int min; /* 0..59 */
    int hour; /* 0..23 */
    int day; /* 1..31 */
    int month; /* 1..12 */
    int year; /* Four digit year */
};

/**
 * os_get_time - Get current time (sec, usec)
 * @t: Pointer to buffer for the time
 * Returns: 0 on success, -1 on failure
 */
int os_get_time(struct os_time *t);
#define os_get_reltime os_get_time

/* Helper macros for handling struct os_time */

#define os_time_before(a, b) \
	((a)->sec < (b)->sec || \
	 ((a)->sec == (b)->sec && (a)->usec < (b)->usec))

#define os_reltime_before os_time_before
#define os_time_sub(a, b, res) do { \
	(res)->sec = (a)->sec - (b)->sec; \
	(res)->usec = (a)->usec - (b)->usec; \
	if ((res)->usec < 0) { \
		(res)->sec--; \
		(res)->usec += 1000000; \
	} \
} while (0)
#define os_reltime_sub os_time_sub

/**
 * os_mktime - Convert broken-down time into seconds since 1970-01-01
 * @year: Four digit year
 * @month: Month (1 .. 12)
 * @day: Day of month (1 .. 31)
 * @hour: Hour (0 .. 23)
 * @min: Minute (0 .. 59)
 * @sec: Second (0 .. 60)
 * @t: Buffer for returning calendar time representation (seconds since
 * 1970-01-01 00:00:00)
 * Returns: 0 on success, -1 on failure
 *
 * Note: The result is in seconds from Epoch, i.e., in UTC, not in local time
 * which is used by POSIX mktime().
 */
int os_mktime(int year, int month, int day, int hour, int min, int sec,
	      os_time_t *t);

int os_gmtime(os_time_t t, struct os_tm *tm);

/**
 * os_daemonize - Run in the background (detach from the controlling terminal)
 * @pid_file: File name to write the process ID to or %NULL to skip this
 * Returns: 0 on success, -1 on failure
 */
int os_daemonize(const char *pid_file);

/**
 * os_daemonize_terminate - Stop running in the background (remove pid file)
 * @pid_file: File name to write the process ID to or %NULL to skip this
 */
void os_daemonize_terminate(const char *pid_file);

/**
 * os_get_random - Get cryptographically strong pseudo random data
 * @buf: Buffer for pseudo random data
 * @len: Length of the buffer
 * Returns: 0 on success, -1 on failure
 */
int os_get_random(unsigned char *buf, size_t len);

/**
 * os_random - Get pseudo random value (not necessarily very strong)
 * Returns: Pseudo random value
 */
unsigned long os_random(void);

/**
 * os_rel2abs_path - Get an absolute path for a file
 * @rel_path: Relative path to a file
 * Returns: Absolute path for the file or %NULL on failure
 *
 * This function tries to convert a relative path of a file to an absolute path
 * in order for the file to be found even if current working directory has
 * changed. The returned value is allocated and caller is responsible for
 * freeing it. It is acceptable to just return the same path in an allocated
 * buffer, e.g., return strdup(rel_path). This function is only used to find
 * configuration files when os_daemonize() may have changed the current working
 * directory and relative path would be pointing to a different location.
 */
char * os_rel2abs_path(const char *rel_path);

/**
 * os_program_init - Program initialization (called at start)
 * Returns: 0 on success, -1 on failure
 *
 * This function is called when a programs starts. If there are any OS specific
 * processing that is needed, it can be placed here. It is also acceptable to
 * just return 0 if not special processing is needed.
 */
int os_program_init(void);

/**
 * os_program_deinit - Program deinitialization (called just before exit)
 *
 * This function is called just before a program exists. If there are any OS
 * specific processing, e.g., freeing resourced allocated in os_program_init(),
 * it should be done here. It is also acceptable for this function to do
 * nothing.
 */
void os_program_deinit(void);

/**
 * os_setenv - Set environment variable
 * @name: Name of the variable
 * @value: Value to set to the variable
 * @overwrite: Whether existing variable should be overwritten
 * Returns: 0 on success, -1 on error
 *
 * This function is only used for wpa_cli action scripts. OS wrapper does not
 * need to implement this if such functionality is not needed.
 */
int os_setenv(const char *name, const char *value, int overwrite);

/**
 * os_unsetenv - Delete environent variable
 * @name: Name of the variable
 * Returns: 0 on success, -1 on error
 *
 * This function is only used for wpa_cli action scripts. OS wrapper does not
 * need to implement this if such functionality is not needed.
 */
int os_unsetenv(const char *name);

/**
 * os_readfile - Read a file to an allocated memory buffer
 * @name: Name of the file to read
 * @len: For returning the length of the allocated buffer
 * Returns: Pointer to the allocated buffer or %NULL on failure
 *
 * This function allocates memory and reads the given file to this buffer. Both
 * binary and text files can be read with this function. The caller is
 * responsible for freeing the returned buffer with os_free().
 */
/* We don't support file reading support */
static inline char *os_readfile(const char *name, size_t *len)
{
	return NULL;
}

/*
 * The following functions are wrapper for standard ANSI C or POSIX functions.
 * By default, they are just defined to use the standard function name and no
 * os_*.c implementation is needed for them. This avoids extra function calls
 * by allowing the C pre-processor take care of the function name mapping.
 *
 * If the target system uses a C library that does not provide these functions,
 * build_config.h can be used to define the wrappers to use a different
 * function name. This can be done on function-by-function basis since the
 * defines here are only used if build_config.h does not define the os_* name.
 * If needed, os_*.c file can be used to implement the functions that are not
 * included in the C library on the target system. Alternatively,
 * OS_NO_C_LIB_DEFINES can be defined to skip all defines here in which case
 * these functions need to be implemented in os_*.c file for the target system.
 */

#ifndef os_malloc
#define os_malloc(s) malloc((s))
#endif
#ifndef os_realloc
#define os_realloc(p, s) realloc((p), (s))
#endif
#ifndef os_zalloc
#define os_zalloc(s) calloc(1, (s))
#endif
#ifndef os_calloc
#define os_calloc(p, s) calloc((p), (s))
#endif

#ifndef os_free
#define os_free(p) free((p))
#endif

#ifndef os_bzero
#define os_bzero(s, n) bzero(s, n)
#endif


#ifndef os_strdup
#ifdef _MSC_VER
#define os_strdup(s) _strdup(s)
#else
#define os_strdup(s) strdup(s)
#endif
#endif
char * ets_strdup(const char *s);

#ifndef os_memcpy
#define os_memcpy(d, s, n) memcpy((d), (s), (n))
#endif
#ifndef os_memmove
#define os_memmove(d, s, n) memmove((d), (s), (n))
#endif
#ifndef os_memset
#define os_memset(s, c, n) memset(s, c, n)
#endif
#ifndef os_memcmp
#define os_memcmp(s1, s2, n) memcmp((s1), (s2), (n))
#endif
#ifndef os_memcmp_const
#define os_memcmp_const(s1, s2, n) memcmp((s1), (s2), (n))
#endif

#ifndef os_strlen
#define os_strlen(s) strlen(s)
#endif
#ifndef os_strcasecmp
#ifdef _MSC_VER
#define os_strcasecmp(s1, s2) _stricmp((s1), (s2))
#else
#define os_strcasecmp(s1, s2) strcasecmp((s1), (s2))
#endif
#endif
#ifndef os_strncasecmp
#ifdef _MSC_VER
#define os_strncasecmp(s1, s2, n) _strnicmp((s1), (s2), (n))
#else
#define os_strncasecmp(s1, s2, n) strncasecmp((s1), (s2), (n))
#endif
#endif
#ifndef os_strchr
#define os_strchr(s, c) strchr((s), (c))
#endif
#ifndef os_strcmp
#define os_strcmp(s1, s2) strcmp((s1), (s2))
#endif
#ifndef os_strncmp
#define os_strncmp(s1, s2, n) strncmp((s1), (s2), (n))
#endif
#ifndef os_strrchr
#define os_strrchr(s, c)  strrchr((s), (c))
#endif
#ifndef os_strstr
#define os_strstr(h, n) strstr((h), (n))
#endif
#ifndef os_strlcpy
#define os_strlcpy(d, s, n) strlcpy((d), (s), (n))
#endif
#ifndef os_strcat
#define os_strcat(d, s) strcat((d), (s))
#endif

#ifndef os_snprintf
#ifdef _MSC_VER
#define os_snprintf _snprintf
#else
#define os_snprintf snprintf
#endif
#endif
#ifndef os_sprintf
#define os_sprintf sprintf
#endif

static inline int os_snprintf_error(size_t size, int res)
{
        return res < 0 || (unsigned int) res >= size;
}

static inline void * os_realloc_array(void *ptr, size_t nmemb, size_t size)
{
	if (size && nmemb > (~(size_t) 0) / size)
		return NULL;
	return os_realloc(ptr, nmemb * size);
}

#ifdef CONFIG_CRYPTO_MBEDTLS
void forced_memzero(void *ptr, size_t len);
#else
/* Try to prevent most compilers from optimizing out clearing of memory that
 * becomes unaccessible after this function is called. This is mostly the case
 * for clearing local stack variables at the end of a function. This is not
 * exactly perfect, i.e., someone could come up with a compiler that figures out
 * the pointer is pointing to memset and then end up optimizing the call out, so
 * try go a bit further by storing the first octet (now zero) to make this even
 * a bit more difficult to optimize out. Once memset_s() is available, that
 * could be used here instead. */
static void * (* const volatile memset_func)(void *, int, size_t) = memset;
static uint8_t forced_memzero_val;

static inline void forced_memzero(void *ptr, size_t len)
{
	memset_func(ptr, 0, len);
	if (len) {
		forced_memzero_val = ((uint8_t *) ptr)[0];
	}
}
#endif

extern const wifi_osi_funcs_t *wifi_funcs;
#define OS_BLOCK OSI_FUNCS_TIME_BLOCKING

#define os_mutex_lock(a) wifi_funcs->_mutex_lock((a))
#define os_mutex_unlock(a) wifi_funcs->_mutex_unlock((a))
#define os_recursive_mutex_create() wifi_funcs->_recursive_mutex_create()

#define os_queue_create(a, b) wifi_funcs->_queue_create((a), (b))
#define os_queue_delete(a) wifi_funcs->_queue_delete(a)
#define os_queue_send(a, b, c) wifi_funcs->_queue_send((a), (b), (c))
#define os_queue_recv(a, b, c) wifi_funcs->_queue_recv((a), (b), (c))

#define os_task_create(a,b,c,d,e,f) wifi_funcs->_task_create((a), (b), (c), (d), (e), (f))
#define os_task_delete(a) wifi_funcs->_task_delete((a))
#define os_task_get_current_task() wifi_funcs->_task_get_current_task()

#define os_semphr_create(a, b) wifi_funcs->_semphr_create((a), (b))
#define os_semphr_delete(a) wifi_funcs->_semphr_delete((a))
#define os_semphr_give(a) wifi_funcs->_semphr_give((a))
#define os_semphr_take(a, b) wifi_funcs->_semphr_take((a), (b))

#define os_task_ms_to_tick(a) wifi_funcs->_task_ms_to_tick((a))
#define os_timer_get_time(void) wifi_funcs->_esp_timer_get_time(void)

static inline void os_timer_setfn(void *ptimer, void *pfunction, void *parg)
{
       return wifi_funcs->_timer_setfn(ptimer, pfunction, parg);
}
static inline void os_timer_disarm(void *ptimer)
{
       return wifi_funcs->_timer_disarm(ptimer);
}
static inline void os_timer_arm_us(void *ptimer,uint32_t u_seconds,bool repeat_flag)
{
       return wifi_funcs->_timer_arm_us(ptimer, u_seconds, repeat_flag);
}
static inline void os_timer_arm(void *ptimer,uint32_t milliseconds,bool repeat_flag)
{
       return wifi_funcs->_timer_arm(ptimer, milliseconds, repeat_flag);
}
static inline void os_timer_done(void *ptimer)
{
       return wifi_funcs->_timer_done(ptimer);
}

#endif /* OS_H */
