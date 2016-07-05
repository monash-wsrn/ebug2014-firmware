
//`#start header` -- edit after this line, do not edit this line
`include "cypress.v"
//`#end` -- edit above this line, do not edit this line
// Generated on 12/11/2015 at 14:11
// Component: FSK_TX_Count7
module FSK_TX_Count7 (
	output reg fsk,
	input   bit,
	input   clk
);
	parameter period_0 = 10;
	parameter period_1 = 8;

//`#start body` -- edit after this line, do not edit this line

parameter [6:0] cmp_0=period_0/2-2;
parameter [6:0] cmp_1=period_1/2-2;
parameter [6:0] max=(cmp_0<cmp_1?cmp_1:cmp_0)+1;
parameter [2:0] max_bit=max[6]?6:max[5]?5:max[4]?4:max[3]?3:max[2]?2:max[1]?1:0;

wire [6:0] count;
wire [max_bit:0] count_up=~count;
reg load;
always @(posedge clk) begin
	if(load) fsk<=!fsk;
	load<=count_up==(bit?cmp_1:cmp_0);
end

cy_psoc3_count7 #(.cy_period(127),.cy_route_ld(`TRUE),.cy_route_en(`FALSE))
Count7(
    /* input          */  .clock(clk),
    /* input          */  .reset(1'b0),
    /* input          */  .load(load),
	/* input          */  .enable(1'b1),
    /* output  [6:0]  */  .count(count),
    /* output         */  .tc()
);

//TODO maybe use two Count7s and mux them? (probably too hard and not worth the extra count7 -- current version only uses 2 macrocells)

//`#end` -- edit above this line, do not edit this line
endmodule
//`#start footer` -- edit after this line, do not edit this line
//`#end` -- edit above this line, do not edit this line
