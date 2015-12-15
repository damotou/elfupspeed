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

static float lastSpeed = 1.0;

static int IsChangeSpeed_gettimeofday = 0;

static int IsChangeSpeed_gettime = 0;

#define doublecap  0x3B9ACA00ull

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

ElfSpeed::ElfSpeed() {

}

ElfSpeed::~ElfSpeed() {

}

static float g_times = 1; //���ٵı���

//static int (*old_gettimeofday)(struct timeval *ptv, struct timezone *ptz);

static uint64_t g_gettimeofday_saved_usecs = 0;

static uint64_t g_gettimeofday_saved_usecs_last = 0;


static uint64_t g_gettimeofday_saved_usecs_real = 0;

int new_gettimeofday(struct timeval *ptv, struct timezone *ptz) {

	int res;

	int64_t diff;

	int64_t cur_usecs;

	int64_t ret_usecs;

	res = old_gettimeofday(ptv, ptz);

	if (res != 0) {

		return res;

	}

	if (g_gettimeofday_saved_usecs == 0) {

		g_gettimeofday_saved_usecs = (ptv->tv_sec * 1000000LL) + ptv->tv_usec; //基数
		g_gettimeofday_saved_usecs_last = (ptv->tv_sec * 1000000LL) + ptv->tv_usec;

	} else {

		cur_usecs = (ptv->tv_sec * 1000000LL) + ptv->tv_usec;

		diff = cur_usecs - g_gettimeofday_saved_usecs;

		diff = diff * g_times;

		ret_usecs = g_gettimeofday_saved_usecs_last + diff;

		g_gettimeofday_saved_usecs = (ptv->tv_sec * 1000000LL) + ptv->tv_usec;

		ptv->tv_sec = (time_t) (ret_usecs / 1000000LL);
		ptv->tv_usec = (suseconds_t) (ret_usecs % 1000000LL);

		g_gettimeofday_saved_usecs_last = ret_usecs;
	}
	//LOGD("elf_gettimeofday up tv_sec = %ld  tv_usec= %ld\n",ptv->tv_sec, ptv->tv_usec);
	return 0;

}

//int elf_gettimeofday(struct timeval* tv, struct timezone *tz) {
//
//	struct timeval * ntv;
//	int value = 0;
//	long long temptime;
//	ntv = tv;
//	//LOGD("elf_gettimeofday up tv_sec = %lu  tv_usec= %lu \n",tv->tv_sec, tv->tv_usec);
//	int res = old_gettimeofday(tv, tz);
//	//LOGD("wo cao a !!!elf_gettimeofday  tv_sec = %lu  tv_usec= %lu \n",tv->tv_sec, tv->tv_usec);
//	//LOGD("timescale = %d",timescale);
//	if (res) {
//		value = res;
//	} else {
//		if (usersec) {
//			//1449137975293497
//			//LOGD("elf_gettimeofday up tv_sec = %ld  tv_usec= %ld\n",ntv->tv_sec, ntv->tv_usec);
//			temptime = ntv->tv_usec + 1000000ull * ntv->tv_sec - usersec;
//			//LOGD("temptime  temptime = %lld \n",temptime);
//			usersec = 1000000ull * ntv->tv_sec + ntv->tv_usec;
//			//LOGD("usersec  usersec = %lld \n",usersec);
//
//			userusec += (temptime * timescale) / 1000;
//			//LOGD("Hook, timescale = %d\n", timescale);
//			//LOGD("userusec  userusec = %lld \n",userusec);
//			ntv->tv_sec = userusec / 1000000ull;
//			ntv->tv_usec = -1000000ll * ntv->tv_sec + userusec;
//			//LOGD("elf_gettimeofday  tv_sec = %ld  tv_usec= %ld\n",ntv->tv_sec, ntv->tv_usec);
//		} else {
//			//LOGD("first elf_gettimeofday  tv_sec = %ld  tv_usec=  %ld\n",ntv->tv_sec, ntv->tv_usec);
//			usersec = 1000000ull * ntv->tv_sec + ntv->tv_usec; //��ǰ΢��
//			userusec = 1000000ull * ntv->tv_sec + ntv->tv_usec;
//			//LOGD("first  log usersec = %lld  userusec= %lld\n",usersec,userusec);
//
//		}
//
//		value = 0;
//	}
//
//	//LOGD("elf_gettimeofday  tv_sec = %d  tv_usec=  %d\n",tv->tv_sec, tv->tv_usec);
//
//	return value;
//}

