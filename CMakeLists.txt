cmake_minimum_required(VERSION 3.10.0)
project(FinalProject)

find_package(OpenGL REQUIRED)
#find_package(GLU REQUIRED)

# Suppress warnings of the deprecation of glut functions on macOS.
if(APPLE)
 add_definitions(-Wno-deprecated-declarations)
endif()

### Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

### Compilation flags: adapt to your needs ###
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /bigobj") ### Enable parallel compilation
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR} )
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR} )
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

### Add src to the include directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")

### Add OpenGL
set(INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
set(LIBRARIES ${OPENGL_LIBRARIES})

### Include Eigen for linear algebra
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/glm")

### Compile GLFW3 statically
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_INSTALL OFF CACHE BOOL " " FORCE)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/glfw" "glfw")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/glfw/include")
set(LIBRARIES "glfw" ${GLFW_LIBRARIES})

### On windows, you also need glew
if((UNIX AND NOT APPLE) OR WIN32)
  set(GLEW_INSTALL OFF CACHE BOOL " " FORCE)
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/glew" "glew")
  include_directories("${CMAKE_CURRENT_SOURCE_DIR}/glew/include")
  list(APPEND LIBRARIES "glew")
endif()

if(APPLE)
list(APPEND LIBRARIES "-framework OpenGL")
endif()

### Compile all the helper files in src
file(GLOB SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/src/Helpers.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/src/Helpers.h"
)

### Collect all the glsl files in src
file(GLOB GLSL
"${CMAKE_CURRENT_SOURCE_DIR}/shader/*.glsl"
)

add_executable(sphere ${CMAKE_CURRENT_SOURCE_DIR}/src/sphere.cpp ${SOURCES} ${GLSL})
target_link_libraries(sphere ${LIBRARIES} ${OPENGL_LIBRARIES})

add_executable(rabbit ${CMAKE_CURRENT_SOURCE_DIR}/src/rabbit.cpp ${SOURCES} ${GLSL})
target_link_libraries(rabbit ${LIBRARIES} ${OPENGL_LIBRARIES})

add_executable(rabbitAreaLight ${CMAKE_CURRENT_SOURCE_DIR}/src/rabbitAreaLight.cpp ${SOURCES} ${GLSL})
target_link_libraries(rabbitAreaLight ${LIBRARIES} ${OPENGL_LIBRARIES})

add_executable(rabbitCrystal ${CMAKE_CURRENT_SOURCE_DIR}/src/rabbitCrystal.cpp ${SOURCES} ${GLSL})
target_link_libraries(rabbitCrystal ${LIBRARIES} ${OPENGL_LIBRARIES})
