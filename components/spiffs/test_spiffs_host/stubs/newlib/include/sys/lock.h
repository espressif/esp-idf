#pragma once

#include <time.h>

typedef int _lock_t;

void _lock_acquire(_lock_t *lock);
void _lock_close(_lock_t *lock);
void _lock_init(_lock_t *lock);
void _lock_release(_lock_t *lock);
