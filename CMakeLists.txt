cmake_minimum_required(VERSION 3.0.0)

project(gstChannelChangeTestNativeApp VERSION 1.0.0 LANGUAGES C)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

set(ENABLE_CYCLE_URLS true CACHE STRING "Enable cycling through the stream URL list.")
set(PLAY_DURATION_SEC 5 CACHE STRING "Stream playback duration in seconds before cycling/teardown.")

find_package(PkgConfig REQUIRED)
find_package(GStreamer REQUIRED)

include(GNUInstallDirs)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(GSTAPP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/gst_sample_app.c)

add_definitions(-DCYCLE_URLS=${ENABLE_CYCLE_URLS})
add_definitions(-DPLAY_DURATION_SEC=${PLAY_DURATION_SEC})

add_executable(gstChannelChangeTestNativeApp ${GSTAPP_SRC_FILES})
target_link_libraries(gstChannelChangeTestNativeApp PRIVATE
    GStreamer::GStreamer)

install(TARGETS gstChannelChangeTestNativeApp
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
