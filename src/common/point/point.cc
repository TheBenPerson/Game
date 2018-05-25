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

#include <math.h>

#include "point.hh"

void Point::operator=(float val) {

	x = val;
	y = val;

}

void Point::operator+=(Point &point) {

	x += point.x;
	y += point.y;

}

void Point::operator-=(Point &point) {

	x -= point.x;
	y -= point.y;

}

void Point::operator*=(Point &point) {

	x *= point.x;
	y *= point.y;

}

void Point::operator+=(float val) {

	x += val;
	y += val;

}

Point Point::operator*(float val) {

	Point point = *this;

	point.x *= val;
	point.y *= val;

	return point;

}

Point Point::operator/(float val) {

	Point point = *this;

	point.x /= val;
	point.y /= val;

	return point;

}

void Point::operator*=(float val) {

	x *= val;
	y *= val;

}

void Point::operator/=(float val) {

	x /= val;
	y /= val;

}

bool Point::operator==(Point &point) {

	if (x != point.x) return false;
	if (y != point.y) return false;

	return true;

}

bool Point::operator<(Point &point) {

	if (x >= point.x) return false;
	if (y >= point.y) return false;

	return true;

}

bool Point::operator>(Point &point) {

	if (x <= point.x) return false;
	if (y <= point.y) return false;

	return true;

}

Point::operator bool() {

	if (x && y) return true;
	return false;

}

void Point::rot(float theta) {

	float before = this->x;

	x = (x * cosf(theta)) - (y * sinf(theta));
	y = (before * sinf(theta)) + (y * cosf(theta));


}

void Point::normalize() {

	float len = sqrtf((x * x) + (y * y));
	if (!len) return;

	x /= len;
	y /= len;

}
