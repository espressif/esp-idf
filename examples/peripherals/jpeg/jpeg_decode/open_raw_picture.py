# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import argparse

import cv2 as cv
import numpy as np
from numpy.typing import NDArray


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


def convert_YUV_to_RGB(Y, U, V):  # type: (NDArray, NDArray, NDArray) -> tuple[NDArray, NDArray, NDArray]
    B = np.clip(Y + 1.7790 * (U - 128), 0, 255).astype(np.uint8)
    G = np.clip(Y - 0.3455 * (U - 128) - 0.7169 * (V - 128), 0, 255).astype(np.uint8)
    R = np.clip(Y + 1.4075 * (V - 128), 0, 255).astype(np.uint8)

    return B, G, R


def picture_show_yuv420(data, h, w):  # type: (list[int], int, int) -> None
    new_u = [0] * (h * w)
    new_v = [0] * (h * w)
    new_y = [0] * (h * w)

    for i in range(int(h * w * 1.5)):
        is_even_row = ((i // (w * 1.5)) % 2 == 0)
        if is_even_row:
            if (i % 3 == 0):
                new_u[(i // 3) * 2] = data[i]
                new_u[(i // 3) * 2 + 1] = data[i]
        else:
            if (i % 3 == 0):
                new_u[(i // 3) * 2] = new_u[int((i - (w * 1.5)) // 3) * 2]
                new_u[(i // 3) * 2 + 1] = new_u[int((i - (w * 1.5)) // 3) * 2 + 1]

    for i in range(int(h * w * 1.5)):
        if (i // (w * 1.5)) % 2 != 0 and (i % 3 == 0):
            idx = (i // 3) * 2
            new_v[idx] = data[i]
            new_v[idx + 1] = data[i]

    for i in range(int(h * w * 1.5)):
        if (i // (w * 1.5)) % 2 == 0 and (i % 3 == 0):
            idx = (i // 3) * 2
            new_v[idx] = new_v[int((i + (w * 1.5)) // 3) * 2]
            new_v[idx + 1] = new_v[int((i + (w * 1.5)) // 3) * 2 + 1]

    new_y = [data[i] for i in range(int(h * w * 1.5)) if i % 3 != 0]

    Y = np.array(new_y)
    U = np.array(new_u)
    V = np.array(new_v)

    B, G, R = convert_YUV_to_RGB(Y, U, V)
    # Merge channels
    new_data = np.stack((B, G, R), axis=-1)
    new_data = np.array(new_data).reshape(h, w, 3).astype(np.uint8)

    # Display the image
    cv.imshow('data', new_data)
    cv.waitKey()


def picture_show_yuv422(data, h, w):  # type: (list[int], int, int) -> None
    # Reshape the input data to a 2D array
    data_array = np.array(data).reshape(h, w * 2)

    # Separate Y, U, and V channels
    Y = data_array[:, 1::2]
    U = data_array[:, 0::4].repeat(2, axis=1)
    V = data_array[:, 2::4].repeat(2, axis=1)

    # Convert YUV to RGB
    B, G, R = convert_YUV_to_RGB(Y, U, V)

    # Merge channels
    new_data = np.stack((B, G, R), axis=-1)

    # Display the image
    cv.imshow('data', new_data)
    cv.waitKey()


def picture_show_yuv444(data, h, w):  # type: (list[int], int, int) -> None
    # Reshape the input data to a 2D array
    data_array = np.array(data).reshape(h, w * 3)

    # Separate Y, U, and V channels
    Y = data_array[:, 2::3]
    U = data_array[:, 1::3]
    V = data_array[:, 0::3]

    # Convert YUV to RGB
    B, G, R = convert_YUV_to_RGB(Y, U, V)

    # Merge channels
    new_data = np.stack((B, G, R), axis=-1)

    # Display the image
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
        choices=['rgb565', 'rgb888', 'gray', 'yuv422', 'yuv420', 'yuv444'])

    parser.add_argument(
        '--height',
        type=int,
        help='the picture height',
        default=480)

    parser.add_argument(
        '--width',
        type=int,
        help='the picture width',
        default=640)

    args = parser.parse_args()

    height = args.height
    width = args.width

    data = open_picture(args.pic_path)
    if (args.pic_type == 'rgb565'):
        picture_show_rgb565(data, height, width)
    elif (args.pic_type == 'rgb888'):
        picture_show_rgb888(data, height, width)
    elif (args.pic_type == 'gray'):
        picture_show_gray(data, height, width)
    elif (args.pic_type == 'yuv420'):
        picture_show_yuv420(data, height, width)
    elif (args.pic_type == 'yuv422'):
        picture_show_yuv422(data, height, width)
    elif (args.pic_type == 'yuv444'):
        picture_show_yuv444(data, height, width)
    else:
        print('This type is not supported in this script!')


if __name__ == '__main__':
    main()
