﻿cmake_minimum_required (VERSION 3.8)

project ("FEngine")
set(CMAKE_CXX_STANDARD 17)

include_directories( code )
include_directories( middleware )
include_directories( middleware/freetype )
include_directories( "C:/VulkanSDK/1.1.106.0/Include" )

# link directories
IF( ${MSVC_VERSION} GREATER_EQUAL 1910 AND ${MSVC_VERSION} LESS_EQUAL 1919 ) # v141 toolset
	set( MSVC_LINK_DIR "2017" )
ELSEIF( ${MSVC_VERSION} GREATER_EQUAL 1920 AND ${MSVC_VERSION} LESS_EQUAL 1929 ) # v142 toolset
	set( MSVC_LINK_DIR "2019" )
ENDIF()

link_directories("C:/VulkanSDK/1.1.106.0/Lib")
IF(${CMAKE_BUILD_TYPE} MATCHES Debug)
    link_directories(lib/${MSVC_LINK_DIR}/debug)
    link_directories("C:/VulkanSDK/1.1.106.0/Lib/${MSVC_LINK_DIR}/debug")
ELSEIF(${CMAKE_BUILD_TYPE} MATCHES Release)
    link_directories(lib/${MSVC_LINK_DIR}/release)
    link_directories("C:/VulkanSDK/1.1.106.0/Lib/${MSVC_LINK_DIR}/release")
ENDIF()

# libs directories
add_subdirectory(code/core)
add_subdirectory(code/render)
add_subdirectory(code/network)
add_subdirectory(code/ecs)
add_subdirectory(code/scene)
add_subdirectory(code/game)
add_subdirectory(code/editor)
add_subdirectory(middleware)

#defines
add_compile_definitions(SFML_STATIC)

# sources
file( GLOB_RECURSE LPP_SRC deps/LivePP/*.h )
list(APPEND APP_SOURCES
    code/fanMain.cpp
    ${LPP_SRC}    
) 

set(APP_LIBRARIES 
    fanCore
    fanRender
    fanNetwork
    fanScene
    fanGame
    fanEditor
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(fengine_editor ${APP_SOURCES} )

target_link_libraries(fengine_editor ${APP_LIBRARIES})

# FORCE C++17 STANDARD
set_target_properties(fengine_editor PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
  # Force W4
if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
endif()

  # Force WX (warning as error)
target_compile_options(fanCore PRIVATE /WX /FUNCTIONPADMIN )
target_compile_options(fanRender PRIVATE /WX /FUNCTIONPADMIN )
target_compile_options(fanNetwork PRIVATE /WX /FUNCTIONPADMIN )
target_compile_options(fanScene PRIVATE /WX /FUNCTIONPADMIN )
target_compile_options(fanGame PRIVATE /WX /FUNCTIONPADMIN )
target_compile_options(fanEditor PRIVATE /WX /FUNCTIONPADMIN )
target_compile_options(fengine_editor PRIVATE /WX /FUNCTIONPADMIN )