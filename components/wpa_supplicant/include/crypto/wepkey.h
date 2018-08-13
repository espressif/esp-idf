#ifndef WEPKEY_H
#define WEPKEY_H

#define WEPKEY_64_BYTES   5
#define WePKEY_128_BYTES  13

unsigned int wepkey_64(uint8_t *out, unsigned int size, const char *in, int n);
unsigned int wepkey_128(uint8_t *out, unsigned int size, const char *in, int n);

#endif /* WEPKEY_H */