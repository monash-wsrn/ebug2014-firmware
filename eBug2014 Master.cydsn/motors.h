#ifndef MOTORS_H
#define MOTORS_H

#include <cytypes.h>
#include "arm_math.h"
void Motors_Start();

arm_pid_instance_q15 Motors_PID_L,Motors_PID_R;
int16 Motors_TargetL,Motors_TargetR;

#endif
