/*  Xtensa Debug-FileSystem definitions  */

/*
 * Copyright (c) 2005-2009 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of Tensilica Inc.
 * They may not be modified, copied, reproduced, distributed, or disclosed to
 * third parties in any manner, medium, or form, in whole or in part, without
 * the prior written consent of Tensilica Inc.
 */

#ifndef __DEBUGFS_H__
#define __DEBUGFS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int xt_dbfs_open(const char *pathname, int flags, mode_t mode);
int xt_dbfs_ftruncate(int fd, off_t length);
int xt_dbfs_truncate(const char * filename, off_t length);
int xt_dbfs_creat(const char *pathname, mode_t mode);
int xt_dbfs_errno(void);
int xt_dbfs_lseek(int fd, off_t offset, int whence);
ssize_t xt_dbfs_write(int fd, const void * buf, size_t bytes);
  ssize_t xt_dbfs_open_append_close(const char * filename, int align,
				  const void * buf, size_t bytes);
ssize_t xt_dbfs_read(int fd, void * buf, size_t bytes);
int xt_dbfs_close(int fd);
int xt_dbfs_unlink(const char *pathname);

/* By default, this function is a wrapper around sbrk, and follows 
   sbrk semantics:
   
   On success, it returns increment bytes of memory allocated from 
   system memory.

   On failure, it returns 0xFFFFFFFF

   
   If you want to use a method of allocating memory other than sbrk,
   implement xt_dbfs_sbrk in your own sources, and the linker will 
   automatically use that copy.
*/
void * xt_dbfs_sbrk(int increment);



#ifdef REPLACE_FS_WITH_DBFS
#define open xt_dbfs_open
#define close xt_dbfs_close
#define creat xt_dbfs_creat
#define lseek xt_dbfs_lseek
#define write xt_dbfs_write
#define read xt_dbfs_read
#define close xt_dbfs_close
#define unlink xt_dbfs_unlink

#define rmdir NOT_IMPLEMENTED_IN_DBFS
#define opendir NOT_IMPLEMENTED_IN_DBFS
#define closedir NOT_IMPLEMENTED_IN_DBFS
#define dirfs NOT_IMPLEMENTED_IN_DBFS
#define readdir NOT_IMPLEMENTED_IN_DBFS
#define scandir NOT_IMPLEMENTED_IN_DBFS
#define seekdir NOT_IMPLEMENTED_IN_DBFS
#define telldir NOT_IMPLEMENTED_IN_DBFS

#define fcntl NOT_IMPLEMENTED_IN_DBFS
#define dup2 NOT_IMPLEMENTED_IN_DBFS
#define dup NOT_IMPLEMENTED_IN_DBFS
#define flock NOT_IMPLEMENTED_IN_DBFS
#define lockf NOT_IMPLEMENTED_IN_DBFS
#define link NOT_IMPLEMENTED_IN_DBFS
#define stat NOT_IMPLEMENTED_IN_DBFS
#define fstat NOT_IMPLEMENTED_IN_DBFS
#define lstat NOT_IMPLEMENTED_IN_DBFS
#define chmod NOT_IMPLEMENTED_IN_DBFS
#define fchmod NOT_IMPLEMENTED_IN_DBFS
#define chmown NOT_IMPLEMENTED_IN_DBFS
#define lchown NOT_IMPLEMENTED_IN_DBFS
#define fchown NOT_IMPLEMENTED_IN_DBFS

#endif

#ifdef __cplusplus
}
#endif

#endif
