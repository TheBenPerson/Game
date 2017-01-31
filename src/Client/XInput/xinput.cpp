#include "Util/Point/point.hpp"
#include "XClient/xclient.hpp"
#include "xinput.hpp"

void (* XInput::KeyCallbacks[256])();
bool XInput::MousePress;
Point XInput::MousePos;

void XInput::Init() {



}
