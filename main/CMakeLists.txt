include_directories(${computer_vision_SOURCE_DIR}/lib/inc)
link_directories(${computer_vision_SOURCE_DIR}/lib/src)

set(OpenCV_FOUND 1)
find_package(OpenCV REQUIRED)

add_executable(main src/main.cpp)

target_link_libraries(main lib)
target_link_libraries(main ${OpenCV_LIBS})
