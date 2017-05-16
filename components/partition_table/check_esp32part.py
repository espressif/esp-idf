#!/usr/bin/python

import os.path
import sys

import gen_esp32part

if __name__ == '__main__':
    args = sys.argv[1:]

    if len(args) % 2 == 0:
        print 'expected odd number of arguments'
        sys.exit(1);

    table = gen_esp32part.PartitionTable.from_csv(open(args[0]).read())

    # For each of the images that we want to write into the flash at a certain offset,
    for i in range(1, len(args) - 1, 2):
        offset = gen_esp32part.parse_int(args[i])
        image = args[i + 1]
        image_size = os.path.getsize(image)

        # .. find the corresponding partition in the table. `partition` may be
        # None in e.g. the case of the bootloader.bin.
        partition = None
        for p in table:
            if p.offset == offset:
                partition = p
                break

        # If we did find the partition, make sure that this image will fit in
        # that partition.
        if partition is not None:
            if partition.size < image_size:
                print '%s (sz=%i) will not fit in partition (sz=%i) starting at %i' % (image, image_size, partition.size, offset)
                sys.exit(1)
        else:
            # Otherwise, it's only an error if the image is overlapping a
            # partition (starting at a different offset).
            for p in table:
                if (offset >= p.offset and offset < p.offset + p.size) \
                    or (offset + image_size >= p.offset and offset + image_size < p.offset + p.size):
                    print '%s is not in a partition, but it overlaps a partition!'
                    sys.exit(1)
