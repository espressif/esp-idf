# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import argparse

import cv2 as cv
import numpy as np


def open_picture(path):  # type: (str) -> list[int]
    with open(path, 'rb') as f:
        data = f.read()
        f.close()
    new_data = [int(x) for x in data]
    return new_data


def picture_show_rgb888(data, h, w):  # type: (list[int], int, int) -> None
    data = np.array(data).reshape(h, w, 3).astype(np.uint8)
    cv.imshow('data', data)
    cv.waitKey()


def picture_show_rgb565(data, h, w):  # type: (list[int], int, int) -> None

    new_data = [0] * ((len(data) // 2) * 3)
    for i in range(len(data)):
        if i % 2 != 0:
            new_data[3 * (i - 1) // 2 + 2] = (data[i] & 0xf8)
            new_data[3 * (i - 1) // 2 + 1] |= (data[i] & 0x7) << 5
        else:
            new_data[3 * i // 2] = (data[i] & 0x1f) << 3
            new_data[3 * i // 2 + 1] |= (data[i] & 0xe0) >> 3

    new_data = np.array(new_data).reshape(h, w, 3).astype(np.uint8)
    cv.imshow('data', new_data)
    cv.waitKey()


def picture_show_gray(data, h, w):  # type: (list[int], int, int) -> None
    new_data = np.array(data).reshape(h, w, 1).astype(np.uint8)
    cv.imshow('data', new_data)
    cv.waitKey()


def main():  # type: () -> None
    parser = argparse.ArgumentParser(description='which mode need to show')

    parser.add_argument(
        '--pic_path',
        type=str,
        help='What is the path of your picture',
        required=True)

    parser.add_argument(
        '--pic_type',
        type=str,
        help='What type you want to show',
        required=True,
        choices=['rgb565', 'rgb888', 'gray'])

    parser.add_argument(
        '--hight',
        type=int,
        help='the picture hight',
        default=480)

    parser.add_argument(
        '--width',
        type=int,
        help='the picture width',
        default=640)

    args = parser.parse_args()

    hight = args.hight
    width = args.width

    data = open_picture(args.pic_path)

    if (args.pic_type == 'rgb565'):
        picture_show_rgb565(data, hight, width)
    elif (args.pic_type == 'rgb888'):
        picture_show_rgb888(data, hight, width)
    elif (args.pic_type == 'gray'):
        picture_show_gray(data, hight, width)
    else:
        print('This type is not supported in this script!')


if __name__ == '__main__':
    main()
