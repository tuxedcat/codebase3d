cmake_minimum_required(VERSION 3.21)
project(gameengine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if (DEFINED SOURCE_PATH)
	set(SOURCES ${CMAKE_SOURCE_DIR}/${SOURCE_PATH})
else()
	message( FATAL_ERROR "You must specify SOURCE_PATH. For example, cmake -DSOURCE_PATH=./main.cpp .." )
endif()

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

find_package(OpenGL REQUIRED)

include_directories(
	${OPENGL_INCLUDE_DIRS}
	${CMAKE_SOURCE_DIR}/incl
	${CMAKE_SOURCE_DIR}/glfw/include)

link_libraries(${OPENGL_LIBRARIES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lGL -lGLU -lglut")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG=1 -O0 -g -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address")
set(CMAKE_LINKER_FLAGS_RELEASE "${CMAKE_LINKER_FLAGS_RELEASE}")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
link_libraries(glfw OpenGL::GL)

add_executable(${PROJECT_NAME}.out ${SOURCES})