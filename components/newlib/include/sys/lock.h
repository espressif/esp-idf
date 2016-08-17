#ifndef _XTENSA_LOCK_H__
#define _XTENSA_LOCK_H__

/* generic lock implementation.

   Weak linked stub _lock functions in lock.c, can be
   replaced with a lock implementation at link time.

 */

typedef int _lock_t;
typedef _lock_t _LOCK_RECURSIVE_T;
typedef _lock_t _LOCK_T;

#include <_ansi.h>

/* NOTE: some parts of newlib statically initialise locks via
   __LOCK_INIT, some initialise at runtime via __lock_init. So need to
   support possibility that a _lock_t is null during first call to
   _lock_acquire or _lock_try_acquire.

   Lock functions all take a pointer to the _lock_t entry, so the
   value stored there can be manipulated.
*/
#define __LOCK_INIT(CLASS,NAME) CLASS _lock_t NAME = 0;
#define __LOCK_INIT_RECURSIVE(CLASS,NAME) CLASS _lock_t NAME = 0;

void _lock_init(_lock_t *lock);
void _lock_init_recursive(_lock_t *lock);
void _lock_close(_lock_t *lock);
void _lock_close_recursive(_lock_t *lock);
void _lock_acquire(_lock_t *lock);
void _lock_acquire_recursive(_lock_t *lock);
int _lock_try_acquire(_lock_t *lock);
int _lock_try_acquire_recursive(_lock_t *lock);
void _lock_release(_lock_t *lock);
void _lock_release_recursive(_lock_t *lock);

#define __lock_init(lock) _lock_init(&(lock))
#define __lock_init_recursive(lock) _lock_init_recursive(&(lock))
#define __lock_close(lock) _lock_close(&(lock))
#define __lock_close_recursive(lock) _lock_close_recursive(&(lock))
#define __lock_acquire(lock) _lock_acquire(&(lock))
#define __lock_acquire_recursive(lock) _lock_acquire_recursive(&(lock))
#define __lock_try_acquire(lock) _lock_try_acquire(&(lock))
#define __lock_try_acquire_recursive(lock) _lock_try_acquire_recursive(&(lock))
#define __lock_release(lock) _lock_release(&(lock))
#define __lock_release_recursive(lock) _lock_release_recursive(&(lock))

#endif /* _XTENSA_LOCK_H__ */
