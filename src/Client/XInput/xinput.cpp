/*

Copyright (C) 2016-2017 Ben Stockett

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

#include <stddef.h>
#include "Util/Point/point.hpp"
#include "XClient/xclient.hpp"
#include "xinput.hpp"

XInput::Key* XInput::keys[] = {

	new Key({95, false, onF11}), new Key({1, false, NULL}), NULL

};

Point XInput::mousePos;

void XInput::cleanup() {

	for (size_t i = 0; XInput::keys[i]; i++)
		delete keys[i];

}

void XInput::init() {



}

void XInput::onF11() {

	XClient::setFullscreen(!XClient::fullscreen);

}
