/*------------------------------------------------------------------------*/
/* A Sample Code of User Provided OS Dependent Functions for FatFs        */
/*------------------------------------------------------------------------*/


#include <string.h>
#include <stdlib.h>
#include "ff.h"
#include "sdkconfig.h"
#ifdef CONFIG_FATFS_ALLOC_PREFER_EXTRAM
#include "esp_heap_caps.h"
#endif

/*------------------------------------------------------------------------*/
/* Allocate/Free a Memory Block                                           */
/*------------------------------------------------------------------------*/

void* ff_memalloc (    /* Returns pointer to the allocated memory block (null if not enough core) */
    unsigned msize     /* Number of bytes to allocate */
)
{
#ifdef CONFIG_FATFS_ALLOC_PREFER_EXTRAM
    return heap_caps_malloc_prefer(msize, 2, MALLOC_CAP_DEFAULT | MALLOC_CAP_SPIRAM,
                                            MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
#else
    return malloc(msize);
#endif
}


void ff_memfree (
    void* mblock    /* Pointer to the memory block to free (no effect if null) */
)
{
    free(mblock);   /* Free the memory block with POSIX API */
}





#if FF_FS_REENTRANT	/* Mutal exclusion */
/*------------------------------------------------------------------------*/
/* Definitions of Mutex                                                   */
/*------------------------------------------------------------------------*/

#define OS_TYPE	3	/* 0:Win32, 1:uITRON4.0, 2:uC/OS-II, 3:FreeRTOS, 4:CMSIS-RTOS */


#if   OS_TYPE == 0	/* Win32 */
#include <windows.h>
static HANDLE Mutex[FF_VOLUMES + 1];	/* Table of mutex handle */

#elif OS_TYPE == 1	/* uITRON */
#include "itron.h"
#include "kernel.h"
static mtxid Mutex[FF_VOLUMES + 1];		/* Table of mutex ID */

#elif OS_TYPE == 2	/* uc/OS-II */
#include "includes.h"
static OS_EVENT *Mutex[FF_VOLUMES + 1];	/* Table of mutex pointers */

#elif OS_TYPE == 3	/* FreeRTOS */
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
static SemaphoreHandle_t Mutex[FF_VOLUMES + 1];	/* Table of mutex handle */

#elif OS_TYPE == 4	/* CMSIS-RTOS */
#include "cmsis_os.h"
static osMutexId Mutex[FF_VOLUMES + 1];	/* Table of mutex ID */

#endif



/*------------------------------------------------------------------------*/
/* Create a Mutex                                                         */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount function to create a new mutex
/  or semaphore for the volume. When a 0 is returned, the f_mount function
/  fails with FR_INT_ERR.
*/

int ff_mutex_create (	/* Returns 1:Function succeeded or 0:Could not create the mutex */
	int vol				/* Mutex ID: Volume mutex (0 to FF_VOLUMES - 1) or system mutex (FF_VOLUMES) */
)
{
#if OS_TYPE == 0	/* Win32 */
	Mutex[vol] = CreateMutex(NULL, FALSE, NULL);
	return (int)(Mutex[vol] != INVALID_HANDLE_VALUE);

#elif OS_TYPE == 1	/* uITRON */
	T_CMTX cmtx = {TA_TPRI,1};

	Mutex[vol] = acre_mtx(&cmtx);
	return (int)(Mutex[vol] > 0);

#elif OS_TYPE == 2	/* uC/OS-II */
	OS_ERR err;

	Mutex[vol] = OSMutexCreate(0, &err);
	return (int)(err == OS_NO_ERR);

#elif OS_TYPE == 3	/* FreeRTOS */
	Mutex[vol] = xSemaphoreCreateMutex();
	return (int)(Mutex[vol] != NULL);

#elif OS_TYPE == 4	/* CMSIS-RTOS */
	osMutexDef(cmsis_os_mutex);

	Mutex[vol] = osMutexCreate(osMutex(cmsis_os_mutex));
	return (int)(Mutex[vol] != NULL);

#endif
}


/*------------------------------------------------------------------------*/
/* Delete a Mutex                                                         */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount function to delete a mutex or
/  semaphore of the volume created with ff_mutex_create function.
*/

void ff_mutex_delete (	/* Returns 1:Function succeeded or 0:Could not delete due to an error */
	int vol				/* Mutex ID: Volume mutex (0 to FF_VOLUMES - 1) or system mutex (FF_VOLUMES) */
)
{
#if OS_TYPE == 0	/* Win32 */
	CloseHandle(Mutex[vol]);

#elif OS_TYPE == 1	/* uITRON */
	del_mtx(Mutex[vol]);

#elif OS_TYPE == 2	/* uC/OS-II */
	OS_ERR err;

	OSMutexDel(Mutex[vol], OS_DEL_ALWAYS, &err);

#elif OS_TYPE == 3	/* FreeRTOS */
	vSemaphoreDelete(Mutex[vol]);

#elif OS_TYPE == 4	/* CMSIS-RTOS */
	osMutexDelete(Mutex[vol]);

#endif
}


/*------------------------------------------------------------------------*/
/* Request a Grant to Access the Volume                                   */
/*------------------------------------------------------------------------*/
/* This function is called on enter file functions to lock the volume.
/  When a 0 is returned, the file function fails with FR_TIMEOUT.
*/

int ff_mutex_take (	/* Returns 1:Succeeded or 0:Timeout */
	int vol			/* Mutex ID: Volume mutex (0 to FF_VOLUMES - 1) or system mutex (FF_VOLUMES) */
)
{
#if OS_TYPE == 0	/* Win32 */
	return (int)(WaitForSingleObject(Mutex[vol], FF_FS_TIMEOUT) == WAIT_OBJECT_0);

#elif OS_TYPE == 1	/* uITRON */
	return (int)(tloc_mtx(Mutex[vol], FF_FS_TIMEOUT) == E_OK);

#elif OS_TYPE == 2	/* uC/OS-II */
	OS_ERR err;

	OSMutexPend(Mutex[vol], FF_FS_TIMEOUT, &err));
	return (int)(err == OS_NO_ERR);

#elif OS_TYPE == 3	/* FreeRTOS */
	return (int)(xSemaphoreTake(Mutex[vol], FF_FS_TIMEOUT) == pdTRUE);

#elif OS_TYPE == 4	/* CMSIS-RTOS */
	return (int)(osMutexWait(Mutex[vol], FF_FS_TIMEOUT) == osOK);

#endif
}



/*------------------------------------------------------------------------*/
/* Release a Grant to Access the Volume                                   */
/*------------------------------------------------------------------------*/
/* This function is called on leave file functions to unlock the volume.
*/

void ff_mutex_give (
	int vol			/* Mutex ID: Volume mutex (0 to FF_VOLUMES - 1) or system mutex (FF_VOLUMES) */
)
{
#if OS_TYPE == 0	/* Win32 */
	ReleaseMutex(Mutex[vol]);

#elif OS_TYPE == 1	/* uITRON */
	unl_mtx(Mutex[vol]);

#elif OS_TYPE == 2	/* uC/OS-II */
	OSMutexPost(Mutex[vol]);

#elif OS_TYPE == 3	/* FreeRTOS */
	xSemaphoreGive(Mutex[vol]);

#elif OS_TYPE == 4	/* CMSIS-RTOS */
	osMutexRelease(Mutex[vol]);

#endif
}

#endif	/* FF_FS_REENTRANT */
