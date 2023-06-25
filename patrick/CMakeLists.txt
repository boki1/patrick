add_library(patrick src/core.cpp)
target_include_directories(patrick PUBLIC include/)
target_link_libraries(patrick PRIVATE fmt)
target_compile_options(patrick PUBLIC -Wall -Wextra -Werror -std=gnu++2b)
