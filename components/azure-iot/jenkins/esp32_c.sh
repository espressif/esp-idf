echo "Cross-compiling a non-working ESP32 example"

export PATH=$PATH:$ESP32_TOOLS/xtensa-esp32-elf/bin

# Verify that the ESP32 SDK is at a tested commit
pushd $IDF_PATH
idf_commit=$(git rev-parse HEAD)
tested_idf_commit=a7a861962daacec17bdb0f34fd59f16651b50b0f
popd
if [ $idf_commit = $tested_idf_commit ]
    then echo "ESP32 SDK commit is okay"
else 
    echo "ESP32 SDK commit is " $idf_commit
    echo "ESP32 SDK commit should be " $tested_idf_commit
    echo "Error: ESP32 SDK commit is wrong"
    exit 1 
fi

cd $IDF_PATH/components/azure-iot/sample
echo "building in" $(pwd)

# Copy the sample files
sample_dir=$IDF_PATH/components/azure-iot/sdk/iothub_client/samples/iothub_client_sample_mqtt
proj_dir=$IDF_PATH/components/azure-iot/sample/main
cp $sample_dir/iothub_client_sample_mqtt.c  $proj_dir
cp $sample_dir/iothub_client_sample_mqtt.h  $proj_dir

make defconfig
result=$?
if [ $result = 0 ]
    then echo "config generated okay"
else 
    echo "!!!FAILED!!! config generation failed"
    exit $result
fi

make
result=$?
if [ $result = 0 ]
    then echo "built okay"
	exit 0
else 
    echo "!!!FAILED!!! make failed"
    exit $result
fi
