from __future__ import print_function

import ttfw_idf

# Timer events
TIMER_EVENT_LIMIT = 3

TIMER_EXPIRY_HANDLING = "TIMER_EVENTS:TIMER_EVENT_EXPIRY: timer_expiry_handler, executed {} out of " + str(TIMER_EVENT_LIMIT) + " times"

# Task events
TASK_ITERATION_LIMIT = 5
TASK_UNREGISTRATION_LIMIT = 3

TASK_ITERATION_POST = "TASK_EVENTS:TASK_ITERATION_EVENT: posting to default loop, {} out of " + str(TASK_ITERATION_LIMIT)
TASK_ITERATION_HANDLING = "TASK_EVENTS:TASK_ITERATION_EVENT: task_iteration_handler, executed {} times"


def _test_timer_events(dut):
    dut.start_app()

    print("Checking timer events posting and handling")

    dut.expect("setting up")
    dut.expect("starting event sources")

    print("Finished setup")

    dut.expect("TIMER_EVENTS:TIMER_EVENT_STARTED: posting to default loop")
    print("Posted timer started event")
    dut.expect("TIMER_EVENTS:TIMER_EVENT_STARTED: timer_started_handler")
    dut.expect("TIMER_EVENTS:TIMER_EVENT_STARTED: timer_any_handler")
    dut.expect("TIMER_EVENTS:TIMER_EVENT_STARTED: all_event_handler")
    print("Handled timer started event")

    for expiries in range(1, TIMER_EVENT_LIMIT + 1):
        dut.expect("TIMER_EVENTS:TIMER_EVENT_EXPIRY: posting to default loop")
        print("Posted timer expiry event {} out of {}".format(expiries, TIMER_EVENT_LIMIT))

        if expiries >= TIMER_EVENT_LIMIT:
            dut.expect("TIMER_EVENTS:TIMER_EVENT_STOPPED: posting to default loop")
            print("Posted timer stopped event")

        dut.expect(TIMER_EXPIRY_HANDLING.format(expiries))
        dut.expect("TIMER_EVENTS:TIMER_EVENT_EXPIRY: timer_any_handler")
        dut.expect("TIMER_EVENTS:TIMER_EVENT_EXPIRY: all_event_handler")

        print("Handled timer expiry event {} out of {}".format(expiries, TIMER_EVENT_LIMIT))

    dut.expect("TIMER_EVENTS:TIMER_EVENT_STOPPED: timer_stopped_handler")
    dut.expect("TIMER_EVENTS:TIMER_EVENT_STOPPED: deleted timer event source")
    print("Handled timer stopped event")


def _test_iteration_events(dut):
    dut.start_app()

    print("Checking iteration events posting and handling")
    dut.expect("setting up")
    dut.expect("starting event sources")
    print("Finished setup")

    for iteration in range(1, TASK_ITERATION_LIMIT + 1):
        dut.expect(TASK_ITERATION_POST.format(iteration))
        print("Posted iteration {} out of {}".format(iteration, TASK_ITERATION_LIMIT))

        if iteration < TASK_UNREGISTRATION_LIMIT:
            dut.expect(TASK_ITERATION_HANDLING.format(iteration))
            dut.expect("TASK_EVENTS:TASK_ITERATION_EVENT: all_event_handler")
        elif iteration == TASK_UNREGISTRATION_LIMIT:
            dut.expect("TASK_EVENTS:TASK_ITERATION_EVENT: unregistering task_iteration_handler")
            dut.expect("TASK_EVENTS:TASK_ITERATION_EVENT: all_event_handler")
            print("Unregistered handler at iteration {} out of {}".format(iteration, TASK_ITERATION_LIMIT))
        else:
            dut.expect("TASK_EVENTS:TASK_ITERATION_EVENT: all_event_handler")

        print("Handled iteration {} out of {}".format(iteration, TASK_ITERATION_LIMIT))

    dut.expect("TASK_EVENTS:TASK_ITERATION_EVENT: deleting task event source")
    print("Deleted task event source")


@ttfw_idf.idf_example_test(env_tag='Example_WIFI')
def test_default_event_loop_example(env, extra_data):
    dut = env.get_dut('default_event_loop', 'examples/system/esp_event/default_event_loop')

    _test_iteration_events(dut)
    _test_timer_events(dut)


if __name__ == '__main__':
    test_default_event_loop_example()
