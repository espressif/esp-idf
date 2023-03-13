/**
 * @file lv_calendar_header_dropdown.h
 *
 */

#ifndef LV_CALENDAR_HEADER_DROPDOWN_H
#define LV_CALENDAR_HEADER_DROPDOWN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../core/lv_obj.h"
#if LV_USE_CALENDAR_HEADER_DROPDOWN

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
extern const lv_obj_class_t lv_calendar_header_dropdown_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a calendar header with drop-drowns to select the year and month
 * @param parent    pointer to a calendar object.
 * @return          the created header
 */
lv_obj_t * lv_calendar_header_dropdown_create(lv_obj_t * parent);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_CALENDAR_HEADER_ARROW*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CALENDAR_HEADER_DROPDOWN_H*/
