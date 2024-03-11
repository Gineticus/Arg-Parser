add_library(argparser ArgParser.h ArgParser.cpp)
add_library(arguments INTERFACE Arguments.hpp)

target_link_libraries(argparser PUBLIC arguments)