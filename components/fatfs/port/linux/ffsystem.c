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

/* 1:Function succeeded, 0:Could not create the sync object */
int ff_cre_syncobj(BYTE vol, FF_SYNC_t* sobj)
{
    *sobj = NULL;
    return 1;
}

/* 1:Function succeeded, 0:Could not delete due to an error */
int ff_del_syncobj(FF_SYNC_t sobj)
{
    return 1;
}

/* 1:Function succeeded, 0:Could not acquire lock */
int ff_req_grant (FF_SYNC_t sobj)
{
    return 1;
}

void ff_rel_grant (FF_SYNC_t sobj)
{
}
