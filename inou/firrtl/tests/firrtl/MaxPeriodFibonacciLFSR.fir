circuit MaxPeriodFibonacciLFSR :   
; module MaxPeriodFibonacciLFSR : 
;   input clock : Clock
;   input reset : UInt<1>
;   output io : {flip seed : {valid : UInt<1>, bits : UInt<1>[2]}, flip increment : UInt<1>, out : UInt<1>[2]}
;   
;   wire _T : UInt<1>[2] @[PRNG.scala 38:28]
;   _T[0] <= UInt<1>("h00") @[PRNG.scala 38:28]
;   _T[1] <= UInt<1>("h01") @[PRNG.scala 38:28]
;   reg state : UInt<1>[2], clock with : (reset => (reset, _T)) @[PRNG.scala 47:50]
;   when io.increment : @[PRNG.scala 61:23]
;     node _T_1 = xor(state[1], state[0]) @[LFSR.scala 15:41]
;     node _T_2 = xor(_T_1, state[1]) @[LFSR.scala 15:41]
;     node _T_3 = xor(_T_2, state[0]) @[LFSR.scala 15:41]
;     state[0] <= _T_3 @[PRNG.scala 62:11]
;     state[1] <= state[0] @[PRNG.scala 62:11]
;   when io.seed.valid : @[PRNG.scala 65:25]
;     state[0] <= io.seed.bits[0] @[PRNG.scala 66:11]
;     state[1] <= io.seed.bits[1] @[PRNG.scala 66:11]
;   io.out[0] <= state[0] @[PRNG.scala 69:10]
;   io.out[1] <= state[1] @[PRNG.scala 69:10]
  module MaxPeriodFibonacciLFSR : 
    input clock : Clock
    input reset : UInt<1>
    output io : {flip seed : {valid : UInt<1>, bits : UInt<1>[3]}, flip increment : UInt<1>, out : UInt<1>[3]}
    
    wire _T : UInt<1>[3] @[PRNG.scala 38:28]
    _T[0] <= UInt<1>("h00") @[PRNG.scala 38:28]
    _T[1] <= UInt<1>("h00") @[PRNG.scala 38:28]
    _T[2] <= UInt<1>("h01") @[PRNG.scala 38:28]
    reg state : UInt<1>[3], clock with : (reset => (reset, _T)) @[PRNG.scala 47:50]
    when io.increment : @[PRNG.scala 61:23]
      node _T_1 = xor(state[2], state[0]) @[LFSR.scala 15:41]
      node _T_2 = xor(_T_1, state[1]) @[LFSR.scala 15:41]
      node _T_3 = xor(_T_2, state[0]) @[LFSR.scala 15:41]
      state[0] <= _T_3 @[PRNG.scala 62:11]
      state[1] <= state[0] @[PRNG.scala 62:11]
      state[2] <= state[1] @[PRNG.scala 62:11]
    when io.seed.valid : @[PRNG.scala 65:25]
      state[0] <= io.seed.bits[0] @[PRNG.scala 66:11]
      state[1] <= io.seed.bits[1] @[PRNG.scala 66:11]
      state[2] <= io.seed.bits[2] @[PRNG.scala 66:11]
    io.out[0] <= state[0] @[PRNG.scala 69:10]
    io.out[1] <= state[1] @[PRNG.scala 69:10]
    io.out[2] <= state[2] @[PRNG.scala 69:10]

