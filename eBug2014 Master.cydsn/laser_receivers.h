#ifndef LASER_RECEIVERS_H
#define LASER_RECEIVERS_H

#include "cytypes.h"

void Receivers_Start();

typedef struct
{
	//uint32 timestamp;
	uint16 state0,state1;
	uint8 length;
	uint8 sensor_id;
} event;
volatile event event_buffer[256]; //ring buffer FIFO for received events
extern volatile uint8 event_head;
extern volatile uint8 event_tail;

#endif
