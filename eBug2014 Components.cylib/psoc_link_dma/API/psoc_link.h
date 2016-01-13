#ifndef `$INSTANCE_NAME`_H
#define `$INSTANCE_NAME`_H
#include <project.h>

#if `$enRX`
	extern uint8 *`$INSTANCE_NAME`_rx_packet;
	extern uint8 `$INSTANCE_NAME`_rx_length;
	#define `$INSTANCE_NAME`_RX (`$INSTANCE_NAME`_irq_rx_ClearPending(),`$INSTANCE_NAME`_irq_rx_Enable())
#endif

#if `$enTX`
	void `$INSTANCE_NAME`_TX(uint8 *packet,uint8 length);
	void `$INSTANCE_NAME`_TX_copy(uint8 *packet,uint8 length);
	void `$INSTANCE_NAME`_TX_copy_header(uint8 header,uint8 *packet,uint8 length);
	uint8 `$INSTANCE_NAME`_TX_busy();
#endif

void `$INSTANCE_NAME`_Start();

#endif
