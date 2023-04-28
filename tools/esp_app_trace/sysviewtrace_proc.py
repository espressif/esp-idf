#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
# This is python script to process various types trace data streams in SystemView format.
# Trace data can be provided in multiple trace files (one per CPU). After processing phase
# script prints report for every type of trace data stream which was found.
#

import argparse
import json
import logging
import os.path
import signal
import sys
import traceback

import espytrace.apptrace as apptrace
import espytrace.sysview as sysview


def main():

    verbosity_levels = [
        logging.CRITICAL,
        logging.ERROR,
        logging.WARNING,
        logging.INFO,
        logging.DEBUG
    ]

    parser = argparse.ArgumentParser(description='ESP32 SEGGER SystemView Trace Parsing Tool')

    parser.add_argument('trace_sources', help='Trace data sources. Format: [file://]/path/to/file.', nargs='+', type=str)
    parser.add_argument('--elf-file', '-b', help='Path to program ELF file.', type=str, default='')
    parser.add_argument('--tmo', '-w', help='Data wait timeout in sec. -1: infinite, 0: no wait', type=int, default=0)
    parser.add_argument('--dump-events', '-d', help='Dump all events.', action='store_true')
    parser.add_argument('--print-events', '-p', help='Print events of selected types. By default only reports are printed', action='store_true')
    parser.add_argument('--include-events', '-i', help='Events types to be included into report.', type=str, choices=['heap', 'log', 'all'], default='all')
    parser.add_argument('--toolchain', '-t', help='Toolchain prefix.', type=str, default='xtensa-esp32-elf-')
    parser.add_argument('--events-map', '-e', help='Events map file.', type=str, default=os.path.join(os.path.dirname(__file__), 'SYSVIEW_FreeRTOS.txt'))
    parser.add_argument('--to-json', '-j', help='Print JSON.', action='store_true', default=False)
    parser.add_argument('--verbose', '-v', help='Verbosity level. Default 1', choices=range(0, len(verbosity_levels)), type=int, default=1)
    args = parser.parse_args()

    def sig_int_handler(signum, frame):
        reader.cleanup()

    signal.signal(signal.SIGINT, sig_int_handler)

    include_events = {'heap': False, 'log': False}
    if args.include_events == 'all':
        for k in include_events:
            include_events[k] = True
    elif args.include_events == 'heap':
        include_events['heap'] = True
    elif args.include_events == 'log':
        include_events['log'] = True

    logging.basicConfig(level=verbosity_levels[args.verbose], format='[%(levelname)s] %(message)s')

    # parse trace files
    parsers = []
    for i, trace_source in enumerate(args.trace_sources):
        try:
            parser = sysview.SysViewMultiTraceDataParser(print_events=False, core_id=i)
            if include_events['heap']:
                parser.add_stream_parser(sysview.SysViewTraceDataParser.STREAMID_HEAP,
                                         sysview.SysViewHeapTraceDataParser(print_events=False, core_id=i))
            if include_events['log']:
                parser.add_stream_parser(sysview.SysViewTraceDataParser.STREAMID_LOG,
                                         sysview.SysViewLogTraceDataParser(print_events=False, core_id=i))
            parsers.append(parser)
        except Exception as e:
            logging.error('Failed to create data parser (%s)!', e)
            traceback.print_exc()
            sys.exit(2)
        reader = apptrace.reader_create(trace_source, args.tmo)
        if not reader:
            logging.error('Failed to create trace reader!')
            sys.exit(2)
        try:
            # logging.info("Parse trace from '{}'...".format(trace_source))
            logging.info("Parse trace from '%s'...", trace_source)
            sysview.parse_trace(reader, parser, args.events_map)
            logging.info('Parsing completed.')
        except (apptrace.ReaderTimeoutError, apptrace.ReaderShutdownRequest) as e:
            logging.info('Stop parsing trace. (%s)', e)
        except Exception as e:
            logging.error('Failed to parse trace (%s)!', e)
            parser.cleanup()
            traceback.print_exc()
            sys.exit(2)
        finally:
            reader.cleanup()

    # merge and process traces
    try:
        proc = sysview.SysViewMultiStreamTraceDataProcessor(traces=parsers, print_events=args.dump_events, keep_all_events=True if args.to_json else False)
        if include_events['heap']:
            proc.add_stream_processor(sysview.SysViewTraceDataParser.STREAMID_HEAP,
                                      sysview.SysViewHeapTraceDataProcessor(args.toolchain, args.elf_file, root_proc=proc, print_heap_events=args.print_events))
        if include_events['log']:
            proc.add_stream_processor(sysview.SysViewTraceDataParser.STREAMID_LOG,
                                      sysview.SysViewLogTraceDataProcessor(root_proc=proc, print_log_events=args.print_events))
    except Exception as e:
        logging.error('Failed to create data processor (%s)!', e)
        traceback.print_exc()
        sys.exit(2)

    try:
        logging.info("Process events from '%s'...", args.trace_sources)
        proc.merge_and_process()
        logging.info('Processing completed.')
    except Exception as e:
        logging.error('Failed to process trace (%s)!', e)
        traceback.print_exc()
        sys.exit(2)
    finally:
        if args.to_json:
            print(json.dumps(proc, cls=sysview.SysViewTraceDataJsonEncoder, indent=4, separators=(',', ': '), sort_keys=True))
        else:
            proc.print_report()
        proc.cleanup()


if __name__ == '__main__':
    main()
