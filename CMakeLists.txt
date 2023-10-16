cmake_minimum_required(VERSION 3.1)
project(mocv LANGUAGES CXX C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
foreach(CFGNAME Debug Release)
  string(TOUPPER ${CFGNAME} UPPER_CFGNAME)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${UPPER_CFGNAME} ${CMAKE_BINARY_DIR}/bin/${CFGNAME})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${UPPER_CFGNAME} ${CMAKE_BINARY_DIR}/lib/${CFGNAME})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${UPPER_CFGNAME} ${CMAKE_BINARY_DIR}/lib/${CFGNAME})
endforeach()


set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(MOCV_SRCS_LIST
    src/array.c
    src/histogram.c
    src/image.c
    src/image_draw.c
    src/image_op.c
    src/image_stats.c
    src/kernel.c
    src/list.c
    src/img.cpp
    src/mathx.c
    src/matrix.c
    src/panorama.c
    src/rect.c
    src/stdx.c
    src/tracking.c
    src/keypoints/keypoint.c
    src/keypoints/harris.c
    src/lk.c)

set(MOCV_HEADERS_LIST
    include/array.h
    include/histogram.h
    include/image.h
    include/image_draw.h
    include/image_op.h
    include/image_stats.h
    include/kernel.h
    include/list.h
    include/img.h
    include/mathx.h
    include/matrix.h
    include/panorama.h
    include/rect.h
    include/stdx.h
    include/tracking.h
    include/keypoints/keypoint.h
    include/keypoints/harris.h
    include/lk.h)


add_library(mocv ${MOCV_SRCS_LIST} ${HEADER_LIST})
find_package( OpenCV REQUIRED )
target_include_directories(mocv PUBLIC include)
target_include_directories(mocv PUBLIC ${OpenCV_INCLUDE_DIRS} )
target_compile_definitions(mocv PRIVATE OPENCV=1)
target_link_libraries(mocv ${OpenCV_LIBS})

add_subdirectory(apps)
