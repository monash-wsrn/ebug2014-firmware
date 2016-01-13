#include "`$INSTANCE_NAME`.h"

void `$INSTANCE_NAME`_Start()
{
	*(reg16 *)`$INSTANCE_NAME`_dp_u0__16BIT_DP_AUX_CTL_REG|=0x101; //single buffer mode for F0
	*`$INSTANCE_NAME`_dp_F0_PTR=`$period`-1;
}