#include <project.h>
#include "LEDs.h"

void LED_enable(uint8 side,uint8 top)
{
	LED_SIDE_EN_SetDriveMode(side?LED_SIDE_EN_DM_OD_LO:LED_SIDE_EN_DM_DIG_HIZ);
	LED_TOP_EN_SetDriveMode(top?LED_TOP_EN_DM_OD_LO:LED_TOP_EN_DM_DIG_HIZ);
}