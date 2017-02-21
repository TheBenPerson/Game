#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "interval.hpp"

void doInterval(void (*callback)(), time_t hertz, bool smooth, bool* control) {

	double frequency = 1000000.0d / hertz;
	double avgTime = 0;

	int i = 1;

	timespec t;

	while (*control) {

		clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t);
		double startTime = t.tv_nsec / 1000.0d;

		callback();

		clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t);

		double dTime = (t.tv_nsec / 1000.0d) - startTime;
		if (smooth) avgTime = ((avgTime * i) + dTime) / ++i;

		usleep((useconds_t) (frequency - (smooth ? avgTime : dTime)));

	}

}
