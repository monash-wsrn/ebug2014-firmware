
//`#start header` -- edit after this line, do not edit this line
`include "cypress.v"
//`#end` -- edit above this line, do not edit this line
// Generated on 06/26/2015 at 19:23
// Component: ShiftRegister
module ShiftRegister (
	output [7:0] p_out,
	output  s_out,
	input   blank,
	input   clk,
	input   load,
	input  [7:0] p_in,
	input   s_in
);
	parameter initial_reg = 0;
	parameter use_datapath = 1;

//`#start body` -- edit after this line, do not edit this line

generate
if(use_datapath) begin: dp
	cy_psoc3_dp #(.a0_init(initial_reg), .a1_init(8'h0), 
.cy_dpconfig(
	{
	    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
	    `CS_SHFT_OP___SL, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
	    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
	    `CS_CMP_SEL_CFGA, /*CFGRAM0:     */
	    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
	    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
	    `CS_FEEDBACK_ENBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
	    `CS_CMP_SEL_CFGA, /*CFGRAM1:     */
	    `CS_ALU_OP_PASS, `CS_SRCA_A1, `CS_SRCB_D0,
	    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
	    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
	    `CS_CMP_SEL_CFGA, /*CFGRAM2:     */
	    `CS_ALU_OP_PASS, `CS_SRCA_A1, `CS_SRCB_D0,
	    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
	    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
	    `CS_CMP_SEL_CFGA, /*CFGRAM3:     */
	    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
	    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
	    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
	    `CS_CMP_SEL_CFGA, /*CFGRAM4:     */
	    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
	    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
	    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
	    `CS_CMP_SEL_CFGA, /*CFGRAM5:     */
	    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
	    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
	    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
	    `CS_CMP_SEL_CFGA, /*CFGRAM6:     */
	    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
	    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
	    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
	    `CS_CMP_SEL_CFGA, /*CFGRAM7:     */
	    8'hFF, 8'h00,  /*CFG9:     */
	    8'hFF, 8'hFF,  /*CFG11-10:     */
	    `SC_CMPB_A1_D1, `SC_CMPA_A1_D1, `SC_CI_B_ARITH,
	    `SC_CI_A_ARITH, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
	    `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_ROUTE,
	    `SC_SI_A_ROUTE, /*CFG13-12:     */
	    `SC_A0_SRC_ACC, `SC_SHIFT_SL, `SC_PI_DYN_EN,
	    1'h0, `SC_FIFO1_BUS, `SC_FIFO0_BUS,
	    `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
	    `SC_FB_NOCHN, `SC_CMP1_NOCHN,
	    `SC_CMP0_NOCHN, /*CFG15-14:     */
	    10'h00, `SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,
	    `SC_FIFO_LEVEL,`SC_FIFO__SYNC,`SC_EXTCRC_DSBL,
	    `SC_WRK16CAT_DSBL /*CFG17-16:     */
	}
	)) dp(
	        /*  input                   */  .reset(1'b0),
	        /*  input                   */  .clk(clk),
	        /*  input   [02:00]         */  .cs_addr({1'b0,blank,load}),
	        /*  input                   */  .route_si(s_in),
	        /*  input                   */  .route_ci(1'b0),
	        /*  input                   */  .f0_load(1'b0),
	        /*  input                   */  .f1_load(1'b0),
	        /*  input                   */  .d0_load(1'b0),
	        /*  input                   */  .d1_load(1'b0),
	        /*  output                  */  .ce0(),
	        /*  output                  */  .cl0(),
	        /*  output                  */  .z0(),
	        /*  output                  */  .ff0(),
	        /*  output                  */  .ce1(),
	        /*  output                  */  .cl1(),
	        /*  output                  */  .z1(),
	        /*  output                  */  .ff1(),
	        /*  output                  */  .ov_msb(),
	        /*  output                  */  .co_msb(),
	        /*  output                  */  .cmsb(),
	        /*  output                  */  .so(s_out),
	        /*  output                  */  .f0_bus_stat(),
	        /*  output                  */  .f0_blk_stat(),
	        /*  output                  */  .f1_bus_stat(),
	        /*  output                  */  .f1_blk_stat(),
	        
	        /* input                    */  .ci(1'b0),     // Carry in from previous stage
	        /* output                   */  .co(),         // Carry out to next stage
	        /* input                    */  .sir(1'b0),    // Shift in from right side
	        /* output                   */  .sor(),        // Shift out to right side
	        /* input                    */  .sil(1'b0),    // Shift in from left side
	        /* output                   */  .sol(),        // Shift out to left side
	        /* input                    */  .msbi(1'b0),   // MSB chain in
	        /* output                   */  .msbo(),       // MSB chain out
	        /* input [01:00]            */  .cei(2'b0),    // Compare equal in from prev stage
	        /* output [01:00]           */  .ceo(),        // Compare equal out to next stage
	        /* input [01:00]            */  .cli(2'b0),    // Compare less than in from prv stage
	        /* output [01:00]           */  .clo(),        // Compare less than out to next stage
	        /* input [01:00]            */  .zi(2'b0),     // Zero detect in from previous stage
	        /* output [01:00]           */  .zo(),         // Zero detect out to next stage
	        /* input [01:00]            */  .fi(2'b0),     // 0xFF detect in from previous stage
	        /* output [01:00]           */  .fo(),         // 0xFF detect out to next stage
	        /* input [01:00]            */  .capi(2'b0),   // Software capture from previous stage
	        /* output [01:00]           */  .capo(),       // Software capture to next stage
	        /* input                    */  .cfbi(1'b0),   // CRC Feedback in from previous stage
	        /* output                   */  .cfbo(),       // CRC Feedback out to next stage
	        /* input [07:00]            */  .pi(p_in),     // Parallel data port
	        /* output [07:00]           */  .po(p_out)          // Parallel data port
	);
end
else begin
	reg [7:0] shift;
	assign p_out=shift;
	assign s_out=shift[7];
	always @(posedge clk)
		if(load) shift<=p_in;
		else shift<={shift[6:0],s_in};
end
endgenerate
//`#end` -- edit above this line, do not edit this line
endmodule
//`#start footer` -- edit after this line, do not edit this line
//`#end` -- edit above this line, do not edit this line
