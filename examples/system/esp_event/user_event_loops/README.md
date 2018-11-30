# Event Loop Library ('esp_event') User Event Loops Example

This example demonstrates the creation and use of user event loops. This example is a supplement
to the `default_event_loop` example, if the default event loop is not sufficient for the user's use case.

Here are the things this example illustrates:

### Creating Event Loops

Creating a loop entails populating the structure `esp_event_loop_args_t` with the desired parameters and calling `esp_event_loop_create`. The call to `esp_event_loop_create` produces a handle to the loop, which is used to perform actions on that loop such as handler registration/unregistration and posting events.

### Running Event Loops

Depending on the parameters, the user can create either a loop with a dedicated task or one without. The purpose of the dedicated task is to unqueue events from the loop and execute its handlers. For loops without the dedicated task, the user should make a call to `esp_event_loop_run` in an application task.

### Handler Registration/Unregistration,

Handler registration and unregistration works the same way as the default event loop, just with a different API, `esp_event_handler_register_with` and `esp_event_handler_register_with` respectively. There are two things this example highlights: (1) the possibility of registering the same handler for different loops and (2) the ability to pass static data to handlers.

### Posting Events to the Default Event Loop

Posting events also works the same way as the default event loop, except with a different API, `esp_event_post_to`.

## Example Flow Explained

The example flow is best explained using the sample log output.

```
I (296) user_event_loops: setting up
I (296) user_event_loops: starting event source
I (296) user_event_loops: starting application task
I (296) user_event_loops: posting TASK_EVENTS:TASK_ITERATION_EVENT to loop_without_task, iteration 1 out of 10
I (316) user_event_loops: application_task: running application task
I (326) user_event_loops: handling TASK_EVENTS:TASK_ITERATION_EVENT from loop_without_task, iteration 1
I (826) user_event_loops: posting TASK_EVENTS:TASK_ITERATION_EVENT to loop_with_task, iteration 2 out of 10
I (826) user_event_loops: handling TASK_EVENTS:TASK_ITERATION_EVENT from loop_with_task, iteration 2
I (1326) user_event_loops: posting TASK_EVENTS:TASK_ITERATION_EVENT to loop_without_task, iteration 3 out of 10
I (1326) user_event_loops: handling TASK_EVENTS:TASK_ITERATION_EVENT from loop_without_task, iteration 3
I (1426) user_event_loops: application_task: running application task
I (1826) user_event_loops: posting TASK_EVENTS:TASK_ITERATION_EVENT to loop_with_task, iteration 4 out of 10
I (1826) user_event_loops: handling TASK_EVENTS:TASK_ITERATION_EVENT from loop_with_task, iteration 4
I (2326) user_event_loops: posting TASK_EVENTS:TASK_ITERATION_EVENT to loop_without_task, iteration 5 out of 10
I (2326) user_event_loops: handling TASK_EVENTS:TASK_ITERATION_EVENT from loop_without_task, iteration 5
I (2526) user_event_loops: application_task: running application task
I (2826) user_event_loops: posting TASK_EVENTS:TASK_ITERATION_EVENT to loop_with_task, iteration 6 out of 10
I (2826) user_event_loops: handling TASK_EVENTS:TASK_ITERATION_EVENT from loop_with_task, iteration 6
I (3326) user_event_loops: posting TASK_EVENTS:TASK_ITERATION_EVENT to loop_without_task, iteration 7 out of 10
I (3326) user_event_loops: handling TASK_EVENTS:TASK_ITERATION_EVENT from loop_without_task, iteration 7
I (3626) user_event_loops: application_task: running application task
I (3826) user_event_loops: posting TASK_EVENTS:TASK_ITERATION_EVENT to loop_with_task, iteration 8 out of 10
I (3826) user_event_loops: handling TASK_EVENTS:TASK_ITERATION_EVENT from loop_with_task, iteration 8
I (4326) user_event_loops: posting TASK_EVENTS:TASK_ITERATION_EVENT to loop_without_task, iteration 9 out of 10
I (4326) user_event_loops: handling TASK_EVENTS:TASK_ITERATION_EVENT from loop_without_task, iteration 9
I (4726) user_event_loops: application_task: running application task
I (4826) user_event_loops: posting TASK_EVENTS:TASK_ITERATION_EVENT to loop_with_task, iteration 10 out of 10
I (4826) user_event_loops: handling TASK_EVENTS:TASK_ITERATION_EVENT from loop_with_task, iteration 10
I (5826) user_event_loops: application_task: running application task
I (5826) user_event_loops: deleting task event source
I (6926) user_event_loops: application_task: running application task
I (8026) user_event_loops: application_task: running application task
I (9126) user_event_loops: application_task: running application task
...
```

### Setting

This example has a single event source: a task with a loop inside. Events are raised for the task event source when the loop iterates.

Two loops are created, one with a dedicated task and one without. Events are posted to either loops,depending on whether the iteration is odd or even. For the loop with a dedicated task, event handlers are automatically executed. However, for the loop without the dedicated task, a call to run the loop is made in one of the application tasks. As a result, the execution of the event handlers for this loop is interspersed with the execution of application task code.

### Step-by-Step Explanation

The following text explains the important points of this example's sample log output.

a.

```
I (296) user_event_loops: setting up
I (296) user_event_loops: starting event source
I (296) user_event_loops: starting application task
```
At this stage the two event loops are created, as well as the handlers for the iteration event registered. The event source is started, which will post the event to the appropriate loop. The application task which makes the call to run the loop without dedicated task, is also created and started.

b.
```
I (296) user_event_loops: posting TASK_EVENTS:TASK_ITERATION_EVENT to loop_without_task, iteration 1 out of 10
I (316) user_event_loops: application_task: running application task
I (326) user_event_loops: handling TASK_EVENTS:TASK_ITERATION_EVENT from loop_without_task, iteration 1
I (826) user_event_loops: posting TASK_EVENTS:TASK_ITERATION_EVENT to loop_with_task, iteration 2 out of 10
I (826) user_event_loops: handling TASK_EVENTS:TASK_ITERATION_EVENT from loop_with_task, iteration 2
I (1326) user_event_loops: posting TASK_EVENTS:TASK_ITERATION_EVENT to loop_without_task, iteration 3 out of 10
I (1326) user_event_loops: handling TASK_EVENTS:TASK_ITERATION_EVENT from loop_without_task, iteration 3
I (1426) user_event_loops: application_task: running application task
I (1826) user_event_loops: posting TASK_EVENTS:TASK_ITERATION_EVENT to loop_with_task, iteration 4 out of 10
...
```
In this section of the log we see the odd iterations posted to the loop without dedicated task, and the even iterations to the loop with a dedicated task. For the event with dedicated task, event handlers are executed automatically. The loop without a dedicated task, on the other hand, runs in the context of the application task.

c.

```
...
I (4826) user_event_loops: posting TASK_EVENTS:TASK_ITERATION_EVENT to loop_with_task, iteration 10 out of 10
I (4826) user_event_loops: handling TASK_EVENTS:TASK_ITERATION_EVENT from loop_with_task, iteration 10
I (5826) user_event_loops: application_task: running application task
I (5826) user_event_loops: deleting task event source
I (6926) user_event_loops: application_task: running application task
I (8026) user_event_loops: application_task: running application task
I (9126) user_event_loops: application_task: running application task
...
```

The last of the iteration event is posted, and the event source is deleted. Because the loop without the task no longer receive events to execute, only the application task code executes.

---

See the README.md file in the upper level 'examples' directory for more information about examples.
