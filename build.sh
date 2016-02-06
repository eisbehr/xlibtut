#! /bin/bash

OUTPUT_DIR="bin/"
mkdir -p "$OUTPUT_DIR"
WARNINGS="-Wall -Wno-unused-function"
LIBRARIES="-lX11"

OPTIMIZATION_LEVEL="-O0" # 0 - none
                         # 1 - moderate
                         # 2 - extensive
                         # 3 - aggressive to the point of being able to degrade code again, needs performance testing.

for f in *.cpp; do
    outputFile=$(echo "$f" | cut -d. -f1)
    echo "$f"
    gcc "$f" -std=c++11 "$OPTIMIZATION_LEVEL" -o "$OUTPUT_DIR$outputFile" -ggdb $WARNINGS "$LIBRARIES"
done
