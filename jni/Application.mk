APP_STL := stlport_static
APP_ABI := armeabi armeabi-v7a x86
APP_CPPFLAGS +=-std=c++11 #允许使用c++11的函数等功能 
APP_CPPFLAGS +=-fpermissive
APP_CPPFLAGS := -fexceptions -frtti