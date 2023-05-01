cmake_minimum_required(VERSION 3.24)
project(arp-pdcurses)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(ArpPDC main.cpp ${CMAKE_SOURCE_DIR}/res/appicon.rc)

link_directories(${CMAKE_SOURCE_DIR}/lib)

target_link_libraries(ArpPDC ${CMAKE_SOURCE_DIR}/lib/pdcurses.a stdc++)

