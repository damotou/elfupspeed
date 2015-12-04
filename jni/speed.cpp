#include <sys/time.h>
#include <time.h>
#include <android/log.h>
#include <jni.h>
#include <substrate.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "ElfIPCServer.h"
#include <pthread.h>
#include "speed.h"


#define LOG_TAG "SPEED"

#define LOGD(fmt, args...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args)




int IsHooked = 1;
static long long usersec = 0;
static long long userusec = 0;

static long long user_tvsec = 0;
static long long user_tvnsec = 0;

static long long user_0_tvsec = 0;
static long long user_0_tvnsec = 0;

#define doublecap  0x3B9ACA00ull

static long long usernsec = 0;
static long usertvsec = 0;
static int IsAddSpeed = 0;
static int timescale = 0x3E8;

long long dword_4D0B0;
long long qword_4D0C8;
long long dword_4D0A8;
long long dword_4D0C0;

#define HIDWORD(l) ((unsigned int)(((unsigned long)(l) >> 32) & 0xFFFFFFFF))
static
int (*old_gettimeofday)(struct timeval* tv, struct timezone *tz);
int (*old_clock_gettime)(clockid_t which_clock, struct timespec * tp);

int do_speed_hook();

unsigned long __PAIR__(unsigned int a, unsigned int b) {
	unsigned long x = a;
	unsigned long z = b;
	x = (x << 32) + b;
}

ElfSpeed::ElfSpeed(){

}

ElfSpeed::~ElfSpeed(){

}


int elf_gettimeofday(struct timeval* tv, struct timezone *tz) {

	struct timeval * ntv;
	int value = 0;
	long long temptime;
	ntv = tv;
	//LOGD("elf_gettimeofday up tv_sec = %lu  tv_usec= %lu \n",tv->tv_sec, tv->tv_usec);
	int res = old_gettimeofday(tv, tz);
	//LOGD("wo cao a !!!elf_gettimeofday  tv_sec = %lu  tv_usec= %lu \n",tv->tv_sec, tv->tv_usec);
	//LOGD("timescale = %d",timescale);
	if (res) {
		value = res;
	} else {
		if (usersec) {
			//1449137975293497
			//LOGD("elf_gettimeofday up tv_sec = %ld  tv_usec= %ld\n",ntv->tv_sec, ntv->tv_usec);
			temptime = ntv->tv_usec + 1000000ull * ntv->tv_sec - usersec;
			//LOGD("temptime  temptime = %lld \n",temptime);
			usersec = 1000000ull * ntv->tv_sec + ntv->tv_usec;
			//LOGD("usersec  usersec = %lld \n",usersec);

			userusec += (temptime * timescale) / 1000;
			//LOGD("Hook, timescale = %d\n", timescale);
			//LOGD("userusec  userusec = %lld \n",userusec);
			ntv->tv_sec = userusec / 1000000ull;
			ntv->tv_usec = -1000000ll * ntv->tv_sec + userusec;
			//LOGD("elf_gettimeofday  tv_sec = %ld  tv_usec= %ld\n",ntv->tv_sec, ntv->tv_usec);
		} else {
			//LOGD("first elf_gettimeofday  tv_sec = %ld  tv_usec=  %ld\n",ntv->tv_sec, ntv->tv_usec);
			usersec = 1000000ull * ntv->tv_sec + ntv->tv_usec; //当前微妙
			userusec = 1000000ull * ntv->tv_sec + ntv->tv_usec;
			//LOGD("first  log usersec = %lld  userusec= %lld\n",usersec,userusec);

		}

		value = 0;
	}

	//LOGD("elf_gettimeofday  tv_sec = %d  tv_usec=  %d\n",tv->tv_sec, tv->tv_usec);

	return value;
}

