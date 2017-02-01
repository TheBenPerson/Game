#include "Util/Point/point.hpp"
#include "XClient/xclient.hpp"
#include "xinput.hpp"

void (* XInput::keyCallbacks[256])();
bool XInput::mousePress;
Point XInput::mousePos;

void XInput::init() {



}
