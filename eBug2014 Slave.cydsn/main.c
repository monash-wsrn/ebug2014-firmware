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

__attribute__((section(".ram")))
static void rx_isr()
{
	uint8 l=link_rx_length;
	uint8 *packet=link_rx_packet;
	link_RX;
	switch(packet[0])
	{
		case 0x10: //Print to LCD top line
			memcpy(LCD_Top,packet+1,l-1<16?l-1:16);
			break;
		case 0x11: //Print to LCD bottom line
			memcpy(LCD_Bottom,packet+1,l-1<16?l-1:16);
			break;
		case 0x12: //LCD backlight control
			LCD_Backlight_Write(!packet[1]); //active low
			break;
		case 0x30: //Centre LED control
			LED_centre_SetDriveMode(packet[1]&8?LED_centre_DM_STRONG:LED_centre_DM_DIG_HIZ);
			break;
		case 0x31: //Set LED values
			LEDR=packet[2];
			LEDG=packet[4];
			LEDB=packet[6];
			break;
	}
}

__attribute__((section(".ram")))
static void LCD_Update()
{
	LCD_Position(0,0);
	LCD_PrintString(LCD_Top);
	LCD_Position(1,0);
	LCD_PrintString(LCD_Bottom);
}

static uint16 LS[16];
__attribute__((section(".ram")))
static void LS_sample()
{
	static uint8 decimate=1;
	if(!--decimate)
	{
		decimate=100;
		while(link_TX_busy());
		link_TX_copy_header(0x50,(uint8*)LS,32);
		
		uint8 bump=BUMP_1_Read()|BUMP_2_Read()<<1|BUMP_3_Read()<<2;
		while(link_TX_busy());
		link_TX_copy_header(0x40,&bump,1);
	}
}

static void hibernate()
{
	LEDB=LEDG=LEDR=0x00;
	LED_centre_SetDriveMode(LED_centre_DM_DIG_HIZ);
	LCD_Stop();
	LCD_Backlight_Write(1);
	*(reg8*)(LCD_V0__0__PC)&=~0x80; //disable bypass so that pin is not driven from clock
	CyGlobalIntDisable;
	CyPmHibernate();
}

int main()
{
    CyGlobalIntEnable;
	reset_StartEx(CySoftwareReset);
	hibernate_ClearPending();
	hibernate_StartEx(hibernate);
	
	LEDB=LEDG=LEDR=0x00;
//	LEDR=0x4c;
//	LEDG=0x93;
//	LEDB=0x20;
//	LED_centre_SetDriveMode(LED_centre_DM_DIG_HIZ);
//	LCD_Backlight_Write(1);
	
	link_Start();
	link_rx_ready_StartEx(rx_isr);
	
	ADC_Start();
	ADC_SAR_CSR2_REG=ADC_SAR_RESOLUTION_12BIT|2;
	{
		uint8 ch=DMA_ADC_DmaInitialize(2,1,HI16(CYDEV_PERIPH_BASE),HI16(CYDEV_SRAM_BASE));
		uint8 td=CyDmaTdAllocate();
		CyDmaTdSetConfiguration(td,sizeof LS,td,CY_DMA_TD_INC_DST_ADR|DMA_ADC__TD_TERMOUT_EN);
		CyDmaTdSetAddress(td,(uint16)(uint32)ADC_SAR_WRK0_PTR,(uint32)LS);
		CyDmaChSetInitialTd(ch,td);
		
		adc_done_ClearPending();
		adc_done_StartEx(LS_sample);
		CyDmaClearPendingDrq(ch);
		CyDmaChEnable(ch,1);
	}
	
	LS_Count7_Start();
	TH_Start();
	LS_IDAC_Start();
	
	LCD_Start();
	
	CyMasterClk_SetSource(CY_MASTER_SOURCE_IMO);
	CyPLL_OUT_Stop();
	CyPLL_OUT_SetSource(CY_PLL_SOURCE_DSI);
	CyPLL_OUT_SetPQ(64,15,5);
	CyIMO_SetFreq(CY_IMO_FREQ_62MHZ);
	
	uint8 lock=CyPLL_OUT_Start(1)==CYRET_SUCCESS;
	if(lock)
	{
		CyMasterClk_SetSource(CY_MASTER_SOURCE_PLL);
		CyIMO_SetFreq(CY_IMO_FREQ_3MHZ);
	}
	else
	{
		CyDelay(1000);
		memcpy(LCD_Top,"  No PLL lock!   ",16);
	}
	
	lcd_refresh_StartEx(LCD_Update);
    for(;;) CY_PM_WFI;
}
