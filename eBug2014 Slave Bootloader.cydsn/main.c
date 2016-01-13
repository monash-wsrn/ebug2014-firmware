#include <project.h>
#include <stdio.h>
#include <stdarg.h>

static char LCD_Top[17];
static char LCD_Bottom[17];

void print_top(const char *fmt, ...)
{
	va_list args;
	va_start(args,fmt);
	vsnprintf(LCD_Top,sizeof(LCD_Top),fmt,args);
	va_end(args);
}

void print_bot(const char *fmt, ...)
{
	va_list args;
	va_start(args,fmt);
	vsnprintf(LCD_Bottom,sizeof(LCD_Bottom),fmt,args);
	va_end(args);
}

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

static void run_app_isr()
{
	persist=1;
	CySoftwareReset();
}

static void LCD_Update()
{
	LCD_Position(0,0);
	LCD_PrintString(LCD_Top);
	LCD_Position(1,0);
	LCD_PrintString(LCD_Bottom);
}

static void flashing_failed()
{
	LEDB_Control=0;
	LEDR_Control=0x7f;
	LCD_Update();
	CyDelay(500);
	persist=2;
	CySoftwareReset();
	//TODO clear first row (number 0x30 currently) to prevent half programmed bootloadable from running
}

static void rx_isr()
{
	uint8 l=link_rx_length;
	link_RX;
	switch(link_rx_packet[0])
	{
		case 0x10: //print on LCD line 0
			memcpy(LCD_Top,link_rx_packet+1,l-1<16?l-1:16);
			break;
		case 0x11: //print on LCD line 1
			memcpy(LCD_Bottom,link_rx_packet+1,l-1<16?l-1:16);
			break;
		case 0x12: //clear screen
			LCD_ClearDisplay(); //TODO this can take too long
			break;
		case 0xf0: //programming data
		{
			print_top("Flashing Slave: ");
			uint8 *packet=link_rx_packet+1;
			uint16 *header=(uint16*)packet;
							
			static uint8 first=1; //are we waiting for welcome packet?
			static uint16 n_rows=1024;
			static uint16 first_row=0;
			static uint16 row=0; //current row being buffered
			static uint8 seq=0; //packet number in current row
			
			if(first)
			{
				first=0;
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
				
				if(packet_row!=row||packet_seq!=seq)
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
					print_bot("Row %3d/%d      ",row-first_row+1,n_rows);
					CyWriteRowData(row>>8,row&0xff,row_buffer);
					seq=0;
					row++;
					if(row==first_row+n_rows)
					{
						LEDB_Control=0;
						LEDG_Control=0x7f;
						run_app_StartEx(run_app_isr);
					}
				}
			}
			
			uint8 ACK=0xf0;
			while(link_TX_busy());
			link_TX_copy(&ACK,1); //send ack back to master
			break;
		}
	}
}

int main()
{
    CyGlobalIntEnable;
	link_Start();
	link_rx_ready_StartEx(rx_isr);
	
	LCD_Start();
	lcd_refresh_StartEx(LCD_Update);
		
	if(persist!=2) run_app_StartEx(run_app_isr);
	LED_centre_SetDriveMode(LED_centre_DM_STRONG);
	
    for(;;) CY_PM_WFI;
}
