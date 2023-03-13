# Timers

LVGL has a built-in timer system. You can register a function to have it be called periodically. The timers are handled and called in `lv_timer_handler()`, which needs to be called every few milliseconds.
See [Porting](/porting/timer-handler) for more information.

Timers are non-preemptive, which means a timer cannot interrupt another timer. Therefore, you can call any LVGL related function in a timer.


## Create a timer
To create a new timer, use `lv_timer_create(timer_cb, period_ms, user_data)`. It will create an `lv_timer_t *` variable, which can be used later to modify the parameters of the timer.
`lv_timer_create_basic()` can also be used. This allows you to create a new timer without specifying any parameters.

A timer callback should have a `void (*lv_timer_cb_t)(lv_timer_t *);` prototype.

For example:
```c
void my_timer(lv_timer_t * timer)
{
  /*Use the user_data*/
  uint32_t * user_data = timer->user_data;
  printf("my_timer called with user data: %d\n", *user_data);

  /*Do something with LVGL*/
  if(something_happened) {
    something_happened = false;
    lv_btn_create(lv_scr_act(), NULL);
  }
}

...

static uint32_t user_data = 10;
lv_timer_t * timer = lv_timer_create(my_timer, 500,  &user_data);

```

## Ready and Reset

`lv_timer_ready(timer)` makes a timer run on the next call of `lv_timer_handler()`.

`lv_timer_reset(timer)` resets the period of a timer. It will be called again after the defined period of milliseconds has elapsed.


## Set parameters
You can modify some timer parameters later:
- `lv_timer_set_cb(timer, new_cb)`
- `lv_timer_set_period(timer, new_period)`

## Repeat count

You can make a timer repeat only a given number of times with `lv_timer_set_repeat_count(timer, count)`. The timer will automatically be deleted after it's called the defined number of times. Set the count to `-1` to repeat indefinitely.


## Measure idle time

You can get the idle percentage time of `lv_timer_handler` with `lv_timer_get_idle()`. Note that, it doesn't measure the idle time of the overall system, only `lv_timer_handler`.
It can be misleading if you use an operating system and call `lv_timer_handler` in a timer, as it won't actually measure the time the OS spends in an idle thread.

## Asynchronous calls

In some cases, you can't perform an action immediately. For example, you can't delete an object because something else is still using it, or you don't want to block the execution now.
For these cases, `lv_async_call(my_function, data_p)` can be used to call `my_function` on the next invocation of `lv_timer_handler`. `data_p` will be passed to the function when it's called.
Note that only the data pointer is saved, so you need to ensure that the variable will be "alive" while the function is called. It can be *static*, global or dynamically allocated data.
If you want to cancel an asynchronous call, call `lv_async_call_cancel(my_function, data_p)`, which will clear all asynchronous calls matching `my_function` and `data_p`.

For example:
```c
void my_screen_clean_up(void * scr)
{
  /*Free some resources related to `scr`*/

  /*Finally delete the screen*/
  lv_obj_del(scr);
}

...

/*Do something with the object on the current screen*/

/*Delete screen on next call of `lv_timer_handler`, not right now.*/
lv_async_call(my_screen_clean_up, lv_scr_act());

/*The screen is still valid so you can do other things with it*/

```

If you just want to delete an object and don't need to clean anything up in `my_screen_cleanup` you could just use `lv_obj_del_async` which will delete the object on the next call to `lv_timer_handler`.

## API

```eval_rst

.. doxygenfile:: lv_timer.h
  :project: lvgl

.. doxygenfile:: lv_async.h
  :project: lvgl

```
