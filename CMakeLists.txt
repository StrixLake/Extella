cmake_minimum_required(VERSION 3.30)

project(Extella)

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    MESSAGE("Clang")
    add_compile_options(-Wall -march=native)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    MESSAGE("MSVC")
    add_compile_options(/EHsc /arch:AVX512 /W4 /wd4267 /wd4244 /wd4305 /wd4100)

endif()


add_executable(main WIN32 main.cpp render.cpp device.cpp wnd.cpp 
                          shader.cpp writer.cpp mesh.cpp objLoader.cpp)


target_include_directories(main PUBLIC ${CMAKE_SOURCE_DIR}
                                PUBLIC "C:/Users/yasha/Documents/Repositories/tinyobjloader"
                                PUBLIC "C:/Users/yasha/Documents/Repositories/stb")

target_link_libraries(main  PUBLIC d3d11
                            PUBLIC d2d1
                            PUBLIC dxgi
                            PUBLIC dwrite
                            PUBLIC d3dcompiler)

add_custom_command(TARGET main PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:main>/shaders"
)

add_custom_command(TARGET main PRE_BUILD
     COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/mesh" "$<TARGET_FILE_DIR:main>/mesh"
)

add_custom_command(TARGET main PRE_BUILD
                    COMMAND "C:/Program Files (x86)/Windows Kits/10/bin/10.0.26100.0/x64/fxc.exe" ${CMAKE_SOURCE_DIR}/shaders/shader.hlsl /E VS /T vs_5_0 /Fo $<TARGET_FILE_DIR:main>/shaders/vshader.cso
                    VERBATIM)

add_custom_command(TARGET main PRE_BUILD
                    COMMAND "C:/Program Files (x86)/Windows Kits/10/bin/10.0.26100.0/x64/fxc.exe" ${CMAKE_SOURCE_DIR}/shaders/shader.hlsl /E PS /T ps_5_0 /Fo $<TARGET_FILE_DIR:main>/shaders/pshader.cso
                    VERBATIM)

