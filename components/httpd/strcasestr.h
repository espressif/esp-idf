#ifndef STRCASESTR_H
#define STRCASESTR_H

#include <sys/types.h>

char *strcasestr(const char *s, const char *find);
char *strncasestr(const char *s, const char * find, size_t slen);

#endif /* STRCASESTR_H */
