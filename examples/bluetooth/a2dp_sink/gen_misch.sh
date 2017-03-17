set -e
export IDF_PATH=$(pwd)/../../esp-idf
echo "----------------------"
echo "PLease Check IDF_PATH"
echo "IDF_PATH"
echo $IDF_PATH 
echo "---------------------"

# make bootloader
# make partitio
#make clean
make


echo "------------------------------"
echo "the testje_bin copy to bin "
echo "the bootloader_bin copy to bin"
echo "the partication_bin copy to bin"
echo "--------------------------------"
cp ./build/bluedroid_demos.bin ../bin/
cp ./build/bootloader/bootloader.bin ../bin
cp ./build/partitions_singleapp.bin ../bin
