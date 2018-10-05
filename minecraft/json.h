#pragma once

#include <string>

namespace Json {

struct StaticString {
  char const *data;
  StaticString(char const *);
  char const *c_str();
  operator char const *();
};

struct Value {
  enum struct ValueType : char {
    Null     = 0,
    Integer  = 1,
    Unsigned = 2,
    Double   = 3,
    String   = 4,
    Boolean  = 5,
    Array    = 6,
    Object   = 7,
  };
  union {
    double val_double;
    long long val_int;
    unsigned long long val_uint;
    bool val_bool;
    char *val_str;
  };
  ValueType type : 2;
  bool release : 1;

  Value(Value const &);
  Value(ValueType);
  Value(int);
  Value(long long);
  Value(unsigned);
  Value(unsigned long long);
  Value(double);
  Value(char const*);
  Value(char const*, char const*);
  Value(std::string const&);
  Value(StaticString);
};

} // namespace Json