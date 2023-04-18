cmake_minimum_required(VERSION 3.15)
project(ThalliumExample)

find_package(thallium)

add_executable(tl-server src/server.cpp)
target_link_libraries(tl-server PRIVATE thallium)
target_include_directories(tl-server PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/vendor)

add_executable(tl-client src/client.cpp)
target_link_libraries(tl-client PRIVATE thallium)
target_include_directories(tl-client PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/vendor)
