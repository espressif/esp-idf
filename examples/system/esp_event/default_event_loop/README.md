# Event Loop Library ('esp_event') Default Event Loop Example

This example illustrates the basics of the event loop library. To keep the example simple, demonstration is limited to the use of the default event loop.
The default event loop is the event loop the system uses for its events. For some use cases, this could be sufficient. 
However, should the user need to create their own event loops, the example `user_event_loops` should be able to provide guidance.

Here are the things this example illustrates:

### Declaring and Defining Events

This example shows the typical setup of having the event base and event IDs declared in a header
file and then having the definitions in a source file. 

Declaration of the event base makes use of the macro `ESP_EVENT_DECLARE_BASE`, while the event IDs are declared as an `enum`; see
``event_source.h``. The source file ``main.c`` holds the definition of the event base using the macro `ESP_EVENT_DEFINE_BASE`.

### Creating the Default Event Loop

This example illustrates the creation of the default event loop using the API function `esp_event_loop_create_default`.

### Posting Events to the Default Event Loop

Simply put, posting an event to a loop is the act of queueing its handlers for execution. For the default loop, this is done using the API `esp_event_post`. The ability to pass event-specific data to the handler is also illustrated.

### Handler Registration/Unregistration

This example illustrates handler registration to the default event loop using `esp_event_handler_register` for (1) specific events, (2) *any* event under a certain base, and (3)  *any* event. This also shows the possbility of registering multiple handlers to the same event.

Unregistering a handler is done using `esp_event_handler_register`. Unregistering a handler means that it no longer executes even when the event it was previously registered to gets posted to the loop.

## Example Flow Explained

The example flow is best explained using a sample log output. 

```
I (297) default_event_loop: setting up
I (297) default_event_loop: starting event sources
I (297) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STARTED: posting to default loop
I (297) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: posting to default loop, 1 out of 5
I (317) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STARTED: all_event_handler
I (327) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STARTED: timer_any_handler
I (337) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STARTED: timer_started_handler
I (347) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: all_event_handler
I (347) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: task_iteration_handler, executed 1 times
I (827) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: posting to default loop, 2 out of 5
I (827) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: all_event_handler
I (827) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: task_iteration_handler, executed 2 times
I (1297) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: posting to default loop
I (1297) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: all_event_handler
I (1307) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: timer_any_handler
I (1317) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: timer_expiry_handler, executed 1 out of 3 times
I (1327) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: posting to default loop, 3 out of 5
I (1337) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: unregistering task_iteration_handler
I (1337) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: all_event_handler
I (1357) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: task_iteration_handler, executed 3 times
I (1867) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: posting to default loop, 4 out of 5
I (1867) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: all_event_handler
I (2297) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: posting to default loop
I (2297) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: all_event_handler
I (2307) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: timer_any_handler
I (2317) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: timer_expiry_handler, executed 2 out of 3 times
I (2367) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: posting to default loop, 5 out of 5
I (2367) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: all_event_handler
I (3297) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: posting to default loop
I (3297) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: all_event_handler
I (3307) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: timer_any_handler
I (3317) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STOPPED: posting to default loop
I (3327) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: timer_expiry_handler, executed 3 out of 3 times
I (3337) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STOPPED: all_event_handler
I (3337) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STOPPED: timer_any_handler
I (3347) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STOPPED: timer_stopped_handler
I (3357) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STOPPED: deleted timer event source
I (3367) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: deleting task event source
```

### Setting

This example uses two event sources: a periodic timer and a task with a loop inside. Events are raised for the periodic timer when (1) the timer is started (2) the timer period expires and (3) the timer is stopped. Events are raised for the when (1) the loop iterates.

All of the events mentioned above has their own specific handler. There are additional handlers, however. One handler executes when *any* event under the periodic timer event is posted; while the other executes if *any* event is posted.

The number of periodic timer expiries and loop iterations are limited. When the limit for the timer expiry is reached,
the timer is stopped. When the limit for the iterarations is reached, the task is deleted. In the case of the loop iteration, there is another limit: the number of iterations for when its handler will be unregistered. 

### Step-by-Step Explanation

The following text explains the important points of the sample log output.

a.

```
I (297)  default_event_loop: setting up
```
At this stage the default event loop is created, as well as the handlers for the different events registered.

b.
```
I (297) default_event_loop: starting event sources
I (297) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STARTED: posting to default loop
I (297) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: posting to default loop, 1 out of 5
```

The two event sources are started. The respective events are posted to the default event loop.

c.
```
I (317) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STARTED: all_event_handler
I (327) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STARTED: timer_any_handler
I (337) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STARTED: timer_started_handler
```
```
I (347) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: all_event_handler
I (347) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: task_iteration_handler, executed 1 times
```
The handlers are executed for the events posted in **(b)**. In addition to event-specific handlers `timer_started_handler`
and `task_iteration_handler`, the handlers `timer_any_handler` and `all_event_handler` also executed.

The handler `timer_any_handler` executes for *any* timer event. It can be seen executing for the timer expiry and timer stopped events in the upcoming parts of the log.

On the other hand, `all_event_handler` executes for *any* event. This is the reason why it executes for both ``TIMER_EVENTS:TIMER_EVENT_STARTED`` and ``TASK_EVENTS:TASK_ITERATION_EVENT``.

The proceeding lines of the log follows the same pattern: the event is posted to the loop and the handlers are executed.

d.

```
...
I (1337) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: unregistering task_iteration_handler
```
At this point in the execution the handler `task_iteration_handler` is unregistered, therefore it no longer executes
when the event ``TASK_EVENTS:TASK_ITERATION_EVENT`` is posted.
```
I (1867) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: posting to default loop, 4 out of 5
I (1867) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: all_event_handler

...

I (2367) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: posting to default loop, 5 out of 5
I (2367) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: all_event_handler
```
The iteration event continues to get posted, but only `all_event_handler` gets executed.

e.
```
...
I (3297) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: posting to default loop
I (3297) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: all_event_handler
I (3307) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: timer_any_handler
I (3317) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STOPPED: posting to default loop
I (3327) default_event_loop: TIMER_EVENTS:TIMER_EVENT_EXPIRY: timer_expiry_handler, executed 3 out of 3 times
...
I (3347) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STOPPED: timer_stopped_handler
I (3357) default_event_loop: TIMER_EVENTS:TIMER_EVENT_STOPPED: deleted timer event source
```
When the periodic timer expiry limit is reached, the event ``TIMER_EVENTS:TIMER_EVENT_STOPPED`` is posted to the loop. The periodic timer is consequently deleted in the handler `timer_stopped_handler`.

```
...
I (3367) default_event_loop: TASK_EVENTS:TASK_ITERATION_EVENT: deleting task event source
...
```
The task containing the loop that posts iteration events also gets deleted. The example ends at this point.

---

See the README.md file in the upper level 'examples' directory for more information about examples.
