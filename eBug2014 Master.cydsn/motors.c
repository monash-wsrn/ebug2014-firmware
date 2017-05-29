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
	//QuadHall_SetOffsets(0,0,0,0); //TODO read from EEPROM
	Hall_Calibrate(40);
	
	memset(&Motor_PID_L,0,sizeof Motor_PID_L);
	memset(&Motor_PID_R,0,sizeof Motor_PID_R);
	
	Motor_Controller_SetVector(Motor_Controller);
	Motor_Controller_SetPriority((uint8)Motor_Controller_INTC_PRIOR_NUMBER);
}

void Hall_Calibrate(uint8 hyst)
{
	uint32 i;
	
	int16 offset[4]={0,0,0,0};
	QuadHall_SetOffsets(offset[0],offset[1],offset[2],offset[3]);
	QuadHall_SetOffsets(offset[0],offset[1],offset[2],offset[3]);
	uint32 initial=Hall_Status;
	uint32 calibrating=0xf;
	while(calibrating)
	{
		QuadHall_SetOffsets(offset[0],offset[1],offset[2],offset[3]);
		QuadHall_SetOffsets(offset[0],offset[1],offset[2],offset[3]); //set offsets twice to ensure outputs have updated
		uint32 current=Hall_Status;
		calibrating&=~initial^current; //if we've crossed the threshold, stop adjusting
		
		for(i=0;i<4;i++) if(calibrating&1<<i)
		{
			if(current&1<<i)
			{
				if(offset[i]>>8>-127+hyst) offset[i]-=0x101; //comparator is high, reduce offset
				else calibrating&=~1<<i; //limit reached, give up
			}
			else
			{
				if(offset[i]>>8<127-hyst) offset[i]+=0x101;  //comparator is low, increase offset
				else calibrating&=~1<<i; //limit reached, give up
			}
		}
	}
	
	for(i=0;i<4;i++) //add hysteresis to offsets
	{
		int32 o=offset[i]>>8;
		offset[i]=(uint8)(o+hyst)|(o-hyst)<<8;
	}
	
	//TODO write offsets to EEPROM
	QuadHall_SetOffsets(offset[0],offset[1],offset[2],offset[3]);
}