#include <project.h>
#include "motors.h"

static int PID(Motor_PID *x,int16 current,int16 current_speed)
{
	int e=x->target-current;
	int ei=x->ei;
	int ed=x->target_speed-current_speed;
	
	ei+=(x->I*e)>>6; //store integral of I*e (6 fractional bits)
	ei=__SSAT(ei,15); //limit effect of I to half of max
	x->ei=ei;
	
	int a=x->P*e+ei+x->D*ed;
	
	return __SSAT(a>>6,10); //6 fractional bits, saturate to 10 bits (limit motor speed to half of maximum)
}

static void Motor_Controller()
{
	int Lspeed=(1<<16)/QuadHall_LeftSpeed(); //QuadHall actually measures motor period (reciprocal of speed)
	int Rspeed=(1<<16)/QuadHall_RightSpeed();
	int L=PID(&Motor_PID_L,QuadHall_LeftCount,Lspeed);
	int R=PID(&Motor_PID_R,QuadHall_RightCount,Rspeed);
	
	MotorDriver_SpeedL=L<0?-L:L; //Update speed
	MotorDriver_SpeedR=R<0?-R:R;
	MotorDriver_Mode=(L>=0)|(R>=0)<<2; //Update direction (and set slow decay mode)
}

void Motors_Start()
{
	MotorDriver_Start();
	boost_12V_EN_Write(1);
	IDAC_12V_adjust_Start();
	IDAC_12V_adjust_SetPolarity(IDAC_12V_adjust_SOURCE);
	IDAC_12V_adjust_Data_REG=69;
	
	Opamp_Hall_Start();
	QuadHall_Start();
	uint16 z=60|-60<<8;
	QuadHall_SetOffsets(z,z,z,z);
	
	memset(&Motor_PID_L,0,sizeof Motor_PID_L);
	memset(&Motor_PID_R,0,sizeof Motor_PID_R);
	
	Motor_Controller_SetVector(Motor_Controller);
	Motor_Controller_SetPriority((uint8)Motor_Controller_INTC_PRIOR_NUMBER);
}