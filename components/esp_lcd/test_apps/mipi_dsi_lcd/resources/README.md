# How to generate the YUV image from the PNG image

```bash
ffmpeg -i hello.png -pix_fmt yuyv422 hello.yuv
```

## Supported YUV422 packing order

| NAME     | NB_COMPONENTS  | BITS_PER_PIXEL | BIT_DEPTHS |
|----------|----------------|----------------|------------|
| yuyv422  | 3              | 16             | 8-8-8      |
| yvyu422  | 3              | 16             | 8-8-8      |
| uyvy422  | 3              | 16             | 8-8-8      |
