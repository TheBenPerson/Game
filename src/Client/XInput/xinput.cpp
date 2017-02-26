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
