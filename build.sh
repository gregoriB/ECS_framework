clear

root="build"
library="library/build"
tests="tests/build"

if [[ -d $root ]]; then
    echo "Removing folder: $root"
    rm -rf build

    if [[ -d $root ]]; then
        echo "FAILED TO REMOVE $root folder !!!!"
    fi
fi

if [[ -d $library ]]; then
    echo "Removing folder: $library"
    rm -rf library/build

    if [[ -d $library ]]; then
        echo "FAILED TO REMOVE $library !!!!"
    fi
fi

if [[ -d $tests ]]; then
    echo "Removing folder: $tests"
    rm -rf tests/build
    if [[ -d $tests ]]; then
        echo "FAILED TO REMOVE $tests !!!!"
    fi
fi

if [[ -L "compile_commands.json" ]]; then
    echo "Removed compile_commands link"
    rm compile_commands.json
fi

mkdir build

cd build || exit

rm -f CMakeCache.txt

cmake --debug-output .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

make VERBOSE=1

ln -s compile_commands.json ../

cd ..
