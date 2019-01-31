cmake_minimum_required(VERSION 3.4.1)

include_directories(src/main/cpp/include)# 导入include，避免每次都写一长串
include_directories(src/main/cpp/libyuv)# 导入include，避免每次都写一长串

add_library( # Sets the name of the library.
             native-lib
             SHARED
             src/main/cpp/native-lib.cpp )

find_library( # Sets the name of the path variable.
              log-lib
              log )

target_link_libraries( # Specifies the target library.
                       native-lib

                       ${log-lib} )

#------------------- start --------------------------

# 解码视频生成文件
add_library( ffmpeg_playerToFile
             SHARED
             src/main/cpp/ffmpeg_playerToFile.c )
target_link_libraries( ffmpeg_playerToFile

                       avcodec-56
                       avdevice-56
                       avfilter-5
                       avformat-56
                       avutil-54
                       postproc-53
                       swresample-1
                       swscale-3
                       yuv

                        ${log-lib} )

# 解码视频并绘制
add_library( ffmpeg_playerToSurface
             SHARED
             src/main/cpp/ffmpeg_playerToSurface.c )
target_link_libraries( ffmpeg_playerToSurface

                       avcodec-56
                       avdevice-56
                       avfilter-5
                       avformat-56
                       avutil-54
                       postproc-53
                       swresample-1
                       swscale-3
                       yuv

                        android

                        ${log-lib} )

# 解码音频
add_library( ffmpeg_voicer
             SHARED
             src/main/cpp/ffmpeg_voicer.c )
target_link_libraries( ffmpeg_voicer

                       avcodec-56
                       avdevice-56
                       avfilter-5
                       avformat-56
                       avutil-54
                       postproc-53
                       swresample-1
                       swscale-3
                       yuv

                        ${log-lib} )

#解码并音视频同步
add_library( ffmpeg_play
             SHARED
             src/main/cpp/ffmpeg_play.c
             src/main/cpp/queue.c
              )
target_link_libraries( ffmpeg_play

                       avcodec-56
                       avdevice-56
                       avfilter-5
                       avformat-56
                       avutil-54
                       postproc-53
                       swresample-1
                       swscale-3
                       yuv

                        android

                        ${log-lib} )
#-------------------- end --------------------------



# ------------------------------------- start --------------------------------------

add_library( avcodec-56 SHARED IMPORTED)
set_target_properties( avcodec-56
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libavcodec-56.so)


add_library( avdevice-56 SHARED IMPORTED)
set_target_properties( avdevice-56
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libavdevice-56.so)


add_library( avfilter-5 SHARED IMPORTED)
set_target_properties( avfilter-5
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libavfilter-5.so)


add_library( avformat-56 SHARED IMPORTED)
set_target_properties( avformat-56
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libavformat-56.so)


add_library( avutil-54 SHARED IMPORTED)
set_target_properties( avutil-54
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libavutil-54.so)

add_library( postproc-53 SHARED IMPORTED)
set_target_properties( postproc-53
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libpostproc-53.so)

add_library( swresample-1 SHARED IMPORTED)
set_target_properties( swresample-1
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libswresample-1.so)


add_library( swscale-3 SHARED IMPORTED)
set_target_properties( swscale-3
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libswscale-3.so)

# ------------------------------------- end --------------------------------------

add_library( yuv SHARED IMPORTED)
set_target_properties( yuv
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libyuv.so)
