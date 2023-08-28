#! /usr/bin/env bash

echo "Copy Bluetooth Mesh v1.1 lib:"

chip=$1

if [[ $chip != "esp32" &&
      $chip != "esp32s3" &&
      $chip != "esp32c3" &&
      $chip != "esp32c6" &&
      $chip != "esp32h2" &&
      $chip != "all" ]]; then
    echo "Invalid Chip Target: $chip"
    exit 0
fi

if [[ $chip == "esp32" ||
      $chip == "esp32s3" ||
      $chip == "esp32c3" ||
      $chip == "esp32c6" ||
      $chip == "esp32h2"  ]]; then
    cp ./build/$chip/libmesh_v1.1.a ./$chip/
    echo "Copy for $chip done!"
elif [[ $chip == "all" ]]; then
    cp ./build/esp32/libmesh_v1.1.a   ./esp32/
    echo "Copy for esp32 done!"

    cp ./build/esp32s3/libmesh_v1.1.a ./esp32s3/
    echo "Copy for esp32s3 done!"

    cp ./build/esp32c3/libmesh_v1.1.a ./esp32c3/
    echo "Copy for esp32c3 done!"

    cp ./build/esp32c6/libmesh_v1.1.a ./esp32c6/
    echo "Copy for esp32c6 done!"

    cp ./build/esp32h2/libmesh_v1.1.a ./esp32h2/
    echo "Copy for esp32h2 done!"
fi
