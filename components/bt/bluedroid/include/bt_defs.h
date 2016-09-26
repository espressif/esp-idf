/**
 * bt_defs.h    Defines useful API for whole Bluedroid
 * 
 */
#ifndef _BT_DEFS_H_
#define _BT_DEFS_H_

#include <stdlib.h>
#include <stdint.h>
#include "bt_trace.h"

#include "osi_arch.h"

#define UNUSED(x)                   (void)(x)

#ifndef SIZE_MAX
#define SIZE_MAX                    128
#endif
/*Timer Related Defination*/

#define alarm_timer_t               uint32_t
#define alarm_timer_setfn(timer, cb, data)           \
do {                                                 \
} while (0)
#define alarm_timer_arm(timer, to, periodic)         \
do {                                                 \
} while (0)
#define alarm_timer_disarm(timer)                    \
do {                                                 \
} while (0)
#define alarm_timer_now()             (0)


/*Thread and locker related defination*/
#define RTOS_SUPPORT
#ifdef RTOS_SUPPORT
#define pthread_mutex_t               osi_mutex_t
#define pthread_mutex_init(mutex, a)  osi_mutex_new(mutex)
#define pthread_mutex_destroy(mutex)  osi_mutex_free(mutex)
#define pthread_mutex_lock            osi_mutex_lock
#define pthread_mutex_unlock          osi_mutex_unlock
#else
#define pthread_mutex_t               uint8_t
#define pthread_mutex_init(x1, x2)    
#define pthread_mutex_destroy(mutex)  
#define pthread_mutex_lock(mutex)     
#define pthread_mutex_unlock(mutex)   
#endif


/*Bluetooth Address*/
typedef struct {
  uint8_t address[6];
} __attribute__ ((__packed__)) bt_bdaddr_t;


#ifndef CPU_LITTLE_ENDIAN
#define CPU_LITTLE_ENDIAN
#endif

inline uint16_t swap_byte_16(uint16_t x) {
  return (((x & 0x00ffU) << 8) |
          ((x & 0xff00U) >> 8));
}

inline uint32_t swap_byte_32(uint32_t x) {
  return (((x & 0x000000ffUL) << 24) |
          ((x & 0x0000ff00UL) << 8) |
          ((x & 0x00ff0000UL) >> 8) |
          ((x & 0xff000000UL) >> 24));
}

inline uint16_t ntohs(uint16_t x) {
#ifdef CPU_LITTLE_ENDIAN
  return swap_byte_16(x);
#else
  return x;
#endif
}

typedef struct vhci_host_callback {
    void (*notify_host_send_available)(void);
    int (*notify_host_recv)(uint8_t *data, uint16_t len);
} vhci_host_callback_t;

bool API_vhci_host_check_send_available(void);
void API_vhci_host_send_packet(uint8_t *data, uint16_t len);
void API_vhci_host_register_callback(const vhci_host_callback_t *callback);

#endif /* _BT_DEFS_H_ */
