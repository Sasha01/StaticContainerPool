cd ..
rm -rf build
mkdir build
cd build
cmake .. -GNinja
../tools/ninja/ninja.exe
cd ../scripts

