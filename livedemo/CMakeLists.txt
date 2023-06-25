add_executable(livedemo main.cpp)
target_link_libraries(livedemo PRIVATE patrick)
target_include_directories(livedemo PRIVATE ../patrick/include/)
