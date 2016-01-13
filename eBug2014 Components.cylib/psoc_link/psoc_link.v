
//`#start header` -- edit after this line, do not edit this line
`include "cypress.v"
//`#end` -- edit above this line, do not edit this line
// Generated on 12/05/2014 at 11:49
// Component: psoc_link
module psoc_link (
	output  drq_rx,
	output  drq_tx,
	output  tx,
	input   clk,
	input   rx
);
	parameter enRX = 1;
	parameter enTX = 1;
	parameter useCount7 = 1;

//`#start body` -- edit after this line, do not edit this line
wire shift_out;
wire stop;
wire f1_load;
wire a0_load;

generate 
if(enRX) begin: RX
	wire [3:0] in_count;
	
	if(useCount7) begin: C7
		wire [2:0] dummy;
		wire counter_enable=in_count!=7||rx;
		cy_psoc3_count7 #(.cy_period(7'h8),.cy_route_ld(`FALSE),.cy_route_en(`TRUE))
			count7(
				.clock(clk),
				.reset(0),
				.enable(counter_enable),
				.load(0),
			 	.count({dummy,in_count}),
			 	.tc(f1_load)
			);
	end
	else begin
		reg [3:0] in_count_;
		always @(posedge clk) in_count_<=in_count==8||(in_count==0&&!rx)?0:in_count+1;
		assign in_count=in_count_;
		assign f1_load=in_count==8;
	end
	
	if(enTX) begin
		reg [3:0] out_count;
		always @(posedge clk) out_count<=out_count==8||(out_count==0&&!a0_load)?0:out_count+1;
		assign a0_load=out_count==0&&in_count==(useCount7?7:0)&&!stop;
		assign tx=out_count==0?a0_load:shift_out;
	end
	else begin
		assign a0_load=0;
		assign tx=0;
	end
end
else if(enTX) begin: TX
	wire [3:0] out_count;
	assign f1_load=0;
	assign a0_load=out_count==0&&!stop;
	assign tx=out_count==0?!stop:shift_out;
	
	if(useCount7) begin: C7
		wire [2:0] dummy;
		wire counter_enable=out_count!=0||!stop;
		cy_psoc3_count7 #(.cy_period(7'h8),.cy_route_ld(`FALSE),.cy_route_en(`TRUE))
		count7(
			.clock(clk),
			.reset(0),
			.enable(counter_enable),
			.load(0),
		 	.count({dummy,out_count}),
		 	.tc()
		);
		
	end
	else begin
		reg [3:0] out_count_;
		always @(posedge clk) out_count_<=out_count==8||(out_count==0&&!a0_load)?0:out_count+1;
		assign out_count=out_count_;
	end
end
endgenerate

parameter dp_config = 
{
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0, `CS_SHFT_OP___SL, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE, `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA, `CS_CMP_SEL_CFGA,  /*CFGRAM0:    */
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0, `CS_SHFT_OP___SL, `CS_A0_SRC___F0, `CS_A1_SRC_NONE, `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA, `CS_CMP_SEL_CFGA,  /*CFGRAM1:    */
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0, `CS_SHFT_OP___SL, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE, `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA, `CS_CMP_SEL_CFGA,  /*CFGRAM2:    */
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0, `CS_SHFT_OP___SL, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE, `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA, `CS_CMP_SEL_CFGA,  /*CFGRAM3:    */
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0, `CS_SHFT_OP___SL, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE, `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA, `CS_CMP_SEL_CFGA,  /*CFGRAM4:    */
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0, `CS_SHFT_OP___SL, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE, `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA, `CS_CMP_SEL_CFGA,  /*CFGRAM5:    */
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0, `CS_SHFT_OP___SL, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE, `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA, `CS_CMP_SEL_CFGA,  /*CFGRAM6:    */
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0, `CS_SHFT_OP___SL, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE, `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA, `CS_CMP_SEL_CFGA,  /*CFGRAM7:    */
    8'hFF, 8'h00, /*CFG9:    */
    8'hFF, 8'hFF, /*CFG11-10:    */
    `SC_CMPB_A1_D1, `SC_CMPA_A1_D1, `SC_CI_B_ARITH, `SC_CI_A_ARITH, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL, `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI, `SC_SI_A_ROUTE, /*CFG13-12:    */ 
    `SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'h0, `SC_SR_SRC_REG, `SC_FIFO1_ALU, `SC_FIFO0_BUS, `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN, `SC_FB_NOCHN, `SC_CMP1_NOCHN, `SC_CMP0_NOCHN, /*CFG15-14:    */
    3'h00, `SC_FIFO_SYNC__ADD, 2'h00, `SC_FIFO1_DYN_OF, `SC_FIFO0_DYN_OF, `SC_FIFO_CLK1_POS, `SC_FIFO_CLK0_POS, `SC_FIFO_CLK__DP, `SC_FIFO_CAP_AX, `SC_FIFO_LEVEL, `SC_FIFO__SYNC, `SC_EXTCRC_DSBL, `SC_WRK16CAT_DSBL /*CFG17-16:    */
};
cy_psoc3_dp8 #(.cy_dpconfig_a(dp_config))
    dp(
        .clk(clk),
        .cs_addr({2'b0,a0_load}),
        .route_si(rx),
        .route_ci(0),
        .f0_load(0),
        .f1_load(f1_load),
        .d0_load(0),
        .d1_load(0),
        .ce0(),
        .cl0(),
        .z0(),
        .ff0(),
        .ce1(),
        .cl1(),
        .z1(),
        .ff1(),
        .ov_msb(),
        .co_msb(),
        .cmsb(),
        .so(shift_out),
        .f0_bus_stat(drq_tx),
        .f0_blk_stat(stop),
        .f1_bus_stat(drq_rx),
        .f1_blk_stat()
    );
//`#end` -- edit above this line, do not edit this line
endmodule
//`#start footer` -- edit after this line, do not edit this line
//`#end` -- edit above this line, do not edit this line


