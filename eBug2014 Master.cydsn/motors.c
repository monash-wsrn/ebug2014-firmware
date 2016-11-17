#include <project.h>
#include "motors.h"


static uint16 hall_buffer[2][20][4]; //double buffered

__attribute__((section(".ram")))
static void hall_sample()
{
	static uint8 decimate=1;
	decimate++;
	decimate%=100;
	uint8 which=decimate%2; //which buffer
	
	int32 x1=0,y1=0,x2=0,y2=0;
	
	uint16 i;
	for(i=0;i<20;i++)
	{
		x1+=hall_buffer[which][i][0]-2048;
		y1+=hall_buffer[which][i][1]-2048;
		x2+=hall_buffer[which][i][2]-2048;
		y2+=hall_buffer[which][i][3]-2048;
	}
	//if(decimate==0) //print("%3d %3d %3d %3d ",x1/410,y1/410,x2/410,y2/410);
	//	print("%5d %5d ",QuadHall_LeftCount,QuadHall_RightCount);
}

static void Motors_Controller()
{
	int L=arm_pid_q15(&Motors_PID_L,QuadHall_LeftCount-Motors_TargetL); //Run PID
	int R=arm_pid_q15(&Motors_PID_R,QuadHall_RightCount-Motors_TargetR);
	
	L>>=5; //scale to 11 bits signed (arm_pid_q15 saturates to 16 bit range)
	R>>=5;
	
	MotorDriver_SpeedL=L<0?-L:L; //Update speed
	MotorDriver_SpeedR=R<0?-R:R;
	MotorDriver_Mode=L>=0|(R>=0)<<2; //Update direction
}

void Motors_Start()
{
	Motors_PID_L.Kp=Motors_PID_R.Kp=1;
	Motors_PID_L.Ki=Motors_PID_R.Ki=0;
	Motors_PID_L.Kd=Motors_PID_R.Kd=0;
	arm_pid_init_q15(&Motors_PID_L,0);
	arm_pid_init_q15(&Motors_PID_R,0);
	Motors_TargetL=Motors_TargetR=0;
	
	MotorDriver_Start();
	boost_12V_EN_Write(1);
	IDAC_12V_adjust_Start();
	IDAC_12V_adjust_SetPolarity(IDAC_12V_adjust_SOURCE);
	IDAC_12V_adjust_Data_REG=69;
	
	Opamp_Hall_Start();
	QuadHall_Start();
	//QuadHall_SetOffsets(128|27,128|31,128|40,128|16); //offsets use 1-bit sign, 7-bit magnitude format (not 2's complement)
	QuadHall_SetOffsets(0,0,0,0);
	QuadHall_TIA_SetResFB(QuadHall_TIA_RES_FEEDBACK_250K);
	
	//TODO ADC_Hall is only really needed for calibration (we use the comparators and quadrature encoders to track motor position)
	
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