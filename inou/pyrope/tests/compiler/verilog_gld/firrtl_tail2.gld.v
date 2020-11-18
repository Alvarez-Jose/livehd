module firrtl_tail2 (
  input        clock,
  input        en,
  input  [3:0] inp,
  output [3:0] out
);

reg [3:0] x_next;
wire [3:0] minus = x - 1;
/* wire [4:0] minus = $signed(x) - $signed(2'h1); */
always @ (*) begin
  if (en == 1)
    x_next = inp;
  else
    /* x_next = minus; */
    x_next = {minus[2], minus[2:0]};
end

reg [3:0] x;
always @ (posedge clock) begin
  x <= x_next;
end

assign out = x;

endmodule
