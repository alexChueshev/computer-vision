include_directories(${computer_vision_SOURCE_DIR}/lib/inc)

link_directories(${computer_vision_SOURCE_DIR}/lib/src)

add_executable(main src/main.cpp)
target_link_libraries(main lib)