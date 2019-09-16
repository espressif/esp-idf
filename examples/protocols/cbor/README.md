# CBOR Example
(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

The [CBOR](https://en.wikipedia.org/wiki/CBOR)(Concise Binary Object Representation) is a binary data serialization format which is similar to JSON but with smaller footprint. This example will illustrate how to encode and decode CBOR data using the APIs provided by [tinycbor](https://github.com/intel/tinycbor).

For detailed information about how CBOR encoding and decoding works, please refer to [REF7049](https://tools.ietf.org/html/rfc7049) or [cbor.io](http://cbor.io/);

## How to use example

### Hardware Required

This example should be able to run on any commonly available ESP32 development board.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build and flash the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```bash
I (320) example: encoded buffer size 67
I (320) example: convert CBOR to JSON
[{"chip":"esp32","unicore":false,"ip":[192,168,1,100]},3.1400001049041748,"simple(99)","2019-07-10 09:00:00+0000","undefined"]
I (340) example: decode CBOR manually
Array[
  Map{
    chip
    esp32
    unicore
    false
    ip
    Array[
      192
      168
      1
      100
    ]
  }
  3.14
  simple(99)
  2019-07-10 09:00:00+0000
  undefined
]
```

## Troubleshooting

For more API usage, please refer to [tinycbor API](https://intel.github.io/tinycbor/current/).

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
