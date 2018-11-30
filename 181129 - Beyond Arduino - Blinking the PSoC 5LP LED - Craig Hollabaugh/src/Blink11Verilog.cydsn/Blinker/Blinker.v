
//`#start header` -- edit after this line, do not edit this line
// ========================================
//
// Copyright YOUR COMPANY, THE YEAR
// All Rights Reserved
// UNPUBLISHED, LICENSED SOFTWARE.
//
// CONFIDENTIAL AND PROPRIETARY INFORMATION
// WHICH IS THE PROPERTY OF your company.
//
// ========================================
`include "cypress.v"
//`#end` -- edit above this line, do not edit this line
// Generated on 11/28/2018 at 07:15
// Component: Blinker
module Blinker (
	output  reg Q,
	input   clock,
	input   trigger
);
	parameter duration = 100;
    reg [7:0] count;

//`#start body` -- edit after this line, do not edit this line

always @(posedge trigger or posedge clock)
begin
    if (trigger)
        begin
            Q = 1;
            count = 0;
        end
    else if (count != duration) 
        begin
            count = count + 1;
        end
    else 
        begin
            Q = 0;
        end
    end

//`#end` -- edit above this line, do not edit this line
endmodule
//`#start footer` -- edit after this line, do not edit this line
//`#end` -- edit above this line, do not edit this line
