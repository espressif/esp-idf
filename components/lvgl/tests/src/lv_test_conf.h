/**
 * @file lv_test_conf.h
 *
 */

#ifndef LV_TEST_CONF_H
#define LV_TEST_CONF_H

#define LV_CONF_SUPPRESS_DEFINE_CHECK 1

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

void lv_test_assert_fail(void);
#define LV_ASSERT_HANDLER lv_test_assert_fail();

/**********************
 *      TYPEDEFS
 **********************/

uint32_t custom_tick_get(void);
#define LV_TICK_CUSTOM_SYS_TIME_EXPR custom_tick_get()

typedef void * lv_user_data_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEST_CONF_H*/
