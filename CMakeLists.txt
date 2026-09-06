cmake_minimum_required(VERSION 3.16)
project(apl VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++ -Wl,-subsystem,console")
endif()

set(LIBS_DIR "${CMAKE_SOURCE_DIR}/assets/lib")

set(SOURCES
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/ast.cpp
    src/value.cpp
    src/env.cpp
    src/interpreter.cpp
    src/compiler.cpp
)

set(HEADERS
    include/apl/common.h
    include/apl/lexer.h
    include/apl/ast.h
    include/apl/parser.h
    include/apl/value.h
    include/apl/env.h
    include/apl/interpreter.h
    include/apl/compiler.h
)

add_executable(apl ${SOURCES} ${HEADERS})

if(WIN32)
    if(EXISTS "${CMAKE_SOURCE_DIR}/resources.rc")
        target_sources(apl PRIVATE resources.rc)
    endif()
endif()

target_include_directories(apl PRIVATE
    include
    ${LIBS_DIR}/bullet/include
    ${LIBS_DIR}/assimp/include
    ${LIBS_DIR}/glm/include
    ${LIBS_DIR}/openal/include
    ${LIBS_DIR}/alut/include
    ${LIBS_DIR}/glfw/include
    ${LIBS_DIR}/glew/include
)

target_link_libraries(apl PRIVATE
    ${LIBS_DIR}/bullet/lib/libBulletDynamics.a
    ${LIBS_DIR}/bullet/lib/libBulletCollision.a
    ${LIBS_DIR}/bullet/lib/libLinearMath.a
    ${LIBS_DIR}/assimp/lib/libassimp.a
    ${LIBS_DIR}/assimp/lib/libzlibstatic.a
    ${LIBS_DIR}/openal/lib/libOpenAL32.a
    ${LIBS_DIR}/alut/lib/libalut.a
    ${LIBS_DIR}/glfw/lib-mingw-w64/libglfw3.a
    ${LIBS_DIR}/glew/lib/libglew32.a
    opengl32
    winmm
    ws2_32
    kernel32
    user32
    gdi32
    shell32
)

target_compile_definitions(apl PRIVATE APL_BUILD_FULL=1 GLEW_STATIC)