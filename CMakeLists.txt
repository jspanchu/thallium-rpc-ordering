cmake_minimum_required(VERSION 3.15)
project(ThalliumExample)

find_package(thallium)

add_executable(tl-server thallium/server.cpp thallium/logging.hpp)
target_link_libraries(tl-server PRIVATE thallium)

add_executable(tl-client thallium/client.cpp thallium/logging.hpp)
target_link_libraries(tl-client PRIVATE thallium)
