
//`#start header` -- edit after this line, do not edit this line
`include "cypress.v"
//`#end` -- edit above this line, do not edit this line
// Generated on 03/05/2016 at 15:04
// Component: Bit_Counter_Dual
module Bit_Counter_Dual (
	output reg captureA,
	output reg captureB,
	output	loadingA,
	output	loadingB,
	input   clk,
	input   fskA,
	input   fskB,
	input   invalidA,
	input   invalidB,
	input   wrong_bitA,
	input   wrong_bitB
);
	parameter preload = 30;

//`#start body` -- edit after this line, do not edit this line

parameter ERROR_RECOVERY=0; //TODO error recovery seems to break things sometimes (maybe captures happen too often causing FIFO overflow?)

wire a0d0,a1d1,a0ff,a1ff,a0zz,a1zz;
reg [2:0] addr;
reg [1:0] stateA,stateB;
always @(posedge clk) begin
	if(invalidA) stateA<=2'b00; //invalid_latch state (reset counter on next fsk)
	else if(fskA) begin
		if(stateA==2'b00) stateA<=2'b01; //preload appropriate number of bits (ignore whether they match the prediction)
		else if(stateA==2'b01&&!a0d0) stateA<=2'b10; //preload finished; count consecutive correct bits
		else if(stateA==2'b10&&!a0d0) stateA<=2'b11; //predictor is now synchronised; capture+reset counter on every wrong bit and on invalid
		else if(stateA==2'b11&&wrong_bitA&&!ERROR_RECOVERY) stateA<=2'b10; //start counting again (if error recovery is disabled)
	end

	if(invalidB) stateB<=2'b00; //invalid_latch state (reset counter on next fsk)
	else if(!fskA&&fskB) begin //don't switch states unless datapath does something
		if(stateB==2'b00) stateB<=2'b01; //preload appropriate number of bits (ignore whether they match the prediction)
		else if(stateB==2'b01&&!a1d1) stateB<=2'b10; //preload finished; count consecutive correct bits
		else if(stateB==2'b10&&!a1d1) stateB<=2'b11; //predictor is now synchronised; capture+reset counter on every wrong bit and on invalid
		else if(stateB==2'b11&&wrong_bitB&&!ERROR_RECOVERY) stateB<=2'b10; //start counting again (if error recovery is disabled)
	end
	
	if(fskA)      addr<={2'b00,stateA==2'b00||(stateA==2'b01&&!a0d0)||(stateA[1]&&wrong_bitA)};
	else if(fskB) addr<={2'b01,stateB==2'b00||(stateB==2'b01&&!a1d1)||(stateB[1]&&wrong_bitB)};
	else addr<=3'b1xx;
	
	captureA<=(fskA&&a0ff&&stateA==2'b11)||(fskA&&!a0zz&&wrong_bitA&&stateA==2'b11)||(invalidA&&!a0zz&&stateA==2'b11);
	captureB<=(fskB&&a1ff&&stateB==2'b11)||(fskB&&!a1zz&&wrong_bitB&&stateB==2'b11)||(invalidB&&!a1zz&&stateB==2'b11);
end

assign loadingA=stateA!=2'b11;
assign loadingB=stateB!=2'b11;

cy_psoc3_dp8 #(.a0_init_a(8'h00), .a1_init_a(8'h00), .d0_init_a(preload), 
.d1_init_a(preload), 
.cy_dpconfig_a(
{
    `CS_ALU_OP__INC, `CS_SRCA_A0, `CS_SRCB_A0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM0:  */
    `CS_ALU_OP__SUB, `CS_SRCA_A0, `CS_SRCB_A0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM1:  */
    `CS_ALU_OP__INC, `CS_SRCA_A1, `CS_SRCB_A1,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC__ALU,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM2:  */
    `CS_ALU_OP__SUB, `CS_SRCA_A1, `CS_SRCB_A1,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC__ALU,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM3:  */
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_A0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM4:  */
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_A0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM5:  */
    `CS_ALU_OP_PASS, `CS_SRCA_A1, `CS_SRCB_A1,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM6:  */
    `CS_ALU_OP_PASS, `CS_SRCA_A1, `CS_SRCB_A1,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM7:  */
    8'hFF, 8'h00,  /*CFG9:                  */
    8'hFF, 8'hFF,  /*CFG11-10:                  */
    `SC_CMPB_A1_D1, `SC_CMPA_A1_D1, `SC_CI_B_ARITH,
    `SC_CI_A_ARITH, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
    `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
    `SC_SI_A_DEFSI, /*CFG13-12:                  */
    `SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'h0,
    1'h0, `SC_FIFO1__A1, `SC_FIFO0__A0,
    `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
    `SC_FB_NOCHN, `SC_CMP1_NOCHN,
    `SC_CMP0_NOCHN, /*CFG15-14:                  */
    10'h00, `SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,
    `SC_FIFO_LEVEL,`SC_FIFO__SYNC,`SC_EXTCRC_DSBL,
    `SC_WRK16CAT_DSBL /*CFG17-16:                  */
}
)) dp(
        /*  input                   */  .reset(1'b0),
        /*  input                   */  .clk(clk),
        /*  input   [02:00]         */  .cs_addr(addr),
        /*  input                   */  .route_si(1'b0),
        /*  input                   */  .route_ci(1'b0),
        /*  input                   */  .f0_load(captureA),
        /*  input                   */  .f1_load(captureB),
        /*  input                   */  .d0_load(1'b0),
        /*  input                   */  .d1_load(1'b0),
        /*  output                  */  .ce0(),
        /*  output                  */  .cl0(a0d0),
        /*  output                  */  .z0(a0zz),
        /*  output                  */  .ff0(a0ff),
        /*  output                  */  .ce1(),
        /*  output                  */  .cl1(a1d1),
        /*  output                  */  .z1(a1zz),
        /*  output                  */  .ff1(a1ff),
        /*  output                  */  .ov_msb(),
        /*  output                  */  .co_msb(),
        /*  output                  */  .cmsb(),
        /*  output                  */  .so(),
        /*  output                  */  .f0_bus_stat(),
        /*  output                  */  .f0_blk_stat(),
        /*  output                  */  .f1_bus_stat(),
        /*  output                  */  .f1_blk_stat()
);
//`#end` -- edit above this line, do not edit this line
endmodule
//`#start footer` -- edit after this line, do not edit this line
//`#end` -- edit above this line, do not edit this line
