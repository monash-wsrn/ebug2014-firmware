#include "`$INSTANCE_NAME`.h"

void `$INSTANCE_NAME`_Start()
{
	#if `$useCount7` //enable Count7
		#if `$enRX`
		*(reg8*)`$INSTANCE_NAME`_RX_C7_count7__CONTROL_AUX_CTL_REG|=0x20;
		#else
		*(reg8*)`$INSTANCE_NAME`_TX_C7_count7__CONTROL_AUX_CTL_REG|=0x20;
		#endif
	#endif
	*(reg8*)`$INSTANCE_NAME`_dp_u0__DP_AUX_CTL_REG|=3;
	*(reg8*)`$INSTANCE_NAME`_dp_u0__DP_AUX_CTL_REG&=~3; //clear FIFOs
}