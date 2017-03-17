#ifndef __BT_APP_COMMON_H__
#define __BT_APP_COMMON_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define BT_APP_TRACE_LEVEL_NONE    0          /* No trace messages to be generated    */
#define BT_APP_TRACE_LEVEL_ERROR   1          /* Error condition trace messages       */
#define BT_APP_TRACE_LEVEL_WARNING 2          /* Warning condition trace messages     */
#define BT_APP_TRACE_LEVEL_API     3          /* API traces                           */
#define BT_APP_TRACE_LEVEL_EVENT   4          /* Debug messages for events            */
#define BT_APP_TRACE_LEVEL_DEBUG   5          /* Full debug messages                  */
#define BT_APP_TRACE_LEVEL_VERBOSE 6          /* Verbose debug messages               */

#define BT_APP_PRINTF      printf

extern int bt_app_trace_level;

#define BT_APP_TRACE_ERROR(fmt, args...)       {if (bt_app_trace_level >= BT_APP_TRACE_LEVEL_ERROR) BT_APP_PRINTF(fmt, ## args);}
#define BT_APP_TRACE_WARNING(fmt, args...)     {if (bt_app_trace_level >= BT_APP_TRACE_LEVEL_WARNING) BT_APP_PRINTF(fmt, ## args);}
#define BT_APP_TRACE_API(fmt, args...)     {if (bt_app_trace_level >= BT_APP_TRACE_LEVEL_API) BT_APP_PRINTF(fmt, ## args);}
#define BT_APP_TRACE_EVENT(fmt, args...)       {if (bt_app_trace_level >= BT_APP_TRACE_LEVEL_EVENT) BT_APP_PRINTF(fmt, ## args);}
#define BT_APP_TRACE_DEBUG(fmt, args...)       {if (bt_app_trace_level >= BT_APP_TRACE_LEVEL_DEBUG) BT_APP_PRINTF(fmt, ## args);}

#define BT_APP_SIG_CONTEXT_SWITCH         (0x90)

typedef void (* bt_app_cb_t) (uint16_t event, void *param);
/* context switch message */
typedef struct {
    uint16_t             sig;      /* signal to bt_app_task */
    uint16_t             event;    /* message event id */
    bt_app_cb_t          cb;       /* context switch callback */
    void                 *param;   /* parameter area needs to be last */
} bt_app_msg_t;

typedef void (* bt_app_copy_cb_t) (bt_app_msg_t *msg, void *p_dest, void *p_src);

bool bt_app_transfer_context (bt_app_cb_t p_cback, uint16_t event, void *p_params, int param_len, bt_app_copy_cb_t p_copy_cback);

void bt_app_task_start_up(void);

void bt_app_task_shut_down(void);
#endif /* __BT_APP_COMMON_H__ */
