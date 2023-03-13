/**
 * @file lv_line.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_line.h"

#if LV_USE_LINE != 0
#include "../misc/lv_assert.h"
#include "../draw/lv_draw.h"
#include "../misc/lv_math.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_line_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_line_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_line_event(const lv_obj_class_t * class_p, lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_line_class = {
    .constructor_cb = lv_line_constructor,
    .event_cb = lv_line_event,
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_line_t),
    .base_class = &lv_obj_class
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_line_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

void lv_line_set_points(lv_obj_t * obj, const lv_point_t points[], uint16_t point_num)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_line_t * line = (lv_line_t *)obj;
    line->point_array    = points;
    line->point_num      = point_num;

    lv_obj_refresh_self_size(obj);

    lv_obj_invalidate(obj);
}

void lv_line_set_y_invert(lv_obj_t * obj, bool en)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_line_t * line = (lv_line_t *)obj;
    if(line->y_inv == en) return;

    line->y_inv = en ? 1U : 0U;

    lv_obj_invalidate(obj);
}

/*=====================
 * Getter functions
 *====================*/

bool lv_line_get_y_invert(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_line_t * line = (lv_line_t *)obj;

    return line->y_inv == 1U;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_line_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_line_t * line = (lv_line_t *)obj;

    line->point_num   = 0;
    line->point_array = NULL;
    line->y_inv       = 0;

    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_line_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_res_t res;

    /*Call the ancestor's event handler*/
    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RES_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if(code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        /*The corner of the skew lines is out of the intended area*/
        lv_coord_t line_width = lv_obj_get_style_line_width(obj, LV_PART_MAIN);
        lv_coord_t * s = lv_event_get_param(e);
        if(*s < line_width) *s = line_width;
    }
    else if(code == LV_EVENT_GET_SELF_SIZE) {
        lv_line_t * line = (lv_line_t *)obj;

        if(line->point_num == 0 || line->point_array == NULL) return;

        lv_point_t * p = lv_event_get_param(e);
        lv_coord_t w = 0;
        lv_coord_t h = 0;

        uint16_t i;
        for(i = 0; i < line->point_num; i++) {
            w = LV_MAX(line->point_array[i].x, w);
            h = LV_MAX(line->point_array[i].y, h);
        }

        lv_coord_t line_width = lv_obj_get_style_line_width(obj, LV_PART_MAIN);
        w += line_width;
        h += line_width;
        p->x = w;
        p->y = h;
    }
    else if(code == LV_EVENT_DRAW_MAIN) {
        lv_line_t * line = (lv_line_t *)obj;
        lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(e);

        if(line->point_num == 0 || line->point_array == NULL) return;

        lv_area_t area;
        lv_obj_get_coords(obj, &area);
        lv_coord_t x_ofs = area.x1 - lv_obj_get_scroll_x(obj);
        lv_coord_t y_ofs = area.y1 - lv_obj_get_scroll_y(obj);
        lv_coord_t h = lv_obj_get_height(obj);

        lv_draw_line_dsc_t line_dsc;
        lv_draw_line_dsc_init(&line_dsc);
        lv_obj_init_draw_line_dsc(obj, LV_PART_MAIN, &line_dsc);

        /*Read all points and draw the lines*/
        uint16_t i;
        for(i = 0; i < line->point_num - 1; i++) {
            lv_point_t p1;
            lv_point_t p2;
            p1.x = line->point_array[i].x + x_ofs;
            p2.x = line->point_array[i + 1].x + x_ofs;

            if(line->y_inv == 0) {
                p1.y = line->point_array[i].y + y_ofs;
                p2.y = line->point_array[i + 1].y + y_ofs;
            }
            else {
                p1.y = h - line->point_array[i].y + y_ofs;
                p2.y = h - line->point_array[i + 1].y + y_ofs;
            }
            lv_draw_line(draw_ctx, &line_dsc, &p1, &p2);
            line_dsc.round_start = 0;   /*Draw the rounding only on the end points after the first line*/
        }
    }
}
#endif
