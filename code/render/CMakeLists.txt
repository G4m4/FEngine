﻿include_directories(
	${CMAKE_SOURCE_DIR}/code
	${CMAKE_SOURCE_DIR}/middleware
)

file(GLOB_RECURSE FAN_RENDER_SRC *.cpp)
file(GLOB_RECURSE FAN_RENDER_H *.hpp)

set(FAN_RENDER_LIBS
	fanCore
	shaderc_combined2
	vulkan-1
	glfw3
)

add_compile_options( ${COMPILATION_FLAGS} )
add_library(fanRender STATIC ${FAN_RENDER_SRC} ${FAN_RENDER_H})
target_link_libraries(fanRender ${FAN_RENDER_LIBS})