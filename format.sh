#!/bin/bash

# Run clang-format to format all relevant files
echo "Running clang-format..."
find . -name '*.h' -o -name '*.cpp' -o -name '*.hpp' -exec clang-format -i {} +

echo "Formatting complete."

tidy=$1

if [[ $1 != "tidy" ]]; then
    exit
fi

# Check for clang-tidy and clang-format installation
if ! command -v clang-tidy &> /dev/null || ! command -v clang-format &> /dev/null; then
    echo "clang-tidy and/or clang-format not found. Please install them before running this script."
    exit 1
fi

# Run clang-tidy on all relevant files
echo "Running clang-tidy..."
find . -name '*.h' -o -name '*.cpp' -o -name '*.hpp' | while read -r file; do
    clang-tidy "$file" -- -std=c++17
done

# Check if clang-tidy reported any issues
if [ $? -ne 0 ]; then
    echo "clang-tidy found issues. Please address them before formatting."
    exit 1
fi

