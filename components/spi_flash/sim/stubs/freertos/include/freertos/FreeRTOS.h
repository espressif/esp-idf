#pragma once

#include "projdefs.h"
#include "semphr.h"

// Avoid redefinition compile error. Put here since this is included
// in flash_ops.c.
#define spi_flash_init()                     overriden_spi_flash_init()
