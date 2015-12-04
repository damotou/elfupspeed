#include <sys/time.h>
#include <time.h>
#include <android/log.h>
#include <jni.h>
#include <substrate.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>

int main(int argc, char** argv) {

	struct timeval ntv;

	struct timezone ntz;



	while (1) {
		gettimeofday(&ntv,&ntz);
		printf("tv_sec = %lu  tv_usec= %lu \n", ntv.tv_sec, ntv.tv_usec);
	}
	return 0;
}
