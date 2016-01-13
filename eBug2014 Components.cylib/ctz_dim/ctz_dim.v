
//`#start header` -- edit after this line, do not edit this line
`include "cypress.v"
//`#end` -- edit above this line, do not edit this line
// Generated on 07/01/2015 at 15:24
// Component: ctz_blank
module ctz_dim (
	output  blank,
	input  [5:0] count,
	input  [5:0] density
);

//`#start body` -- edit after this line, do not edit this line

wire [5:0] c=~count;
wire [5:0] d=~density;

assign blank=c[0]?d[5]:c[1]?d[4]:c[2]?d[3]:c[3]?d[2]:c[4]?d[1]:c[5]?d[0]:0;

//`#end` -- edit above this line, do not edit this line
endmodule
//`#start footer` -- edit after this line, do not edit this line
//`#end` -- edit above this line, do not edit this line
