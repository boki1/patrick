add_library(cpp-template
	src/foo.cpp)
target_include_directories(cpp-template PUBLIC
	include/)
target_compile_options(cpp-template PUBLIC
	-Wall -Wextra -Werror -std=gnu++2b)