int elf_clock_gettime(clockid_t which_clock, struct timespec * tp) { //纳秒级别

	clockid_t v13;
	timespec * v12;
	int value = 0;
	long v8;
	long long temnsecrange;
	long long temp;
	int v9;
	int v10;
	int v5;
	long long v3;
	long v4;
	int v6;
	v13 = which_clock;
	v12 = tp;

	int res = old_clock_gettime(which_clock, tp);

	if (res) {
		value = res;
	} else {
		if (user_tvsec && user_0_tvsec) {

			if (v13) {
				if (v13 == CLOCK_MONOTONIC) {
//
//					if (user_tvsec <= 0xA) {
//						v8 = 1000000000ull * v12->tv_sec;
//						v9 = v12->tv_nsec;
//						v10 = 1000000000ull * v12->tv_sec;
//						user_tvnsec = __PAIR__(HIDWORD(v8), v10) + v9;
//						user_tvsec = __PAIR__(HIDWORD(v8), v10) + v9;
//
//					} else {
//					LOGD( "换算前 **** %lu: user_tvsec = %lld, user_tvnsec= %lld",
//							which_clock, user_tvsec, user_tvnsec);
					temnsecrange = tp->tv_nsec + doublecap * tp->tv_sec
							- user_tvsec;

					LOGD(" temnsecrange ++++ %lld", temnsecrange);

					temp = tp->tv_nsec + 1000000000ull * tp->tv_sec;

					LOGD(" temp ++++ %lld", temp);

					user_tvnsec = timescale * temnsecrange / 1000 + user_tvsec;

					//LOGD(" user_tvnsec **** %lld", user_tvnsec);

					tp->tv_sec = user_tvnsec / doublecap;
					//LOGD(" tv_sec **** %lld", user_tvnsec / doublecap);
					tp->tv_nsec = user_tvnsec - doublecap * tp->tv_sec;
					//LOGD(" tv_nsec **** %lld", user_tvnsec - doublecap * tp->tv_sec);
					//LOGD("CLOCK_MONOTONIC **** %d: tv_sec = %ld, tv_nsec= %ld",which_clock, tp->tv_sec, tp->tv_nsec);
					//user_tvsec = temp;
//					}
				}
			} else {

				temnsecrange = tp->tv_nsec + doublecap * tp->tv_sec
						- user_0_tvsec;

				LOGD(" temnsecrange **** %lld", temnsecrange);

				temp = tp->tv_nsec + 1000000000ull * tp->tv_sec;

				LOGD(" temp **** %lld", temp);

				user_0_tvnsec = timescale * temnsecrange / 1000 + user_0_tvsec;

				//LOGD(" user_tvnsec **** %lld", user_tvnsec);

				tp->tv_sec = user_0_tvnsec / doublecap;
				//LOGD(" tv_sec **** %lld", user_tvnsec / doublecap);
				tp->tv_nsec = user_0_tvnsec - doublecap * tp->tv_sec;
			}
		} else {

			if (which_clock == CLOCK_MONOTONIC) {
				user_tvsec = tp->tv_nsec + doublecap * tp->tv_sec;
				user_tvnsec = tp->tv_nsec + doublecap * tp->tv_sec;
				LOGD("first **** %lu: tv_sec = %ld, tv_nsec= %ld ",
						which_clock, tp->tv_sec, tp->tv_nsec);
				LOGD( "first **** %lu: user_tvsec = %lld, user_tvnsec= %lld",
						which_clock, user_tvsec, user_tvnsec);
			} else {
				user_0_tvsec = tp->tv_nsec + doublecap * tp->tv_sec;
				user_0_tvnsec = tp->tv_nsec + doublecap * tp->tv_sec;
				LOGD("first **** %lu: tv_sec = %ld, tv_nsec= %ld ",
						which_clock, tp->tv_sec, tp->tv_nsec);
				LOGD( "first **** %lu: user_tvsec = %lld, user_tvnsec= %lld",
						which_clock, user_0_tvsec, user_0_tvnsec);
			}

		}
		value = 0;
	}

	//LOGD("%d: %d, %d",which_clock, tp->tv_sec, tp->tv_nsec);

	return value;
}

//2.0  0XBB8
//1.0  0X7D0
//0.0  0x3E8
//-1.0 0X1F4
//-2.0 0x14D
//-3.0 0XFA
//-4.0 0XC8
//-5.0 0XA6
//-10.0 0x5A
//3.0 0XFA0
//4.0 0x1388
//5.0 0x1770
//10  0x2aF8

int ElfSpeed::SetTimeScale(int result) {


	if (IsHooked) {
		do_speed_hook();
		IsHooked = 0;
	}
	switch (result) {
	case 0:
		timescale = 0x3e8;
	case 1:
		timescale = 0X7D0;
		break;
	case 2:
		timescale = 0XBB8;
		break;
	case 3:
		timescale = 0XFA0;
		break;
	case 4:
		timescale = 0x1388;
		break;
	case 5:
		timescale = 0x1770;
		break;
	case 10:
		timescale = 0x2aF8;
		break;
	case -1:
		timescale = 0X1F4;
		break;
	case -2:
		timescale = 0x14D;
		break;
	case -3:
		timescale = 0XFA;
		break;
	case -4:
		timescale = 0XC8;
		break;
	case -5:
		timescale = 0XA6;
		break;
	case -10:
		timescale = 0x5A;
		break;
	}
	LOGD("timescale = %d",timescale);
	return 0;
}

