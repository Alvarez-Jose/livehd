//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.
#pragma once

#include <vector>

#include "iassert.hpp"
#include "boost/multiprecision/cpp_int.hpp"
#include "absl/types/span.h"

using Bits_t  = uint32_t;  // bits type (future use)
constexpr int     Bits_bits      = 17;

class Lconst {
private:
  constexpr static int char_to_bits[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 0, 0, 0, 0, 0, 0,
    0, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };
  constexpr static int char_to_val[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
  };
  constexpr static int char_to_shift_mode[256] = {
    /* 00 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 10 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 20 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 30 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 40 */ 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    /* 50 */ 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0,
    /* 60 */ 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    /* 70 */ 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0,
    /* 80 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 90 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* A0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* B0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* C0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* D0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* E0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* F0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };

  static Bits_t read_bits(std::string_view txt);
  void process_ending(std::string_view txt, size_t pos);

protected:
  using Number=boost::multiprecision::cpp_int;

  bool     explicit_str;
  bool     explicit_sign;
  bool     explicit_bits;
  bool     sign;

  Bits_t bits;
  Number   num;

  void add_pyrope_bits(std::string *str) const;

  std::string_view skip_underscores(std::string_view txt) const;

  Lconst(bool str, bool a, bool b, bool c, Bits_t d, Number n) : explicit_str(str), explicit_sign(a), explicit_bits(b), sign(c), bits(d), num(n) {}

  Bits_t calc_bits() const {
    if (num == 0)
      return 1;
    if (num>0)
      return msb(num)+2; // +2 because values are signed
    return msb(-num)+2;
  }
  bool same_explicit_bits(const Lconst &o) const {
    bool s1 = explicit_bits && o.explicit_bits && bits == o.bits;
    bool s2 = !explicit_bits || !o.explicit_bits;

    return s1 || s2;
  }

  Number get_num() const { return num; }
public:
  using Container=std::vector<unsigned char>;

  Lconst(absl::Span<unsigned char> v);
  Lconst(const Container &v);
  Lconst(std::string_view txt);
  Lconst(Number v);
  Lconst(int64_t v);
  //Lconst(int64_t v, Bits_t bits);
  Lconst();

  Container serialize() const;
  uint64_t hash() const;

  void dump() const;

  [[nodiscard]] Lconst add_op(const Lconst &o) const;
  [[nodiscard]] Lconst sub_op(const Lconst &o) const;
  [[nodiscard]] Lconst lsh_op(Bits_t amount) const;
  [[nodiscard]] Lconst rsh_op(Bits_t amount) const;
  [[nodiscard]] Lconst or_op(const Lconst &o) const;
  [[nodiscard]] Lconst and_op(const Lconst &o) const;

  [[nodiscard]] bool   eq_op(const Lconst &o) const;

  [[nodiscard]] Lconst adjust_bits(Bits_t amount) const;

  bool     is_unsigned() const { return !sign; }
  // WARNING: unsigned can still be negative. It is a way to indicate as many 1s are needed
  bool     is_negative() const { return sign && num < 0; }
  bool     is_explicit_sign() const { return explicit_sign; }
  bool     is_explicit_bits() const { return explicit_bits; }
  bool     is_string() const { return explicit_str; }
  Bits_t get_bits() const { return bits; }

  bool is_i() const { return !explicit_str && bits <= 62; } // 62 to handle sign (int)
  int64_t to_i() const; // must fit in int or exception raised

  std::string to_yosys() const;
  std::string to_verilog() const;
  std::string to_string() const;
  std::string to_string_no_xz() const;
  std::string to_pyrope() const;
  std::string to_firrtl() const;

  // Operator list
  [[nodiscard]] const Lconst operator+(const Lconst &other) const { return add_op(other); }
  [[nodiscard]] const Lconst operator+(int64_t other) const { return add_op(Lconst(other)); }

  [[nodiscard]] const Lconst operator-(const Lconst &other) const { return sub_op(other); }
  [[nodiscard]] const Lconst operator-(int64_t other) const { return sub_op(Lconst(other)); }

  [[nodiscard]] const Lconst operator<<(const Lconst &other) const { return lsh_op(other.to_i()); }
  [[nodiscard]] const Lconst operator<<(Bits_t other) const { return lsh_op(other); }

  [[nodiscard]] const Lconst operator|(const Lconst &other) const { return or_op(other); }
  [[nodiscard]] const Lconst operator|(int64_t other) const { return or_op(Lconst(other)); }

#if 0
  bool equals_op(const Lconst &other) const {
    // similar to ==, but ignore explicit bits
    auto b = std::max(bits,other.bits);
    return get_num(b) == other.get_num(b);
  }
#endif

  bool operator==(const Lconst &other) const {
    return get_num() == other.get_num() && same_explicit_bits(other);
  }
  bool operator!=(const Lconst &other) const {
    return get_num() != other.get_num() || !same_explicit_bits(other);
  }

  bool operator==(int other) const {
    if (bits>63)
      return false;
    return get_num() == other && !explicit_bits;
  }
  bool operator!=(int other) const {
    if (bits>63)
      return true;
    return get_num() != other || explicit_bits;
  }

  bool operator<(const Lconst &other) const  { return num <  other.num; }
  bool operator<=(const Lconst &other) const { return num <= other.num; }
  bool operator>(const Lconst &other) const  { return num >  other.num; }
  bool operator>=(const Lconst &other) const { return num >= other.num; }

  Number get_raw_num() const { return num; } // for debugging mostly
};

