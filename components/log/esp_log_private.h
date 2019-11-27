#pragma once
#include <stdbool.h>

void esp_log_impl_lock(void);
bool esp_log_impl_lock_timeout(void);
void esp_log_impl_unlock(void);
