/* Generated by Yosys 0.9+3582 (git sha1 de7997837, gcc 9.3.0-15 -fPIC -Os) */

module counter_single_if(reset, clock, c1, cnt_o);
  wire _00_;
  wire _01_;
  wire _02_;
  wire _03_;
  wire _04_;
  wire _05_;
  wire _06_;
  wire _07_;
  wire _08_;
  wire _09_;
  wire _10_;
  input [1:0] c1;
  input clock;
  output [4:0] cnt_o;
  input reset;
  sky130_fd_sc_hs__a31oi_1 _11_ (
    .A1(c1[0]),
    .A2(cnt_o[1]),
    .A3(cnt_o[0]),
    .B1(cnt_o[2]),
    .Y(_04_)
  );
  sky130_fd_sc_hs__and4_1 _12_ (
    .A(c1[0]),
    .B(cnt_o[2]),
    .C(cnt_o[1]),
    .D(cnt_o[0]),
    .X(_05_)
  );
  sky130_fd_sc_hs__nor3_1 _13_ (
    .A(reset),
    .B(_04_),
    .C(_05_),
    .Y(_02_)
  );
  sky130_fd_sc_hs__a21oi_1 _14_ (
    .A1(c1[0]),
    .A2(cnt_o[0]),
    .B1(cnt_o[1]),
    .Y(_06_)
  );
  sky130_fd_sc_hs__a31o_1 _15_ (
    .A1(c1[0]),
    .A2(cnt_o[1]),
    .A3(cnt_o[0]),
    .B1(reset),
    .X(_07_)
  );
  sky130_fd_sc_hs__nor2_1 _16_ (
    .A(_06_),
    .B(_07_),
    .Y(_01_)
  );
  sky130_fd_sc_hs__o21bai_1 _17_ (
    .A1(c1[0]),
    .A2(cnt_o[0]),
    .B1_N(reset),
    .Y(_08_)
  );
  sky130_fd_sc_hs__a21oi_1 _18_ (
    .A1(c1[0]),
    .A2(cnt_o[0]),
    .B1(_08_),
    .Y(_00_)
  );
  sky130_fd_sc_hs__a41oi_1 _19_ (
    .A1(c1[0]),
    .A2(cnt_o[2]),
    .A3(cnt_o[1]),
    .A4(cnt_o[0]),
    .B1(cnt_o[3]),
    .Y(_09_)
  );
  sky130_fd_sc_hs__and4_1 _20_ (
    .A(cnt_o[3]),
    .B(c1[0]),
    .C(cnt_o[2]),
    .D(cnt_o[0]),
    .X(_10_)
  );
  sky130_fd_sc_hs__a211oi_1 _21_ (
    .A1(cnt_o[1]),
    .A2(_10_),
    .B1(_09_),
    .C1(reset),
    .Y(_03_)
  );
  sky130_fd_sc_hs__dfxtp_1 _22_ (
    .CLK(clock),
    .D(_00_),
    .Q(cnt_o[0])
  );
  sky130_fd_sc_hs__dfxtp_1 _23_ (
    .CLK(clock),
    .D(_01_),
    .Q(cnt_o[1])
  );
  sky130_fd_sc_hs__dfxtp_1 _24_ (
    .CLK(clock),
    .D(_02_),
    .Q(cnt_o[2])
  );
  sky130_fd_sc_hs__dfxtp_1 _25_ (
    .CLK(clock),
    .D(_03_),
    .Q(cnt_o[3])
  );
  assign cnt_o[4] = 1'b0;
endmodule
