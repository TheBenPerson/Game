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

#ifndef GAME_COMMON_POINT
#define GAME_COMMON_POINT

#include <stdint.h>

#define SIZE_TPOINT (sizeof(float) * 2)

typedef struct Point {

	float x;
	float y;

	Point();
	Point(float x, float y);
	Point(uint8_t *buff);
	void pack(uint8_t *buf);

	void operator=(float val);

	void operator+=(Point &point);
	void operator-=(Point &point);
	void operator*=(Point &point);

	void operator+=(float val);

	Point operator-(Point &point);
	Point operator*(float val);
	Point operator/(float val);

	void operator*=(float val);
	void operator/=(float val);

	bool operator==(Point &point);
	bool operator<(Point &point);
	bool operator>(Point &point);

	operator bool();

	void rot(float theta);
	void normalize();

} Point;

#endif
