cmake_minimum_required(VERSION 2.8)
project(libenl)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)

file(GLOB	src_stdlib		src/stdlib/*.cpp	)
file(GLOB	src_utils		src/utils/*.cpp		)

add_definitions(-DENL_LIBRARY)

add_library(enl SHARED ${src_stdlib} ${src_utils})

install(TARGETS enl DESTINATION lib/)

file(GLOB include_headers include/enl/*.h)
install(FILES ${include_headers} DESTINATION include/enl/)
