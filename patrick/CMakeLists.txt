find_package(fmt REQUIRED)
find_package(Eigen3 REQUIRED)

add_library(patrick src/core.cpp)
target_include_directories(patrick PUBLIC include/)
target_link_libraries(patrick PUBLIC fmt::fmt Eigen3::Eigen3)
target_compile_options(patrick PUBLIC -Wall -Wextra -Werror -std=gnu++2b)
