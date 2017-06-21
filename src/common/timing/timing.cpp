/*

Game Development Build
https://github.com/TheBenPerson/Game

Copyright (C) 2016-2017 Ben Stockett <thebenstockett@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "timing.hpp"

namespace Timing {

	thread createThread(void* (*entry)(void*), void* arg) {

		pthread_t thread;

		pthread_attr_t attrib;
		pthread_attr_init(&attrib);

		pthread_create(&thread, &attrib, entry, arg);

		pthread_attr_destroy(&attrib);

		return thread;

	}

	void waitForThread(thread t) {

		pthread_join(t, NULL);

	}

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

			if (!(++i)) i = 1;
			if (smooth) avgTime = ((avgTime * i) + dTime) / i;

			usleep((useconds_t) (frequency - (smooth ? avgTime : dTime)));

		}

	}

	void lock(mutex *m) {

		pthread_mutex_lock(m);

	}

	void unlock(mutex *m) {

		pthread_mutex_unlock(m);

	}

}
