#ifndef MOTORS_H
#define MOTORS_H

#include <cytypes.h>
void Motors_Start();

typedef struct
{
	int16 P,I,D;
	int16 ei;
	int16 target,target_speed;
} Motor_PID;

Motor_PID Motor_PID_L,Motor_PID_R;

#endif
