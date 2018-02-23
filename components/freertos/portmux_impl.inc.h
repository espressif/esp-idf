/*
    Copyright (C) 2016-2017 Espressif Shanghai PTE LTD
    Copyright (C) 2015 Real Time Engineers Ltd.

    All rights reserved

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

	***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
	***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html
*/


/*
  Warning: funky preprocessor hackery ahead. Including these headers will generate two
  functions, which names are defined by the preprocessor macros 
  PORTMUX_AQUIRE_MUX_FN_NAME and PORTMUX_RELEASE_MUX_FN_NAME. In order to do the compare
  and exchange function, they will use whatever PORTMUX_COMPARE_SET_FN_NAME resolves to.

  In some scenarios, this header is included *twice* in portmux_impl.h: one time 
  for the 'normal' mux code which uses a compare&exchange routine, another time 
  to generate code for a second set of these routines that use a second mux 
  (in internal ram) to fake a compare&exchange on a variable in external memory.
*/



static inline bool __attribute__((always_inline))
#ifdef CONFIG_FREERTOS_PORTMUX_DEBUG
PORTMUX_AQUIRE_MUX_FN_NAME(portMUX_TYPE *mux, int timeout_cycles, const char *fnName, int line) {
#else
PORTMUX_AQUIRE_MUX_FN_NAME(portMUX_TYPE *mux, int timeout_cycles) {
#endif


#if !CONFIG_FREERTOS_UNICORE
	uint32_t res;
	portBASE_TYPE coreID, otherCoreID;
	uint32_t ccount_start;
	bool set_timeout = timeout_cycles > portMUX_NO_TIMEOUT;
#ifdef CONFIG_FREERTOS_PORTMUX_DEBUG
	if (!set_timeout) {
		timeout_cycles = 10000; // Always set a timeout in debug mode
		set_timeout = true;
	}
#endif
	if (set_timeout) { // Timeout
		RSR(CCOUNT, ccount_start);
	}

#ifdef CONFIG_FREERTOS_PORTMUX_DEBUG
	uint32_t owner = mux->owner;
	if (owner != portMUX_FREE_VAL && owner != CORE_ID_PRO && owner != CORE_ID_APP) {
		ets_printf("ERROR: vPortCPUAcquireMutex: mux %p is uninitialized (0x%X)! Called from %s line %d.\n", mux, owner, fnName, line);
		mux->owner=portMUX_FREE_VAL;
	}
#endif

	/* Spin until we own the core */

	RSR(PRID, coreID);
	/* Note: coreID is the full 32 bit core ID (CORE_ID_PRO/CORE_ID_APP),
	   not the 0/1 value returned by xPortGetCoreID()
	*/
	otherCoreID = CORE_ID_XOR_SWAP ^ coreID;
	do {
		/* mux->owner should be one of portMUX_FREE_VAL, CORE_ID_PRO,
		   CORE_ID_APP:

		   - If portMUX_FREE_VAL, we want to atomically set to 'coreID'.
		   - If "our" coreID, we can drop through immediately.
		   - If "otherCoreID", we spin here.
		 */
		res = coreID;
		PORTMUX_COMPARE_SET_FN_NAME(&mux->owner, portMUX_FREE_VAL, &res);

		if (res != otherCoreID) {
			break; // mux->owner is "our" coreID
		}

		if (set_timeout) {
			uint32_t ccount_now;
			RSR(CCOUNT, ccount_now);
			if (ccount_now - ccount_start > (unsigned)timeout_cycles) {
#ifdef CONFIG_FREERTOS_PORTMUX_DEBUG
				ets_printf("Timeout on mux! last non-recursive lock %s line %d, curr %s line %d\n", mux->lastLockedFn, mux->lastLockedLine, fnName, line);
				ets_printf("Owner 0x%x count %d\n", mux->owner, mux->count);
#endif
				return false;
			}
		}
	} while (1);

	assert(res == coreID || res == portMUX_FREE_VAL); /* any other value implies memory corruption or uninitialized mux */
	assert((res == portMUX_FREE_VAL) == (mux->count == 0)); /* we're first to lock iff count is zero */
    assert(mux->count < 0xFF); /* Bad count value implies memory corruption */

	/* now we own it, we can increment the refcount */
	mux->count++;


#ifdef CONFIG_FREERTOS_PORTMUX_DEBUG
	if (res==portMUX_FREE_VAL) { //initial lock
		mux->lastLockedFn=fnName;
		mux->lastLockedLine=line;
	} else {
		ets_printf("Recursive lock: count=%d last non-recursive lock %s line %d, curr %s line %d\n", mux->count-1,
				   mux->lastLockedFn, mux->lastLockedLine, fnName, line);
	}
#endif /* CONFIG_FREERTOS_PORTMUX_DEBUG */
#endif /* CONFIG_FREERTOS_UNICORE */
	return true;
}

#ifdef CONFIG_FREERTOS_PORTMUX_DEBUG
static inline void PORTMUX_RELEASE_MUX_FN_NAME(portMUX_TYPE *mux, const char *fnName, int line) {
#else
static inline void PORTMUX_RELEASE_MUX_FN_NAME(portMUX_TYPE *mux) {
#endif


#if !CONFIG_FREERTOS_UNICORE
	portBASE_TYPE coreID;
#ifdef CONFIG_FREERTOS_PORTMUX_DEBUG
	const char *lastLockedFn=mux->lastLockedFn;
	int lastLockedLine=mux->lastLockedLine;
	mux->lastLockedFn=fnName;
	mux->lastLockedLine=line;
	uint32_t owner = mux->owner;
	if (owner != portMUX_FREE_VAL && owner != CORE_ID_PRO && owner != CORE_ID_APP) {
		ets_printf("ERROR: vPortCPUReleaseMutex: mux %p is invalid (0x%x)!\n", mux, mux->owner);
	}
#endif

#if CONFIG_FREERTOS_PORTMUX_DEBUG || !defined(NDEBUG)
	RSR(PRID, coreID);
#endif

#ifdef CONFIG_FREERTOS_PORTMUX_DEBUG
	if (coreID != mux->owner) {
		ets_printf("ERROR: vPortCPUReleaseMutex: mux %p was already unlocked!\n", mux);
		ets_printf("Last non-recursive unlock %s line %d, curr unlock %s line %d\n", lastLockedFn, lastLockedLine, fnName, line);
	}
#endif

	assert(coreID == mux->owner); // This is a mutex we didn't lock, or it's corrupt
	assert(mux->count > 0); // Indicates memory corruption
	assert(mux->count < 0x100); // Indicates memory corruption

	mux->count--;
	if(mux->count == 0) {
		mux->owner = portMUX_FREE_VAL;
	}
#ifdef CONFIG_FREERTOS_PORTMUX_DEBUG_RECURSIVE
	else {
		ets_printf("Recursive unlock: count=%d last locked %s line %d, curr %s line %d\n", mux->count, lastLockedFn, lastLockedLine, fnName, line);
	}
#endif
#endif //!CONFIG_FREERTOS_UNICORE
}