int xx_clock_gettime(clockid_t which_clock, struct timespec * tp) { //纳秒级别
	int v2; // r3@2
	long long v3; // ST18_8@7
	long long
	v4; // r0@8
	int v5; // r2@8
	int v6; // kr04_4@8
	long long
	v7; // ST28_8@10
	long long
	v8; // r0@11
	int v9; // r2@11
	int v10; // kr0C_4@11
	struct timespec * v12; // [sp+0h] [bp-3Ch]@1
	int v13; // [sp+4h] [bp-38h]@1
	int v14; // [sp+Ch] [bp-30h]@1
	v13 = which_clock ;
	v12 = tp ;
	v14 = old_clock_gettime(which_clock, tp);
	if (v14) {
		v2 = v14;
	} else {
		if (v13) {
			if (v13 == 1) {
				if ( (unsigned long long)qword_4D0C8 <= 0xA) {
					v8 = 1000000000uLL * v12->tv_sec;
					v9 = v12->tv_nsec;

					v10 = 1000000000ull * v12->tv_sec;

					dword_4D0B0 = __PAIR__(HIDWORD(v8), v10) + v9;
					qword_4D0C8 = __PAIR__(HIDWORD(v8), v10) + v9;
					LOGD("!!!!!%d: %d, %d",which_clock, v12->tv_sec, v12->tv_nsec);
				} else {
					v7 = v12->tv_nsec + 1000000000uLL * v12->tv_sec
							- qword_4D0C8;
					qword_4D0C8 = v12->tv_nsec
							+ 1000000000uLL * v12->tv_sec;

					dword_4D0B0 += timescale * v7 / 1000;
					v12->tv_sec = dword_4D0B0 / doublecap;
					v12->tv_nsec = -1000000000ull * v12->tv_sec
							+ dword_4D0B0;
					LOGD("%d: %d, %d",which_clock, v12->tv_sec, v12->tv_nsec);
				}
			}
		}
//		else if ((unsigned int) dword_4D0C0 > 0) {
//			v4 = 1000000000uLL * v12->tv_sec;
//			v5 = v12->tv_nsec;
//			v6 = 1000000000ull * v12->tv_sec;
//
//			dword_4D0A8 = __PAIR__(HIDWORD(v4), v6) + v5;
//			dword_4D0C0 = __PAIR__(HIDWORD(v4), v6) + v5;
//			LOGD("~~~~~~%d: %d, %d",which_clock, v12->tv_sec, v12->tv_nsec);
//
//		}
		else {
			v3 = v12->tv_nsec + 1000000000uLL *  v12->tv_sec
					- dword_4D0C0;
			dword_4D0C0 = v12->tv_nsec
					+ 1000000000uLL * v12->tv_sec;
			dword_4D0A8 += timescale * v3 / 1000;
			v12->tv_sec = dword_4D0A8 / doublecap;
			v12->tv_nsec= -1000000000ull * v12->tv_sec + dword_4D0A8;
			LOGD("++++++%d: %d, %d",which_clock, v12->tv_sec, v12->tv_nsec);
		}
		v2 = 0;
	}
	return v2;

}

int do_speed_hook() {

	MSHookFunction(&gettimeofday, &elf_gettimeofday, &old_gettimeofday);
	MSHookFunction(&clock_gettime, &elf_clock_gettime, &old_clock_gettime);

	return 0;
}

//

void * startServer( void * arg)
{
	do_speed_hook();
	IsHooked = 0;
	elfIPCServer * IPCServer = new elfIPCServer();
	IPCServer->StartIPCServer();

}

extern "C" int hook_entry(char * arg) {

	//int speed = atoi(arg);
//	LOGD("Hook success, arg = %s\n", arg);
//	LOGD("********Start hooking **********\n");
	LOGD("Hook success, pid = %d\n", getpid());
//	LOGD("Hook success, speed = %d\n", speed);
	//启动server
	LOGD("********Start hooking **********\n");


	//在这里开启IPC 进行监听
	pthread_t pt;
	int err = pthread_create(&pt,NULL,startServer,NULL);


	LOGD("Hook success, pid = %d\n", getpid());

	//SetTimeScale(speed);
}
