/**
 * @file lv_monkey.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_monkey.h"

#if LV_USE_MONKEY != 0

/*********************
 *      DEFINES
 *********************/
#define MONKEY_PERIOD_RANGE_MIN_DEF    100
#define MONKEY_PERIOD_RANGE_MAX_DEF    1000

/**********************
 *      TYPEDEFS
 **********************/
typedef struct _lv_monkey {
    lv_monkey_config_t config;
    lv_indev_drv_t indev_drv;
    lv_indev_data_t indev_data;
    lv_indev_t * indev;
    lv_timer_t * timer;
#if LV_USE_USER_DATA
    void * user_data;
#endif
} lv_monkey_t;

static const lv_key_t lv_key_map[] = {
    LV_KEY_UP,
    LV_KEY_DOWN,
    LV_KEY_RIGHT,
    LV_KEY_LEFT,
    LV_KEY_ESC,
    LV_KEY_DEL,
    LV_KEY_BACKSPACE,
    LV_KEY_ENTER,
    LV_KEY_NEXT,
    LV_KEY_PREV,
    LV_KEY_HOME,
    LV_KEY_END,
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_monkey_read_cb(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static int32_t lv_monkey_random(int32_t howsmall, int32_t howbig);
static void lv_monkey_timer_cb(lv_timer_t * timer);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_monkey_config_init(lv_monkey_config_t * config)
{
    lv_memset_00(config, sizeof(lv_monkey_config_t));
    config->type = LV_INDEV_TYPE_POINTER;
    config->period_range.min = MONKEY_PERIOD_RANGE_MIN_DEF;
    config->period_range.max = MONKEY_PERIOD_RANGE_MAX_DEF;
}

lv_monkey_t * lv_monkey_create(const lv_monkey_config_t * config)
{
    lv_monkey_t * monkey = lv_mem_alloc(sizeof(lv_monkey_t));
    LV_ASSERT_MALLOC(monkey);

    lv_memset_00(monkey, sizeof(lv_monkey_t));

    monkey->config = *config;

    lv_indev_drv_t * drv = &monkey->indev_drv;
    lv_indev_drv_init(drv);
    drv->type = config->type;
    drv->read_cb = lv_monkey_read_cb;
    drv->user_data = monkey;

    monkey->timer = lv_timer_create(lv_monkey_timer_cb, monkey->config.period_range.min, monkey);
    lv_timer_pause(monkey->timer);

    monkey->indev = lv_indev_drv_register(drv);

    return monkey;
}

lv_indev_t * lv_monkey_get_indev(lv_monkey_t * monkey)
{
    LV_ASSERT_NULL(monkey);
    return monkey->indev;
}

void lv_monkey_set_enable(lv_monkey_t * monkey, bool en)
{
    LV_ASSERT_NULL(monkey);
    en ? lv_timer_resume(monkey->timer) : lv_timer_pause(monkey->timer);
}

bool lv_monkey_get_enable(lv_monkey_t * monkey)
{
    LV_ASSERT_NULL(monkey);
    return !monkey->timer->paused;
}

#if LV_USE_USER_DATA

void lv_monkey_set_user_data(lv_monkey_t * monkey, void * user_data)
{
    LV_ASSERT_NULL(monkey);
    monkey->user_data = user_data;
}

void * lv_monkey_get_user_data(lv_monkey_t * monkey)
{
    LV_ASSERT_NULL(monkey);
    return monkey->user_data;
}

#endif

void lv_monkey_del(lv_monkey_t * monkey)
{
    LV_ASSERT_NULL(monkey);

    lv_timer_del(monkey->timer);
    lv_indev_delete(monkey->indev);
    lv_mem_free(monkey);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_monkey_read_cb(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    lv_monkey_t * monkey = indev_drv->user_data;

    data->btn_id = monkey->indev_data.btn_id;
    data->point = monkey->indev_data.point;
    data->enc_diff = monkey->indev_data.enc_diff;
    data->state = monkey->indev_data.state;
}

static int32_t lv_monkey_random(int32_t howsmall, int32_t howbig)
{
    if(howsmall >= howbig) {
        return howsmall;
    }
    int32_t diff = howbig - howsmall;
    return (int32_t)lv_rand(0, diff) + howsmall;
}

static void lv_monkey_timer_cb(lv_timer_t * timer)
{
    lv_monkey_t * monkey = timer->user_data;
    lv_indev_data_t * data = &monkey->indev_data;

    switch(monkey->indev_drv.type) {
        case LV_INDEV_TYPE_POINTER:
            data->point.x = (lv_coord_t)lv_monkey_random(0, LV_HOR_RES - 1);
            data->point.y = (lv_coord_t)lv_monkey_random(0, LV_VER_RES - 1);
            break;
        case LV_INDEV_TYPE_ENCODER:
            data->enc_diff = (int16_t)lv_monkey_random(monkey->config.input_range.min, monkey->config.input_range.max);
            break;
        case LV_INDEV_TYPE_BUTTON:
            data->btn_id = (uint32_t)lv_monkey_random(monkey->config.input_range.min, monkey->config.input_range.max);
            break;
        case LV_INDEV_TYPE_KEYPAD: {
                int32_t index = lv_monkey_random(0, sizeof(lv_key_map) / sizeof(lv_key_map[0]) - 1);
                data->key = lv_key_map[index];
                break;
            }
        default:
            break;
    }

    data->state = lv_monkey_random(0, 100) < 50 ? LV_INDEV_STATE_RELEASED : LV_INDEV_STATE_PRESSED;

    lv_timer_set_period(monkey->timer, lv_monkey_random(monkey->config.period_range.min, monkey->config.period_range.max));
}

#endif /*LV_USE_MONKEY*/
