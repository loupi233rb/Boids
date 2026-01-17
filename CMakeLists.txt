cmake_minimum_required(VERSION 3.15)

set(IncludePath ./include)
set(glfw ./external/glfw-3.4)
set(glad ./external/glad)
set(imgui ./external/imgui)

file(GLOB SOURCE ./src/*.cpp ${glad}/src/gl.c ${imgui}/src/*.cpp)

add_subdirectory(external/glm)
add_subdirectory(external/stb_image)
if(WIN32)
    add_subdirectory(test)
endif()

add_executable(Boid ${SOURCE})

target_include_directories(Boid PRIVATE ${IncludePath} ${glad}/include ${imgui}/include)
if(WIN32)
    target_include_directories(Boid PRIVATE ${glfw}/include)
    target_link_libraries(Boid PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${glfw}/bin/libglfw3.a)
else()
    find_package(glfw3 CONFIG QUIET)
    target_link_libraries(Boid PRIVATE glfw)
endif()
target_link_libraries(Boid PRIVATE glm stb_image)