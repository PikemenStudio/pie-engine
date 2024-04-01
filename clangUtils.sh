#!/bin/bash

# Directory containing the source files
SRC_DIR="./src"

# Find all .cpp and .h files recursively in the source directory
find "$SRC_DIR" -type f \( -name "*.cpp" -o -name "*.h" \) | while IFS= read -r file; do
    # Run clang-format on the file
    clang-format --dry-run --Werror --i --style=file "$file"
    echo "Formatted: $file"
done

echo "Tidy"
# Find all .cpp and .h files recursively in the source directory
find "$SRC_DIR" -type f \( -name "*.cpp" -o -name "*.h" \) | while IFS= read -r file; do
    # Run clang-tidy on the file
    clang-tidy "$file" -p cmake-build-debug
    echo "Tidy check: $file"
done
