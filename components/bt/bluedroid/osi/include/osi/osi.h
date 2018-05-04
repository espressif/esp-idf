
#ifndef _OSI_H_
#define _OSI_H_

#include <stdbool.h>
#include <stdint.h>

#define UNUSED_ATTR __attribute__((unused))

#define CONCAT(a, b) a##b
#define COMPILE_ASSERT(x)

int osi_init(void);
void osi_deinit(void);

#endif /*_OSI_H_*/
