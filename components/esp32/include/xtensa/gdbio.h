/*  Xtensa Debug-FileSystem definitions
 *
 * Copyright (c) 2006-2009 by Tensilica Inc.  ALL RIGHTS RESERVED.
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
#include <errno.h>

  int _gdbio_open_r(void * ptr, const char *pathname, int flags, mode_t mode);
  int _gdbio_creat_r(void * ptr, const char *pathname, mode_t mode);
  int _gdbio_lseek_r(void * ptr, int fd, off_t offset, int whence);
  ssize_t _gdbio_write_r(void * ptr, int fd, const void * buf, size_t bytes);
  ssize_t _gdbio_read_r(void * ptr, int fd, void * buf, size_t bytes);
  int _gdbio_close_r(void * ptr, int fd);
  int _gdbio_unlink_r(void * ptr, const char * pathname);
  
  static inline
  int gdbio_open(const char *pathname, int flags, mode_t mode) {
    return _gdbio_open_r(&errno, pathname, flags, mode);
  }
  static inline int 
  gdbio_creat(const char *pathname, mode_t mode) {
    return _gdbio_open_r(&errno, pathname, O_CREAT|O_WRONLY|O_TRUNC, mode);
  }
  static inline int 
  gdbio_errno(void) {
    return errno;
  }
  static inline int 
  gdbio_lseek(int fd, off_t offset, int whence) {
    return _gdbio_lseek_r(&errno, fd, offset, whence);
  }
  static inline 
  ssize_t gdbio_write(int fd, const void * buf, size_t bytes) {
    return  _gdbio_write_r(&errno, fd, buf, bytes);
  }
  static inline
  ssize_t gdbio_read(int fd, void * buf, size_t bytes) {
    return  _gdbio_read_r(&errno, fd, buf, bytes);
  }
  static inline int 
  gdbio_close(int fd) {
    return _gdbio_close_r(&errno, fd);
  }
  static inline int 
  gdbio_unlink(const char * pathname) {
    return _gdbio_unlink_r(&errno, pathname);
  }

#ifdef REPLACE_FS_WITH_GDBIO
#define open gdbio_open
#define close gdbio_close
#define creat gdbio_creat
#define lseek gdbio_lseek
#define write gdbio_write
#define read gdbio_read
#define close gdbio_close
#define unlink gdbio_unlink

#endif

#ifdef __cplusplus
}
#endif

#endif
