#ifndef `$INSTANCE_NAME`_H
#define `$INSTANCE_NAME`_H
#include <project.h>

void `$INSTANCE_NAME`_Start();
#define `$INSTANCE_NAME`_SpeedL (*`$INSTANCE_NAME`_dp_D0_PTR)
#define `$INSTANCE_NAME`_SpeedR (*`$INSTANCE_NAME`_dp_D1_PTR)
#define `$INSTANCE_NAME`_Mode `$INSTANCE_NAME`_mode_Control //control direction and fast/slow decay of each motor

#endif
