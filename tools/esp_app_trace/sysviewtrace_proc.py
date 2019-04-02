#!/usr/bin/env python
#
# Copyright 2019 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# This is python script to process various types trace data streams in SystemView format.
# Trace data can be provided in multiple trace files (one per CPU). After processing phase
# script prints report for every type of trace data stream which was found.
#

import argparse
import sys
import os.path
import signal
import traceback
import espytrace.apptrace as apptrace
import espytrace.sysview as sysview


def main():

    parser = argparse.ArgumentParser(description='ESP32 SEGGER SystemView Trace Parsing Tool')

    parser.add_argument('trace_sources', help='Trace data sources. Format: [file://]/path/to/file.', nargs='+', type=str)
    parser.add_argument('elf_file', help='Path to program ELF file.', type=str)
    parser.add_argument('--tmo', '-w', help='Data wait timeout in sec. -1: infinite, 0: no wait', type=int, default=0)
    parser.add_argument('--dump-events', '-d', help='Dump all events.', action='store_true')
    parser.add_argument('--print-events', '-p', help='Print events of selected types. By default only reports are printed', action='store_true')
    parser.add_argument('--include-events', '-i', help='Events types to be included into report.', type=str, choices=['heap', 'log', 'all'], default='all')
    parser.add_argument('--toolchain', '-t', help='Toolchain prefix.', type=str, default='xtensa-esp32-elf-')
    parser.add_argument('--events-map', '-e', help='Events map file.', type=str, default=os.path.join(os.path.dirname(__file__), 'SYSVIEW_FreeRTOS.txt'))
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
            print("Failed to create data parser ({})!".format(e))
            traceback.print_exc()
            sys.exit(2)
        reader = apptrace.reader_create(trace_source, args.tmo)
        if not reader:
            print("Failed to create trace reader!")
            sys.exit(2)
        try:
            print("Parse trace from '{}'...".format(trace_source))
            sysview.parse_trace(reader, parser, args.events_map)
            print("Parsing completed.")
        except (apptrace.ReaderTimeoutError, apptrace.ReaderShutdownRequest) as e:
            print("Stop parsing trace. ({})".format(e))
        except Exception as e:
            print("Failed to parse trace ({})!".format(e))
            parser.cleanup()
            traceback.print_exc()
            sys.exit(2)
        finally:
            reader.cleanup()

    # merge and process traces
    try:
        proc = sysview.SysViewMultiTraceDataProcessor(traces=parsers, print_events=args.dump_events)
        if include_events['heap']:
            proc.add_stream_processor(sysview.SysViewTraceDataParser.STREAMID_HEAP,
                                      sysview.SysViewHeapTraceDataProcessor(args.toolchain, args.elf_file, print_heap_events=args.print_events))
        if include_events['log']:
            proc.add_stream_processor(sysview.SysViewTraceDataParser.STREAMID_LOG,
                                      sysview.SysViewLogTraceDataProcessor(print_log_events=args.print_events))
    except Exception as e:
        print("Failed to create data processor ({})!".format(e))
        traceback.print_exc()
        sys.exit(2)
    try:
        print("Process events from '{}'...".format(args.trace_sources))
        proc.merge_and_process()
        print("Processing completed.")
    except Exception as e:
        print("Failed to process trace ({})!".format(e))
        traceback.print_exc()
        sys.exit(2)
    finally:
        proc.print_report()


if __name__ == '__main__':
    main()
