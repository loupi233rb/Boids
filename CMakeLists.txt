cmake_minimum_required(VERSION 3.15)

set(IncludePath ./include)
set(EasyXInclude F:/environment/easyX/include)
set(glfw ./external/glfw-3.4)
set(glad ./external/glad)

file(GLOB SOURCE ./src/*.cpp ${glad}/src/gl.c)
file(GLOB EasyXLib F:/environment/easyX/lib/*.a)

add_subdirectory(external/glm)
add_subdirectory(external/stb_image)

add_executable(Boid ${SOURCE})

target_include_directories(Boid PRIVATE ${IncludePath} ${EasyXInclude} ${glfw}/include ${glad}/include)
if(WIN32)
    target_link_libraries(Boid PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${glfw}/bin/libglfw3.a)
endif()
target_link_libraries(Boid PRIVATE glm stb_image)
target_link_libraries(Boid PRIVATE ${EasyXLib})