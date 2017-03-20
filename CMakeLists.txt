cmake_minimum_required(VERSION 3.6)
project(TotalTest)

set(CMAKE_C_STANDARD 99)

#set(SOURCE_FILES test/xxplayer.c)
#
#add_executable(stapler ${SOURCE_FILES})
#
#target_link_libraries(stapler avdevice avfilter swscale avformat avcodec avutil asound SDL2 Xv X11 Xext xcb xcb-xfixes xcb-render xcb-shape sndio asound swresample dl m z pthread)


set(SOURCE_FILES mov_things.c)

add_executable(mp4par ${SOURCE_FILES})


set(SOURCE_FILES ts_test.c)

add_executable(ts_dump ${SOURCE_FILES})
target_link_libraries(ts_dump ssl crypto)