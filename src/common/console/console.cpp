/*
 *
 * Game Development Build
 * https://github.com/TheBenPerson/Game
 *
 * Copyright (C) 2016-2017 Ben Stockett <thebenstockett@gmail.com>
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

#include <stdarg.h>
#include <stdlib.h>

#include "console.hpp"

void cputs(char *color, char *string) {

	printf("%s\e[1m%s%s\n", color, string, "\e[m");

}

void cprintf(char *color, char *format, ...) {

	printf("%s\e[1m", color);

	va_list args;
	va_start(args, format);

	vprintf(format, args);

	va_end(args);

	printf("\e[m");

}

void ceputs(char *color, char *string) {

	fprintf(stderr, "%s\e[1m%s%s\n", color, string, "\e[m");

}

void ceprintf(char *color, char *format, ...) {

	fprintf(stderr, "%s\e[1m", color);

	va_list args;
	va_start(args, format);

	vfprintf(stderr, format, args);

	va_end(args);

	fprintf(stderr, "\e[m");

}