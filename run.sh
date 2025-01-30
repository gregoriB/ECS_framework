if [[ $1 == "clean" ]]; then
    echo "Cleaning up existing build..."

    if [[ -L "compile_commands.json" ]]; then
        echo "Removed compile_commands link"
        rm compile_commands.json
    fi

    if [[ -d ".cache" ]]; then
        echo "Removed folder: .cache/"
        rm -rf .cache
    fi

    if [[ -d "build" ]]; then
        echo "Removed folder: build/"
        rm -rf build
    fi

fi

echo "Building..."
./build.sh 

cd build || exit

ln -s compile_commands.json ../

if [[ -f "./main" ]]; then
    rm main
fi

make

echo "Running..."
./main

cd ..
