#!/usr/bin/env python
#

import argparse
import struct
import sys

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    
def main():
    ESP32_TRACE_BLOCK_HDR_SZ = 8
    ESP32_TRACE_BLOCK_TASK_IDX = 0
    ESP32_TRACE_BLOCK_TS_IDX = 1
    ESP32_TRACE_BLOCK_DATA_IDX = 2
    
    parser = argparse.ArgumentParser(description='ESP32 App Trace Parse Tool')

    parser.add_argument('file', help='Path to app trace file', type=str)
    parser.add_argument('--print-tasks', '-p', help='Print tasks', action='store_true')
    parser.add_argument('--print-details', '-d', help='Print detailed stats', action='store_true')
    parser.add_argument('--no-errors', '-n', help='Do not print errors', action='store_true')
    parser.add_argument('--block-len', '-b', help='Block length', type=int, default=1024)

    args = parser.parse_args()

    print "===================================================================="
    try:
        ftrc = open(args.file, 'rb')
    except IOError as e:
        print "Failed to open trace file (%s)!" % e
        sys.exit(2)

    passed = True
    off = 0
    data_stats = {}
    last_ts = None
    tot_discont = 0
    while True:
        #ftrc.seek(off)
        task = None
        ts = 0
        trc_buf = ftrc.read(args.block_len)
        if len(trc_buf) == 0:
#             print 'EOF'
            break
        trc_data = struct.unpack('<LL%sB' % (len(trc_buf) - ESP32_TRACE_BLOCK_HDR_SZ), trc_buf)
        if len(trc_data):
#             print "%x %x, len %d" % (trc_data[0], trc_data[1], len(trc_data) - 2)
#             print trc_data[2:]
#             sys.exit(0)
            task = trc_data[ESP32_TRACE_BLOCK_TASK_IDX]
            ts = trc_data[ESP32_TRACE_BLOCK_TS_IDX]
#             print ts
            if last_ts and last_ts >= ts:
#                 print "Global TS discontinuity %x -> %x, task %x, stamp %x at %x" % (last_ts, ts, task, data_stats[task]['stamp'], off)
                if args.print_details:
                    print "Global TS discontinuity %x -> %x, task %x at %x" % (last_ts, ts, task, off)
#                 tot_discont += 1
#                 passed = False
            last_ts = ts
            if not task in data_stats:
                print "%x: NEW TASK" % task
                data_stats[task] = {'stamp' : trc_data[ESP32_TRACE_BLOCK_DATA_IDX], 'last_ts' : ts, 'count' : 1, 'discont_offs' : [], 'inv_stamps_offs' : []}
            else:
                if data_stats[task]['last_ts'] == ts:
                    print "Task TS discontinuity %x -> %x, task %x, stamp %x at %x" % (last_ts, ts, task, data_stats[task]['stamp'], off)
                    data_stats[task]['discont_offs'].append(off)
                    tot_discont += 1
                    passed = False
                data_stats[task]['last_ts'] = ts
                data_stats[task]['count'] += 1
            if len(trc_data) > ESP32_TRACE_BLOCK_DATA_IDX:
#                 print "DATA = %x %x %x %x" % (trc_data[-4], trc_data[-3], trc_data[-2], trc_data[-1])
                if args.print_tasks:
                    print "Task[%d] %x, ts %08x, stamp %x"  % (off/args.block_len, task, ts, trc_data[ESP32_TRACE_BLOCK_DATA_IDX])
            else:
                print "%x: NO DATA" % task
        else:
            print "Failed to unpack data!"
            sys.exit(2)

        # check data
        for i in range(ESP32_TRACE_BLOCK_DATA_IDX, len(trc_data)):
            if trc_data[i] != data_stats[task]['stamp']:
                if not args.no_errors:
                    print "Invalid stamp %x->%x at %x, task %x" % (data_stats[task]['stamp'], trc_data[i], off + ESP32_TRACE_BLOCK_HDR_SZ + i, task)
                passed = False
                data_stats[task]['stamp'] = trc_data[i]
                data_stats[task]['inv_stamps_offs'].append(off)
#                 break
        if len(trc_buf) < args.block_len:
            print 'Last block (not full)'
            break

        if data_stats[task]['stamp'] != None:
            data_stats[task]['stamp'] = (data_stats[task]['stamp'] + 1) & 0xFF
#             print "stamp=%x" % data_stats[task][ESP32_TRACE_STAMP_IDX]
        off += args.block_len

    ftrc.close()
    print "===================================================================="
    print "Trace size %d bytes, discont %d\n" % (off, tot_discont)
    for t in data_stats:
        print "Task %x. Total count %d. Inv stamps %d. TS Discontinuities %d." % (t, data_stats[t]['count'], len(data_stats[t]['inv_stamps_offs']), len(data_stats[t]['discont_offs']))
        if args.print_details:
            print 'Invalid stamps offs: [{}]'.format(', '.join(hex(x) for x in data_stats[t]['inv_stamps_offs']))
            print 'TS Discontinuities offs: [{}]'.format(', '.join(hex(x) for x in data_stats[t]['discont_offs']))
            print "\n"
        
    if passed:
        print "Data - OK"
    else:
        print "Data - FAILED!"

if __name__ == '__main__':
    main()