static int64_t g_clock_gettime_saved_nsecs_clk_MONO = 0;

static uint64_t g_clock_gettime_saved_nsecs_clk_Real = 0;

static int64_t g_clock_gettime_saved_nsecs_clk_MONO_last = 0;

static uint64_t g_clock_gettime_saved_nsecs_clk_Real_last = 0;

int new_clock_gettime(clockid_t clk_id, struct timespec *ptp) { //clk_id CLOCK_MONOTONIC=1 CLOCK_REALTIME=0

	int res;

	int64_t diff;

	int64_t cur_nsecs;

	int64_t ret_nsecs;

	char isposttive_num;

	res = old_clock_gettime(clk_id, ptp);

	if (res != 0) {

		return res;

	}

	if (clk_id == CLOCK_MONOTONIC) {

		if (g_clock_gettime_saved_nsecs_clk_MONO == 0) {

			g_clock_gettime_saved_nsecs_clk_MONO = (ptp->tv_sec * 1000000000LL)

			+ ptp->tv_nsec;

			g_clock_gettime_saved_nsecs_clk_MONO_last = (ptp->tv_sec * 1000000000LL)

			+ ptp->tv_nsec;

			return 0;

		} else {

			cur_nsecs = (ptp->tv_sec * 1000000000LL) + ptp->tv_nsec;

			diff = cur_nsecs - g_clock_gettime_saved_nsecs_clk_MONO;


			diff = diff * g_times ;

			ret_nsecs = g_clock_gettime_saved_nsecs_clk_MONO_last + diff;

			g_clock_gettime_saved_nsecs_clk_MONO = (ptp->tv_sec * 1000000000LL)
					+ ptp->tv_nsec;

			ptp->tv_sec = (time_t) (ret_nsecs / 1000000000LL);

			ptp->tv_nsec = (long) (ret_nsecs % 1000000000LL);

			g_clock_gettime_saved_nsecs_clk_MONO_last = ret_nsecs;
		}

	} else {

		if (g_clock_gettime_saved_nsecs_clk_Real == 0) {

			g_clock_gettime_saved_nsecs_clk_Real = (ptp->tv_sec * 1000000000LL)

			+ ptp->tv_nsec;
			g_clock_gettime_saved_nsecs_clk_Real_last = (ptp->tv_sec * 1000000000LL)

			+ ptp->tv_nsec;
			return 0;

		} else {

			cur_nsecs = (ptp->tv_sec * 1000000000LL) + ptp->tv_nsec;

			diff = cur_nsecs - g_clock_gettime_saved_nsecs_clk_Real;

			diff = diff * g_times ;


			ret_nsecs = g_clock_gettime_saved_nsecs_clk_Real_last + diff;

			g_clock_gettime_saved_nsecs_clk_Real = (ptp->tv_sec * 1000000000LL)
					+ ptp->tv_nsec;

			ptp->tv_sec = (time_t) (ret_nsecs / 1000000000LL);

			ptp->tv_nsec = (long) (ret_nsecs % 1000000000LL);

			g_clock_gettime_saved_nsecs_clk_Real_last = ret_nsecs;
		}

	}

	return 0;

}

