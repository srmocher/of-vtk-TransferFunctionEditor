project(A06P01)
set(APP A06P01)

cmake_minimum_required(VERSION 3.3)

set(OF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../of/" CACHE PATH "The root directory of ofnode/of project.")
include(${OF_ROOT}/openFrameworks.cmake)


find_package(VTK REQUIRED)
include(${VTK_USE_FILE})


ofxaddon(ofxImGui)


set(SOURCES
    src/main.cpp
    src/ofApp.cpp
    src/volumeRenderThread.cpp
    src/controlpoint.h
    src/tfeditor.cpp
    src/controlpoint.cpp
        )

add_executable(
    ${APP}
    MACOSX_BUNDLE
    ${SOURCES}
    ${OFXADDONS_SOURCES}
)

target_link_libraries(
    ${APP}
    ${OPENFRAMEWORKS_LIBRARIES}
    ${VTK_LIBRARIES}
)
