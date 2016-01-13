#include <stdio.h>
#include <stdarg.h>
#include "LCD.h"
#include <project.h>

void print_LCD(uint8 row,const char *fmt, ...)
{
	char packet[18];
	packet[0]=row?0x11:0x10; //header byte
	va_list args;
	va_start(args,fmt);
	uint8 n=vsnprintf(packet+1,sizeof(packet)-1,fmt,args);
	va_end(args);
	while(link_TX_busy());
	link_TX_copy((uint8*)packet,n+2);
}