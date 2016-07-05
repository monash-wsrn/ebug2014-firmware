
//`#start header` -- edit after this line, do not edit this line
`include "cypress.v"
//`#end` -- edit above this line, do not edit this line
// Generated on 03/05/2016 at 14:45
// Component: FSK_RX
module FSK_RX (
	output reg bit,
	output  clk_out,
	output  invalid,
	input   clk,
	input   fsk
);
	parameter threshold = 72;
	parameter too_fast = 56;
	parameter too_slow = 88;

//`#start body` -- edit after this line, do not edit this line
parameter [6:0] period=too_slow-1;
parameter [6:0] cmp=too_slow-threshold; //TODO check that we don't need +1 or -1

wire [6:0] count;
cy_psoc3_count7 #(.cy_period(period),.cy_route_ld(`TRUE),.cy_route_en(`FALSE))
Count7(
    /* input          */  .clock(clk),
    /* input          */  .reset(1'b0),
    /* input          */  .load(fsk),
	/* input          */  .enable(1'b1),
    /* output  [6:0]  */  .count(count),
    /* output         */  .tc(tc)
);

assign invalid=tc;
//TODO also invalidate when period is too short using too_fast parameter (may require more clever trickery)
//TODO another option is to suppress the output clock when period is too short
//TODO suppressing clock is probably better since it limits the clock rate to something much slower than the bus clock

reg clk_out_reg;
always @(posedge clk) clk_out_reg<=fsk||invalid;
cy_psoc3_udb_clock_enable_v1_0 #(.sync_mode(`TRUE)) clkEN (
    .enable(clk_out_reg),   /* Enable from interconnect */
    .clock_in(clk), /* Clock from interconnect */
    .clock_out(clk_out) /* Clock to be used for UDB elements in this component */
); //Level sensitive clock enable

reg bit_;
always @(posedge clk) bit_<=(count>period)?1'bx:(count>=cmp);
always @(posedge clk_out) bit<=bit_;
//`#end` -- edit above this line, do not edit this line
endmodule
//`#start footer` -- edit after this line, do not edit this line
//`#end` -- edit above this line, do not edit this line
