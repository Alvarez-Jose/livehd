const assert = require('assert');
assert(50 < 70, 'this is not true');
// CITATION
// https://tc39.es/proposal-bigint/#sec-exp-operator
// https://stackoverflow.com/questions/54758130/how-to-obtain-the-amount-of-bits-of-a-bigint

// 🐅🐅🐅testing workspace🐀🐀🐀

// helper function
function BigIntnumberConversion(target = '0', from_base = 10, to_base = 10) {
  const numberString = parseInt(target, from_base).toString(to_base);
  /* console.log(target, "numberString", numberString) */
  return BigInt(numberString);
}

function isDigit(str) {
  return /^\d+$/.test(str);
}

// console.log('answer', BigIntnumberConversion('1', -1));

class Lconst {
  constructor(
    number,
    explicit_str = false,
    bits = 0,
    num = 0,
    has_unknown = false
  ) {
    this.number = number;
    this.explicit_str = explicit_str;
    this.bits = bits;
    this.num = num;
    this.binary = undefined;
    this.unknown = undefined;
    this.has_unknown = has_unknown;
    this.initialized();
  }

  initialized() {
    if (this.number) {
      if (typeof this.number === 'string') {
        this.explicit_str = false;
        this.bits = 0;
        this.num = 0;
      } else {
        //question: how to deal with the size of number is int64_t or Number(bigInt)
        this.explicit_str = false;
        this.num = this.number;
        this.bits = Lconst.calc_num_bits(this.num);
      }
    }
  }
  // ======== support functions ======================
  static calc_num_bits(number) {
    // count implicit sign bits
    const bigI = number > 0 ? BigInt(number) : -1n * BigInt(number);
    const binaryForm = bigI.toString(2);
    return binaryForm.length + 1;
  }

  static new_lconst(explicit_str, bits, num, has_unknown = false) {
    const new_l = new Lconst();
    new_l.explicit_str = explicit_str;
    new_l.bits = bits;
    new_l.num = num;
    new_l.has_unknown = has_unknown;
    return new_l;
  }

  // ======= new Lconst returned =======================
  static from_pyrope(number_str) {
    // check, the input must be a string
    if (typeof number_str != 'string') {
      throw 'the input must be a string';
    }

    const txt = number_str.toLowerCase();

    // special cases
    if (txt === 'true') {
      return Lconst(false, 1, -1);
    } else if (txt == 'false') {
      return Lconst(false, 1, 0);
    }

    let skip_chars = 0;
    let shift_mode = -1;
    var negative = false; // does it have negative sign?
    let unsigned_result = false; // true if start with 0sb

    if (txt[0] === '-') {
      negative = true;
      skip_chars = 1;
    } else if (txt[0] === '+') {
      skip_chars = 1;
    }

    if (txt.length >= skip_chars + 1 && isDigit(txt[skip_chars])) {
      shift_mode = 10;
      if (txt.length >= 2 + skip_chars && txt[skip_chars] === '0') {
        skip_chars += 1;
        const sel_ch = txt[skip_chars];

        if (sel_ch === 's') {
          skip_chars += 1;
          sel_ch = txt[skip_chars];
          if (sel_ch != 'b') {
            throw `ERROR: ${number_str} unknown pyrope encoding only binary can be signed 0sb...`;
          }
          assert(
            !unsigned_result,
            `ERROR: ${number_str} have unsigned_result = FALSE...`
          );
        } else {
          unsigned_result = true;
        }

        if (sel_ch === 'x') {
          shift_mode = 16;
          skip_chars += 1;
        } else if (sel_ch === 'b') {
          shift_mode = 2;
          skip_chars += 1;
        } else if (sel_ch === 'd') {
          shift_mode = 10;
          skip_chars += 1;
        } else if (isDigit(sel_ch)) {
          shift_mode = 8;
        } else if (sel_ch === 'o') {
          shift_mode = 8;
          skip_chars += 1;
        } else {
          throw `ERROR: ${number_str} unknown pyrope encoding (leading ${sel_ch})...`;
        }
      }
    }

    let num = BigInt(0);
    let to_power = -1n;

    if (shift_mode === 10) {
      for (let i = skip_chars; i < txt.length; i++) {
        if (txt[i] >= 0) {
          num = 10n * num + BigInt(txt[i]);
        } else {
          if (txt[i] === '_') {
            continue;
          }
          throw `ERROR: ${number_str} encoding could not use ${txt[i]}`;
        }
      }
    } else if (shift_mode === 2) {
      let v = Lconst.from_binary(txt.substring(skip_chars), unsigned_result);
      if (!negative) return v;
      // Prob:
      num = -v.num;
      return Lconst.new_lconst(false, this.calc_num_bits(num), num);
    } else {
      assert(
        shift_mode === 16 || shift_mode === 8,
        `ERROR: ${number_str} should be either hexa or octal...`
      );
      for (let i = txt.length - 1; i >= skip_chars; --i) {
        if (txt[i] === '_') {
          continue;
        }
        to_power += 1n;
        // console.log(i + ' current letter is ' + txt[i] + ' shift mode ' + shift_mode + ' to power ' + to_power);
        num +=
          BigIntnumberConversion(txt[i], shift_mode) *
          BigInt(shift_mode) ** to_power;
      }
    }

    if (negative) {
      num = -num;
      if (unsigned_result && num < 0n) {
        throw `ERROR, ${number_str} negative value but it must be unsigned`;
      }
    }

    return Lconst.new_lconst(false, Lconst.calc_num_bits(num), num);
  } // end of from_pyrope

