#include <project.h>
#include "motors.h"

static uint16 hall_buffer[2][64][4]; //double buffered
static int16 hall_DC[4];

__attribute__((section(".ram")))
static void hall_sample()
{
	static uint8 which=1;
	which^=1;
	
	static int p=0;
	p++;
	p%=64;
	
	static uint16 max_ring[64][4];
	static uint16 min_ring[64][4];
	
	int i,j;
	for(i=0;i<4;i++)
	{
		max_ring[p][i]=0;
		min_ring[p][i]=4096;
		for(j=0;j<64;j++)
		{
			uint16 x=hall_buffer[which][j][i];
			if(x>max_ring[p][i]) max_ring[p][i]=x;
			if(x<min_ring[p][i]) min_ring[p][i]=x;
		}
		uint16 max=0;
		uint16 min=4096;
		for(j=0;j<64;j++)
		{
			if(max_ring[j][i]>max) max=max_ring[j][i];
			if(min_ring[j][i]<min) min=min_ring[j][i];
		}
		hall_DC[i]=max+min-4096;
	}
}

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
	//QuadHall_SetOffsets(128|27,128|31,128|40,128|16); //offsets use 1-bit sign, 7-bit magnitude format (not 2's complement)
	QuadHall_SetOffsets(0,0,0,0);
	
	if(0) //TODO ADC_Hall is only really needed for calibration (we use the comparators and quadrature encoders to track motor position)
	{
		uint8 ch=DMA_Hall_DmaInitialize(2,1,HI16(CYDEV_PERIPH_BASE),HI16(CYDEV_SRAM_BASE));
		uint8 td0=CyDmaTdAllocate();
		uint8 td1=CyDmaTdAllocate();
		CyDmaTdSetConfiguration(td0,sizeof hall_buffer[0],td1,CY_DMA_TD_INC_DST_ADR|DMA_Hall__TD_TERMOUT_EN);
		CyDmaTdSetConfiguration(td1,sizeof hall_buffer[1],td0,CY_DMA_TD_INC_DST_ADR|DMA_Hall__TD_TERMOUT_EN);
		CyDmaTdSetAddress(td0,(uint16)(uint32)ADC_Hall_SAR_WRK0_PTR,(uint32)hall_buffer[0]);
		CyDmaTdSetAddress(td1,(uint16)(uint32)ADC_Hall_SAR_WRK0_PTR,(uint32)hall_buffer[1]);
		CyDmaChSetInitialTd(ch,td0);
		
		hall_sample_StartEx(hall_sample);
		
		CyDmaChEnable(ch,1);
		
		QuadHall_Sync();
		ADC_Hall_Start();
	}
	
	memset(&Motor_PID_L,0,sizeof Motor_PID_L);
	memset(&Motor_PID_R,0,sizeof Motor_PID_R);
	
	Motor_Controller_SetVector(Motor_Controller);
	Motor_Controller_SetPriority((uint8)Motor_Controller_INTC_PRIOR_NUMBER);
}