#pragma once

#include <stdint.h>

/*
* Sets volume. Allowed range is  0-127
*/
void bt_app_set_volume(uint32_t level);

/*
* Changes an input data according to volume level. 
*/
void bt_app_adjust_volume(uint8_t *data, size_t size);