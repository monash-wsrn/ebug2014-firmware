#include "`$INSTANCE_NAME`.h"

#if `$enRX`
	uint8 *`$INSTANCE_NAME`_rx_packet;
	uint8 `$INSTANCE_NAME`_rx_length;
	static uint8 ch_rx,td_rx[2];
	static uint8 rx_buffer[2][`$maxPacketSize`];
	static uint8 rx_length[2];
	static void rx_len() //TODO use bitbanding to save 3 loads
	{
		static uint8 which_buffer=0;
		which_buffer^=1;
		
		`$INSTANCE_NAME`_rx_packet=rx_buffer[which_buffer];
		`$INSTANCE_NAME`_rx_length=rx_length[which_buffer]=`$INSTANCE_NAME`_link_RX;
		if(`$INSTANCE_NAME`_rx_length==0) return; //prevent indefinite DMA transfer
		CY_DMA_TDMEM_STRUCT_PTR[td_rx[which_buffer]].TD0[0]=rx_length[which_buffer];
		CY_DMA_CH_STRUCT_PTR[ch_rx].action[0]|=CY_DMA_CPU_TERM_CHAIN;
		CY_DMA_CH_STRUCT_PTR[ch_rx].basic_cfg[0]|=CY_DMA_CH_BASIC_CFG_EN;
		CY_DMA_CH_STRUCT_PTR[ch_rx].basic_status[1]=td_rx[which_buffer];
		CY_DMA_CH_STRUCT_PTR[ch_rx].basic_cfg[0]|=CY_DMA_CH_BASIC_CFG_EN;
		`$INSTANCE_NAME`_irq_rx_Disable();
	}
#endif

#if `$enTX`
	static uint8 ch_tx,td_tx;
	uint8 `$INSTANCE_NAME`_TX_busy()
	{
		return CY_DMA_CH_STRUCT_PTR[ch_tx].basic_cfg[0]&CY_DMA_CH_BASIC_CFG_EN;
	}
	
	void `$INSTANCE_NAME`_TX(uint8 *packet,uint8 length)
	{
		`$INSTANCE_NAME`_link_TX=length;
		if(length==0) return; //prevent indefinite DMA transfer
		CY_DMA_TDMEM_STRUCT_PTR[td_tx].TD0[0]=length;
		reg16 *td_src=(reg16*)CY_DMA_TDMEM_STRUCT_PTR[td_tx].TD1;
		*td_src=LO16((uint32)packet);
		CY_DMA_CH_STRUCT_PTR[ch_tx].basic_status[1]=td_tx;
		CY_DMA_CH_STRUCT_PTR[ch_tx].basic_cfg[0]|=CY_DMA_CH_BASIC_CFG_EN;
	}

	static uint8 buffer[`$maxPacketSize`];
	void `$INSTANCE_NAME`_TX_copy(uint8 *packet,uint8 length)
	{
		memcpy(buffer,packet,length);
		`$INSTANCE_NAME`_TX(buffer,length);
	}
	
	void `$INSTANCE_NAME`_TX_copy_header(uint8 header,uint8 *packet,uint8 length)
	{
		buffer[0]=header;
		memcpy(buffer+1,packet,length);
		`$INSTANCE_NAME`_TX(buffer,length+1);
	}
#endif

void `$INSTANCE_NAME`_Start()
{
	`$INSTANCE_NAME`_link_Start();
	
	#if `$useDMA`
		#if `$enRX`
			ch_rx=`$INSTANCE_NAME`_DMA_RX_DmaInitialize(1,1,HI16(CYDEV_PERIPH_BASE),HI16(CYDEV_SRAM_BASE));
			td_rx[0]=CyDmaTdAllocate();
			td_rx[1]=CyDmaTdAllocate();
			CyDmaTdSetConfiguration(td_rx[0],0,CY_DMA_DISABLE_TD,CY_DMA_TD_INC_DST_ADR|`$INSTANCE_NAME`_DMA_RX__TD_TERMOUT_EN);
			CyDmaTdSetConfiguration(td_rx[1],0,CY_DMA_DISABLE_TD,CY_DMA_TD_INC_DST_ADR|`$INSTANCE_NAME`_DMA_RX__TD_TERMOUT_EN);
			CyDmaTdSetAddress(td_rx[0],LO16((uint32)&`$INSTANCE_NAME`_link_RX),LO16((uint32)rx_buffer[0]));
			CyDmaTdSetAddress(td_rx[1],LO16((uint32)&`$INSTANCE_NAME`_link_RX),LO16((uint32)rx_buffer[1]));
			`$INSTANCE_NAME`_irq_rx_ClearPending();
			`$INSTANCE_NAME`_irq_rx_StartEx(rx_len);
			CY_DMA_CH_STRUCT_PTR[ch_rx].basic_cfg[0]|=CY_DMA_CH_BASIC_CFG_WORK_SEP;
		#endif
		
		#if `$enTX`
			*(reg8*)`$INSTANCE_NAME`_link_dp_u0__DP_AUX_CTL_REG|=4; //set FIFO0_LVL so that there is still space in F0 when DMA stops (allows easy back-to-back packet transfers)
			ch_tx=`$INSTANCE_NAME`_DMA_TX_DmaInitialize(1,1,HI16(CYDEV_SRAM_BASE),HI16(CYDEV_PERIPH_BASE));
			td_tx=CyDmaTdAllocate();
			CyDmaTdSetConfiguration(td_tx,0,CY_DMA_DISABLE_TD,CY_DMA_TD_INC_SRC_ADR|`$INSTANCE_NAME`_DMA_TX__TD_TERMOUT_EN);
			CyDmaTdSetAddress(td_tx,0,LO16((uint32)&`$INSTANCE_NAME`_link_TX));
			CY_DMA_CH_STRUCT_PTR[ch_tx].basic_cfg[0]|=CY_DMA_CH_BASIC_CFG_WORK_SEP;
		#endif
	
	#else
		#error "Not implemented yet; please use DMA instead."
	#endif
}