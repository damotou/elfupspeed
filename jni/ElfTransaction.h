/*
 * ElfTransaction.h
 *
 *  Created on: 2015-7-23
 *      Author: HelloKitty
 */
#include <jni.h>
#include <android_runtime/AndroidRuntime.h>
#include <binder/IServiceManager.h>
#include <binder/Binder.h>
#include <DummyJavaBBinder.h>
#ifndef ELFTRANSACTION_H_
#define ELFTRANSACTION_H_


using namespace android;
class ElfTransaction {
public:
	ElfTransaction();
	virtual ~ElfTransaction();
	int elfHookRunningApp(char * arg);
	jobject getSystemClassLoader();
};

#endif /* ELFTRANSACTION_H_ */
