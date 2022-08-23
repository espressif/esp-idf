# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import logging

import pytest
from pytest_embedded import Dut

# Timer events
TIMER_EVENT_LIMIT = 3

TIMER_EXPIRY_HANDLING = 'TIMER_EVENTS:TIMER_EVENT_EXPIRY: timer_expiry_handler, executed {} out of ' + str(TIMER_EVENT_LIMIT) + ' times'

# Task events
TASK_ITERATION_LIMIT = 5
TASK_UNREGISTRATION_LIMIT = 3

TASK_ITERATION_POST = 'TASK_EVENTS:TASK_ITERATION_EVENT: posting to default loop, {} out of ' + str(TASK_ITERATION_LIMIT)
TASK_ITERATION_HANDLING = 'TASK_EVENTS:TASK_ITERATION_EVENT: task_iteration_handler, executed {} times'


@pytest.mark.supported_targets
@pytest.mark.generic
def test_default_event_loop_example_iteration_events(dut: Dut) -> None:
    logging.info('Checking iteration events posting and handling')
    dut.expect_exact('setting up')
    dut.expect_exact('starting event sources')
    logging.info('Finished setup')

    for iteration in range(1, TASK_ITERATION_LIMIT + 1):
        dut.expect_exact(TASK_ITERATION_POST.format(iteration))
        logging.info('Posted iteration {} out of {}'.format(iteration, TASK_ITERATION_LIMIT))

        if iteration < TASK_UNREGISTRATION_LIMIT:
            dut.expect_exact(TASK_ITERATION_HANDLING.format(iteration))
            dut.expect_exact('TASK_EVENTS:TASK_ITERATION_EVENT: all_event_handler')
        elif iteration == TASK_UNREGISTRATION_LIMIT:
            dut.expect(['TASK_EVENTS:TASK_ITERATION_EVENT: unregistering task_iteration_handler',
                        'TASK_EVENTS:TASK_ITERATION_EVENT: all_event_handler'], expect_all=True)
            logging.info('Unregistered handler at iteration {} out of {}'.format(iteration, TASK_ITERATION_LIMIT))
        else:
            dut.expect_exact('TASK_EVENTS:TASK_ITERATION_EVENT: all_event_handler')

        logging.info('Handled iteration {} out of {}'.format(iteration, TASK_ITERATION_LIMIT))

    dut.expect_exact('TASK_EVENTS:TASK_ITERATION_EVENT: deleting task event source')
    logging.info('Deleted task event source')


@pytest.mark.supported_targets
@pytest.mark.generic
def test_default_event_loop_example_timer_events(dut: Dut) -> None:
    logging.info('Checking timer events posting and handling')

    dut.expect_exact('setting up')
    dut.expect_exact('starting event sources')

    logging.info('Finished setup')

    dut.expect_exact('TIMER_EVENTS:TIMER_EVENT_STARTED: posting to default loop')
    logging.info('Posted timer started event')
    dut.expect_exact('TIMER_EVENTS:TIMER_EVENT_STARTED: timer_started_handler, instance 0')
    dut.expect_exact('TIMER_EVENTS:TIMER_EVENT_STARTED: timer_started_handler, instance 1')
    dut.expect_exact('TIMER_EVENTS:TIMER_EVENT_STARTED: timer_started_handler_2')
    dut.expect_exact('TIMER_EVENTS:TIMER_EVENT_STARTED: timer_any_handler')
    dut.expect_exact('TIMER_EVENTS:TIMER_EVENT_STARTED: all_event_handler')
    logging.info('Handled timer started event')

    for expiries in range(1, TIMER_EVENT_LIMIT + 1):
        dut.expect_exact('TIMER_EVENTS:TIMER_EVENT_EXPIRY: posting to default loop')
        logging.info('Posted timer expiry event {} out of {}'.format(expiries, TIMER_EVENT_LIMIT))

        if expiries >= TIMER_EVENT_LIMIT:
            dut.expect_exact('TIMER_EVENTS:TIMER_EVENT_STOPPED: posting to default loop')
            logging.info('Posted timer stopped event')

        dut.expect_exact(TIMER_EXPIRY_HANDLING.format(expiries))
        dut.expect_exact('TIMER_EVENTS:TIMER_EVENT_EXPIRY: timer_any_handler')
        dut.expect_exact('TIMER_EVENTS:TIMER_EVENT_EXPIRY: all_event_handler')

        logging.info('Handled timer expiry event {} out of {}'.format(expiries, TIMER_EVENT_LIMIT))

    dut.expect_exact('TIMER_EVENTS:TIMER_EVENT_STOPPED: timer_stopped_handler')
    dut.expect_exact('TIMER_EVENTS:TIMER_EVENT_STOPPED: deleted timer event source')
    logging.info('Handled timer stopped event')