//int elf_clock_gettime(clockid_t which_clock, struct timespec * tp) { //���뼶��
//
//	clockid_t v13;
//	timespec * v12;
//	int value = 0;
//	long v8;
//	long long temnsecrange;
//	long long temp;
//	int v9;
//	int v10;
//	int v5;
//	long long v3;
//	long v4;
//	int v6;
//	v13 = which_clock;
//	v12 = tp;
//
//	int res = old_clock_gettime(which_clock, tp);
//
//	if (res) {
//		value = res;
//	} else {
//		if (user_tvsec && user_0_tvsec) {
//
//			if (which_clock) {
//				if (which_clock == CLOCK_MONOTONIC) {
//					temnsecrange = tp->tv_nsec + doublecap * tp->tv_sec
//							- user_tvsec;
//					temp = tp->tv_nsec + 1000000000ull * tp->tv_sec;
//					LOGD(" temp ++++ %lld", temp);
//					user_tvnsec = timescale * temnsecrange / 1000 + user_tvsec;
//					tp->tv_sec = user_tvnsec / doublecap;
//					tp->tv_nsec = user_tvnsec - doublecap * tp->tv_sec;
//				}
//			} else {
//
//				temnsecrange = tp->tv_nsec + doublecap * tp->tv_sec
//						- user_0_tvsec;
//
//				LOGD(" temnsecrange **** %lld", temnsecrange);
//
//				temp = tp->tv_nsec + 1000000000ull * tp->tv_sec;
//
//				LOGD(" temp **** %lld", temp);
//
//				user_0_tvnsec = timescale * temnsecrange / 1000 + user_0_tvsec;
//
//				//LOGD(" user_tvnsec **** %lld", user_tvnsec);
//
//				tp->tv_sec = user_0_tvnsec / doublecap;
//				//LOGD(" tv_sec **** %lld", user_tvnsec / doublecap);
//				tp->tv_nsec = user_0_tvnsec - doublecap * tp->tv_sec;
//			}
//		} else {
//			if (which_clock == CLOCK_MONOTONIC) {
//				user_tvsec = tp->tv_nsec + doublecap * tp->tv_sec;
//				user_tvnsec = tp->tv_nsec + doublecap * tp->tv_sec;
//				LOGD("first **** %lu: tv_sec = %ld, tv_nsec= %ld ",
//						which_clock, tp->tv_sec, tp->tv_nsec);
//				LOGD( "first **** %lu: user_tvsec = %lld, user_tvnsec= %lld",
//						which_clock, user_tvsec, user_tvnsec);
//			} else {
//				user_0_tvsec = tp->tv_nsec + doublecap * tp->tv_sec;
//				user_0_tvnsec = tp->tv_nsec + doublecap * tp->tv_sec;
//				LOGD("first **** %lu: tv_sec = %ld, tv_nsec= %ld ",
//						which_clock, tp->tv_sec, tp->tv_nsec);
//				LOGD( "first **** %lu: user_tvsec = %lld, user_tvnsec= %lld",
//						which_clock, user_0_tvsec, user_0_tvnsec);
//			}
//		}
//		value = 0;
//	}
//
//	//LOGD("%d: %d, %d",which_clock, tp->tv_sec, tp->tv_nsec);
//
//	return value;
//}

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

int ElfSpeed::SetTimeScale(float result) {

	if (IsHooked) {
		do_speed_hook();
		IsHooked = 0;
	}

	result += 1;

	g_times = result;

	LOGD("result = %f  g_times = %f ", result, g_times);
	return 0;
}

int do_speed_hook() {

	MSHookFunction(&gettimeofday, &new_gettimeofday, &old_gettimeofday);
	MSHookFunction(&clock_gettime, &new_clock_gettime, &old_clock_gettime);

	return 0;
}

//

void * startServer(void * arg) {
	do_speed_hook();
	IsHooked = 0;
	elfIPCServer * IPCServer = new elfIPCServer();
	IPCServer->StartIPCServer();

}

extern "C" int hook_entry(char * arg) {

	//int speed = atoi(arg);
//	LOGD("Hook success, arg = %s\n", arg);
//	LOGD("********Start hooking **********\n");
//	LOGD("Hook success, pid = %d\n", getpid());
//	LOGD("Hook success, speed = %d\n", speed);
	//����server
	LOGD("********Start hooking **********\n");

	//�����￪��IPC ���м���
	pthread_t pt;
	int err = pthread_create(&pt, NULL, startServer, NULL);

	LOGD("Hook success, pid = %d\n", getpid());

	//SetTimeScale(speed);
}
