BUILD_TYPE="Debug"
if [ ! -z "$1" ]; then
    BUILD_TYPE=${1}
    shift
fi
cmake -S. -B build/$BUILD_TYPE -DCMAKE_BUILD_TYPE=$BUILD_TYPE
cmake --build build/$BUILD_TYPE --config $BUILD_TYPE
