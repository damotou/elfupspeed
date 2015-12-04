#include <stdio.h>
#include <stddef.h>
#include <jni.h>
#include <android_runtime/AndroidRuntime.h>
#include <binder/IServiceManager.h>
#include <binder/Binder.h>
#include <utils/RefBase.h>
#include <DummyJavaBBinder.h>
#include <dlfcn.h>
#include <android/log.h>
#include <unistd.h>
#include <ElfTransaction.h>
#include <pthread.h>

#define LOG_TAG "HOOKINJECT"
#define LOGD(fmt, args...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args)

//void * startServer( void * arg)
//{
//	elfIPCServer * IPCServer = new elfIPCServer();
//	IPCServer->StartIPCServer();
//}

extern "C" int hook_entry(char * arg) {

	LOGD("********Start hooking **********\n");
<<<<<<< HEAD


=======
	LOGD("Hook success, pid = %d\n", getpid());
	int err = -1 ;
>>>>>>> origin/master
	//在这里开启IPC 进行监听
//	pthread_t pt;
//	int err = pthread_create(&pt,NULL,startServer,NULL);

	ElfTransaction * elftransaction = new ElfTransaction();
	err = elftransaction->elfHookRunningApp("");


	LOGD("Hook success, pid = %d\n", getpid());

	return err;
}
