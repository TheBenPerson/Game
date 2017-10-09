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

#ifndef GAME_COMMON_TIMING

#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define MTX_DEFAULT PTHREAD_MUTEX_INITIALIZER

namespace Timing {

	typedef pthread_mutex_t mutex;
	typedef pthread_t thread;

	typedef struct {

		pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
		pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

	} condition;

	thread createThread(void* (*callback)(void*), void* arg);
	void waitFor(thread t);

	void doInterval(void (*callback)(), time_t hertz, bool smooth, bool *control);

	void lock(mutex *m);
	void unlock(mutex *m);

	void waitFor(condition *cond);
	bool waitFor(condition *cond, time_t secs);
	void signal(condition *cond);

}

#define GAME_COMMON_TIMING
#endif
