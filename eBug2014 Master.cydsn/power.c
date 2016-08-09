#include <project.h>
#include "power.h"
#include "LCD.h"
#include "LEDs.h"
#include "wireless.h"

static int32 Power_data[3];
__attribute__((section(".ram")))
static void power_sample()
{
	static uint8 i=0;
	static const int16 *ADC_OFFSET=(int16*)CYDEV_EE_BASE;
	
	ADC_Power_StopConvert();
	PowerMux_Next();
	Power_data[i++]=ADC_Power_GetResult32();
	ADC_Power_StartConvert();
	
	if(i==sizeof(Power_data)/sizeof(Power_data[0]))
	{
		i=0;
		
		int32 I=-Power_data[0]-*ADC_OFFSET;
		I*=100;
		I/=256;
		battery_current=I;
		
		int32 V=Power_data[1]+*ADC_OFFSET;
		V*=147;
		V/=47<<8;
		battery_voltage=V;
		if(V<2700)
		{
			print_top("Battery too low ");
			print_bot("Going to sleep  ");
			CyDelay(1000);
			hibernate();
		}
		
		static int decimate=0;
		if(++decimate==10) decimate=0,print_bot("%4dmV %4dmA   ",V,I);
		
		int32 C=Power_data[2]+*ADC_OFFSET;
		C/=512;
		C+=IDAC_Charge_Data_REG*8;
		charge_current=C;
		
		Temp_GetTemp(&die_temperature);
		//print_top("%2d%cC %4dmA     ",die_temperature,'\xdf',C);
	}
}

void hibernate()
{
	PSoC_PSoC_5_Write(0);
	LED_enable(0,0);
	LEDB=LEDG=LEDR=0x00;
	Wireless_Sleep();
	MotorDriver_SpeedL=0;
	MotorDriver_SpeedR=0;
	LED_centre_SetDriveMode(LED_centre_DM_DIG_HIZ);
	boost_12V_EN_Write(0);
	*(reg8*)(boost_5V_SYNC__0__PC)&=~0x80; //disable bypass so that pin is not driven from clock
	CyGlobalIntDisable;
	CyPmHibernate();
}

void ADC_Power_Calibrate()
{
	int32 o=-Power_data[0];
	int16 offset=o>32767?32767:o<-32768?-32768:o;
	EEPROM_UpdateTemperature();
	EEPROM_WriteByte(offset,0);
	EEPROM_WriteByte(offset>>8,1);
}

void Power_Start()
{
	IDAC_Charge_Start();
	EEPROM_Start();
	PowerMux_Start();
	PowerMux_Next();
	ADC_Power_Start();
	ADC_Power_StartConvert();
	power_sample_StartEx(power_sample);
}