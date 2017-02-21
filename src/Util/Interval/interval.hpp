#ifndef HG_Util_Interval

#include <unistd.h>

void doInterval(void (*callback)(), time_t hertz, bool smooth, bool* control);

#define HG_Util_Interval
#endif