  static from_binary(txt, unsigned_result) {
    assert(
      typeof unsigned_result === 'boolean',
      `ERROR: unsigned_result ${unsigned_result} is not Boolean type`
    );
    const ori_txt = txt.replace(/_/g, '');
    let binary = '';
    let unknown = '';
    let unknown_found = false;
    let num = 0n;

    if (unsigned_result) {
      binary = unknown = '0';
    } else {
      binary += ori_txt[0];
      unknown += '0';
    }

    for (const char of ori_txt) {
      console.log('-----current char----', char);
      console.log('check the value of binary: ', binary);
      if (char === '?' || char === 'x' || char === 'z') {
        binary += '0';
        unknown += '1';
        unknown_found = true;
      } else if (char === '0') {
        if (binary !== '0') {
          console.log('get inside of the loop: ', binary);
          binary += '0';
          unknown += '0';
          num = num << 1n;
          console.log('what1', num);
        }
      } else if (char === '1') {
        if (binary !== '1') {
          console.log('get inside of the loop: ', binary);
          binary += '1';
          unknown += '0';
          num = (num << 1n) | 1n;
          console.log('what2', num);
        }
      } else {
        throw `ERROR: ${txt} binary encoding could not use ${char}`;
      }
    }

    if (!unsigned_result && binary[0] === '1') {
      console.log(num);
      num = num - (1n << (BigInt(binary.length) - 1n));
    }

    let return_Lconst = Lconst.new_lconst(
      false,
      Lconst.calc_num_bits(num),
      num,
      unknown_found
    );

    return_Lconst.binary = binary;
    return_Lconst.unknown = unknown;
    return return_Lconst;
  }

  // restriction: only from decimal to pyrope
  to_pyrope() {
    let output = '0x';
    return output + this.num.toString(16);
  }

  // ========= operation =============
  // restriction: the num of these two objects do not have underscore and '?'
  xor_op(com_lconst) {
    const num = this.num ^ com_lconst.num;
    return Lconst.new_lconst(false, Lconst.calc_num_bits(num), num);
  }

  sayHello() {
    console.log('I am a Lconst object :)');
  }
} // end of the class ————————————————————————————————————————————

// 🐕testing workspace for Lconst🐇
let testing = Lconst.from_binary('__111110', false);
console.log(testing.binary, testing.unknown, testing.num, testing.has_unknown);

module.exports = Lconst;
