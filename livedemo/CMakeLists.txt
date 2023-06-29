find_package(fmt REQUIRED)

add_executable(livedemo src/main.cpp src/command.cpp src/channel.cpp)
target_link_libraries(livedemo PRIVATE patrick fmt::fmt)
target_include_directories(livedemo PRIVATE include/)
