cmake_minimum_required(VERSION 3.22)
project(dat2geotiff C)

set(CMAKE_C_STANDARD 11)

add_executable(dat2geotiff dat2geotiff.c)
target_link_libraries(dat2geotiff tiff geotiff m)
