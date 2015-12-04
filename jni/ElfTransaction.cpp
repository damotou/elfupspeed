/*
 * ElfTransaction.cpp
 *
 *  Created on: 2015-7-23
 *      Author: HelloKitty
 */

#include "ElfTransaction.h"

static const char JSTRING[] = "Ljava/lang/String;";
static const char JCLASS_LOADER[] = "Ljava/lang/ClassLoader;";
static const char JCLASS[] = "Ljava/lang/Class;";
static JNIEnv* jni_env;
static char sig_buffer[512];

#define LOG_TAG "HOOK"
#define LOGD(fmt, args...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args)

ElfTransaction::ElfTransaction() {
	// TODO Auto-generated constructor stub

}

ElfTransaction::~ElfTransaction() {
	// TODO Auto-generated destructor stub
}

jobject ElfTransaction::getSystemClassLoader() {

	LOGD("getSystemClassLoader start...");
	jclass class_loader_claxx = jni_env->FindClass("java/lang/ClassLoader");
	snprintf(sig_buffer, 512, "()%s", JCLASS_LOADER);
	jmethodID getSystemClassLoader_method = jni_env->GetStaticMethodID(
			class_loader_claxx, "getSystemClassLoader", sig_buffer);
	return jni_env->CallStaticObjectMethod(class_loader_claxx,
			getSystemClassLoader_method);
}

int ElfTransaction::elfHookRunningApp(char * arg) {

	LOGD("elfHookRunningAPP %s",arg);

	JavaVM* jvm = AndroidRuntime::getJavaVM();
	if (jvm == NULL) {
		LOGD("jvm is NULL");
		return -1;
	}
	jvm->AttachCurrentThread(&jni_env, NULL);

	if (jni_env == NULL) {
		LOGD("jni_env is NULL");
		return -1;
	}

	jstring apk_path = jni_env->NewStringUTF("/data/ElfInject.apk");
	jstring dex_out_path = jni_env->NewStringUTF("/data/");
	jclass dexloader_claxx = jni_env->FindClass("dalvik/system/DexClassLoader");

	snprintf(sig_buffer, 512, "(%s%s%s%s)V", JSTRING, JSTRING, JSTRING,
			JCLASS_LOADER);

	jmethodID dexloader_init_method = jni_env->GetMethodID(dexloader_claxx,
			"<init>", sig_buffer);

	if (dexloader_init_method == NULL) {
		LOGD("dexloader_init_method is NULL");
		return -1;
	}
	snprintf(sig_buffer, 512, "(%s)%s", JSTRING, JCLASS);
	jmethodID loadClass_method = jni_env->GetMethodID(dexloader_claxx,
			"loadClass", sig_buffer);

	if (loadClass_method == NULL) {
		LOGD("loadClass_method is NULL");
		return -1;
	}

	jobject class_loader = getSystemClassLoader();

	if (class_loader == NULL) {
		LOGD("class_loader is NULL");
		return -1;
	}

	LOGD("getSystemClassLoader finished...");

	jobject dex_loader_obj = jni_env->NewObject(dexloader_claxx,
			dexloader_init_method, apk_path, dex_out_path, NULL, class_loader);

	LOGD("dex_loader_obj is %p", dex_loader_obj);
	if (dex_loader_obj == NULL) {
		LOGD("dex_loader_obj is NULL");
		return -1;
	}

	jstring class_name = jni_env->NewStringUTF("com.elf.inject.EntryClass");

	jclass entry_class = static_cast<jclass>(jni_env->CallObjectMethod(
			dex_loader_obj, loadClass_method, class_name));

	if (entry_class == NULL) {
		LOGD("entry_class is NULL");
		return -1;
	}

	LOGD("entry_class is %p", entry_class);

	jmethodID invoke_method = jni_env->GetStaticMethodID(entry_class, "invoke",
			"()[Ljava/lang/Object;");

	if (invoke_method == NULL) {
		LOGD("invoke_method is NULL");
		return -1;
	}

	//	//check_value(invoke_method);
	//
	jobjectArray objectarray = (jobjectArray) jni_env->CallStaticObjectMethod(
			entry_class, invoke_method, 0);
	if (objectarray == NULL) {
		LOGD("objectarray is NULL");
		return -1;
	}
	//check_value(objectarray);

	jsize size = jni_env->GetArrayLength(objectarray);
	sp<IServiceManager> servicemanager = defaultServiceManager();

	LOGD("size is : %d", size);

	for (jsize i = 0; i < size; i += 2) {

		jstring name = static_cast<jstring>(jni_env->GetObjectArrayElement(
				objectarray, i));
		jobject obj = jni_env->GetObjectArrayElement(objectarray, i + 1);

		const char* c_name = jni_env->GetStringUTFChars(name, NULL);

		LOGD("c_name is %s", c_name);

		DummyJavaBBinder* binder = (DummyJavaBBinder*) servicemanager->getService(String16(c_name)).get();
		binder->changObj(jni_env->NewGlobalRef(obj));
	}

	LOGD("******* hooking end  *****\n");
}
