#include <project.h>
#include "laser_receivers.h"

volatile uint8 event_head=0;
volatile uint8 event_tail=0;


static void push_event(event e)
{
	if(event_head+1!=event_tail) //prevent overflows (discard new event if buffer is full)
		event_buffer[event_head++]=e; //if interrupts have different priorities, then this needs to be wrapped in a critical section (but equal priorities cannot interrupt each other)
}

static void capture0A()
{
	event e={Receiver_0_state0A,Receiver_0_state1A,Receiver_0_lengthA,0};
	push_event(e);
}

static void capture0B()
{
	event e={Receiver_0_state0B,Receiver_0_state1B,Receiver_0_lengthB,1};
	push_event(e);
}

void Receivers_Start()
{
	Comp_Start();
	IDAC_Start();
	
	Receiver_0_Start();
	
	capture0A_StartEx(capture0A);
	capture0B_StartEx(capture0B);
}
