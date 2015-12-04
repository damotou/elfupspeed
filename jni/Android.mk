LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE:= elfinject

LOCAL_SRC_FILES := elfinject.c

LOCAL_LDLIBS := -llog

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

#LOCAL_MODULE:= inject
#LOCAL_CPP_EXTENSION := .cpp
#LOCAL_SRC_FILES := inject.cpp \
				   ElfIPCServer.cpp \
LOCAL_MODULE:= inject
#LOCAL_CPP_EXTENSION := .cpp
#LOCAL_SRC_FILES := inject.cpp \
				   ElfTransaction.cpp \
				   
#LOCAL_CFLAGS    := -I./jni/include/
#LOCAL_LDLIBS	:= -llog  -landroid_runtime -lbinder -lutils -lcutils 

#include $(BUILD_SHARED_LIBRARY)

#include $(CLEAR_VARS)

LOCAL_MODULE:= speed
LOCAL_CPP_EXTENSION := .cpp
LOCAL_SRC_FILES := speed.cpp ElfIPCServer.cpp 
 				   
LOCAL_LDLIBS	:= -llog -lsubstrate 

include $(BUILD_SHARED_LIBRARY)

#include $(CLEAR_VARS)


include $(CLEAR_VARS)
LOCAL_MODULE    := lib_serverClient
LOCAL_SRC_FILES := serverClient.cpp
LOCAL_LDLIBS	:= -llog
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := lib_sharedserverClient
LOCAL_SRC_FILES := serverClient.cpp
LOCAL_LDLIBS	:= -llog
LOCAL_STATIC_LIBRARIES := lib_serverClient

include $(BUILD_SHARED_LIBRARY)

#include $(CLEAR_VARS)

#LOCAL_MODULE:= client

#LOCAL_SRC_FILES := client.c
#LOCAL_LDLIBS := -llog

#include $(BUILD_SHARED_LIBRARY)

#include $(CLEAR_VARS)

#LOCAL_MODULE:= server

#LOCAL_SRC_FILES := ipcserver.cpp \
				   ElfIPCServer.cpp \
				   ElfTransaction.cpp \
				   
#LOCAL_CFLAGS    := -I./jni/include/
#LOCAL_LDLIBS	:= -llog  -landroid_runtime -lbinder -lutils -lcutils 


#include $(BUILD_EXECUTABLE)

#include $(CLEAR_VARS)

#LOCAL_MODULE    := changemem
#LOCAL_SRC_FILES := changemem.cpp
#LOCAL_LDLIBS := -llog
#include $(BUILD_EXECUTABLE)
include $(CLEAR_VARS)

LOCAL_MODULE:= testclient

LOCAL_SRC_FILES := testclient.cpp  serverClient.cpp

LOCAL_LDLIBS := -llog

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_MODULE:= testserver

LOCAL_SRC_FILES := epollserver.cpp
 
LOCAL_LDLIBS := -llog   

include $(BUILD_EXECUTABLE)

#include $(CLEAR_VARS)

#LOCAL_MODULE:= test

#LOCAL_SRC_FILES := test.c
#LOCAL_LDLIBS := -llog

#include $(BUILD_SHARED_LIBRARY)