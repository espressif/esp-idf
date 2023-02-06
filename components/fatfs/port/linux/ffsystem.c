/*------------------------------------------------------------------------*/
/* OS Dependent Functions for FatFs                                       */
/* (C)ChaN, 2018                                                          */
/*------------------------------------------------------------------------*/


#include "ff.h"
#include <stdlib.h>

/* This is the implementation for host-side testing on Linux.
 * Host-side tests are single threaded, so lock functionality isn't needed.
 */

void* ff_memalloc(UINT msize)
{
    return malloc(msize);
}

void ff_memfree(void* mblock)
{
    free(mblock);
}

static int* Mutex[FF_VOLUMES + 1]; /* Table of mutex handle */

/* 1:Function succeeded, 0:Could not create the mutex */
int ff_mutex_create(int vol)
{
    Mutex[vol] = NULL;
    return 1;
}

void ff_mutex_delete(int vol)
{
}

/* 1:Function succeeded, 0:Could not acquire lock */
int ff_mutex_take(int vol)
{
    return 1;
}

void ff_mutex_give(int vol)
{
}
