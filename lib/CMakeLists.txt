include_directories(${computer_vision_SOURCE_DIR}/lib/inc)

set(OpenCV_FOUND 1)
find_package(OpenCV REQUIRED)

add_library(lib
        src/image.cpp inc/image.h
        src/utils.cpp inc/utils.h
        src/operations.cpp inc/operations.h
        src/kernels.cpp inc/kernels.h
        )

target_link_libraries(lib ${OpenCV_LIBS})