; module MaxPeriodFibonacciLFSR : 
;   input clock : Clock
;   input reset : UInt<1>
;   output io : {flip seed : {valid : UInt<1>, bits : UInt<1>[16]}, flip increment : UInt<1>, out : UInt<1>[16]}
;   
;   wire _T : UInt<1>[16] @[PRNG.scala 38:28]
;   _T[0] <= UInt<1>("h00") @[PRNG.scala 38:28]
;   _T[1] <= UInt<1>("h00") @[PRNG.scala 38:28]
;   _T[2] <= UInt<1>("h00") @[PRNG.scala 38:28]
;   _T[3] <= UInt<1>("h00") @[PRNG.scala 38:28]
;   _T[4] <= UInt<1>("h00") @[PRNG.scala 38:28]
;   _T[5] <= UInt<1>("h00") @[PRNG.scala 38:28]
;   _T[6] <= UInt<1>("h00") @[PRNG.scala 38:28]
;   _T[7] <= UInt<1>("h00") @[PRNG.scala 38:28]
;   _T[8] <= UInt<1>("h00") @[PRNG.scala 38:28]
;   _T[9] <= UInt<1>("h00") @[PRNG.scala 38:28]
;   _T[10] <= UInt<1>("h00") @[PRNG.scala 38:28]
;   _T[11] <= UInt<1>("h00") @[PRNG.scala 38:28]
;   _T[12] <= UInt<1>("h00") @[PRNG.scala 38:28]
;   _T[13] <= UInt<1>("h00") @[PRNG.scala 38:28]
;   _T[14] <= UInt<1>("h00") @[PRNG.scala 38:28]
;   _T[15] <= UInt<1>("h01") @[PRNG.scala 38:28]
;   reg state : UInt<1>[16], clock with : (reset => (reset, _T)) @[PRNG.scala 47:50]
;   when io.increment : @[PRNG.scala 61:23]
;     node _T_1 = xor(state[15], state[13]) @[LFSR.scala 15:41]
;     node _T_2 = xor(_T_1, state[12]) @[LFSR.scala 15:41]
;     node _T_3 = xor(_T_2, state[10]) @[LFSR.scala 15:41]
;     state[0] <= _T_3 @[PRNG.scala 62:11]
;     state[1] <= state[0] @[PRNG.scala 62:11]
;     state[2] <= state[1] @[PRNG.scala 62:11]
;     state[3] <= state[2] @[PRNG.scala 62:11]
;     state[4] <= state[3] @[PRNG.scala 62:11]
;     state[5] <= state[4] @[PRNG.scala 62:11]
;     state[6] <= state[5] @[PRNG.scala 62:11]
;     state[7] <= state[6] @[PRNG.scala 62:11]
;     state[8] <= state[7] @[PRNG.scala 62:11]
;     state[9] <= state[8] @[PRNG.scala 62:11]
;     state[10] <= state[9] @[PRNG.scala 62:11]
;     state[11] <= state[10] @[PRNG.scala 62:11]
;     state[12] <= state[11] @[PRNG.scala 62:11]
;     state[13] <= state[12] @[PRNG.scala 62:11]
;     state[14] <= state[13] @[PRNG.scala 62:11]
;     state[15] <= state[14] @[PRNG.scala 62:11]
;     skip @[PRNG.scala 61:23]
;   when io.seed.valid : @[PRNG.scala 65:25]
;     state[0] <= io.seed.bits[0] @[PRNG.scala 66:11]
;     state[1] <= io.seed.bits[1] @[PRNG.scala 66:11]
;     state[2] <= io.seed.bits[2] @[PRNG.scala 66:11]
;     state[3] <= io.seed.bits[3] @[PRNG.scala 66:11]
;     state[4] <= io.seed.bits[4] @[PRNG.scala 66:11]
;     state[5] <= io.seed.bits[5] @[PRNG.scala 66:11]
;     state[6] <= io.seed.bits[6] @[PRNG.scala 66:11]
;     state[7] <= io.seed.bits[7] @[PRNG.scala 66:11]
;     state[8] <= io.seed.bits[8] @[PRNG.scala 66:11]
;     state[9] <= io.seed.bits[9] @[PRNG.scala 66:11]
;     state[10] <= io.seed.bits[10] @[PRNG.scala 66:11]
;     state[11] <= io.seed.bits[11] @[PRNG.scala 66:11]
;     state[12] <= io.seed.bits[12] @[PRNG.scala 66:11]
;     state[13] <= io.seed.bits[13] @[PRNG.scala 66:11]
;     state[14] <= io.seed.bits[14] @[PRNG.scala 66:11]
;     state[15] <= io.seed.bits[15] @[PRNG.scala 66:11]
;     skip @[PRNG.scala 65:25]
;   io.out[0] <= state[0] @[PRNG.scala 69:10]
;   io.out[1] <= state[1] @[PRNG.scala 69:10]
;   io.out[2] <= state[2] @[PRNG.scala 69:10]
;   io.out[3] <= state[3] @[PRNG.scala 69:10]
;   io.out[4] <= state[4] @[PRNG.scala 69:10]
;   io.out[5] <= state[5] @[PRNG.scala 69:10]
;   io.out[6] <= state[6] @[PRNG.scala 69:10]
;   io.out[7] <= state[7] @[PRNG.scala 69:10]
;   io.out[8] <= state[8] @[PRNG.scala 69:10]
;   io.out[9] <= state[9] @[PRNG.scala 69:10]
;   io.out[10] <= state[10] @[PRNG.scala 69:10]
;   io.out[11] <= state[11] @[PRNG.scala 69:10]
;   io.out[12] <= state[12] @[PRNG.scala 69:10]
;   io.out[13] <= state[13] @[PRNG.scala 69:10]
;   io.out[14] <= state[14] @[PRNG.scala 69:10]
;   io.out[15] <= state[15] @[PRNG.scala 69:10]