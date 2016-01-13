#include <project.h>
#include "LCD.h"
extern void Wireless_Start();

volatile uint8 persist __attribute__((section(".noinit")));
__attribute__((constructor(101)))
void boot()
{
	if(*(uint32*)0x3004!=0x3011) //make sure reset vector is set correctly in bootloadable
	{
		persist=2;
		return;
	}
	
	if(CY_GET_REG8(CYREG_PHUB_CFGMEM23_CFG1)&CY_RESET_SW) //software reset
	{
		if(persist==1)
		{
			persist=0;
			asm("B 0x3010"); //TODO determine address of bootloadable (dynamically)
		}
	}
	else persist=0;
}

int main()
{
    CyGlobalIntEnable;
	
	CapSense_Start();
	CyDelay(10);
	
	if(persist!=2&&buttons_Status!=3) persist=1,CySoftwareReset();
	LED_centre_SetDriveMode(LED_centre_DM_STRONG);
	
	link_Start();
	uint16 i;
	for(i=0;i<256;i++) link_link_TX=0,CyDelay(1);
	
	print_top("eBug Bootloader ");
	print_bot("Ready...        ");
	
	LED_SIDE_EN_Write(0);
	LED_TOP_EN_Write(0);
	
	Wireless_Start();
	for(;;) CY_PM_WFI;
}
