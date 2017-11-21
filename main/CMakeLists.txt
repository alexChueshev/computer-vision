include_directories(${computer_vision_SOURCE_DIR}/lib/inc)
link_directories(${computer_vision_SOURCE_DIR}/lib/src)
include_directories(${computer_vision_SOURCE_DIR}/main/inc)

set(CMAKE_CXX_STANDARD 17)

set(OpenCV_FOUND 1)
find_package(OpenCV REQUIRED)

set(SOURCES
    src/main.cpp
    src/utils.cpp inc/utils.h)

add_executable(main ${SOURCES})

target_link_libraries(main lib)
target_link_libraries(main ${OpenCV_LIBS})
