include_directories(${computer_vision_SOURCE_DIR}/lib/inc)

set(CMAKE_CXX_STANDARD 17)

add_library(lib
        src/operations.cpp inc/operations.h
        src/kernels.cpp inc/kernels.h
        src/filters.cpp inc/filters.h
        src/borders.cpp inc/borders.h
        src/img.cpp inc/img.h
        src/pyramid.cpp inc/pyramid.h
        src/detectors.cpp inc/detectors.h)
