#include "`$INSTANCE_NAME`.h"

void `$INSTANCE_NAME`_Start()
{
	`$INSTANCE_NAME`_TIA_Start();
	`$INSTANCE_NAME`_SH_Start();
	`$INSTANCE_NAME`_IDAC_Start();
	`$INSTANCE_NAME`_Comparator_Start();
	
	*(reg8*)`$INSTANCE_NAME`_Quadrature_L_dp_u0__DP_AUX_CTL_REG|=1;
	*(reg8*)`$INSTANCE_NAME`_Quadrature_R_dp_u0__DP_AUX_CTL_REG|=1;
	*(reg8*)`$INSTANCE_NAME`_Quadrature_L_dp_u0__F0_REG=0;
	*(reg8*)`$INSTANCE_NAME`_Quadrature_R_dp_u0__F0_REG=0;
	`$INSTANCE_NAME`_LeftCount=0;
	`$INSTANCE_NAME`_RightCount=0;
}

void `$INSTANCE_NAME`_SetOffsets(uint8 X1,uint8 Y1,uint8 X2,uint8 Y2)
{
	while(*(reg8*)`$INSTANCE_NAME`_Hall_Select_dp_dp__A0_REG==0x88);
	while(*(reg8*)`$INSTANCE_NAME`_Hall_Select_dp_dp__A0_REG!=0x88);
	*(reg8*)`$INSTANCE_NAME`_DAC_Seq_dp__A0_REG=X1;
	while(*(reg8*)`$INSTANCE_NAME`_Hall_Select_dp_dp__A0_REG!=0x11);
	*(reg8*)`$INSTANCE_NAME`_DAC_Seq_dp__A0_REG=Y1;
	while(*(reg8*)`$INSTANCE_NAME`_Hall_Select_dp_dp__A0_REG!=0x22);
	*(reg8*)`$INSTANCE_NAME`_DAC_Seq_dp__A0_REG=X2;
	while(*(reg8*)`$INSTANCE_NAME`_Hall_Select_dp_dp__A0_REG!=0x44);
	*(reg8*)`$INSTANCE_NAME`_DAC_Seq_dp__A0_REG=Y2;
}

void `$INSTANCE_NAME`_Sync()
{
	while(*(reg8*)`$INSTANCE_NAME`_Hall_Select_dp_dp__A0_REG==0x11);
	while(*(reg8*)`$INSTANCE_NAME`_Hall_Select_dp_dp__A0_REG!=0x11);
}