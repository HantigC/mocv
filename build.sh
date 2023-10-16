BUILD_TYPE="Debug"
if [ ! -z "$1" ]; then
    BUILD_TYPE=${1}
    shift
fi
echo "Start build for ${BUILD_TYPE}"
cmake -S. -B build/${BUILD_TYPE} -DCMAKE_BUILD_TYPE=${BUILD_TYP}
cmake --build build/${BUILD_TYPE} --config ${BUILD_TYPE}
echo "End build for ${BUILD_TYPE}"
