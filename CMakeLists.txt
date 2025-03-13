cmake_minimum_required(VERSION 3.28)
project(lab1_dmytro)

set(CMAKE_CXX_STANDARD 17)

add_executable(tcp_server tcp_server.cpp)
add_executable(tcp_client tcp_client.cpp)
add_executable(udp_server udp_server.cpp)
add_executable(udp_client udp_client.cpp)

target_link_libraries(tcp_server ws2_32)
target_link_libraries(tcp_client ws2_32)
target_link_libraries(udp_server ws2_32)
target_link_libraries(udp_client ws2_32)
