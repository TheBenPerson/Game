/*
 *
 * Game Development Build
 * https://github.com/TheBenPerson/Game
 *
 * Copyright (C) 2016-2018 Ben Stockett <thebenstockett@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <pthread.h>
#include <signal.h>
#include <time.h>

#include "timing.hh"

namespace Timing {

	thread createThread(void* (*entry)(void*), void *arg) {

		pthread_t t;

		pthread_attr_t attr;
		pthread_attr_init(&attr);

		pthread_create(&t, &attr, entry, arg);

		return t;

	}

	void waitFor(thread t) {

		pthread_join(t, NULL);

	}

	void doInterval(void (*callback)(), time_t hertz, bool smooth, bool* control) {

		double frequency = 1000000.0 / hertz;
		double avgTime = 0;

		int i = 1;

		timespec t;

		while (*control) {

			clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t);
			double startTime = t.tv_nsec / 1000.0;

			callback();

			clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t);

			double dTime = (t.tv_nsec / 1000.0) - startTime;

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

	void waitFor(condition *cond) {

		pthread_mutex_lock(&cond->m);
		pthread_cond_wait(&cond->cond, &cond->m);
		pthread_mutex_unlock(&cond->m);

	}

	bool waitFor(condition *cond, time_t secs) {

		timespec time;

		clock_gettime(CLOCK_REALTIME, &time);
		time.tv_sec += secs;

		return !pthread_cond_timedwait(&cond->cond, &cond->m, &time);

	}

	void waitStop(condition *cond) {

		pthread_mutex_unlock(&cond->m);

	}

	void signal(condition *cond) {

		pthread_mutex_lock(&cond->m);
		pthread_cond_signal(&cond->cond);
		pthread_mutex_unlock(&cond->m);

	}

}
