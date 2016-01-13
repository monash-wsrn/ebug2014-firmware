#include <project.h>
#include "LEDs.h"
#include "LCD.h"
#include "power.h"
#include "wireless.h"
#include "motors.h"

int main()
{
    CyGlobalIntEnable;
	
	CapSense_Start();
	
	link_Start();
	link_rx_ready_StartEx(link_rx_isr);
	
	LED_Count7_Start();
	LEDB=LEDG=LEDR=0x00;
	LED_Brightness_Control=0;
	LED_enable(0,0);
//	LEDR=0x30;
//	LEDG=0x41;
//	LEDB=0x8e;
//	LED_centre_SetDriveMode(LED_centre_DM_DIG_HIZ);
	
	Motors_Start();
	Wireless_Start();
	Power_Start();
	
	CyDelay(100); //give slave some time to start up
	
	uint32 x=CyEnterCriticalSection(); //link_TX_* functions and hence print_* are not reentrant, so things die when printing at different interrupt levels (and in main)
	print_top("----eBug2014----");
	print_bot("                ");
	CyExitCriticalSection(x);
	
	for(;;) CY_PM_WFI;
}
