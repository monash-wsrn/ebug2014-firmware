extern "C"
{
	#include <project.h>
	#include "LCD.h"
	void Wireless_Start();
	volatile uint32 time;
	extern volatile uint8 persist;
}
#include "RF24.h"

RF24 nrf(0,0);

static void SysTick_isr()
{
	time++;
}

static volatile bool ack=false;

static void flashing_failed()
{
	LEDB_Control=0;
	LEDR_Control=0x7f;
	CyDelay(500);
	persist=2;
	CySoftwareReset();
	//TODO clear first row (number 0x30 currently) to prevent half programmed bootloadable from running
}

static void nrf_isr()
{
	uint8 pipe;
	while(nrf.available(&pipe))
	{
		uint8 n=nrf.getDynamicPayloadSize();
		
		static uint8 packet_buffer[33]; //1 extra byte for header to send to slave
		uint8 *packet=packet_buffer+1;
		nrf.read(packet,n);
		if(n!=32) continue;
		
		uint16 *header=(uint16*)packet;
		if(*header==0xffff)
		{
			print_bot("Resetting...    ");
			CyDelay(200);
			persist=1;
			CySoftwareReset();
		}
		
		switch(pipe)
		{
			case 1:
			{
				print_top("Flashing Master:");
				
				static bool first=true; //are we waiting for welcome packet?
				static uint16 n_rows=1024;
				static uint16 first_row=0;
				static uint16 row=0; //current row being buffered
				static uint8 seq=0; //packet number in current row
				
				if(first)
				{
					first=false;
					if(*header!=0xff00) //ensure first packet is welcome packet
					{
						print_top("Error flashing: ");
						print_bot("Not welcome %04x",*header);
						flashing_failed();
					}
					first_row=row=header[1]; //first row to program
					//TODO ensure first row is past end of bootloader (0x30 rows currently)
					n_rows=header[2];
					
					if(*(uint32*)(packet+6)!=*(reg32*)CYDEV_PANTHER_DEVICE_ID) //verify JTAG ID
					{
						print_top("Error flashing: ");
						print_bot("Wrong JTAG ID   ");
						CyDelay(500);
						print_bot("0x%08x      ",*(uint32*)(packet+6));
						flashing_failed();
					}
					CySetTemp(); //once should be enough if temp doesn't change by more than 10C
				}
				else
				{
					//TODO timeout (if no packets for a while then bail and restart)
					
					uint16 packet_row=(*header)&0x3ff;
					uint8 packet_seq=(*header)>>10;
					
					if(packet_row!=row or packet_seq!=seq)
					{
						print_top("Error flashing: ");
						print_bot("Wrong row/seq no");
						flashing_failed();
					}
					static uint8 row_buffer[256];
					memcpy(row_buffer+seq*30,packet+2,seq==8?16:30);
					
					if(seq++==8) //last packet for row
					{
						uint8 progress=(row-first_row)*8/n_rows;
						LEDB_Control=(1<<progress)-1;
						print_bot("Row %3d/%d     ",row-first_row+1,n_rows);
						CyWriteRowData(row>>8,row&0xff,row_buffer);
						seq=0;
						row++;
						if(row==first_row+n_rows)
						{
							LEDB_Control=0;
							LEDG_Control=0x7f;
						}
					}
				}
				break;
			}
			case 2: //packet is for slave; forward directly
			{
				packet_buffer[0]=0xf0;
				while(link_TX_busy());
				link_TX_copy((uint8*)packet_buffer,33);
				ack=false;
				while(!ack); //wait for ACK from slave
				break;
			}
		}
	}
}

static void rx_isr()
{
	uint8 l=link_rx_length;
	link_RX;
	switch(link_rx_packet[0])
	{
		case 0xf0:
			ack=true;
			break;
	}
}

void Wireless_Start()
{
	CyIntSetSysVector(SysTick_IRQn+16,SysTick_isr);
	SysTick_Config(BCLK__BUS_CLK__HZ/1000); //1ms SysTick
	
	link_rx_ready_StartEx(rx_isr);
	
	uint8 rx_address[]={0x19,0x8a,0xaf};  //for master programming
	uint8 rx_address2[]={0x1a,0x8a,0xaf}; //for slave programming
	
	nrf.begin();
	nrf.setAddressWidth(3);
	nrf.setRetries(1,15);
	nrf.setChannel(0);
	nrf.enableAckPayload();
	nrf.enableDynamicPayloads();
	nrf.enableDynamicAck();
	nrf.setDataRate(RF24_2MBPS);
	nrf.setCRCLength(RF24_CRC_16);
	nrf.openReadingPipe(1,rx_address);
	nrf.openReadingPipe(2,rx_address2);
	nrf.setAutoAck(1,true);
	nrf.setAutoAck(2,true);
	nrf.startListening();
	
	nrf_packet_ClearPending();
	nrf_packet_StartEx(nrf_isr);
}