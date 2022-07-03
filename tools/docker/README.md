<!-- This is a brief version of docs/en/api-guides/tools/idf-docker-image.rst
     intended to be displayed on the Docker Hub page: https://hub.docker.com/r/espressif/idf.
     When changing this page, please keep idf-docker-image.rst in sync.
     (Keep the differences between Markdown and restructuredText in mind.)
 -->

# ESP-IDF Docker Image

This is a Docker image for the [Espressif IoT Development Framework (ESP-IDF)](https://github.com/espressif/esp-idf). It is intended for building applications and libraries with specific versions of ESP-IDF, when doing automated builds.

This image contains a copy of ESP-IDF and all the tools necessary to build ESP-IDF projects.

## Tags

Multiple tags of this image are maintained:

- `latest`: tracks `master` branch of ESP-IDF
- `vX.Y`: corresponds to ESP-IDF release `vX.Y`
- `release-vX.Y`: tracks `release/vX.Y` branch of ESP-IDF

## Basic Usage

Build a project located in the current directory using `idf.py build` command:

```bash
docker run --rm -v $PWD:/project -w /project espressif/idf:latest idf.py build
```

## Documentation

For more information about this image and the detailed usage instructions, please refer to the ESP-IDF Programming Guide page: [IDF Docker Image](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/tools/idf-docker-image.html).
