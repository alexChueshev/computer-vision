include_directories(${computer_vision_SOURCE_DIR}/lib/inc)

set(OpenCV_FOUND 1)
find_package(OpenCV REQUIRED)

add_library(lib
        src/image.cpp inc/image.h
        src/utils.cpp inc/utils.h)

target_link_libraries(lib ${OpenCV_LIBS})