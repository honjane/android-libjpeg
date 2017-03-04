cmake_minimum_required(VERSION 3.4.1)


add_library(
             hjpeg

             SHARED

             src/main/cpp/native-lib.cpp )

#include 这个目录下所有的文件
include_directories(src/main/cpp/include)

#导入jpeg动态库 SHARED；静态库为STATIC
add_library(jpeg SHARED IMPORTED )

#对应so目录
set_target_properties(jpeg PROPERTIES IMPORTED_LOCATION ../../../../libs/armeabi-v7a/libjpeg.so)

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )


target_link_libraries( # Specifies the target library.
                       hjpeg
                       jpeg
                       #jnigraphics这个是android下面的bitmap.h对应的库
                       jnigraphics
                       ${log-lib})
