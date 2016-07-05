extern "C"
{
	#include <project.h>
	#include "wireless.h"
	#include "LCD.h"
	#include "motors.h"
	#include "power.h"
	#include "LEDs.h"
	#include "version.h"
	volatile uint32 time;
}
#include "RF24.h"

RF24 nrf(0,0);

__attribute__((section(".ram")))
static void SysTick_isr()
{
	time++;
}


static uint8 bump;
static uint16 LS[16];
__attribute__((section(".ram")))
void link_rx_isr()
{
	uint8 l=link_rx_length;
	uint8 *packet=link_rx_packet;
	link_RX;
	switch(packet[0])
	{
		case 0x40: //Bump sensors
			if(l==2) bump=packet[1]|BUMP_4_Read()<<3|BUMP_5_Read()<<4|BUMP_6_Read()<<5;
			break;
		case 0x50: //Light sensors
			if(l==33) memcpy(LS,packet+1,32);
			break;
	}
}


volatile uint8 persist __attribute__((section(".noinit")));
bool unicast_address_set=false;

__attribute__((section(".ram")))
static void nrf_isr()
{
	uint8 pipe;
	while(nrf.available(&pipe))
	{
		uint8 n=nrf.getDynamicPayloadSize();
		
		uint8 packet[32];
		nrf.read(packet,n);
		if(!n) return;
		
		if(pipe==1) //multicast packet
		{
			switch(packet[0])
			{
				case 0xb0: //Neighbour discovery request (only if no unicast address set)
					if(unicast_address_set) break;
				case 0xb1: //Neighbour discovery request
					{
						uint32 delay=((uint8*)CYDEV_FLSHID_CUST_TABLES_BASE)[packet[1]];
						CyDelayUs(50*delay); //delay based on byte in serial number
						//TODO is it better to have some PRNG seeded with the serial number to set the delay? it's easy to set one up in hardware (could even use a true RNG using 2 analogue blocks -- probably overkill)
					}
					memcpy(packet+1,(uint8*)CYDEV_FLSHID_CUST_TABLES_BASE,7); //reply with serial number
					nrf.stopListening();
					nrf.startWrite(packet,8,true);
					CyDelayUs(200);
					nrf.startListening();
					break;
				case 0xb2: //Set unicast address
					if(!memcmp(packet+1,(uint8*)CYDEV_FLSHID_CUST_TABLES_BASE,7)) //if serial number matches
						unicast_address_set=true,nrf.openReadingPipe(2,packet+8);
					break;
				case 0xb3: //Forget unicast address
					unicast_address_set=false,nrf.closeReadingPipe(2);
					break;
			}
			
			continue;
		}
		
		switch(packet[0])
		{
			case 0x00: //Dummy packet to get pending ACKs
				break;
			case 0x01: //Serial number and device type
				memcpy(packet+1,(uint8*)CYDEV_FLSHID_CUST_TABLES_BASE,7); //send 7 bytes of ID (location of die, wafer number, lot number, week/year of manufacturer)
				packet[8]=0; //device type: 0 means eBug 2014
				nrf.writeAckPayload(2,packet,9);
				break;
			case 0x02: //Firmware version
				{
					char version[]=GIT_VERSION;
					uint32 len=sizeof(version)-1;
					if(len>30) len=30;
					memcpy(packet+1,version,len);
					nrf.writeAckPayload(2,packet,len+1);
				}
				break;
			case 0x03: //JTAG ID
				*(uint32*)(packet+1)=*(reg32*)CYDEV_PANTHER_DEVICE_ID;
				nrf.writeAckPayload(2,packet,5);
				break;
			case 0x10: //Print to LCD top line
				print_top("%s",packet+1);
				break;
			case 0x11: //Print to LCD bottom line
				print_bot("%s",packet+1);
				break;
			case 0x12: //LCD backlight control
				while(link_TX_busy());
				link_TX_copy(packet,n);
				break;
			case 0x20: //Set motor speed/direction
				MotorDriver_SpeedL=*(uint16*)(packet+1); //2 bytes for left speed
				MotorDriver_SpeedR=*(uint16*)(packet+3); //2 bytes for right speed
				MotorDriver_Mode=packet[5]; //1 byte for directions/decay modes
				break;
			case 0x21: //Adjust boost converter voltage
				boost_12V_EN_Write(packet[1]&1); //enable/disable boost converter
				IDAC_12V_adjust_SetPolarity(packet[1]&2?IDAC_12V_adjust_SINK:IDAC_12V_adjust_SOURCE); //source/sink mode for IDAC (SOURCE decreases output voltage/SINK increases)
				IDAC_12V_adjust_Data_REG=packet[2]; //magnitude of current source/sink into feedback pin (1/8uA corresponds to 87.25mV per bit adjustment from nominal 12V)
				break;
			case 0x22: //Get motor feedback and reset counters
			case 0x23: //Get motor feedback and don't reset counters
				*(int16*)(packet+1)=QuadHall_LeftCount; //2 bytes for left count
				*(int16*)(packet+3)=QuadHall_RightCount; //2 bytes for right count
				if(packet[0]==0x21)
				{
					QuadHall_LeftCount=0;
					QuadHall_RightCount=0;
				}
				nrf.writeAckPayload(2,packet,5); //send as ACK payload for pipe 1
				break;
			case 0x24: //Calibrate Hall effect sensors
				//TODO write calibration function
				//TODO should spin motors at some nominal speed and sample Hall sensors using ADC_Hall
				//TODO adjust IDAC offsets until approximately centred
				//TODO store offsets in EEPROM
				break;
			case 0x30: //Enable side/top LEDs and centre LEDs
				LED_enable(packet[1]&1,packet[1]&2);
				LED_centre_SetDriveMode(packet[1]&4?LED_centre_DM_STRONG:LED_centre_DM_DIG_HIZ);
				while(link_TX_busy());
				link_TX_copy(packet,n); //let slave control centre LED
				break;
			case 0x31: //Set LED values
				LEDR=packet[1];
				LEDG=packet[3];
				LEDB=packet[5];
				while(link_TX_busy());
				link_TX_copy(packet,n); //actually only need to send 3 bytes, but simpler to send whole packet
				break;
			case 0x32: //Set LED brightness
				LED_Brightness_Control=packet[1];
				break;
			case 0x40: //Get bump sensor state
				packet[1]=~bump; //bump switches are active low
				nrf.writeAckPayload(2,packet,2);
				break;
			case 0x41: //Get touch button state
				packet[1]=buttons_Status;
				nrf.writeAckPayload(2,packet,2); //send as ACK payload for pipe 1
				break;
			case 0x50: //Get light sensor values
				for(uint32 i=0;i<8;i++)
				{
					packet[1+i*3]=LS[i*2];
					packet[1+i*3+1]=LS[i*2]>>8|(LS[i*2+1]&0xf)<<4;
					packet[1+i*3+2]=LS[i*2+1]>>4;
				}
				nrf.writeAckPayload(2,packet,25);
				break;
			case 0x51: //Control light sensor sensitivity
				//TODO set LS_CLK speed
				//TODO set bias current source (LS_IDAC)
				break;
			case 0x60: //Battery voltage/current
				*(int32*)(packet+1)=battery_voltage; //4 bytes for voltage
				*(int32*)(packet+5)=battery_current; //4 bytes for current
				*(int32*)(packet+9)=charge_current; //4 bytes for charger current
				*(int16*)(packet+13)=die_temperature; //2 bytes for temperature
				nrf.writeAckPayload(2,packet,15); //send as ACK payload for pipe 1
				break;
			case 0x61: //Calibrate delta-sigma ADC (for battery measurements)
				ADC_Power_Calibrate();
				break;
			case 0x62: //Increase charge current
				IDAC_Charge_Data_REG=packet[1];
				break;
			case 0xb3: //Forget unicast address
				unicast_address_set=false,nrf.closeReadingPipe(2);
				break;
			case 0xff: //Reset to bootloader
				persist=2;
			case 0xfe: //Reset
				CySoftwareReset();
		}
	}
}

void Wireless_Start()
{
	CyIntSetSysVector(SysTick_IRQn+16,SysTick_isr);
	SysTick_Config(BCLK__BUS_CLK__HZ/1000); //1ms SysTick
	
	uint8 multicast_address[]={0xff,0x21,0xc8};
	
	nrf.begin();
	nrf.setAddressWidth(3);
	nrf.setRetries(1,15);
	nrf.setChannel(0);
	nrf.enableAckPayload();
	nrf.enableDynamicPayloads();
	nrf.enableDynamicAck();
	nrf.setDataRate(RF24_2MBPS);
	nrf.setCRCLength(RF24_CRC_16);
	nrf.maskIRQ(true,true,false);
	
	nrf.openReadingPipe(1,multicast_address);
	nrf.setAutoAck(1,false); //do not ACK on multicast pipe
	multicast_address[0]=0x00; //address to reply to multicast packets
	nrf.openWritingPipe(multicast_address);	
	
	nrf.startListening();
		
	nrf_packet_ClearPending();
	nrf_packet_StartEx(nrf_isr);
}

void Wireless_Sleep()
{
	nrf.powerDown();
}