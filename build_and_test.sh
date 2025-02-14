./build.sh

cd build

ctest --verbose --output-on-failure --timeout 0

cd ..
