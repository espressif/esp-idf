| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Default Event Loop Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

Note: Should users need to create their own event loops, refer to the **user event loops** [documentation](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/esp_event.html#using-esp-event-apis) and [example](https://github.com/espressif/esp-idf/tree/master/examples/system/esp_event/user_event_loops).

The [**default event loop**](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/esp_event.html#default-event-loop) is an [event loop](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/esp_event.html#) the system uses to post and handle events (e.g. Wi-Fi events). This example demonstrates the usage of the following default event loop features:

### Declaring and Defining Events

This example shows the typical setup of having the event base and event IDs declared in a header file, and having the definitions in a source file. Declaration of the event base makes use of the macro `ESP_EVENT_DECLARE_BASE()`, whilst the event IDs are declared as an `enum` (see `event_source.h`). The source file `main.c` holds the definition of the event base using the `ESP_EVENT_DEFINE_BASE()` macro.

### Creating the Default Event Loop

This example illustrates the creation of the default event loop using the API function `esp_event_loop_create_default()`.

### Posting Events to the Default Event Loop

Simply put, posting an event to a loop is the act of queueing its handlers for execution. For the default loop, this is done using the API `esp_event_post()`. The ability to pass event-specific data to the handler is also demonstrated.

### Handler Registration/Unregistration

This example demonstrates handler registration to the default event loop using `esp_event_handler_register` for (1) specific events, (2) **any** event under a certain base, and (3) **any** event. This also shows the possibility of registering multiple handlers to the same event as well as registering one handler to the same event multiple times.

Unregistering a handler is done using `esp_event_handler_unregister()`. Unregistering a handler means that it no longer executes even when the event it was previously registered to gets posted to the loop.

## How to use example

### Hardware Required

This example should be able to run on any commonly available ESP32 development board.

### Configure the project

```
idf.py menuconfig
```

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

The example should have the following log output:

```
I (328) default_event_loop: setting up
I (338) default_event_loop: starting event sources
I (338) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STARTED: posting to default loop
I (338) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: posting to default loop, 1 out of 5
I (358) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STARTED: timer_started_handler, instance 0
I (368) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STARTED: timer_started_handler, instance 1
I (378) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STARTED: timer_started_handler_2
I (388) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STARTED: timer_any_handler
I (388) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STARTED: all_event_handler
I (398) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: task_iteration_handler, executed 1 times
I (408) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: all_event_handler
I (858) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: posting to default loop, 2 out of 5
I (858) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: task_iteration_handler, executed 2 times
I (858) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: all_event_handler
I (1338) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: posting to default loop
I (1338) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: timer_expiry_handler, executed 1 out of 3 times
I (1348) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: timer_any_handler
I (1358) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: all_event_handler
I (1358) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: posting to default loop, 3 out of 5
I (1368) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: unregistering task_iteration_handler
I (1368) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: task_iteration_handler, executed 3 times
I (1388) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: all_event_handler
I (1898) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: posting to default loop, 4 out of 5
I (1898) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: all_event_handler
I (2338) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: posting to default loop
I (2338) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: timer_expiry_handler, executed 2 out of 3 times
I (2348) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: timer_any_handler
I (2358) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: all_event_handler
I (2398) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: posting to default loop, 5 out of 5
I (2398) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: all_event_handler
I (3338) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: posting to default loop
I (3338) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STOPPED: posting to default loop
I (3348) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: timer_expiry_handler, executed 3 out of 3 times
I (3358) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: timer_any_handler
I (3358) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: all_event_handler
I (3368) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STOPPED: timer_stopped_handler
I (3378) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STOPPED: deleted timer event source
I (3388) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STOPPED: timer_any_handler
I (3398) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STOPPED: all_event_handler
I (3398) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: deleting task event source
```

## Example Breakdown

### Setting of Event Sources

This example uses two event sources:

- A periodic timer. An event is raised when (1) the timer is started (2) the timer period expires and (3) the timer is stopped
- A task with a loop inside. An event is raised for the when (1) the loop iterates.

All of the events mentioned above have their own specific handler, however there are the following additional handlers.

- One handler executes when **any** event under the periodic timer event is posted
- The other handler executes if **any** event is posted.

The number of periodic timer expiries and loop iterations are limited. When the limit for the number of timer expiries is reached, the timer is stopped. When the limit for the number of iterations is reached, the task is deleted. In the case of the loop iteration, there is another limit: the number of iterations for when its handler will be unregistered.

### Step-by-Step Explanation

The following text explains the important points of the [sample log output](#Example-Output).

#### 1. Setting up of default event loop and event handlers

```
I (297)  default_event_loop: setting up
```
At this stage the default event loop is created, and the handlers for the different events are registered.


#### 2. Posting to the event loop

```
I (276) default_event_loop: starting event sources
I (276) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STARTED: posting to default loop
I (276) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: posting to default loop, 1 out of 5
```
The two event sources are started. The respective events are posted to the default event loop.

#### 3. Execution of handlers

```
I (358) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STARTED: timer_started_handler, instance 0
I (368) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STARTED: timer_started_handler, instance 1
I (378) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STARTED: timer_started_handler_2
I (306) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STARTED: timer_any_handler
I (316) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STARTED: all_event_handler
...
I (326) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: task_iteration_handler, executed 1 times
I (336) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: all_event_handler
```
The handlers are executed for the events that were posted in **(2)**. Note how `timer_started_handler()` is called twice on the same event post. This shows the ability to register a single event handler to a single event multiple times. Note also how `timer_started_handler_2()` is called on the same event post. This shows that multiple handlers can be registered to a single event.
In addition to the event-specific handlers `timer_started_handler()` (registered twice), `timer_started_handler_2()` and `task_iteration_handler()`, the `timer_any_handler()` and `all_event_handler()` are also executed.

The `timer_any_handler()` executes for **any** timer event. It can be seen executing for the timer expiry and timer stopped events in the subsequent parts of the log.

On the other hand, `all_event_handler()` executes for **any** event, hence why it executes for both ``TIMER_EVENTS:TIMER_EVENT_STARTED`` and ``TASK_EVENTS:TASK_ITERATION_EVENT``.

For both the timer and task events, notice that the handlers are executed in the same order they are registered in the code. This is a guarantee that the `esp_event` library provides.

The subsequent lines of the log follows the same pattern: the event is posted to the loop and the handlers are executed.

#### 4. Unregistering the `task_iteration_handler()`

```
...
I (1316) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: unregistering task_iteration_handler
```
At this point in the execution the handler `task_iteration_handler()` is unregistered, therefore it no longer executes when the event ``TASK_EVENTS:TASK_ITERATION_EVENT`` is posted.
```
I (1867) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: posting to default loop, 4 out of 5
I (1867) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: all_event_handler
...
I (1846) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: posting to default loop, 4 out of 5
I (1846) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: all_event_handle
```
The iteration event continues to get posted, but only the `all_event_handler()` gets executed.

#### 5. Iteration Limit

```
...
I (3276) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: posting to default loop
I (3276) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STOPPED: posting to default loop
I (3286) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: timer_expiry_handler, executed 3 out of 3 times
I (3296) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: timer_any_handler
I (3306) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: all_event_handler
I (3316) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STOPPED: timer_stopped_handler
I (3326) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STOPPED: deleted timer event source
```
When the periodic timer expiry limit is reached, the event ``TIMER_EVENTS:TIMER_EVENT_STOPPED`` is posted to the loop. The periodic timer is consequently deleted in the handler `timer_stopped_handler()`.

```
...
I (3346) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: deleting task event source
...
```
The task containing the loop that posts iteration events also gets deleted. The example ends at this point.
