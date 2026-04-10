# How to get a YUV image from a PNG image

```bash
ffmpeg -i hello.png -pix_fmt yuyv422 -f rawvideo hello.yuv
```

## Supported YUV422 packing order

| NAME     | NB_COMPONENTS  | BITS_PER_PIXEL | BIT_DEPTHS |
|----------|----------------|----------------|------------|
| yuyv422  | 3              | 16             | 8-8-8      |
| yvyu422  | 3              | 16             | 8-8-8      |
| uyvy422  | 3              | 16             | 8-8-8      |

# How to get a gray8 raw image from a PNG image

```bash
ffmpeg -i hello.png -vf format=gray -pix_fmt gray -f rawvideo hello.gray
```
