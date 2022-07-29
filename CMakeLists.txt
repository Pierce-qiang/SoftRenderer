cmake_minimum_required(VERSION 3.14)

project(SoftRenderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CONFIGURATION_TYPES "Release")

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -GL -fp:fast")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -flto -ffast-math")
endif()

set(HEADERS
        src/include/camera.h
        src/include/CubeMap.h
        src/include/macro.h
        src/include/maths.h
        src/include/model.h
        src/include/object.h
        src/include/params.h
        src/include/payload.h
        src/include/pipeline.h
        src/include/payload.h
        src/include/Shader.h
        src/include/SkyBox.h
        src/include/stb_image.h
        src/include/stb_image_impl.h
        src/include/Texture.h
        src/platform/win32.h
        )

set(SOURCES
        src/core/camera.cpp
        src/core/CubeMap.cpp
        src/core/maths.cpp
        src/core/model.cpp
        src/core/PhongShader.cpp
        src/core/pipeline.cpp
        src/core/Texture.cpp
        Application.cpp
        src/platform/win32.cpp
        )

add_executable(SoftRenderer  ${HEADERS} ${SOURCES})

if(MSVC)
    target_compile_options(SoftRenderer PRIVATE /fp:fast)
else()
    target_compile_options(SoftRenderer PRIVATE -ffast-math)
    target_link_libraries(SoftRenderer m)
endif()

set_directory_properties(PROPERTIES VS_STARTUP_PROJECT SoftRenderer)
source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${HEADERS} ${SOURCES})