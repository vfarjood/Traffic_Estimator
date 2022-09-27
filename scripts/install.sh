#! /bin/sh

echo "-Building CMake..."
#rm -rf $(pwd)/../build
#mkdir $(pwd)/../build
cmake -S $(pwd)/../ -B $(pwd)/../build
echo "Finished!"
echo "--------------------"
echo "Building MakeFile..."
make -C $(pwd)/../build
echo "Finished!"
#cp $(pwd)/../build/estimator .
ln -sf $(pwd)/../build/estimator estimate

