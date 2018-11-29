/*------------------------------------------------------------------------*/
/* Sample Code of OS Dependent Functions for FatFs                        */
/* (C)ChaN, 2017                                                          */
/*------------------------------------------------------------------------*/


#include <string.h>
#include "ff.h"
#include "sdkconfig.h"
#ifdef CONFIG_FATFS_ALLOC_EXTRAM_FIRST
#include "esp_heap_caps.h"
#endif



/*------------------------------------------------------------------------*/
/* Allocate a memory block                                                */
/*------------------------------------------------------------------------*/

void* ff_memalloc (	/* Returns pointer to the allocated memory block (null on not enough core) */
	UINT msize		/* Number of bytes to allocate */
)
{
#ifdef CONFIG_FATFS_ALLOC_EXTRAM_FIRST
	return heap_caps_malloc_prefer(size, 2, MALLOC_CAP_DEFAULT | MALLOC_CAP_SPIRAM,
											MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
#else
	return malloc(msize);
#endif
}

/*------------------------------------------------------------------------*/
/* Allocate and zero out memory block                                     */
/*------------------------------------------------------------------------*/


void* ff_memcalloc (UINT num, UINT size)
{
#ifdef CONFIG_FATFS_ALLOC_EXTRAM_FIRST
	return heap_caps_calloc_prefer(num, size, 2, MALLOC_CAP_DEFAULT | MALLOC_CAP_SPIRAM,
												MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
#else
	return calloc(num, size);
#endif
}


/*------------------------------------------------------------------------*/
/* Free a memory block                                                    */
/*------------------------------------------------------------------------*/

void ff_memfree (
	void* mblock	/* Pointer to the memory block to free (nothing to do for null) */
)
{
	free(mblock);	/* Free the memory block with POSIX API */
}




#if FF_FS_REENTRANT	/* Mutal exclusion */

/*------------------------------------------------------------------------*/
/* Create a Synchronization Object                                        */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount() function to create a new
/  synchronization object for the volume, such as semaphore and mutex.
/  When a 0 is returned, the f_mount() function fails with FR_INT_ERR.
*/


int ff_cre_syncobj (	/* 1:Function succeeded, 0:Could not create the sync object */
	BYTE vol,			/* Corresponding volume (logical drive number) */
	FF_SYNC_t *sobj		/* Pointer to return the created sync object */
)
{
    *sobj = xSemaphoreCreateMutex();
    return (*sobj != NULL) ? 1 : 0;
}


/*------------------------------------------------------------------------*/
/* Delete a Synchronization Object                                        */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount() function to delete a synchronization
/  object that created with ff_cre_syncobj() function. When a 0 is returned,
/  the f_mount() function fails with FR_INT_ERR.
*/

int ff_del_syncobj (	/* 1:Function succeeded, 0:Could not delete due to an error */
	FF_SYNC_t sobj		/* Sync object tied to the logical drive to be deleted */
)
{
    vSemaphoreDelete(sobj);
    return 1;
}


/*------------------------------------------------------------------------*/
/* Request Grant to Access the Volume                                     */
/*------------------------------------------------------------------------*/
/* This function is called on entering file functions to lock the volume.
/  When a 0 is returned, the file function fails with FR_TIMEOUT.
*/

int ff_req_grant (	/* 1:Got a grant to access the volume, 0:Could not get a grant */
	FF_SYNC_t sobj	/* Sync object to wait */
)
{
    return (xSemaphoreTake(sobj, FF_FS_TIMEOUT) == pdTRUE) ? 1 : 0;
}


/*------------------------------------------------------------------------*/
/* Release Grant to Access the Volume                                     */
/*------------------------------------------------------------------------*/
/* This function is called on leaving file functions to unlock the volume.
*/

void ff_rel_grant (
	FF_SYNC_t sobj	/* Sync object to be signaled */
)
{
    xSemaphoreGive(sobj);
}

#endif
