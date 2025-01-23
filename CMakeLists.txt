cmake_minimum_required(VERSION 3.28)
project(my_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_LISTS)

add_library(common SHARED ${SRC_LISTS})

target_include_directories(common PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(server ${PROJECT_SOURCE_DIR}/server.cpp)
target_include_directories(server PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(server PUBLIC common)

add_executable(client ${PROJECT_SOURCE_DIR}/client.cpp)
target_include_directories(client PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(client PUBLIC common)
