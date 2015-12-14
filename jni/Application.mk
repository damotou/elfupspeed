APP_STL := stlport_static
APP_ABI := armeabi armeabi-v7a x86
APP_CPPFLAGS +=-std=c++11  
APP_CPPFLAGS +=-fpermissive
APP_CPPFLAGS := -fexceptions -frtti
#APP_MODULES := libserverClient